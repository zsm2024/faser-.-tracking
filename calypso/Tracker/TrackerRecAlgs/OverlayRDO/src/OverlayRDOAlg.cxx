#include "OverlayRDOAlg.h"
#include "TrkTrack/Track.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "Identifier/Identifier.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include <cmath>



OverlayRDOAlg::OverlayRDOAlg(const std::string &name, 
                                    ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
    {}

StatusCode OverlayRDOAlg::initialize() 
{
  ATH_CHECK(detStore()->retrieve(m_sctHelper, "FaserSCT_ID"));
  ATH_CHECK(m_posTrackKey.initialize());
  ATH_CHECK(m_negTrackKey.initialize());
  ATH_CHECK(m_posRdoKey.initialize());
  ATH_CHECK(m_negRdoKey.initialize());
  ATH_CHECK(m_posEdgeModeRdoKey.initialize());
  ATH_CHECK(m_negEdgeModeRdoKey.initialize());
  ATH_CHECK(m_outRdoKey.initialize());
  ATH_CHECK(m_outEdgeModeRdoKey.initialize());
  ATH_CHECK(m_outputTrackCollection.initialize());

  return StatusCode::SUCCESS;
}


StatusCode OverlayRDOAlg::execute(const EventContext &ctx) const 
{

  SG::ReadHandle<TrackCollection> posTrackCollection {m_posTrackKey, ctx};
  ATH_CHECK(posTrackCollection.isValid());
  SG::ReadHandle<TrackCollection> negTrackCollection {m_negTrackKey, ctx};
  ATH_CHECK(negTrackCollection.isValid());
  SG::ReadHandle<FaserSCT_RDO_Container> posRdoContainer {m_posRdoKey, ctx};
  ATH_CHECK(posRdoContainer.isValid());
  SG::ReadHandle<FaserSCT_RDO_Container> negRdoContainer {m_negRdoKey, ctx};
  ATH_CHECK(negRdoContainer.isValid());
  SG::ReadHandle<FaserSCT_RDO_Container> posEdgeModeRdoContainer {m_posEdgeModeRdoKey, ctx};
  ATH_CHECK(posEdgeModeRdoContainer.isValid());
  SG::ReadHandle<FaserSCT_RDO_Container> negEdgeModeRdoContainer {m_negEdgeModeRdoKey, ctx};
  ATH_CHECK(negEdgeModeRdoContainer.isValid());

  // SCT_RDOs

  std::map<IdentifierHash, std::vector<const FaserSCT_RDO_Collection*> > rdoDB {};
  processContainer(*posRdoContainer, rdoDB);
  processContainer(*negRdoContainer, rdoDB);

  SG::WriteHandle<FaserSCT_RDO_Container> outRdoContainer {m_outRdoKey, ctx};
  ATH_CHECK(outRdoContainer.record(std::make_unique<FaserSCT_RDO_Container>(m_sctHelper->wafer_hash_max())));

  processDB(rdoDB, outRdoContainer, [](int i) {return true;});

  // SCT_EDGEMODE_RDOs

  rdoDB.clear();
  processContainer(*posEdgeModeRdoContainer, rdoDB);
  processContainer(*negEdgeModeRdoContainer, rdoDB);

  SG::WriteHandle<FaserSCT_RDO_Container> outEdgeModeRdoContainer {m_outEdgeModeRdoKey, ctx};
  ATH_CHECK(outEdgeModeRdoContainer.record(std::make_unique<FaserSCT_RDO_Container>(m_sctHelper->wafer_hash_max())));

  processDB(rdoDB, outEdgeModeRdoContainer, [](int hitPattern) { return (((hitPattern & 0x2) == 0 ) || ((hitPattern & 0x4) != 0) ) ? false : true;});

  // Tracks

  SG::WriteHandle<TrackCollection> outputTrackCollection {m_outputTrackCollection, ctx};
  std::unique_ptr<TrackCollection> outputTracks = std::make_unique<TrackCollection>();

  for (auto theTrack : *posTrackCollection)
  {
    outputTracks->push_back(cloneTrack(theTrack) );
  }

  for (auto theTrack : *negTrackCollection)
  {
    outputTracks->push_back(cloneTrack(theTrack) );
  }

  ATH_CHECK(outputTrackCollection.record(std::move(outputTracks)));

  return StatusCode::SUCCESS;
}


void 
OverlayRDOAlg::processDB(const std::map<IdentifierHash, std::vector<const FaserSCT_RDO_Collection*> >& rdoDB, SG::WriteHandle<FaserSCT_RDO_Container>& outRdoContainer, std::function<bool(int)> lambda) const
{
  for (auto& entry : rdoDB)
  {
    IdentifierHash waferHash = entry.first;
    Identifier id = m_sctHelper->wafer_id(waferHash);
    auto vec = entry.second;
    if (vec.size() == 0)
    {
      std::cout << "Unexpected zero-length collection vector for ID = " << id << " ..." << std::endl;
      continue;
    }
    std::unique_ptr<FaserSCT_RDO_Collection> current_collection = std::make_unique<FaserSCT_RDO_Collection>(waferHash);
    current_collection->setIdentifier(id);
    std::map<int, unsigned int> stripMap;
    for (auto& collection : vec)
    {
      for (auto rawData : *collection)
      {
        // const FaserSCT3_RawData* data = dynamic_cast<const FaserSCT3_RawData*>(rawData);
        Identifier stripID = rawData->identify();
        int stripNumber = m_sctHelper->strip(stripID);
        int groupSize = rawData->getGroupSize();
        unsigned int dataWord = rawData->getWord();
        // int time = data->getTimeBin();
        // std::cout << "Word: " << std::hex << dataWord << std::dec << " Time: " << time << std::endl;
	for (int i = stripNumber; i < stripNumber + groupSize; i++)
	  stripMap[i] |= dataWord;
      }
    }

    for (auto stripEntry : stripMap)
    {
      Identifier rdoID {m_sctHelper->strip_id(id, stripEntry.first)};
      if (!lambda(stripEntry.second>>22)) continue;
      current_collection->emplace_back(new FaserSCT3_RawData(rdoID, stripEntry.second, std::vector<int> {}));
    }

    outRdoContainer->getWriteHandle(waferHash).addOrDelete(std::move(current_collection)).ignore();
  }
}


Trk::Track* OverlayRDOAlg::cloneTrack(const Trk::Track* theTrack) const
{
  Trk::TrackInfo i { theTrack->info() };
  auto q = std::make_unique<Trk::FitQuality>( *(theTrack->fitQuality()) );
  auto s = std::make_unique<Trk::TrackStates>();  

  for (auto oldState : (*theTrack->trackStateOnSurfaces()))
  {
    const Trk::CurvilinearParameters* oldParam = dynamic_cast<const Trk::CurvilinearParameters*>(oldState->trackParameters());
    std::unique_ptr<Trk::TrackParameters> newParam { new Trk::CurvilinearParameters { *oldParam } };
    Trk::TrackStateOnSurface* newState = new Trk::TrackStateOnSurface { nullptr, std::move(newParam) };
    s->push_back(newState);
  }
  // std::unique_ptr<DataVector<const Trk::TrackStateOnSurface>> sink {s};
  return new Trk::Track {i, std::move(s) , std::move(q) } ;
}

void OverlayRDOAlg::processContainer(const FaserSCT_RDO_Container& container, std::map<IdentifierHash, std::vector<const FaserSCT_RDO_Collection*> >& rdoDB) const
{
  size_t nLong{0};
  size_t nData{0};
  for( const auto& collection : container)
  {
      if (collection->empty()) continue;
      Identifier id = collection->identify();
      IdentifierHash hash = m_sctHelper->wafer_hash(id);
      if (rdoDB.count(hash))
      {
        rdoDB[hash].push_back(collection);
      }
      else
      {
        rdoDB[hash] = std::vector<const FaserSCT_RDO_Collection*>{collection};
      }
      for(const auto& rawdata : *collection)
      {
        nData++;
        if (rawdata->getGroupSize() > 1) nLong++;
      }
  }
  std::cout << nLong << " long data out of " << nData << " total RDOs" << std::endl;
}


StatusCode OverlayRDOAlg::finalize() 
{
  return StatusCode::SUCCESS;
}
