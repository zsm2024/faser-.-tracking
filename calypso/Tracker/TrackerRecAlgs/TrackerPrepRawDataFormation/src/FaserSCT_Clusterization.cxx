/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

/**   @file SCT_Clusterization.cxx
 *   Implementation file for the SCT_Clusterization class.
 *   @author Paul Bell, Tommaso Lari, Shaun Roe, Carl Gwilliam
 *   @date 08 July 2008
 */

#include "FaserSCT_Clusterization.h"

#include "FaserDetDescr/FaserDetectorID.h"    
#include "TrackerIdentifier/FaserSCT_ID.h"
// #include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
// #include "TrackerRawData/TrackerClusterContainer.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"
#include "TrackerRawData/FaserSCT_RDORawData.h"
#include "FaserSiClusterizationTool/IFaserSCT_ClusteringTool.h"
#include "StoreGate/WriteHandle.h"

namespace Tracker
{

static const std::string moduleFailureReason{"FaserSCT_Clusterization: Exceeds max fired strips"};

// Constructor with parameters:
FaserSCT_Clusterization::FaserSCT_Clusterization(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator),
  m_idHelper{nullptr}
{
  //  //  // Get parameter values from jobOptions file
  declareProperty("ClusterContainerCacheKey", m_clusterContainerCacheKey="");
}

// Initialize method:
StatusCode FaserSCT_Clusterization::initialize() {
  ATH_MSG_INFO("FaserSCT_Clusterization::initialize()!");

  // Get the conditions summary service (continue anyway, just check the pointer 
  // later and declare everything to be 'good' if it is NULL)
  if (m_checkBadModules.value()) {
   ATH_MSG_INFO("Clusterization has been asked to look at bad module info");
  ATH_CHECK(m_pSummaryTool.retrieve());
  } else {
   m_pSummaryTool.disable();
  }

  //  m_clusterContainerLinkKey = m_clusterContainerKey.key();

  ATH_CHECK(m_rdoContainerKey.initialize());
  ATH_CHECK(m_clusterContainerKey.initialize());
  ATH_CHECK(m_clusterContainerLinkKey.initialize());
  ATH_CHECK(m_clusterContainerCacheKey.initialize(not m_clusterContainerCacheKey.key().empty()));
  // ATH_CHECK(m_flaggedCondDataKey.initialize());

  // Get the clustering tool
  ATH_CHECK(m_clusteringTool.retrieve());

  // Get the SCT ID helper
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode FaserSCT_Clusterization::execute(const EventContext& ctx) const {
  ++m_numberOfEvents;                                                                
  // Register the IdentifiableContainer into StoreGate
  SG::WriteHandle<Tracker::FaserSCT_ClusterContainer> clusterContainer{m_clusterContainerKey, ctx};
  if (m_clusterContainerCacheKey.key().empty()) {
    ATH_CHECK(clusterContainer.record(std::make_unique<Tracker::FaserSCT_ClusterContainer>(m_idHelper->wafer_hash_max())));
  } else {
    SG::UpdateHandle<Tracker::FaserSCT_ClusterContainerCache> clusterContainercache{m_clusterContainerCacheKey, ctx};
    ATH_CHECK(clusterContainer.record(std::make_unique<Tracker::FaserSCT_ClusterContainer>(clusterContainercache.ptr())));
  }
  ATH_MSG_DEBUG("Container '" << clusterContainer.name() << "' initialised");

  ATH_CHECK(clusterContainer.symLink(m_clusterContainerLinkKey));
  ATH_CHECK(clusterContainer.isValid());
  ATH_MSG_DEBUG("FaserSCT clusters '" << clusterContainer.name() << "' symlinked in StoreGate");

  // SG::WriteHandle<SCT_FlaggedCondData> flaggedCondData{m_flaggedCondDataKey, ctx};
  // ATH_CHECK(flaggedCondData.record(std::make_unique<SCT_FlaggedCondData>()));

  // First, we have to retrieve and access the container, not because we want to 
  // use it, but in order to generate the proxies for the collections, if they 
  // are being provided by a container converter.
  SG::ReadHandle<FaserSCT_RDO_Container> rdoContainer{m_rdoContainerKey, ctx};
  ATH_CHECK(rdoContainer.isValid());

  // Anything to dereference the DataHandle will trigger the converter
  FaserSCT_RDO_Container::const_iterator rdoCollections{rdoContainer->begin()};
  FaserSCT_RDO_Container::const_iterator rdoCollectionsEnd{rdoContainer->end()};
  bool dontDoClusterization{false};
  //new code to remove large numbers of hits (what is large?)
  if (m_maxTotalOccupancyPercent.value()!=100) {
    constexpr unsigned int totalNumberOfChannels{110592};
    const unsigned int maxAllowableStrips{(totalNumberOfChannels*m_maxTotalOccupancyPercent.value())/100};//integer arithmetic, should be ok
    unsigned int totalFiredStrips{0};
    for (; rdoCollections != rdoCollectionsEnd; ++rdoCollections) {
      for (const FaserSCT_RDORawData* rdo: **rdoCollections) {
	      totalFiredStrips += rdo->getGroupSize();
      }
    } //iterator is now at the end
    //reset the iterator
    rdoCollections = rdoContainer->begin();
    if (totalFiredStrips > maxAllowableStrips) {
      ATH_MSG_WARNING("This event has too many hits in the FaserSCT: " << totalFiredStrips << " > " << maxAllowableStrips);
      dontDoClusterization = true;
    }
  }
  if (not dontDoClusterization) {
    for (; rdoCollections != rdoCollectionsEnd; ++rdoCollections) {
      ++m_numberOfRDOCollection;
//      for (const FaserSCT_RDORawData* rdo: **rdoCollections) {
//      }
      const TrackerRawDataCollection<FaserSCT_RDORawData>* rd{*rdoCollections};
      ATH_MSG_DEBUG("RDO collection size=" << rd->size() << ", Hash=" << rd->identifyHash());
      Tracker::FaserSCT_ClusterContainer::IDC_WriteHandle lock{clusterContainer->getWriteHandle(rdoCollections.hashId())};
      if (lock.alreadyPresent()) {
        ATH_MSG_DEBUG("Item already in cache , Hash=" << rd->identifyHash());
        continue;
      }
      bool goodModule{m_checkBadModules.value() && false? m_pSummaryTool->isGood(rd->identifyHash(),ctx) : true};
      // Check the RDO is not empty and that the wafer is good according to the conditions
      if ((not rd->empty()) && goodModule ) {
  	// If more than a certain number of RDOs set module to bad
	if (m_maxFiredStrips.value()) {
	  unsigned int nFiredStrips{0};
	  for (const FaserSCT_RDORawData* rdo: *rd) {
	    ++m_numberOfRDO;
	    nFiredStrips += rdo->getGroupSize();
	  }
	  // if (nFiredStrips > m_maxFiredStrips.value()) {
	  //   flaggedCondData->insert(std::make_pair(rd->identifyHash(), moduleFailureReason));
	  //   continue;
	  // }
	}
	// Use one of the specific clustering AlgTools to make clusters    
	//FaserSCT_ClusterCollection* clusterCollection{m_clusteringTool->clusterize(*rd, *m_idHelper)};
	std::unique_ptr<FaserSCT_ClusterCollection> clusterCollection{m_clusteringTool->clusterize(*rd, *m_idHelper)};
	if (clusterCollection) {
	  if (not clusterCollection->empty()) {
	    ++m_numberOfClusterCollection;
	    const IdentifierHash hash{clusterCollection->identifyHash()};
	    ATH_MSG_DEBUG("Clusters with key '" << hash << "' added to Container\n");
	    ATH_MSG_DEBUG("Clusters size " <<clusterCollection->size()<< "' added to Container\n");
	    Tracker::FaserSCT_ClusterCollection::const_iterator clustersNext = clusterCollection->begin();
	    Tracker::FaserSCT_ClusterCollection::const_iterator clustersFinish = clusterCollection->end();
	    for (; clustersNext!=clustersFinish; clustersNext++){
	    ++m_numberOfCluster;
	      ATH_MSG_DEBUG("Clusters information");
	      ATH_MSG_DEBUG("Clusters " <<(*clustersNext)<< "' added to Container\n");
	      //		(*clustersNext)->dump(msg(MSG::INFO)) ;
	    }

	    //Using get because I'm unsure of move semantec status
	    ATH_CHECK(lock.addOrDelete(std::move(clusterCollection)));

	  } else {
	    ATH_MSG_DEBUG("Don't write empty collections\n");
	  }
	} else {
	  ATH_MSG_DEBUG("Clustering algorithm found no clusters\n");
	}
	//}
      }
    }
  }
  // Set container to const
  ATH_CHECK(clusterContainer.setConst());
  ATH_MSG_DEBUG("clusterContainer->numberOfCollections() " << clusterContainer->numberOfCollections());
  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode FaserSCT_Clusterization::finalize() 
{
  ATH_MSG_INFO("FaserSCT_Clusterization::finalize()");
  ATH_MSG_INFO( m_numberOfEvents << " events processed" );
  ATH_MSG_INFO( m_numberOfRDOCollection << " RDO collections processed" );
  ATH_MSG_INFO( m_numberOfRDO<< " RawData" );
  ATH_MSG_INFO( m_numberOfClusterCollection<< " cluster collections generated" );
  ATH_MSG_INFO( m_numberOfCluster<< " cluster generated" );

  return StatusCode::SUCCESS;
}
}
