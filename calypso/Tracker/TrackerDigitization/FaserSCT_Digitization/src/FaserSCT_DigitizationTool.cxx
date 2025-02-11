/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_DigitizationTool.h"

// Mother Package includes
#include "FaserSiDigitization/SiHelper.h"
#include "FaserSiDigitization/SiChargedDiodeCollection.h"

// EDM includes
#include "TrackerRawData/FaserSCT1_RawData.h"
#include "TrackerRawData/FaserSCT3_RawData.h"

// Hit class includes
#include "TrackerSimEvent/FaserSiHit.h"
#include "Identifier/Identifier.h"

// Det Descr includes
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerReadoutGeometry/SCT_ModuleSideDesign.h"

// Data Handle
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadHandle.h"

// Random Number Generation
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"

// C++ Standard Library
#include <memory>
#include <sstream>

// Barcodes at the HepMC level are int

using TrackerDD::SiCellId;

FaserSCT_DigitizationTool::FaserSCT_DigitizationTool(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent) :
  base_class(type, name, parent) {
  m_WriteSCT1_RawData.declareUpdateHandler(&FaserSCT_DigitizationTool::SetupRdoOutputType, this);
}

FaserSCT_DigitizationTool::~FaserSCT_DigitizationTool() {
  delete m_thpcsi;
  for (FaserSiHitCollection* hit: m_hitCollPtrs) {
    hit->Clear();
    delete hit;
  }
  m_hitCollPtrs.clear();
}

// ----------------------------------------------------------------------
// Initialize method:
// ----------------------------------------------------------------------
StatusCode FaserSCT_DigitizationTool::initialize() {
  ATH_MSG_DEBUG("FaserSCT_DigitizationTool::initialize()");

  // +++ Init the services
  ATH_CHECK(initServices());

  // +++ Get the Surface Charges Generator tool
  ATH_CHECK(initSurfaceChargesGeneratorTool());

  // +++ Get the Front End tool
  ATH_CHECK(initFrontEndTool());

  // +++ Initialise for disabled cells from the random disabled cells tool
  // +++ Default off, since disabled cells taken form configuration in
  // reconstruction stage
  if (m_randomDisabledCells) {
    ATH_CHECK(initDisabledCells());
    ATH_MSG_INFO("Use of Random disabled cells");
  } else {
    m_sct_RandomDisabledCellGenerator.disable();
  }

  // check the input object name
  if (m_hitsContainerKey.key().empty()) {
    ATH_MSG_FATAL("Property InputObjectName not set !");
    return StatusCode::FAILURE;
  }
  if(m_onlyUseContainerName) m_inputObjectName = m_hitsContainerKey.key();
  ATH_MSG_DEBUG("Input objects in container : '" << m_inputObjectName << "'");

  // Initialize ReadHandleKey
  ATH_CHECK(m_hitsContainerKey.initialize(!m_onlyUseContainerName));

  // +++ Initialize WriteHandleKey
  ATH_CHECK(m_rdoContainerKey.initialize());
  ATH_CHECK(m_simDataCollMapKey.initialize());

  // Initialize ReadCondHandleKey
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  ATH_MSG_DEBUG("FaserSCT_DigitizationTool::initialize() complete");

  return StatusCode::SUCCESS;
}

namespace {
  class SiDigitizationSurfaceChargeInserter
    : public ISiSurfaceChargesInserter
  {
  public:
    SiDigitizationSurfaceChargeInserter(const TrackerDD::SiDetectorElement* sielement,
                                        SiChargedDiodeCollection* chargedDiodes)
      : m_sielement(sielement),
        m_chargedDiodes(chargedDiodes) {
    }

    void operator () (const SiSurfaceCharge& scharge) const;
  private:
    const TrackerDD::SiDetectorElement* m_sielement;
    SiChargedDiodeCollection* m_chargedDiodes;
  };


  void SiDigitizationSurfaceChargeInserter::operator ()
    (const SiSurfaceCharge& scharge) const {
    // get the diode in which this charge is
    SiCellId diode{m_sielement->cellIdOfPosition(scharge.position())};

    if (diode.isValid()) {
      // add this charge to the collection (or merge in existing charged diode)
      m_chargedDiodes->add(diode, scharge.charge());
    }
  }
} // anonymous namespace

// ----------------------------------------------------------------------
// Initialise the surface charge generator Tool
// ----------------------------------------------------------------------
StatusCode FaserSCT_DigitizationTool::initSurfaceChargesGeneratorTool() {
  ATH_CHECK(m_sct_SurfaceChargesGenerator.retrieve());

  if (m_cosmicsRun and m_tfix > -998) {
    m_sct_SurfaceChargesGenerator->setFixedTime(m_tfix);
    ATH_MSG_INFO("Use of FixedTime = " << m_tfix << " in cosmics");
  }

  ATH_MSG_DEBUG("Retrieved and initialised tool " << m_sct_SurfaceChargesGenerator);

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// Initialise the Front End electronics Tool
// ----------------------------------------------------------------------
StatusCode FaserSCT_DigitizationTool::initFrontEndTool() {
  ATH_CHECK(m_sct_FrontEnd.retrieve());

  storeTool(&(*m_sct_FrontEnd));

  ATH_MSG_DEBUG("Retrieved and initialised tool " << m_sct_FrontEnd);
  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// Initialize the different services
// ----------------------------------------------------------------------
StatusCode FaserSCT_DigitizationTool::initServices() {
  // Get SCT ID helper for hash function and Store them using methods from the
  // SiDigitization.
  ATH_CHECK(detStore()->retrieve(m_detID, "FaserSCT_ID"));

  ATH_CHECK(m_mergeSvc.retrieve());
  ATH_CHECK(m_rndmSvc.retrieve());

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// Initialize the disabled cells for cosmics or CTB cases
// ----------------------------------------------------------------------
StatusCode FaserSCT_DigitizationTool::initDisabledCells() {
  // +++ Retrieve the SCT_RandomDisabledCellGenerator
  ATH_CHECK(m_sct_RandomDisabledCellGenerator.retrieve());

  storeTool(&(*m_sct_RandomDisabledCellGenerator));

  ATH_MSG_INFO("Retrieved the SCT_RandomDisabledCellGenerator tool:" << m_sct_RandomDisabledCellGenerator);
  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_DigitizationTool::processAllSubEvents(const EventContext& ctx) {
  if (prepareEvent(ctx, 0).isFailure()) {
    return StatusCode::FAILURE;
  }
  // Set the RNG to use for this event.
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );
  CLHEP::HepRandomEngine *rndmEngine = rngWrapper->getEngine(ctx);

  ATH_MSG_VERBOSE("Begin digitizeAllHits");
  if (m_enableHits and (not getNextEvent(ctx).isFailure())) {
    digitizeAllHits(ctx, &m_rdoContainer, &m_simDataCollMap, &m_processedElements, m_thpcsi, rndmEngine);
  } else {
    ATH_MSG_DEBUG("no hits found in event!");
  }
  ATH_MSG_DEBUG("Digitized Elements with Hits");

  // loop over elements without hits
  if (not m_onlyHitElements) {
    digitizeNonHits(ctx, &m_rdoContainer, &m_simDataCollMap, &m_processedElements, rndmEngine);
    ATH_MSG_DEBUG("Digitized Elements without Hits");
  }

  delete m_thpcsi;
  m_thpcsi = nullptr;

  ATH_MSG_VERBOSE("Digitize success!");
  return StatusCode::SUCCESS;
}

// ======================================================================
// prepareEvent
// ======================================================================
StatusCode FaserSCT_DigitizationTool::prepareEvent(const EventContext& ctx, unsigned int /*index*/) {
  ATH_MSG_VERBOSE("FaserSCT_DigitizationTool::prepareEvent()");
  // Create the IdentifiableContainer to contain the digit collections Create
  // a new RDO container
  m_rdoContainer = SG::makeHandle(m_rdoContainerKey, ctx);
  ATH_CHECK(m_rdoContainer.record(std::make_unique<FaserSCT_RDO_Container>(m_detID->wafer_hash_max())));

  // Create a map for the SDO and register it into StoreGate
  m_simDataCollMap = SG::makeHandle(m_simDataCollMapKey, ctx);
  ATH_CHECK(m_simDataCollMap.record(std::make_unique<TrackerSimDataCollection>()));

  m_processedElements.clear();
  m_processedElements.resize(m_detID->wafer_hash_max(), false);

  m_thpcsi = new TimedHitCollection<FaserSiHit>();
//   m_HardScatterSplittingSkipper = false;
  return StatusCode::SUCCESS;
}

// =========================================================================
// mergeEvent
// =========================================================================
StatusCode FaserSCT_DigitizationTool::mergeEvent(const EventContext& ctx) {
  ATH_MSG_VERBOSE("FaserSCT_DigitizationTool::mergeEvent()");

  // Set the RNG to use for this event.
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );
  CLHEP::HepRandomEngine *rndmEngine = rngWrapper->getEngine(ctx);

  if (m_enableHits) {
    digitizeAllHits(ctx, &m_rdoContainer, &m_simDataCollMap, &m_processedElements, m_thpcsi, rndmEngine);
  }

  if (not m_onlyHitElements) {
    digitizeNonHits(ctx, &m_rdoContainer, &m_simDataCollMap, &m_processedElements, rndmEngine);
  }

  for (FaserSiHitCollection* hit: m_hitCollPtrs) {
    hit->Clear();
    delete hit;
  }
  m_hitCollPtrs.clear();

  delete m_thpcsi;
  m_thpcsi = nullptr;

  ATH_MSG_DEBUG("Digitize success!");
  return StatusCode::SUCCESS;
}

void FaserSCT_DigitizationTool::digitizeAllHits(const EventContext& ctx, SG::WriteHandle<FaserSCT_RDO_Container>* rdoContainer, SG::WriteHandle<TrackerSimDataCollection>* simDataCollMap, std::vector<bool>* processedElements, TimedHitCollection<FaserSiHit>* thpcsi, CLHEP::HepRandomEngine * rndmEngine) const {
  /////////////////////////////////////////////////
  //
  // In order to process all element rather than just those with hits we
  // create a vector to keep track of which elements have been processed.
  // NB. an element is an sct module
  //
  /////////////////////////////////////////////////
  ATH_MSG_DEBUG("Digitizing hits");
  int hitcount{0}; // First, elements with hits.

  SiChargedDiodeCollection chargedDiodes;

  while (digitizeElement(ctx, &chargedDiodes, thpcsi, rndmEngine)) {
    ATH_MSG_DEBUG("Hit collection ID=" << m_detID->show_to_string(chargedDiodes.identify()));

    hitcount++;  // Hitcount will be a number in the hit collection minus
    // number of hits in missing mods

    ATH_MSG_DEBUG("in digitize elements with hits: station - layer - eta - phi  "
                  << m_detID->station(chargedDiodes.identify()) << " - "
                  << m_detID->layer(chargedDiodes.identify()) << " - "
                  << m_detID->eta_module(chargedDiodes.identify()) << " - "
                  << m_detID->phi_module(chargedDiodes.identify()) << " - "
                  << " processing hit number " << hitcount);

    // Have a flag to check if the module is present or not
    // Generally assume it is:

    IdentifierHash idHash{chargedDiodes.identifyHash()};

    assert(idHash < processedElements->size());
    (*processedElements)[idHash] = true;

    // create and store RDO and SDO

    if (not chargedDiodes.empty()) {
      StatusCode sc{createAndStoreRDO(&chargedDiodes, rdoContainer)};
      if (sc.isSuccess()) { // error msg is given inside
        // createAndStoreRDO()
        addSDO(&chargedDiodes, simDataCollMap);
      }
    }

    chargedDiodes.clear();
  }
  ATH_MSG_DEBUG("hits processed");
  return;
}

// digitize elements without hits
void FaserSCT_DigitizationTool::digitizeNonHits(const EventContext& ctx, SG::WriteHandle<FaserSCT_RDO_Container>* rdoContainer, SG::WriteHandle<TrackerSimDataCollection>* simDataCollMap, const std::vector<bool>* processedElements, CLHEP::HepRandomEngine * rndmEngine) const {
  // Get SCT_DetectorElementCollection
  SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey, ctx);
  const TrackerDD::SiDetectorElementCollection* elements{sctDetEle.retrieve()};
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return;
  }

  ATH_MSG_DEBUG("processing elements without hits");
  SiChargedDiodeCollection chargedDiodes;

  for (unsigned int i{0}; i < processedElements->size(); i++) {
    if (not (*processedElements)[i]) {
      IdentifierHash idHash{i};
      if (not idHash.is_valid()) {
        ATH_MSG_ERROR("SCT Detector element id hash is invalid = " << i);
      }

      const TrackerDD::SiDetectorElement* element{elements->getDetectorElement(idHash)};
      if (element) {
        ATH_MSG_DEBUG("In digitize of untouched elements: layer - phi - eta  "
                      << m_detID->layer(element->identify()) << " - "
                      << m_detID->phi_module(element->identify()) << " - "
                      << m_detID->eta_module(element->identify()) << " - "
                      << "size: " << processedElements->size());

        chargedDiodes.setDetectorElement(element);
        ATH_MSG_DEBUG("calling applyProcessorTools() for NON hits");
        applyProcessorTools(&chargedDiodes, rndmEngine);

        // Create and store RDO and SDO
        // Don't create empty ones.
        if (not chargedDiodes.empty()) {
          StatusCode sc{createAndStoreRDO(&chargedDiodes, rdoContainer)};
          if (sc.isSuccess()) {// error msg is given inside
            // createAndStoreRDO()
            addSDO(&chargedDiodes, simDataCollMap);
          }
        }

        chargedDiodes.clear();
      }
    }
  }

  return;
}

bool FaserSCT_DigitizationTool::digitizeElement(const EventContext& ctx, SiChargedDiodeCollection* chargedDiodes, TimedHitCollection<FaserSiHit>*& thpcsi, CLHEP::HepRandomEngine * rndmEngine) const {
  if (nullptr == thpcsi) {
    ATH_MSG_ERROR("thpcsi should not be nullptr!");

    return false;
  }

  // get the iterator pairs for this DetEl

  TimedHitCollection<FaserSiHit>::const_iterator i, e;
  if (thpcsi->nextDetectorElement(i, e) == false) { // no more hits
    return false;
  }

  // create the identifier for the collection:
  ATH_MSG_DEBUG("create ID for the hit collection");
  const TimedHitPtr<FaserSiHit>& firstHit{*i};
  int barrel{firstHit->getStation()};
  Identifier id{m_detID->wafer_id(barrel,
                                  firstHit->getPlane(),
                                  firstHit->getRow(),
                                  firstHit->getModule(),
                                  firstHit->getSensor())};
  IdentifierHash waferHash{m_detID->wafer_hash(id)};

  // Get SCT_DetectorElementCollection
  SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey, ctx);
  const TrackerDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return false;
  }

  // get the det element from the manager
  const TrackerDD::SiDetectorElement* sielement{elements->getDetectorElement(waferHash)};

  if (sielement == nullptr) {
    ATH_MSG_DEBUG("Station=" << barrel << " layer=" << firstHit->getPlane() << " Row=" << firstHit->getRow() << " Module=" << firstHit->getModule() << " Side=" << firstHit->getSensor());
    ATH_MSG_ERROR("detector manager could not find element with id = " << id);
    return false;
  }
  // create the charged diodes collection
  chargedDiodes->setDetectorElement(sielement);

  // Loop over the hits and created charged diodes:
  while (i != e) {
    TimedHitPtr<FaserSiHit> phit{*i++};

    // skip hits which are more than 10us away
    if (fabs(phit->meanTime()) < 10000. * CLHEP::ns) {
      ATH_MSG_DEBUG("HASH = " << m_detID->wafer_hash(m_detID->wafer_id(phit->getStation(),
                                                                       phit->getPlane(),
                                                                       phit->getRow(),
                                                                       phit->getModule(),
                                                                       phit->getSensor())));
      ATH_MSG_DEBUG("calling process() for all methods");
      m_sct_SurfaceChargesGenerator->process(sielement, phit, SiDigitizationSurfaceChargeInserter(sielement, chargedDiodes), rndmEngine, ctx);
      ATH_MSG_DEBUG("charges filled!");
    }
  }
  applyProcessorTools(chargedDiodes, rndmEngine); // !< Use of the new AlgTool surface
  // charges generator class
  return true;
}

// -----------------------------------------------------------------------------
// Applies processors to the current detector element for the current element:
// -----------------------------------------------------------------------------
void FaserSCT_DigitizationTool::applyProcessorTools(SiChargedDiodeCollection* chargedDiodes, CLHEP::HepRandomEngine * rndmEngine) const {
  ATH_MSG_DEBUG("applyProcessorTools()");
  int processorNumber{0};

  for (ISiChargedDiodesProcessorTool* proc: m_diodeCollectionTools) {
    proc->process(*chargedDiodes, rndmEngine);

    processorNumber++;
    ATH_MSG_DEBUG("Applied processor # " << processorNumber);
  }
}

StatusCode FaserSCT_DigitizationTool::processBunchXing(int bunchXing,
                                                  SubEventIterator bSubEvents,
                                                  SubEventIterator eSubEvents) {
    ATH_MSG_VERBOSE("FaserSCT_DigitizationTool::processBunchXing() " << bunchXing);

    typedef PileUpMergeSvc::TimedList<FaserSiHitCollection>::type TimedHitCollList;
    TimedHitCollList hitCollList;

    if ((not (m_mergeSvc->retrieveSubSetEvtData(m_inputObjectName, hitCollList, bunchXing,
                                                bSubEvents, eSubEvents).isSuccess())) and
        hitCollList.size() == 0) {
      ATH_MSG_ERROR("Could not fill TimedHitCollList");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_VERBOSE(hitCollList.size() << " SiHitCollections with key " <<
		      m_inputObjectName << " found");
    }

    TimedHitCollList::iterator endColl{hitCollList.end()};
    for (TimedHitCollList::iterator iColl{hitCollList.begin()}; iColl != endColl; iColl++) {
      FaserSiHitCollection *hitCollPtr{new FaserSiHitCollection(*iColl->second)};
      PileUpTimeEventIndex timeIndex{iColl->first};
      ATH_MSG_DEBUG("SiHitCollection found with " << hitCollPtr->size() <<
                    " hits");
      ATH_MSG_VERBOSE("time index info. time: " << timeIndex.time()
		      << " index: " << timeIndex.index()
		      << " type: " << timeIndex.type());
      m_thpcsi->insert(timeIndex, hitCollPtr);
      m_hitCollPtrs.push_back(hitCollPtr);
    }

    return StatusCode::SUCCESS;

}

// =========================================================================
// property handlers
// =========================================================================
void FaserSCT_DigitizationTool::SetupRdoOutputType(Gaudi::Details::PropertyBase&) {
}

// Does nothing, but required by Gaudi

// ----------------------------------------------------------------------
// Digitisation of non hit elements
// ----------------------------------------------------------------------

class DigitizeNonHitElementsDebugPrinter
{
public:
  DigitizeNonHitElementsDebugPrinter(const FaserSCT_ID* detID) :
    m_detID{detID}, m_msgNo{-1} {
    }

  std::string msg(const TrackerDD::SiDetectorElement* element) {
    std::ostringstream ost;

    ost << "Digitized unprocessed elements: layer - phi - eta - side  "
        << m_detID->layer(element->identify()) << " - "
        << m_detID->phi_module(element->identify()) << " - "
        << m_detID->eta_module(element->identify()) << " - "
        << m_detID->side(element->identify()) << " - "
        << " unprocessed hit number: " << ++m_msgNo << '\n';

    return ost.str();
  }

private:
  const FaserSCT_ID* m_detID;
  int m_msgNo;
};

// ----------------------------------------------------------------------//
// createAndStoreRDO                                                     //
// ----------------------------------------------------------------------//
StatusCode FaserSCT_DigitizationTool::createAndStoreRDO(SiChargedDiodeCollection* chDiodeCollection, SG::WriteHandle<FaserSCT_RDO_Container>* rdoContainer) const {

  // Create the RDO collection
  FaserSCT_RDO_Collection* RDOColl{createRDO(chDiodeCollection)};

  // Add it to storegate
  if ((*rdoContainer)->addCollection(RDOColl, RDOColl->identifyHash()).isFailure()) {
      ATH_MSG_FATAL("SCT RDO collection could not be added to container!");
      delete RDOColl;
      RDOColl = nullptr;
      return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
} // SCT_Digitization::createAndStoreRDO()

// ----------------------------------------------------------------------
// createRDO
// ----------------------------------------------------------------------
FaserSCT_RDO_Collection* FaserSCT_DigitizationTool::createRDO(SiChargedDiodeCollection* collection) const {

  // create a new SCT RDO collection
  FaserSCT_RDO_Collection* p_rdocoll{nullptr};

  // need the DE identifier
  const Identifier id_de{collection->identify()};
  IdentifierHash idHash_de{collection->identifyHash()};
  try {
    p_rdocoll = new FaserSCT_RDO_Collection(idHash_de);
  } catch (const std::bad_alloc&) {
    ATH_MSG_FATAL("Could not create a new FaserSCT_RDORawDataCollection !");
  }
  p_rdocoll->setIdentifier(id_de);

  SiChargedDiodeIterator i_chargedDiode{collection->begin()};
  SiChargedDiodeIterator i_chargedDiode_end{collection->end()};
  // Choice of producing FaserSCT1_RawData or FaserSCT3_RawData
  if (m_WriteSCT1_RawData.value()) {
    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
      unsigned int flagmask{static_cast<unsigned int>((*i_chargedDiode).second.flag() & 0xFE)};

      if (!flagmask) { // now check it wasn't masked:
        // create new SCT RDO, using method 1 for mask:
        // GroupSize=1: need readout id, make use of
        // SiTrackerDetDescr
        TrackerDD::SiReadoutCellId roCell{(*i_chargedDiode).second.getReadoutCell()};
        int strip{roCell.strip()};
        if (strip > 0xffff) { // In upgrade layouts strip can be bigger
          // than 4000
          ATH_MSG_FATAL("Strip number too big for SCT1 raw data format.");
        }
        const Identifier id_readout{m_detID->strip_id(collection->identify(), strip)};

        // build word, masks taken from SiTrackerEvent/SCTRawData.cxx
        const unsigned int strip_rdo{static_cast<unsigned int>((strip & 0xFFFF) << 16)};

        // user can define what GroupSize is, here 1: TC. Incorrect,
        // GroupSize >= 1
        int size{SiHelper::GetStripNum((*i_chargedDiode).second)};
        unsigned int size_rdo{static_cast<unsigned int>(size & 0xFFFF)};

        // TC. Need to check if there are disabled strips in the cluster
        int cluscounter{0};
        if (size > 1) {
          SiChargedDiodeIterator it2{i_chargedDiode};
          ++it2;
          for (; it2 != i_chargedDiode_end; ++it2) {
            ++cluscounter;
            if (cluscounter >= size) {
              break;
            }
            if (it2->second.flag() & 0xDE) {
              int tmp{cluscounter};
              while ((it2 != i_chargedDiode_end) and (cluscounter < size - 1) and (it2->second.flag() & 0xDE)) {
                it2++;
                cluscounter++;
              }
              if ((it2 != collection->end()) and !(it2->second.flag() & 0xDE)) {
                SiHelper::ClusterUsed(it2->second, false);
                SiHelper::SetStripNum(it2->second, size - cluscounter, &msg());
              }
              // groupSize=tmp;
              size_rdo = tmp & 0xFFFF;
              break;
            }
          }
        }
        unsigned int SCT_Word{strip_rdo | size_rdo};
        FaserSCT1_RawData* p_rdo{new FaserSCT1_RawData(id_readout, SCT_Word)};
        if (p_rdo) {
          p_rdocoll->push_back(p_rdo);
        }
      }
    }
  } else {
    // Under the current scheme time bin and ERRORS are hard-coded to
    // default values.
    int ERRORS{0};
    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
      unsigned int flagmask{static_cast<unsigned int>((*i_chargedDiode).second.flag() & 0xFE)};

      if (!flagmask) { // Check it wasn't masked
        int tbin{SiHelper::GetTimeBin((*i_chargedDiode).second)};
        // create new SCT RDO
        TrackerDD::SiReadoutCellId roCell{(*i_chargedDiode).second.getReadoutCell()};
        int strip{roCell.strip()};
        Identifier id_readout;
        id_readout = m_detID->strip_id(collection->identify(), strip);
                
        // build word (compatible with
        // SCT_RawDataByteStreamCnv/src/SCT_RodDecoder.cxx)
        int size{SiHelper::GetStripNum((*i_chargedDiode).second)};
        int groupSize{size};

        // TC. Need to check if there are disabled strips in the cluster
        int cluscounter{0};
        if (size > 1) {
          SiChargedDiode* diode{i_chargedDiode->second.nextInCluster()};
          while (diode) {//check if there is a further strip in the cluster
            ++cluscounter;
            if (cluscounter >= size) {
              ATH_MSG_WARNING("Cluster size reached while neighbouring strips still defined.");
              break;
            }
            if (diode->flag() & 0xDE) {//see if it is disabled/below threshold/disconnected/etc (0xDE corresponds to BT_SET | DISABLED_SET | BADTOT_SET | DISCONNECTED_SET | MASKOFF_SET)
              int tmp{cluscounter};
              while ((cluscounter < size - 1) and (diode->flag() & 0xDE)) { //check its not the end and still disabled
                diode = diode->nextInCluster();
                cluscounter++;
              }
              if (diode and !(diode->flag() & 0xDE)) {
                SiHelper::ClusterUsed(*diode, false);
                SiHelper::SetStripNum(*diode, size - cluscounter, &msg());
              }
              groupSize = tmp;
              break;
            }
            diode = diode->nextInCluster();
          }
        }

        int stripIn11bits{strip & 0x7ff};
        if (stripIn11bits != strip) {
          ATH_MSG_DEBUG("Strip number " << strip << " doesn't fit into 11 bits - will be truncated");
        }
                
        unsigned int SCT_Word{static_cast<unsigned int>(groupSize | (stripIn11bits << 11) | (tbin << 22) | (ERRORS << 25))};
        FaserSCT3_RawData *p_rdo{new FaserSCT3_RawData(id_readout, SCT_Word, std::vector<int>{})};
        if (p_rdo) {
          p_rdocoll->push_back(p_rdo);
        }
      }
    }
  }
  return p_rdocoll;
} // SCT_Digitization::createRDO()

// ------------------------------------------------------------
// Get next event and extract collection of hit collections:
// ------------------------------------------------------------
StatusCode FaserSCT_DigitizationTool::getNextEvent(const EventContext& ctx) {
  ATH_MSG_DEBUG("FaserSCT_DigitizationTool::getNextEvent()");
  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<FaserSiHitCollection>::type TimedHitCollList;
  // this is a list<pair<time_t, DataLink<SiHitCollection> >

  // In case of single hits container just load the collection using read handles
  if (!m_onlyUseContainerName) {
    SG::ReadHandle<FaserSiHitCollection> hitCollection(m_hitsContainerKey, ctx);
    if (!hitCollection.isValid()) {
      ATH_MSG_ERROR("Could not get SCT SiHitCollection container " << hitCollection.name() << " from store " << hitCollection.store());
      return StatusCode::FAILURE;
    }

    // create a new hits collection
    m_thpcsi = new TimedHitCollection<FaserSiHit>{1};
    m_thpcsi->insert(0, hitCollection.cptr());
    ATH_MSG_DEBUG("SiHitCollection found with " << hitCollection->size() << " hits");

    return StatusCode::SUCCESS;
  }

  TimedHitCollList hitCollList;
  unsigned int numberOfSiHits{0};
  if (not (m_mergeSvc->retrieveSubEvtsData(m_inputObjectName, hitCollList, numberOfSiHits).isSuccess()) and hitCollList.size() == 0) {
    ATH_MSG_ERROR("Could not fill TimedHitCollList");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG(hitCollList.size() << " SiHitCollections with key " << m_inputObjectName << " found");
  }
  // create a new hits collection
  m_thpcsi = new TimedHitCollection<FaserSiHit>{numberOfSiHits};
  // now merge all collections into one
  TimedHitCollList::iterator endColl{hitCollList.end()};
  for (TimedHitCollList::iterator iColl{hitCollList.begin()}; iColl != endColl; ++iColl) {
    const FaserSiHitCollection* p_collection{iColl->second};
    m_thpcsi->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG("SiTrackerHitCollection found with " << p_collection->size() << " hits"); // loop on the hit collections
  }
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------
// Convert a SiTotalCharge to a TrackerSimData, and store it.
// -----------------------------------------------------------------------------------------------
void FaserSCT_DigitizationTool::addSDO(SiChargedDiodeCollection* collection, SG::WriteHandle<TrackerSimDataCollection>* simDataCollMap) const {
  typedef SiTotalCharge::list_t list_t;
  std::vector<TrackerSimData::Deposit> deposits;
  deposits.reserve(5); // no idea what a reasonable number for this would be
  // with pileup
  // loop over the charged diodes
  SiChargedDiodeIterator EndOfDiodeCollection{collection->end()};
  for (SiChargedDiodeIterator i_chargedDiode{collection->begin()}; i_chargedDiode != EndOfDiodeCollection; ++i_chargedDiode) {
    deposits.clear();
    const list_t& charges{(*i_chargedDiode).second.totalCharge().chargeComposition()};

    bool real_particle_hit{false};
    // loop over the list
    list_t::const_iterator EndOfChargeList{charges.end()};
    for (list_t::const_iterator i_ListOfCharges{charges.begin()}; i_ListOfCharges != EndOfChargeList; ++i_ListOfCharges) {
      const HepMcParticleLink& trkLink{i_ListOfCharges->particleLink()};
      const int barcode{trkLink.barcode()};
      if (HepMC::ignoreTruthLink(barcode, m_vetoPileUpTruthLinks)) {
        continue;
      }
      if (not real_particle_hit) {
        // Types of SiCharges expected from SCT
        // Noise:                        barcode==0 and
        // processType()==SiCharge::noise
        // Delta Rays:                   barcode==0 and
        // processType()==SiCharge::track
        // Pile Up Tracks With No Truth: barcode!=0 and
        // processType()==SiCharge::cut_track
        // Tracks With Truth:            barcode!=0 and
        // processType()==SiCharge::track
        if (barcode != 0 and i_ListOfCharges->processType() == SiCharge::track) {
          real_particle_hit = true;
        }
      }
      // check if this track number has been already used.
      std::vector<TrackerSimData::Deposit>::reverse_iterator theDeposit{deposits.rend()};  // dummy value
      std::vector<TrackerSimData::Deposit>::reverse_iterator depositsR_end{deposits.rend()};
      std::vector<TrackerSimData::Deposit>::reverse_iterator i_Deposit{deposits.rbegin()};
      for (; i_Deposit != depositsR_end; ++i_Deposit) {
        if ((*i_Deposit).first == trkLink) {
          theDeposit = i_Deposit;
          break;
        }
      }

      // if the charge has already hit the Diode add it to the deposit
      if (theDeposit != depositsR_end) {
        (*theDeposit).second += i_ListOfCharges->charge();
      } else { // create a new deposit
        TrackerSimData::Deposit deposit(trkLink, i_ListOfCharges->charge());
        deposits.push_back(deposit);
      }
    }

    // add the simdata object to the map:
    if (real_particle_hit or m_createNoiseSDO) {
      TrackerDD::SiReadoutCellId roCell{(*i_chargedDiode).second.getReadoutCell()};
      int strip{roCell.strip()};
      Identifier id_readout;
      id_readout = m_detID->strip_id(collection->identify(),strip);
      (*simDataCollMap)->insert(std::make_pair(id_readout, TrackerSimData(deposits, (*i_chargedDiode).second.flag())));
    }
  }
}
