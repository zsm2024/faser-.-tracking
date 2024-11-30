/* -*- C++ -*- */

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_DIGITZATION_FASERSCT_DIGITZATIONTOOL_H
#define FASERSCT_DIGITZATION_FASERSCT_DIGITZATIONTOOL_H
/** @file FaserSCT_DigitizationTool.h
 * @brief Digitize the SCT using an implementation of IPileUpTool
 * $Id: FaserSCT_DigitizationTool.h,v 1.0 2009-09-22 18:34:42 jchapman Exp $
 * @author John Chapman - ATLAS Collaboration
 */

//Base class header
#include "PileUpTools/PileUpToolBase.h"

// Athena headers
#include "AthenaKernel/IAthRNGSvc.h"
#include "HitManagement/TimedHitCollection.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "FaserSCT_Digitization/ISCT_FrontEnd.h"
#include "FaserSCT_Digitization/ISCT_RandomDisabledCellGenerator.h"
#include "FaserSCT_Digitization/ISCT_SurfaceChargesGenerator.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// STL headers
#include <limits>
#include <string>

// Forward declarations
class ISiChargedDiodesProcessorTool;
class FaserSCT_ID;
class SiChargedDiodeCollection;

namespace CLHEP
{
  class HepRandomEngine;
}

class FaserSCT_DigitizationTool : public extends<PileUpToolBase, IPileUpTool>
{
public:
  static const InterfaceID& interfaceID();
  FaserSCT_DigitizationTool(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);
  virtual ~FaserSCT_DigitizationTool();
  /**
     @brief Called before processing physics events
  */
  virtual StatusCode prepareEvent(const EventContext& ctx, unsigned int) override final;
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;

  virtual StatusCode initialize() override final;
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

protected:

  bool digitizeElement(const EventContext& ctx, SiChargedDiodeCollection* chargedDiodes, TimedHitCollection<FaserSiHit>*& thpcsi, CLHEP::HepRandomEngine * rndmEngine) const ; //!
  void applyProcessorTools(SiChargedDiodeCollection* chargedDiodes, CLHEP::HepRandomEngine * rndmEngine) const; //!
  void addSDO(SiChargedDiodeCollection* collection, SG::WriteHandle<TrackerSimDataCollection>* simDataCollMap) const;

  void storeTool(ISiChargedDiodesProcessorTool* p_processor) {m_diodeCollectionTools.push_back(p_processor);}

private:

  /**
     @brief initialize the required services
  */
  StatusCode initServices();
  /**
     @brief Initialize the SCT_FrontEnd AlgTool
  */
  StatusCode initFrontEndTool();
  /**
     @brief Initialize the SCT_RandomDisabledCellGenerator AlgTool
  */
  StatusCode initDisabledCells();
  /**
     @brief Initialize the SCT_SurfaceChargesGenerator AlgTool
  */
  StatusCode initSurfaceChargesGeneratorTool();

  /** RDO and SDO methods*/
  /**
     @brief Create RDOs from the SiChargedDiodeCollection for the current wafer and save to StoreGate
     @param chDiodeCollection       list of the SiChargedDiodes on the current wafer
  */
  StatusCode createAndStoreRDO(SiChargedDiodeCollection* chDiodeCollection, SG::WriteHandle<FaserSCT_RDO_Container>* rdoContainer) const;
  /**
     @brief Create RDOs from the SiChargedDiodeCollection for the current wafer
     @param chDiodeCollection       list of the SiChargedDiodes on the current wafer
  */
  FaserSCT_RDO_Collection* createRDO(SiChargedDiodeCollection* collection) const;

  StatusCode getNextEvent(const EventContext& ctx);
  void       digitizeAllHits(const EventContext& ctx, SG::WriteHandle<FaserSCT_RDO_Container>* rdoContainer, SG::WriteHandle<TrackerSimDataCollection>* simDataCollMap, std::vector<bool>* processedElements, TimedHitCollection<FaserSiHit>* thpcsi, CLHEP::HepRandomEngine * rndmEngine) const; //!< digitize all hits
  void       digitizeNonHits(const EventContext& ctx, SG::WriteHandle<FaserSCT_RDO_Container>* rdoContainer, SG::WriteHandle<TrackerSimDataCollection>* simDataCollMap, const std::vector<bool>* processedElements, CLHEP::HepRandomEngine * rndmEngine) const;     //!< digitize SCT without hits

  /**
     @brief Called when m_WriteSCT1_RawData is altered. Does nothing, but required by Gaudi.
  */
  void SetupRdoOutputType(Gaudi::Details::PropertyBase&);

  FloatProperty m_tfix{this, "FixedTime", -999., "Fixed time for Cosmics run selection"};
  BooleanProperty m_enableHits{this, "EnableHits", true, "Enable hits"};
  BooleanProperty m_onlyHitElements{this, "OnlyHitElements", false, "Process only elements with hits"};
  BooleanProperty m_cosmicsRun{this, "CosmicsRun", false, "Cosmics run selection"};
  BooleanProperty m_randomDisabledCells{this, "RandomDisabledCells", false, "Use Random disabled cells, default no"};
  BooleanProperty m_createNoiseSDO{this, "CreateNoiseSDO", false, "Create SDOs for strips with only noise hits (huge increase in SDO collection size"};
  BooleanProperty m_WriteSCT1_RawData{this, "WriteSCT1_RawData", false, "Write out SCT1_RawData rather than SCT3_RawData"};

  BooleanProperty m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKey<FaserSiHitCollection> m_hitsContainerKey{this, "InputObjectName", "SCT_Hits", "Input HITS collection name"};
  std::string m_inputObjectName{""};

  SG::WriteHandleKey<FaserSCT_RDO_Container> m_rdoContainerKey{this, "OutputObjectName", "SCT_RDOs", "Output Object name"};
  SG::WriteHandle<FaserSCT_RDO_Container> m_rdoContainer; //!< RDO container handle
  SG::WriteHandleKey<TrackerSimDataCollection> m_simDataCollMapKey{this, "OutputSDOName", "SCT_SDO_Map", "Output SDO container name"};
  SG::WriteHandle<TrackerSimDataCollection>            m_simDataCollMap; //!< SDO Map handle
  SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  ToolHandle<ISCT_FrontEnd> m_sct_FrontEnd{this, "FrontEnd", "FaserSCT_FrontEnd", "Handle the Front End Electronic tool"};
  ToolHandle<ISCT_SurfaceChargesGenerator> m_sct_SurfaceChargesGenerator{this, "SurfaceChargesGenerator", "FaserSCT_SurfaceChargesGenerator", "Choice of using a more detailed charge drift model"};
  ToolHandle<ISCT_RandomDisabledCellGenerator> m_sct_RandomDisabledCellGenerator{this, "RandomDisabledCellGenerator", "FaserSCT_RandomDisabledCellGenerator", ""};
  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service
  ServiceHandle <PileUpMergeSvc> m_mergeSvc{this, "MergeSvc", "PileUpMergeSvc", "Merge service used in Pixel & SCT digitization"}; //!

  const FaserSCT_ID* m_detID{nullptr}; //!< Handle to the ID helper
  TimedHitCollection<FaserSiHit>* m_thpcsi{nullptr};
  std::list<ISiChargedDiodesProcessorTool*> m_diodeCollectionTools;
  std::vector<bool> m_processedElements; //!< vector of processed elements - set by digitizeHits() */
  std::vector<FaserSiHitCollection*> m_hitCollPtrs;
};

static const InterfaceID IID_ISCT_DigitizationTool("FaserSCT_DigitizationTool", 1, 0);
inline const InterfaceID& FaserSCT_DigitizationTool::interfaceID() {
  return IID_ISCT_DigitizationTool;
}

#endif // FASERSCT_DIGITZATION_FASERSCT_DIGITZATIONTOOL_H
