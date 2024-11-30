/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

/***************************************************************************
  -------------------
  ATLAS Collaboration
 ***************************************************************************/

//<<<<<< INCLUDES >>>>>>


#include "TrackerSpacePointFinder.h"

// For processing clusters
#include "TrackerReadoutGeometry/SiLocalPosition.h"
#include "TrackerReadoutGeometry/SiDetectorDesign.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

// Space point Classes,
#include "TrackerSpacePoint/FaserSCT_SpacePointCollection.h"
// #include "TrkSpacePoint/SpacePointCLASS_DEF.h"
#include "TrackerIdentifier/FaserSCT_ID.h"


// general Atlas classes
#include "FaserDetDescr/FaserDetectorID.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadCondHandle.h"

#include "AthenaMonitoringKernel/Monitored.h"

namespace Tracker
{

//------------------------------------------------------------------------
TrackerSpacePointFinder::TrackerSpacePointFinder(const std::string& name,
    ISvcLocator* pSvcLocator)
: AthReentrantAlgorithm(name, pSvcLocator)
{ 
  declareProperty("ProcessSCTs", m_selectSCTs=true);
  declareProperty("ProcessOverlaps", m_overlap=true, "process overlaps of SCT wafers.");
  declareProperty("AllClusters", m_allClusters=false, "process all clusters without limits.");
  declareProperty("OverlapLimitOpposite", m_overlapLimitOpposite=2.8, "overlap limit for opposite-neighbour.");
  declareProperty("OverlapLimitPhi", m_overlapLimitPhi=5.64, "overlap limit for phi-neighbours.");
  declareProperty("OverlapLimitEtaMin", m_overlapLimitEtaMin=1.68, "low overlap limit for eta-neighbours.");
  declareProperty("OverlapLimitEtaMax", m_overlapLimitEtaMax=3.0, "high overlap limit for eta-neighbours.");
  //  declareProperty("OverrideBeamSpot", m_overrideBS=false);
  declareProperty("VertexX", m_xVertex=0.);
  declareProperty("VertexY", m_yVertex=0.);
  declareProperty("VertexZ", m_zVertex=0.);

  declareProperty("SpacePointCacheSCT", m_SpacePointCache_SCTKey="");

}

//-----------------------------------------------------------------------
StatusCode TrackerSpacePointFinder::initialize()
{
  ATH_MSG_DEBUG( "initialize()" );

  // Check that clusters, space points and ids have names
  if (m_selectSCTs && m_Sct_clcontainerKey.key().empty()){
    ATH_MSG_FATAL( "SCTs selected and no name set for SCT clusters");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_Sct_clcontainerKey.initialize(m_selectSCTs) );

  if (m_selectSCTs && m_SpacePointContainer_SCTKey.key().empty()){
    ATH_MSG_FATAL( "No name set for SCT space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_SpacePointContainer_SCTKey.initialize(m_selectSCTs) );

  if (m_spacepointoverlapCollectionKey.key().empty()){
    ATH_MSG_FATAL( "No name set for overlap space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_spacepointoverlapCollectionKey.initialize() );

  // create containers (requires the Identifier Helpers)
  if (m_selectSCTs) {
    ATH_CHECK(detStore()->retrieve(m_idHelper,"FaserSCT_ID"));

    // Initialize the key of input FaserSiElementPropertiesTable and SiDetectorElementCollection for SCT
    ATH_CHECK(m_SCTPropertiesKey.initialize());
    ATH_CHECK(m_SCTDetEleCollKey.initialize());
  }

  ATH_CHECK(m_SiSpacePointMakerTool.retrieve());
  //  ATH_CHECK(m_beamSpotKey.initialize(!m_overrideBS));

  ATH_CHECK(m_SpacePointCache_SCTKey.initialize(!m_SpacePointCache_SCTKey.key().empty()));
  m_cachemode = !m_SpacePointCache_SCTKey.key().empty();

  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());

  ATH_MSG_INFO( "Initialized" );
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------

StatusCode TrackerSpacePointFinder::execute (const EventContext& ctx) const
{


  ++m_numberOfEvents;
  const TrackerDD::SiDetectorElementCollection* elements = nullptr;
  const TrackerDD::SiElementPropertiesTable* properties = nullptr;

  auto nReceivedClustersSCT = Monitored::Scalar<int>( "numSctClusters" , 0 );
  auto nSCTspacePoints = Monitored::Scalar<int>( "numSctSpacePoints"   , 0 );

  //auto mon = Monitored::Group( m_monTool, nReceivedClustersPIX,nReceivedClustersSCT, nPIXspacePoints, nSCTspacePoints );

  if (m_selectSCTs) {
    SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey, ctx);
    elements = sctDetEle.retrieve();
    if (elements==nullptr) {
      ATH_MSG_FATAL("Pointer of SiDetectorElementCollection (" << m_SCTDetEleCollKey.fullKey() << ") could not be retrieved");
      return StatusCode::SUCCESS;
    }
    SG::ReadCondHandle<TrackerDD::SiElementPropertiesTable> sctProperties(m_SCTPropertiesKey, ctx);
    properties = sctProperties.retrieve();
    if (properties==nullptr) {
      ATH_MSG_FATAL("Pointer of FaserSiElementPropertiesTable (" << m_SCTPropertiesKey.fullKey() << ") could not be retrieved");
      return StatusCode::SUCCESS;
    }
  }
  SPFCache r_cache(ctx);
  //  if (! m_overrideBS){
  //    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
  //    const InDet::BeamSpotData* beamSpot = *beamSpotHandle;
  //    r_cache.vertex = beamSpot->beamVtx().position();
  //  } else {
  r_cache.vertex = Amg::Vector3D(m_xVertex,m_yVertex,m_zVertex);
  //  }

  // register the IdentifiableContainer into StoreGate
  SG::WriteHandle<FaserSCT_SpacePointContainer> spacePointContainer_SCT;
  if (m_selectSCTs){
    spacePointContainer_SCT = SG::WriteHandle<FaserSCT_SpacePointContainer>( m_SpacePointContainer_SCTKey, ctx );
    if(m_SpacePointCache_SCTKey.key().empty()){
      ATH_CHECK( spacePointContainer_SCT.record( std::make_unique<FaserSCT_SpacePointContainer>(m_idHelper->wafer_hash_max()) ) );
      ATH_MSG_DEBUG("Created SpacePointContainer " << m_SpacePointContainer_SCTKey.key() << " N= " << m_idHelper->wafer_hash_max());
    }else{
      SG::UpdateHandle<FaserSCT_SpacePointCache> updateH( m_SpacePointCache_SCTKey, ctx );
      ATH_CHECK(updateH.isValid());
      ATH_CHECK( spacePointContainer_SCT.record( std::make_unique<FaserSCT_SpacePointContainer>(updateH.ptr()) ) );
      ATH_MSG_DEBUG("Created SpacePointContainer " << m_SpacePointContainer_SCTKey.key() << " from cache " << m_SpacePointCache_SCTKey.key());
    }
  }

  SG::WriteHandle<FaserSCT_SpacePointOverlapCollection> spacepointoverlapCollection( m_spacepointoverlapCollectionKey, ctx );
  ATH_CHECK( spacepointoverlapCollection.record( std::make_unique<FaserSCT_SpacePointOverlapCollection>() ) );

  ATH_MSG_DEBUG( "Container '" << spacepointoverlapCollection.name() << "' initialised" );

  int sctCacheCount = 0;

  if (m_selectSCTs){

    // retrieve SCT cluster container
    SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> sct_clcontainer( m_Sct_clcontainerKey, ctx );
    if (!sct_clcontainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< sct_clcontainer.name() << " !" << endmsg;
      return StatusCode::RECOVERABLE;
    }

    r_cache.SCTCContainer = sct_clcontainer.cptr();

    ATH_MSG_DEBUG( "SCT Cluster container found: " << sct_clcontainer->size() << " collections" );
    // Get hold of all clusters and iterate through them.
    // Pixel clusters will be converted immediately to pixel space points.
    // For SCT clusters, posssible pairs will be found and space points computed.
    // It is possible, though unlikely, that there are no clusters for the
    // event. Therefore we do not terminate the job if we don't find any.
    Tracker::FaserSCT_ClusterContainer::const_iterator it = sct_clcontainer->begin();
    Tracker::FaserSCT_ClusterContainer::const_iterator itend = sct_clcontainer->end();

    for (; it != itend; ++it){
      ++m_numberOfClusterCollection;
      const Tracker::FaserSCT_ClusterCollection *colNext=&(**it);
      nReceivedClustersSCT = colNext->size();
      m_numberOfCluster+=colNext->size();;

      // Create SpacePointCollection
      IdentifierHash idHash = colNext->identifyHash();
      FaserSCT_SpacePointContainer::IDC_WriteHandle lock = spacePointContainer_SCT->getWriteHandle(idHash);
      if(lock.alreadyPresent()){
        ATH_MSG_DEBUG("SCT Hash " << idHash << " is already in cache");
        ++sctCacheCount;
        continue; //Skip if already present in cache
      }

      Identifier elementID = colNext->identify();
      auto spacepointCollection = std::make_unique<FaserSCT_SpacePointCollection>(idHash);
      spacepointCollection->setIdentifier(elementID);

        ATH_MSG_DEBUG( "SCT cluster collection size "<<colNext->size() );
      if ( colNext->size() != 0){
        addSCT_SpacePoints(colNext, properties, elements,
            spacepointCollection.get(), spacepointoverlapCollection.ptr(), r_cache);
      } else {
        ATH_MSG_DEBUG( "Empty SCT cluster collection" );
      }
      size_t size = spacepointCollection->size();
      nSCTspacePoints = size;
      if (size == 0){
        ATH_MSG_VERBOSE( "Algorithm found no space points" );
      } else {
        //In a MT environment the cache maybe filled by another thread in which case this will delete the duplicate
        m_numberOfSP+=spacepointCollection->size();
        ++m_numberOfSPCollection;
        StatusCode sc= lock.addOrDelete( std::move(spacepointCollection) );
        if (sc.isFailure()){
          ATH_MSG_ERROR( "Failed to add SpacePoints to container" );
          return StatusCode::RECOVERABLE;
        }
        ATH_MSG_VERBOSE( size << " SpacePoints successfully added to Container !" );
      }
    }
  }


  // store the overlap space points.
  // check that the set isn't empty.
  if (spacepointoverlapCollection->size()==0)
  {
    ATH_MSG_DEBUG( "No overlap space points found" );
  }
  else
  {
    ATH_MSG_DEBUG( spacepointoverlapCollection->size() <<" overlap space points registered." );
  }
  if (m_selectSCTs) {
    auto c = spacePointContainer_SCT->numberOfCollections();
    m_numberOfSCT   += c;
  }
  if(m_cachemode)//Prevent unnecessary atomic counting
  {
    m_sctCacheHits  += sctCacheCount;
  }


  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode TrackerSpacePointFinder::finalize()
{
  ATH_MSG_INFO( "finalize()" );
  ATH_MSG_INFO( m_numberOfEvents << " events processed" );
  ATH_MSG_INFO( m_numberOfSCT << " SCT collections processed" );
  ATH_MSG_INFO( m_numberOfClusterCollection << " cluster collections processed" );
  ATH_MSG_INFO( m_numberOfCluster<< " clusters processed" );
  ATH_MSG_INFO( m_numberOfSPCollection<< " spacepoint collections generated" );
  ATH_MSG_INFO( m_numberOfSP<< " spacepoints generated" );
  if(m_cachemode){
    //These are debug messages because they can be indeterminate in an MT environment and it could
    //lead to confusing log comparisons.
    ATH_MSG_DEBUG( m_sctCacheHits << " sct cache hits" );
  }
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

void TrackerSpacePointFinder::
addSCT_SpacePoints(const Tracker::FaserSCT_ClusterCollection* next,
    const TrackerDD::SiElementPropertiesTable* properties,
    const TrackerDD::SiDetectorElementCollection* elements,
    FaserSCT_SpacePointCollection* spacepointCollection, FaserSCT_SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache &r_cache) const
{

  IdentifierHash thisHash(next->identifyHash());

  // if it is not the stereo side
  const TrackerDD::SiDetectorElement *element = elements->getDetectorElement(thisHash);
  if (element && !(element->isStereo())){
    // Space points are created from clusters for all possibly
    // overlapping pairs in the given range if m_overlap=true.
    // Otherwise just the opposite pair are processed.
    // Pick up the five neighbours of detector, and check ranges
    // for which overlap is possible. 
    // "check1" is used for opposite and eta overlaps.
    // check2 for phi overlaps

    const std::vector<IdentifierHash>*
      others(properties->neighbours(thisHash));
    if (others==0 || others->empty() ) return;
    std::vector<IdentifierHash>::const_iterator otherHash = others->begin();

    bool overlapColl = false;
    // check opposite wafer
    checkForSCT_Points(next, *otherHash,
	elements,
	-m_overlapLimitOpposite, +m_overlapLimitOpposite,
	spacepointCollection,overlapColl,spacepointOverlapCollection, r_cache);

    if (! m_overlap) return;

    // check phi overlaps
    overlapColl = true;
    ++otherHash;
    if (otherHash == others->end() ) return;
    float hwidth(properties->halfWidth(thisHash));
    // half-width of wafer

    checkForSCT_Points(next, *otherHash,
	elements,
	-hwidth, -hwidth+m_overlapLimitPhi,
	+hwidth-m_overlapLimitPhi, +hwidth,spacepointOverlapCollection, r_cache);
    ++otherHash;
    if (otherHash == others->end() ) return;
    checkForSCT_Points(next, *otherHash,
	elements,
	+hwidth-m_overlapLimitPhi, +hwidth,
	-hwidth, -hwidth+m_overlapLimitPhi,spacepointOverlapCollection, r_cache);

    // if barrel, check the eta overlaps too.
    // In this case, action depends on whether layer is even or odd
    // Also, whether we look at "lower in eta", which comes first,
    // or "higher".
    ++otherHash;
    if (otherHash == others->end() )  return;
    // get Identifier without hashing or so
    //Identifier thisID = element->identify();

    checkForSCT_Points(next, *otherHash,
	elements,
	+m_overlapLimitEtaMin,
	+m_overlapLimitEtaMax,
	spacepointCollection,overlapColl,spacepointOverlapCollection, r_cache);
    ++otherHash;
    if (otherHash == others->end() )  return;

    checkForSCT_Points(next, *otherHash,
	elements,
	-m_overlapLimitEtaMax,
	-m_overlapLimitEtaMin,
	spacepointCollection,overlapColl,spacepointOverlapCollection, r_cache);
  }
}


//--------------------------------------------------------------------------

void TrackerSpacePointFinder::
checkForSCT_Points(const Tracker::FaserSCT_ClusterCollection* clusters1,
    const IdentifierHash id2,
    const TrackerDD::SiDetectorElementCollection* elements,
    double min, double max,
    FaserSCT_SpacePointCollection* spacepointCollection, bool overlapColl, FaserSCT_SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache &r_cache) const
{


  // Get the cluster collections for these two detectors.
  // Require that (xPhi2 - xPhi1) lie in the range specified.
      // Used for opposite and eta modules

      //indexFindPtr is faster in the MT implementation
      const Tracker::FaserSCT_ClusterCollection * clusters2 = r_cache.SCTCContainer->indexFindPtr(id2);
      if (clusters2==nullptr) return;

      if (!overlapColl) {
	m_SiSpacePointMakerTool->fillSCT_SpacePointCollection(clusters1, clusters2, min, max, m_allClusters, r_cache.vertex, elements, spacepointCollection);
      }
      else {
	m_SiSpacePointMakerTool->fillSCT_SpacePointEtaOverlapCollection(clusters1, clusters2, min, max, m_allClusters, r_cache.vertex, elements, spacepointOverlapCollection);
      }
    }
  //--------------------------------------------------------------------------
  void TrackerSpacePointFinder::
    checkForSCT_Points(const Tracker::FaserSCT_ClusterCollection* clusters1,
	const IdentifierHash id2,
	const TrackerDD::SiDetectorElementCollection* elements,
	double min1, double max1, double min2, double max2, FaserSCT_SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache &r_cache) const
    {

      // get the cluster collections for these two detectors. Clus1 must lie
      // within min1 and max1 and clus between min2 and max2. Used for phi
      // overlaps.
      const Tracker::FaserSCT_ClusterCollection * clusters2 (r_cache.SCTCContainer->indexFindPtr(id2));
      if (clusters2==nullptr) return;

      m_SiSpacePointMakerTool->fillSCT_SpacePointPhiOverlapCollection(clusters1, clusters2, min1, max1, min2, max2, m_allClusters, r_cache.vertex, elements, spacepointOverlapCollection);
    }

}
