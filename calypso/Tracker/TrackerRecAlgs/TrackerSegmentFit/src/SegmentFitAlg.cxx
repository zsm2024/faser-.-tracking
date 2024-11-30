/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

/**   @file SegmentFitAlg.cxx
 *   Implementation file for the SegmentFitAlg class.
 *   @author Dave Casper
 *   @date 28 February 2021
 */

#include "SegmentFitAlg.h"

#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "FaserDetDescr/FaserDetectorID.h"    
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "StoreGate/ReadHandle.h"

#include <list>

namespace Tracker
{

int SegmentFitAlg::clusterInfo::nEdgeStrips {0};
size_t SegmentFitAlg::clusterInfo::nClusters {0};
double SegmentFitAlg::fitInfo::zCenter {0.0};
static const std::string moduleFailureReason{"SegmentFitAlg: Exceeds max clusters"};

// Constructor with parameters:
SegmentFitAlg::SegmentFitAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
  , m_idHelper{nullptr}
{
}

// Initialize method:
StatusCode SegmentFitAlg::initialize() {
  ATH_MSG_INFO("SegmentFitAlg::initialize()");
  if (m_maxClustersPerStation > compatibilityMaxSize)
  {
      ATH_MSG_WARNING("Requested ceiling on maximum clusters per station exceeds the allowed range. " << compatibilityMaxSize << " will be used instead.");
      m_maxClustersPerStation = compatibilityMaxSize;
  }

  if (m_minClustersPerStation > m_maxClustersPerStation)
  {
    ATH_MSG_ERROR("Requested minimum number of clusters per station exceeds the maximum number allowed.  Check your job configuration.");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_clusterContainerKey.initialize());
  ATH_CHECK(m_trackCollection.initialize());

  // Get the SCT ID helper
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

  // Get the detector manager
  ATH_CHECK(detStore()->retrieve(m_detMgr, "SCT"));
 
  // Define the edge region
  clusterInfo::nEdgeStrips = m_edgeStrips;

  return StatusCode::SUCCESS;
}

// Process one station
StatusCode SegmentFitAlg::reconstructStation(ClusterContainerHandle container, int index, std::unique_ptr<TrackCollection>& tracks) const
{
  int station = m_detMgr->numerology().stationId(index);
  ATH_MSG_VERBOSE("Processing station " << station);

  // Quickly determine whether to trash the data from this station due to excessive noise or not enough hits
  auto occupancy = stationOccupancy(container, station);
  if (occupancy < m_minClustersPerStation)
  {
    ATH_MSG_VERBOSE("Station " << station << ": not enough clusters (" << occupancy << ").");
    m_numberLowOccupancy++;
    return StatusCode::SUCCESS;
  }
  if (occupancy > m_maxClustersPerStation)
  {
    ATH_MSG_VERBOSE("Station " << station << ": too many clusters (" << occupancy << ").");
    m_numberExcessOccupancy++;
    return StatusCode::SUCCESS;
  }
  // We can proceed...
  m_numberOfGoodStations++;

  // find station center (the reference point for fits)
  fitInfo::zCenter = findCenter(station);

  clusterInfo::nClusters = occupancy;
  ClusterList l = tabulateClusters(container, station);
  ATH_MSG_VERBOSE("Station " << station << ": tabulated " << l.size() << " clusters.");
  if (l.size() < m_minClustersPerStation) return StatusCode::SUCCESS;

  // flag clusters that are compatible with each other
  checkCompatibility(l);

  int pairs {0};
  for (auto c : l)
  {
    pairs += (c->compatible.count() - 1);  // every cluster is compatible with itself
  }
  ATH_MSG_VERBOSE("Found " << pairs/2 << " compatible cluster pairs from " << l.size() << " clusters (average " << ((double)pairs/2)/l.size() << ").");

  // form viable (containing both views) groups of four compatible clusters
  FitList f = enumerateSeeds(l);
  ATH_MSG_VERBOSE("Found " << f.size() << " fit seeds from " << l.size() << " clusters.");

  // fit the seeds
  FitMap goodSeeds;
  MaskSet badSeeds;
  findGoodSeeds(f, goodSeeds, badSeeds);
  if (goodSeeds.size() == 0) return StatusCode::SUCCESS;

  // try adding clusters to the good seeds
  FitMap allFits = enumerateFits(goodSeeds, badSeeds);
  ATH_MSG_VERBOSE("Found " << allFits.size() << " fits after enumeration from " << goodSeeds.size() << " seeds.");
  FitMap goodFits = checkFitQuality(allFits);
  ATH_MSG_VERBOSE(goodFits.size() << " out of " << allFits.size() << " fits pass quality cut.");
  for (auto f : goodFits)
  {
    auto fit = f.second;
    if (fit->candidates.size() > 4)
    {
      ATH_MSG_VERBOSE("Fit Parameters: (" << fit->fitParams(0) << ", " << fit->fitParams(1) << ", " << fit->fitParams(2) << ", " << fit->fitParams(3) << "); chi2 = " << fit->fitChi2 << "; DoF = " << fit->candidates.size() - 4 );
      for (size_t i = fit->clusterMask.find_first(); i != ClusterSet::npos; i = fit->clusterMask.find_next(i))
      {
        Amg::Vector3D seedPos = l[i]->cluster.globalPosition();
        ATH_MSG_VERBOSE("Fit cluster [" << l[i]->layer << "|" << l[i]->eta << "|" << l[i]->phi << 
                        "|" << l[i]->side << "]  at (" << seedPos.x() << ", " << seedPos.y() << ", " << seedPos.z() << ")");
      }
    }
  }
  FitMap selectedFits = selectFits(goodFits);
  ATH_MSG_VERBOSE("Selected " << selectedFits.size() << " fits for output.");
  for (auto f : selectedFits)
  {
    auto fit = f.second;
    ATH_MSG_VERBOSE("Fit Parameters: (" << fit->fitParams(0) << ", " << fit->fitParams(1) << ", " << fit->fitParams(2) << ", " << fit->fitParams(3) << "); chi2 = " << fit->fitChi2 << "; DoF = " << fit->candidates.size() - 4 );
    for (size_t i = fit->clusterMask.find_first(); i != ClusterSet::npos; i = fit->clusterMask.find_next(i))
    {
      Amg::Vector3D seedPos = l[i]->cluster.globalPosition();
      ATH_MSG_VERBOSE("Fit cluster [" << l[i]->layer << "|" << l[i]->eta << "|" << l[i]->phi << 
                      "|" << l[i]->side << "]  at (" << seedPos.x() << ", " << seedPos.y() << ", " << seedPos.z() << ")");
    }
    ATH_CHECK(AddTrack(tracks, fit));
  }
  return StatusCode::SUCCESS;
}

size_t SegmentFitAlg::stationOccupancy(ClusterContainerHandle container, int station) const
{
  size_t occupancy {0};
  FaserSCT_ClusterContainer::const_iterator clusterCollections {container->begin()};
  FaserSCT_ClusterContainer::const_iterator clusterCollectionsEnd {container->end()};
  for (; clusterCollections != clusterCollectionsEnd; ++clusterCollections) 
  {
    if (getStation(*clusterCollections) == station) occupancy += (*clusterCollections)->size();
  }
  return occupancy;
}

double SegmentFitAlg::findCenter(int station) const
{
  double zMean = 0.0;
  size_t zCount = 0;
  for (auto element : *(m_detMgr->getDetectorElementCollection()))
  {
    if (m_idHelper->station(element->identify()) == station)
    {
      zMean += element->center().z();
      zCount++;
    }
  }
  if (zCount == 0)
  {
    ATH_MSG_ERROR("No detector elements found for station " << station);
    return zMean;
  }
  return zMean/zCount;
}

SegmentFitAlg::ClusterList SegmentFitAlg::tabulateClusters(ClusterContainerHandle container, int station) const
{
  ClusterList clusters;
  int index {0};
  FaserSCT_ClusterContainer::const_iterator clusterCollections {container->begin()};
  FaserSCT_ClusterContainer::const_iterator clusterCollectionsEnd {container->end()};
  for (; clusterCollections != clusterCollectionsEnd; ++clusterCollections) 
  {
    if (getStation(*clusterCollections) == station)
    {
      m_numberOfClusterCollection++;
      for (auto cluster : **clusterCollections)
      {
        m_numberOfClusters++;
        clusters.push_back(std::make_shared<clusterInfo>(index++, *cluster, m_idHelper));
      }
    }
  }
  return clusters;
}

void SegmentFitAlg::checkCompatibility(ClusterList& clusters) const
{
  for (size_t i = 0; i < clusters.size() - 1; i++)
  {
    if (clusters[i]->view == 0) continue;
    for (size_t j = i + 1; j < clusters.size(); j++)
    {
      if (clusters[j]->view == 0) continue;
      //
      //  Rule 1: Clusters in different layers are compatible
      //
      if (clusters[i]->layer != clusters[j]->layer)
      {
        m_numberOfRule1Pairs++;
        clusters[i]->compatible.set(j, true);
        clusters[j]->compatible.set(i, true);
        continue;
      }
      //
      //  Rule 2: Clusters on opposite sides of same module are compatible
      //
      if (clusters[i]->layer == clusters[j]->layer &&
          clusters[i]->moduleIndex == clusters[j]->moduleIndex &&
          clusters[i]->side != clusters[j]->side)
      {
        m_numberOfRule2Pairs++;
        clusters[i]->compatible.set(j, true);
        clusters[j]->compatible.set(i, true);
        continue;
      }
      if (clusters[i]->moduleIndex == clusters[j]->moduleIndex) continue; // same-module clusters are incompatible if they get here
      //
      //  Rule 3: Clusters in modules of the same layer with same phi + different eta may be compatible if their ends are close enough
      //
      if (m_allowEtaOverlap && clusters[i]->phi == clusters[j]->phi && abs(clusters[i]->yEnd - clusters[j]->yEnd) <= 2 * m_edgeStrips)
      {
        m_numberOfRule3Pairs++;
        clusters[i]->compatible.set(j, true);
        clusters[j]->compatible.set(i, true);
        continue;
      }
      //
      //  Rule 4: Edge clusters in modules of the same layer with different phi may be compatible with edge clusters in neighboring modules
      //
      const double maximumEdgeSeparation = 20 * Gaudi::Units::mm;  // Loose cut just to ensure both clusters are on neighboring edges
      if (m_allowPhiOverlap && clusters[i]->edge && clusters[j]->edge && 
          (clusters[i]->phi != clusters[j]->phi) && (abs(clusters[i]->yCenter - clusters[j]->yCenter) <= maximumEdgeSeparation) )
      {
        if (clusters[i]->eta == clusters[j]->eta) 
        {
          m_numberOfRule4Pairs++;
          clusters[i]->compatible.set(j, true);
          clusters[j]->compatible.set(i, true);
          continue;    
        }
        if (m_allowEtaOverlap && clusters[i]->eta != clusters[j]->eta)
        {
          m_numberOfRule4Pairs++;
          clusters[i]->compatible.set(j, true);
          clusters[j]->compatible.set(i, true);
          continue;    
        }
      }
    }
  }
}

SegmentFitAlg::FitList 
SegmentFitAlg::enumerateSeeds(const ClusterList& clusters) const
{
  FitList fits;
  for (size_t i = 0; i < clusters.size()-3; i++)
  {
    size_t i_views[2] {0 , 0};
    if (clusters[i]->view == 0)
    {
      continue;
    }
    else if (clusters[i]->view > 0)
    {
      i_views[1] = 1;
    }
    else
    {
      i_views[0]= 1;
    }
    auto i_compat = clusters[i]->compatible;
    std::vector<size_t> i_vec {i};
    for (size_t j = i_compat.find_next(i); j != ClusterSet::npos; j = i_compat.find_next(j))
    {
      size_t ij_views[2] {i_views[0], i_views[1]};
      if (clusters[j]->view > 0)
      {
        ij_views[1]++;
      }
      else if (clusters[j]->view < 0)
      {
        ij_views[0]++;
      }      
      auto ij_compat = clusters[j]->compatible & i_compat;
      for (size_t k = ij_compat.find_next(j); k != ClusterSet::npos; k = ij_compat.find_next(k))
      {
        size_t ijk_views[2] {ij_views[0], ij_views[1]};
        if (clusters[k]->view > 0)
        {
          ijk_views[1]++;
        }
        else if (clusters[k]->view < 0)
        {
          ijk_views[0]++;
        }      
        if (ijk_views[0] == 0 || ijk_views[1] == 0) continue; // Need two hits in each view to fit
        auto ijk_compat = clusters[k]->compatible & ij_compat;
        for (size_t l = ijk_compat.find_next(k); l != ClusterSet::npos; l = ijk_compat.find_next(l))
        {
          size_t ijkl_views[2] {ijk_views[0], ijk_views[1]};
          if (clusters[l]->view > 0)
          {
            ijkl_views[1]++;
          }
          else if (clusters[l]->view < 0)
          {
            ijkl_views[0]++;
          }
          if (ijkl_views[0] != 2 || ijkl_views[1] != 2) continue; // Need two hits in each view to fit
          if ((clusters[i]->layer == clusters[j]->layer && clusters[j]->layer == clusters[k]->layer) ||
              (clusters[j]->layer == clusters[k]->layer && clusters[k]->layer == clusters[l]->layer) ||
              (clusters[k]->layer == clusters[l]->layer && clusters[l]->layer == clusters[i]->layer)) continue;  // Can't fit 3+ hits in same layer
          m_numberOfSeeds++;
          fits.push_back(std::make_shared<fitInfo>(clusters, std::vector<size_t> {i, j, k, l}));
        }
      }
    }
  }
  return fits;
}

void 
SegmentFitAlg::findGoodSeeds(FitList& allSeeds, FitMap& goodSeeds, MaskSet& badSeeds) const
{
  int nGoodSeeds {0};
  for (auto seed : allSeeds)
  {
    fitClusters(*seed);

    bool seedOK = checkFit(*seed);
    if (seedOK) 
    {
      ATH_MSG_VERBOSE("Seed Parameters: (" << seed->fitParams(0) << ", " << seed->fitParams(1) << ", " << seed->fitParams(2) << ", " << seed->fitParams(3) << "); chi2 = " << seed->fitChi2 << "; additional compatible clusters: " << seed->compatibleMask.count());
      goodSeeds[seed->clusterMask] = std::shared_ptr<fitInfo> {seed};
      nGoodSeeds++;
    }
    else
    {
      badSeeds.insert(seed->clusterMask);
    }
  }
  ATH_MSG_VERBOSE("Found " << nGoodSeeds << " good seeds out of " << allSeeds.size());
  m_numberOfGoodSeeds += nGoodSeeds;
}

void
SegmentFitAlg::fitClusters(fitInfo& fit) const
{
    m_numberOfFits++;
    Eigen::Matrix< double, 5, 5 > s;
    s << fit.sums[1]  , fit.sums[2] ,  fit.sums[4] ,  fit.sums[5] ,  fit.sums[10] ,
         fit.sums[2]  , fit.sums[3] ,  fit.sums[5] ,  fit.sums[6] ,  fit.sums[11] ,
         fit.sums[4]  , fit.sums[5] ,  fit.sums[7] ,  fit.sums[8] ,  fit.sums[12] ,
         fit.sums[5]  , fit.sums[6] ,  fit.sums[8] ,  fit.sums[9] ,  fit.sums[13] ,
         fit.sums[10] , fit.sums[11] , fit.sums[12] , fit.sums[13] , fit.sums[14];
    Eigen::Matrix< double, 4, 4 > s4 = s.block<4,4>(0,0);
    Eigen::Matrix< double, 4, 1 > v;
    v << fit.sums[10] , fit.sums[11] , fit.sums[12] , fit.sums[13];
    // ATH_MSG_VERBOSE("s4 \n" << s4);
    // ATH_MSG_VERBOSE("v \n" << v);
    // ATH_MSG_VERBOSE("s4^-1 \n" << s4.inverse());
//    Eigen::Matrix< double, 4, 1 > x = s4.colPivHouseholderQr().solve(v);                
    fit.fitParams = s4.fullPivLu().solve(v);    
    // ATH_MSG_VERBOSE("x \n" << fit.fitParams);
    // ATH_MSG_VERBOSE("Check solution");
    // ATH_MSG_VERBOSE("solution: \n" << s4*fit.fitParams);
    // ATH_MSG_VERBOSE("target: \n" << v);
    Eigen::Matrix< double, 5, 1 > x5;
    x5 << fit.fitParams(0), fit.fitParams(1), fit.fitParams(2), fit.fitParams(3), -1;
    fit.fitChi2 = x5.transpose()*s*x5;  
    s4.computeInverseWithCheck(fit.fitCovariance, fit.hasCovariance);
    // ATH_MSG_VERBOSE("covariance \n" << fit.fitCovariance);
    // ATH_MSG_VERBOSE("chi2: " << fit.fitChi2);
    return;
}

bool
SegmentFitAlg::checkFit(const Tracker::SegmentFitAlg::fitInfo& seed) const
{
  if (!seed.hasCovariance) 
  {
    ATH_MSG_VERBOSE("Rejected fit without covariance matrix.");
    return false;
  }
  if (m_tanThetaCut > 0.0 && pow(seed.fitParams(2),2.0) + pow(seed.fitParams(3), 2.0) > m_tanThetaCut * m_tanThetaCut) return false;
  if (m_tanThetaXZCut > 0.0 && pow(seed.fitParams(2), 2.0) > m_tanThetaXZCut * m_tanThetaXZCut) return false;
  for (auto i : seed.candidates)
  {
    double z = seed.clusters[i]->cluster.globalPosition().z();
    HepGeom::Point3D<double> extrapolation = seed.extrapolateFit(z);
    if (seed.clusters[i]->cluster.detectorElement()->inDetector(extrapolation, m_waferTolerance, m_waferTolerance).out())
    {
      // ATH_MSG_VERBOSE("Extrapolation out of tolerance at z = " << seed.clusters[i]->cluster.globalPosition().z() << " : (" << extrapolation.x() << ", " << extrapolation.y() << ")");
      return false;
    }
    else
    {
      // ATH_MSG_VERBOSE("Extrapolation inside tolerance at z = " << seed.clusters[i]->cluster.globalPosition().z() << " : (" << extrapolation.x() << ", " << extrapolation.y() << ")");
    }
  }
  return true;
}

SegmentFitAlg::FitMap 
SegmentFitAlg::enumerateFits(const FitMap& seeds, const MaskSet& veto) const
{
  FitMap allFits {seeds};
  FitMap newSeeds {seeds};
  MaskSet allFailures {veto};
  while (true)
  {
    FitMap addedFits;
    for (auto p : newSeeds)
    {
      auto seed = p.second;
      for (size_t i = seed->compatibleMask.find_first(); i != ClusterSet::npos; i = seed->compatibleMask.find_next(i))
      {
        if (abs(seed->fitParams(1) + seed->fitParams(3) * (seed->clusters[i]->z - fitInfo::zCenter) - seed->clusters[i]->yCenter) > m_yResidualCut)
          continue;
        ClusterSet mask { seed->clusterMask };
        mask.set(i);
        if (allFits.find(mask) == allFits.end() && !vetoFit(allFailures, mask))
        {
          auto candidate = std::make_shared<fitInfo>(*seed);
          candidate->addCluster(i);
          fitClusters(*candidate);
          if (checkFit(*candidate))
          {
            allFits[candidate->clusterMask] = candidate;
            addedFits[candidate->clusterMask] = candidate;
          }
          else
          {
            allFailures.insert(candidate->clusterMask);
          }
        }
      }
    }

    if (addedFits.size() == 0) break;
    newSeeds = std::move(addedFits);
  }
  return allFits;
}

SegmentFitAlg::FitMap
SegmentFitAlg::checkFitQuality(const FitMap& fits) const
{
  FitMap goodFits { };
  for (auto f : fits)
  {
    auto candidate = f.second;
    size_t nDoF = candidate->clusterMask.count() - 4;
    if (nDoF == 0 || candidate->fitChi2/nDoF <= m_reducedChi2Cut) goodFits[candidate->clusterMask] = candidate;
  }
  m_numberOfGoodFits += goodFits.size();
  return goodFits;
}

SegmentFitAlg::FitMap
SegmentFitAlg::selectFits(const FitMap& fits) const
{
  // Select the input fit with the most clusters (using lowest chi2 as tie-breaker)
  // Discard any other input fits using those clusters
  // Repeat until no input fits remain
  FitMap selectedFits { };
  std::list<std::shared_ptr<fitInfo> > info {};
  for (auto fit : fits)
  {
    info.push_back(fit.second);
  }
  info.sort([](const std::shared_ptr<fitInfo>& left, const std::shared_ptr<fitInfo>& right)
            {
              if (left->clusterMask.count() > right->clusterMask.count()) return true;
              if (left->clusterMask.count() < right->clusterMask.count()) return false;
              if (left->fitChi2 < right->fitChi2) return true;
              return false;
            });

  ATH_MSG_VERBOSE("Sorted input fits:");
  for (auto it = info.begin(); it != info.end(); ++it)
  {
    auto content = *it;
    ATH_MSG_VERBOSE("clusters: " << content->clusterMask.count() << " / chi2: " << content->fitChi2) ;
  }

  while (info.size() > 0)
  {
    auto selected = info.front();
    ATH_MSG_VERBOSE("Selected nclust = " << selected->clusterMask.count() << " with chi2 = " << selected->fitChi2);
    selectedFits[selected->clusterMask] = selected;

    // remove fits for which the fraction of shared hits is larger than m_sharedFraction or the number of hits is
    // smaller than m_minClustersPerFit
    info.remove_if([&](std::shared_ptr<fitInfo> p) {return
        ((p->clusterMask & selected->clusterMask).count() > m_sharedHitFraction * p->clusterMask.count())
        || ((selectedFits.size() >= 4) && (p->clusterMask.count() < m_minClustersPerFit)) ;}
    );
  }

  m_numberOfSelectedFits += selectedFits.size();
  return selectedFits;
}

bool 
SegmentFitAlg::vetoFit(const MaskSet& badSeeds, const ClusterSet& hypothesis) const
{
  if (badSeeds.count(hypothesis) > 0) return true;
  return false;
}


// Execute method:
StatusCode 
SegmentFitAlg::execute(const EventContext& ctx) const
{
  ++m_numberOfEvents;  
//   setFilterPassed(false, ctx);

  SG::WriteHandle trackContainer{m_trackCollection, ctx};
  std::unique_ptr<TrackCollection> outputTracks = std::make_unique<TrackCollection>();

  ClusterContainerHandle clusterContainer{m_clusterContainerKey, ctx};
  ATH_CHECK(clusterContainer.isValid());
  ATH_MSG_VERBOSE("Retrieved cluster data from event store");

  for (int stationIdx = 0; stationIdx < m_detMgr->numerology().numStations(); stationIdx++)
  {
      ATH_CHECK(reconstructStation(clusterContainer, stationIdx, outputTracks));
  }

  // Loop over stations
//   if (!stations.empty())
//   {
//       for (int thisStation : stations)
//       {
//           if (nGoodLayers == 3)
//           {
//               Eigen::Matrix< double, 4, 1 > bestFit;
//               Amg::MatrixX bestCov(4,4);
//               std::vector<const clusterInfo*> bestClusters;
//               double bestChi2 = -1;
//               for (auto info : bestClusters)
//               {
//                   if (info->sinAlpha * info->cosAlpha < 0) continue;
//                   double dz = info->z - m_zCenter[thisStation];
//                   double xFit = bestFit(0) + bestFit(2) * dz;
//                   double yFit = bestFit(1) + bestFit(3) * dz;
//                   double uFit = yFit * info->cosAlpha + xFit * info->sinAlpha;
//               }
//               for (auto info : bestClusters)
//               {
//                   if (info->sinAlpha * info->cosAlpha > 0) continue;
//                   double dz = info->z - m_zCenter[thisStation];
//                   double xFit = bestFit(0) + bestFit(2) * dz;
//                   double yFit = bestFit(1) + bestFit(3) * dz;
//                   double uFit = yFit * info->cosAlpha + xFit * info->sinAlpha;
//                   ATH_MSG_VERBOSE("Event: " << eventNumber << " z: " << info->z << " z-zC: " << dz<< " uFit: " << uFit << " u: " << info->u);
//               }
//               //Residuals(bestClusters);
//               ATH_CHECK(AddTrack(outputTracks, bestFit, bestCov, bestClusters, bestChi2, bestClusters.size()-4));
//               setFilterPassed(true, ctx);
//               ++m_numberOfFits;
//           }
//           // clean-up
//           for (auto layerEntry : layerCombos)
//           {
//               for (auto combo : layerEntry)
//               {
//                   if (combo != nullptr) delete combo;
//               }
//               layerEntry.clear();
//           }
//           layerCombos.clear();
//       }
//   }
//   else
//   {
//       ATH_MSG_DEBUG("No stations with clusters found.");
//   }

  ATH_CHECK(trackContainer.record(std::move(outputTracks)));

//   // Clean up
//   for (auto entry : clusterMap)
//   {
//       for (clusterInfo* info : entry.second.first)
//       {
//           if (info != nullptr) delete info;
//       }
//       entry.second.first.clear();
//       for (clusterInfo* info : entry.second.second)
//       {
//           if (info != nullptr) delete info;
//       }
//       entry.second.second.clear();
//   }
//   clusterMap.clear();

  // Done
  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode SegmentFitAlg::finalize() 
{
  ATH_MSG_INFO("SegmentFitAlg::finalize()");
  ATH_MSG_INFO( m_numberOfEvents << " events processed." );
  ATH_MSG_INFO( m_numberLowOccupancy << " stations rejected for low occupancy.");
  ATH_MSG_INFO( m_numberExcessOccupancy << " stations rejected for high occupancy.");
  ATH_MSG_INFO( m_numberOfGoodStations << " good stations processed.");
  ATH_MSG_INFO( m_numberOfClusterCollection << " cluster collections processed." );
  ATH_MSG_INFO( m_numberOfClusters << " clusters processed." );
  ATH_MSG_INFO( m_numberOfRule1Pairs << " Rule 1 (different layer) pairs" );
  ATH_MSG_INFO( m_numberOfRule2Pairs << " Rule 2 (same module different side) pairs" );
  ATH_MSG_INFO( m_numberOfRule3Pairs << " Rule 3 (eta overlap) pairs" );
  ATH_MSG_INFO( m_numberOfRule4Pairs << " Rule 4 (phi overlap) pairs" );
  ATH_MSG_INFO( m_numberOfSeeds << " seeds fit.");
  ATH_MSG_INFO( m_numberOfGoodSeeds << " good seed fits.");
  ATH_MSG_INFO( m_numberOfFits << " fits performed." );
  ATH_MSG_INFO( m_numberOfGoodFits << " fits passed quality cuts." );
  ATH_MSG_INFO( m_numberOfSelectedFits << " fits selected for output." );


  return StatusCode::SUCCESS;
}

// Method to create and store Trk::Track from fit results
StatusCode 
SegmentFitAlg::AddTrack(std::unique_ptr<TrackCollection>& tracks, 
                        const std::shared_ptr<fitInfo>& theFit) const
{
    Trk::TrackInfo i { Trk::TrackInfo::TrackFitter::Unknown, Trk::ParticleHypothesis::muon };
    auto q = std::make_unique<Trk::FitQuality>(theFit->fitChi2, static_cast<double>(theFit->clusterMask.count() - 4));
    auto s = std::make_unique<Trk::TrackStates>();
    // DataVector<const Trk::TrackStateOnSurface>* s = new DataVector<const Trk::TrackStateOnSurface> {};

    // translate parameters to nominal fit point
    s->push_back( GetState(theFit->fitParams, theFit->fitCovariance, nullptr) );

    for (int clusterIndex : theFit->candidates)
    {
      s->push_back( GetState( theFit->fitParams, theFit->fitCovariance, &theFit->clusters[clusterIndex]->cluster) );
    }

    // for (const clusterInfo* cInfo : fitClusters)
    // {
	  //     s->push_back( GetState(fitResult, fitCovariance, cInfo->cluster, station) );
    // }

    // Create and store track
    // std::unique_ptr<DataVector<const Trk::TrackStateOnSurface>> sink(s);
    tracks->push_back(new Trk::Track(i, std::move(s) , std::move(q)));
    return StatusCode::SUCCESS;
}

Trk::TrackStateOnSurface*
SegmentFitAlg::GetState( const Eigen::Matrix< double, 4, 1 >& fitResult, 
                         const Eigen::Matrix< double, 4, 4 >& fitCovariance,  
                         const FaserSCT_Cluster* fitCluster) const
{
    // position of fit point:
    // int station = m_idHelper->station(fitCluster->detectorElement()->identify());
    double zFit = fitInfo::zCenter;
    if (fitCluster != nullptr) zFit = fitCluster->globalPosition()[2];
    Amg::Vector3D pos { fitResult[0] + fitResult[2] * (zFit - fitInfo::zCenter), fitResult[1] + fitResult[3] * (zFit - fitInfo::zCenter), zFit };
    double phi = atan2( fitResult[3], fitResult[2] );
    double theta = atan( sqrt(fitResult[2]*fitResult[2] + fitResult[3]*fitResult[3]) );
    double qoverp = 1.0/100000.0;

    Eigen::Matrix< double, 4, 4 > jacobian = Eigen::Matrix< double, 4, 4 >::Zero();
    jacobian << 1, 0, zFit - fitInfo::zCenter, 0, 
                0, 1, 0, zFit - fitInfo::zCenter,
                0, 0, fitResult[3]/(fitResult[2]*fitResult[2]+fitResult[3]*fitResult[3]), 
                      fitResult[2]/(fitResult[2]*fitResult[2]+fitResult[3]*fitResult[3]),
                0, 0, fitResult[2]/(sqrt(fitResult[2]*fitResult[2]+fitResult[3]*fitResult[3])*(1+fitResult[2]*fitResult[2]+fitResult[3]*fitResult[3])),
                      fitResult[3]/(sqrt(fitResult[2]*fitResult[2]+fitResult[3]*fitResult[3])*(1+fitResult[2]*fitResult[2]+fitResult[3]*fitResult[3]));
    Eigen::Matrix< double, 4, 4 > covPar {jacobian * fitCovariance * jacobian.transpose()};
    std::unique_ptr<AmgSymMatrix(5)> covPar5 { new AmgSymMatrix(5){ AmgSymMatrix(5)::Zero() } };
    covPar5->block<4,4>(0,0) = covPar;
    covPar5->block<1,1>(4,4) = Eigen::Matrix< double, 1, 1 > { (50000.0)*qoverp*qoverp };

    std::unique_ptr<FaserSCT_ClusterOnTrack> rot = nullptr;
    if (fitCluster != nullptr)
    {
        auto locpar = std::make_unique<Trk::LocalParameters>(Trk::DefinedParameter{ fitCluster->localPosition()[0], Trk::loc1}, 
                                                             Trk::DefinedParameter{ fitCluster->localPosition()[1], Trk::loc2});
        auto locerr = std::make_unique<Amg::MatrixX>(fitCluster->localCovariance());
        rot = std::make_unique<FaserSCT_ClusterOnTrack>(fitCluster, 
                                                        std::move(*locpar), std::move(*locerr),
                                          //  Trk::LocalParameters { Trk::DefinedParameter { fitCluster->localPosition()[0], Trk::loc1 }, 
                                          //                         Trk::DefinedParameter { fitCluster->localPosition()[1], Trk::loc2 } }, 
                                          //  fitCluster->localCovariance(), 
                                           m_idHelper->wafer_hash(fitCluster->detectorElement()->identify()));
    }

    std::unique_ptr<Trk::TrackParameters> p { new Trk::CurvilinearParameters { pos, phi, theta, qoverp, *(covPar5.release()) } };
    return new Trk::TrackStateOnSurface { std::move(rot), std::move(p) };
}

// void
// SegmentFitAlg::Residuals(std::vector<const clusterInfo*>& fitClusters) const
// {
//     for (const clusterInfo* cI : fitClusters)
//     {
//         double zFit = cI->z;
//         layerCombo* zero = new layerCombo{ };
//         layerCombo* one = new layerCombo{ };
//         layerCombo* two = new layerCombo{ };
//         zero->initialize();
//         one->initialize();
//         two->initialize();
//         if (fitClusters[0]->z != zFit) zero->addCluster(fitClusters[0], zFit);
//         if (fitClusters[1]->z != zFit) zero->addCluster(fitClusters[1], zFit);
//         if (fitClusters[2]->z != zFit) one->addCluster(fitClusters[2], zFit);
//         if (fitClusters[3]->z != zFit) one->addCluster(fitClusters[3], zFit);
//         if (fitClusters[4]->z != zFit) two->addCluster(fitClusters[4], zFit);
//         if (fitClusters[5]->z != zFit) two->addCluster(fitClusters[5], zFit);
//         std::tuple<Eigen::Matrix<double, 4, 1>, Eigen::Matrix<double, 4, 4>, double> newFit = ClusterFit( zero, one, two);
//         double deltaU = std::get<0>(newFit)[1] * cI->cosAlpha + std::get<0>(newFit)[0] * cI->sinAlpha - cI->u;
//         double pullU = deltaU/sqrt(cI->sigmaSq + std::get<1>(newFit)(0,0)*cI->sinAlpha*cI->sinAlpha + std::get<1>(newFit)(1,1)*cI->cosAlpha*cI->cosAlpha);

//         const TrackerDD::SiDetectorElement* elem = cI->cluster->detectorElement();
//         IdentifierHash hash = m_idHelper->wafer_hash(elem->identify());
//         ATH_MSG_VERBOSE("hash, residual, pull, n:" << hash << " : " << deltaU << " : " << pullU << " : " << zero->sums[0] + one->sums[0] + two->sums[0]);

//         delete zero;
//         delete one;
//         delete two;
//     }
// }

}
