/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#include "TrackerDataDecoderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrackerRawData/FaserSCT3_RawData.h"
#include "EventFormats/TrackerDataFragment.hpp"

#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"


#include <vector>
#include <map>

static const InterfaceID IID_ITrackerDataDecoderTool("TrackerDataDecoderTool", 1, 0);

const InterfaceID& TrackerDataDecoderTool::interfaceID() {
  return IID_ITrackerDataDecoderTool;
}

TrackerDataDecoderTool::TrackerDataDecoderTool(const std::string& type, 
      const std::string& name,const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<TrackerDataDecoderTool>(this);

  // No properties to declare (yet)

}

TrackerDataDecoderTool::~TrackerDataDecoderTool()
{
}

StatusCode
TrackerDataDecoderTool::initialize() 
{
  ATH_MSG_DEBUG("initialize()");

  if (msgStream().level() >= MSG::WARNING ) TrackerDataFragment::set_debug_on(true);

  ATH_CHECK(detStore()->retrieve(m_sctID, "FaserSCT_ID"));

  // auto first_wafer = m_sctID->wafer_begin();
  // m_trb0Station = m_sctID->station(*first_wafer);

  m_sctContext = m_sctID->wafer_context();
  m_phiReversed.resize(m_sctID->wafer_hash_max(), false);

  const TrackerDD::SCT_DetectorManager* detMgr{nullptr};
  ATH_CHECK(detStore()->retrieve(detMgr, "SCT"));

  const TrackerDD::SiDetectorElementCollection* sctDetElementColl{detMgr->getDetectorElementCollection()};
  size_t nReversed = 0;
  for (const TrackerDD::SiDetectorElement* element: *sctDetElementColl)
  {
    if (element->swapPhiReadoutDirection()) 
    {
      m_phiReversed[element->identifyHash()] = true;
      nReversed++;
    }
  }

  ATH_MSG_DEBUG("Initialized " << m_sctID->wafer_hash_max() << " wafers with " << nReversed << " having phi reversed.");

  return StatusCode::SUCCESS;
}

StatusCode
TrackerDataDecoderTool::finalize() 
{
  ATH_MSG_DEBUG("TrackerDataDecoderTool::finalize()");
  return StatusCode::SUCCESS;
}

StatusCode
TrackerDataDecoderTool::convert(const DAQFormats::EventFull* re, 
    FaserSCT_RDO_Container* container,
    std::string key,
    const std::map<int, std::pair<int, int> >& cableMapping)

{
  ATH_MSG_DEBUG("TrackerDataDecoderTool::convert()");

  if (!re) {
    ATH_MSG_ERROR("EventFull passed to convert() is null!");
    return StatusCode::FAILURE;
  }

  if (!container) {
    ATH_MSG_ERROR("TrackerDataContainer passed to convert() is null!");
    return StatusCode::FAILURE;
  }

  // User can emulate more restrictive hardware hit modes by requesting a different container nametag
  enum class HitMode { HIT = 0, LEVEL, EDGE };
  HitMode hitMode {HitMode::HIT};
  if (key.find("EDGE") != std::string::npos)
  {
    hitMode = HitMode::EDGE;
  }
  else if (key.find("LEVEL") != std::string::npos)
  {
    hitMode = HitMode::LEVEL;
  }

  std::map<IdentifierHash, std::unique_ptr<FaserSCT_RDO_Collection>>  collectionMap;

  const DAQFormats::EventFragment* frag = NULL;
  size_t validFragments = 0;
  for(const auto &id : re->getFragmentIDs()) {
    frag=re->find_fragment(id);

    if ((frag->source_id()&0xFFFF0000) != DAQFormats::SourceIDs::TrackerSourceID) continue;
    ATH_MSG_DEBUG("Fragment:\n" << *frag);
    uint32_t trb = frag->source_id() & 0x0000FFFF;
    if (cableMapping.count(trb) == 0)
    {
      ATH_MSG_ERROR("Invalid trb number " << trb << " not in mapping DB");
      return StatusCode::FAILURE;
    }
    // FIXME: 1 by default; needs to be 0 for IFT
    // int station = m_trb0Station + trb / TrackerDataFragment::PLANES_PER_STATION; 
    // int plane = trb % TrackerDataFragment::PLANES_PER_STATION;
    int station = cableMapping.at(trb).first;
    int plane   = cableMapping.at(trb).second;

    // Exceptions are a no-no in Athena/Calypso, so catch any thrown by faser-common
    try
    {
      TrackerDataFragment trackFrag{ frag->payload<const uint32_t*>(), frag->payload_size() };

      if (!trackFrag.valid())
      {
        ATH_MSG_WARNING("Invalid tracker data fragment for TRB " << trb);
        // FIXME: What else to do if error?  Return FAILURE?
        // FIXME: Validity checking in TrackerDataFragment is a placeholder
        continue;
      }
      if (trackFrag.event_id() != re->event_id())
      {
        ATH_MSG_ERROR("Event ID mismatch for tracker data fragment from trb " << trb << 
                      "; found " << trackFrag.event_id() << " but expected " << re->event_id());
        // FIXME: Is returning FAILURE the right thing to do here?
        return StatusCode::FAILURE;
      }
      validFragments++;
      // auto bcid = trackFrag.bc_id();
      ATH_MSG_DEBUG("Processing tracker data fragment for TRB " << trb);

      for (auto sctEvent : trackFrag)
      {
        if (sctEvent != nullptr)
        {
          unsigned short onlineModuleID = sctEvent->GetModuleID(); 
          // if (plane == 2) onlineModuleID = (onlineModuleID + 4) % 8;
          if (onlineModuleID >= TrackerDataFragment::MODULES_PER_FRAGMENT)
          {
            // FIXME: Should we return failure here?
            ATH_MSG_ERROR("Invalid module ID (" << onlineModuleID << ") from trb " << trb);
            continue;
          }
          if (sctEvent->BCIDMismatch())
          {
            // FIXME: Should we return failure here?
            ATH_MSG_ERROR("Module data BCID mismatch between sides for online module " << onlineModuleID);
            continue;
          }
          else if (sctEvent->HasError())
          {
            // FIXME: Should we return failure here?
            ATH_MSG_ERROR("Online module " << onlineModuleID << " reports one or more errors.");
            continue;
          }
          else if (sctEvent->MissingData())
          {
            // FIXME: Should we return failure here?
            ATH_MSG_ERROR("Online module " << onlineModuleID << " reports missing data.");
            continue;
          }
          else if (!sctEvent->IsComplete())
          {
            // FIXME: Should we return failure here?
            ATH_MSG_ERROR("Online module " << onlineModuleID << " reports not complete.");
            continue;
          }
          // Not satisfied, at least for cosmics data
          // else if (sctEvent->GetBCID() != bcid)
          // {
          //   // FIXME: Should we return failure here?
          //   ATH_MSG_ERROR("Module data BCID (" << sctEvent->GetBCID() << ")for online module " << 
          //                  onlineModuleID << " differs from TrackerDataFragment (" << bcid << ")");
          //   continue;
          // }
          ATH_MSG_DEBUG("Processing online module #" << onlineModuleID);
          size_t chipIndex{0};
          for (auto hitVector : sctEvent->GetHits())
          {
            if (hitVector.size() > 0)
            {
              int side = chipIndex / TrackerDataFragment::CHIPS_PER_SIDE;
              uint32_t chipOnSide = chipIndex % TrackerDataFragment::CHIPS_PER_SIDE;
              for (auto hit : hitVector)
              {
                uint32_t stripOnChip = hit.first;
                if (stripOnChip >= TrackerDataFragment::STRIPS_PER_CHIP)
                {
                  // FIXME: Return failure?
                  ATH_MSG_ERROR("Invalid strip number on chip: " << stripOnChip );
                  continue;
                }
                uint32_t hitPattern  = hit.second;
                if (hitMode == HitMode::EDGE && (((hitPattern & 0x2) == 0 ) || ((hitPattern & 0x4) != 0) ) ) continue; // 01X
                if (hitMode == HitMode::LEVEL && ((hitPattern & 0x2) == 0)) continue; // X1X
                int phiModule = 3 - (onlineModuleID % 4); // 0 to 3 from bottom to top: (module#,Phi#) =  (0,3),(1,2),(2,1),(3,0),(4,3),(5,2),(6,1),(7,0)
                int etaModule = -2*(onlineModuleID/4) + 1; // eta = +1 for modules 0,1,2,3 and eta = -1 for 4,5,6,7 
                // ATH_MSG_DEBUG("Getting wafer_id for station, plane, phi, eta, side = " << station << " " << plane << " " << phiModule << " " << etaModule << " " << side);
                Identifier id = m_sctID->wafer_id(station, plane, phiModule, etaModule, side);
                IdentifierHash waferHash = m_sctID->wafer_hash(id);  // this will be the collection number in the container
                int stripOnSide = chipOnSide * TrackerDataFragment::STRIPS_PER_CHIP + stripOnChip;
                // ATH_MSG_DEBUG("Checking phi reversal for waferhash = " << waferHash << "(" << waferHash.value() << ") strip on side = " << stripOnSide);
                if (m_phiReversed[waferHash]) stripOnSide = TrackerDataFragment::STRIPS_PER_SIDE - stripOnSide - 1;
                if (stripOnSide < 0 || static_cast<uint32_t>(stripOnSide) >= TrackerDataFragment::STRIPS_PER_SIDE)
                {
                  // FIXME: Return failure?
                  ATH_MSG_ERROR("Invalid strip number on side: " << stripOnSide);
                  continue;
                }
                Identifier digitID {m_sctID->strip_id(id, stripOnSide)};
                int errors{0};
                int groupSize{1};
                unsigned int rawDataWord{static_cast<unsigned int>(groupSize | (stripOnSide << 11) | (hitPattern <<22) | (errors << 25))};

                if (collectionMap.count(waferHash) == 0)
                {
                  {
                    std::unique_ptr<FaserSCT_RDO_Collection> current_collection = std::make_unique<FaserSCT_RDO_Collection>(waferHash);
                    current_collection->setIdentifier(id);
                    collectionMap[waferHash] = std::move(current_collection);
                  }
                }
                collectionMap[waferHash]->emplace_back(new FaserSCT3_RawData(digitID, rawDataWord, std::vector<int>() ));
              }
            }
            chipIndex++;
          }
        }
      }
    } 
    catch (const TrackerData::TrackerDataException& exc)
    {
      ATH_MSG_ERROR("TrackerDataException: " << exc.what());
      continue;
    }
  }

  if (validFragments == 0) 
  {
    ATH_MSG_DEBUG("Failed to find any valid Tracker fragments in raw event!");
    return StatusCode::SUCCESS;
  }

  for (auto& [hash, collection] : collectionMap)
  {
    if (collection == nullptr) continue;
    if (collection->empty())
    {
      collection.reset();
      continue;
    }
    ATH_CHECK(container->getWriteHandle(hash).addOrDelete(std::move(collection)));
  }

  // Can check if you found something
  // ATH_MSG_DEBUG("Tracker Fragment:\n" << *trackerFragments[0]);

  // Now we have a list of fragments, need to loop over all fragments
  // and fill containers based on their data.
  // See  InnerDetector/InDetEventCnv/SCT_RawDataByteStreamCnv/src/SCT_RodDecoder.cxx
  // for the (complicated) way ATLAS does this.
  // for (auto fragmentEntry: trackerFragments) {
  //   size_t trb = fragmentEntry.first;
  //   auto fragment = fragmentEntry.second;
  //   ATH_MSG_DEBUG("Processing tracker data fragment for TRB " << trb);
  //   // Must loop over data in this fragment, find the collection each 
  //   // hit belongs to, create the collection if it doesn't exist, and fill 
  //   // the RDOs into the proper collection.
    
  //   // Some snippets are here
  //   FaserSCT_RDO_Collection* sctRDOColl{nullptr};

  //   // Find collection in container, check if NULL, add add if it doesn't exist
  //   // id = collectionID(data)
  //   // if (!container->hasID(id)) {
  //   //  ATH_MSG_DEBUG(" Collection ID = " << id << " does not exist, create it ");
  //   //  sctRDOColl = new SCT_RD0_Collection(id);
  //   //  sctRDOColl->setIdentifier(id);
  //   //  CHECK( container->addCollection(sctRDOColl, id) );
  //   // }
  //   // sctRDOColl = container->get(id);  // Not sure how this works...
  //   //
  //   // Must loop over figure out which collection this data belongs to and 
  //   // set pointer appropriately

  //   FaserSCT3_RawData* rdo = new FaserSCT3_RawData();

  //   // Do the conversion here using whatever it takes
  //   // try {
  //   //  rdo->setData( fragment );
  //   //} catch ( TrackerData::TrackerDataException& e ) {
  //   //  ATH_MSG_INFO("TrackerDataDecoderTool:\n"
  //   //	   <<e.what()
  //   //		   << "\nTracker data not filled!\n");

  //   // sctRDOColl->push_back(rdo);

  // }  // end of loop over fragments

  // Don't spring a leak
  // This may not quite be correct syntax, but the fragements we created above 
  // need to be deleted 
  // for (auto fragEntry : trackerFragments) 
  //   delete fragEntry.second;

  // We also should probably use std::make_unique to create the RDOs

  ATH_MSG_DEBUG( "TrackerDataDecoderTool created container with size=" << container->size());
  return StatusCode::SUCCESS; 
}
