/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

/**   @file ClusterFitAlg.cxx
 *   Implementation file for the ClusterFitAlg class.
 *   @author Dave Casper
 *   @date 28 February 2021
 */

#include "ClusterFitAlg.h"

#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "FaserDetDescr/FaserDetectorID.h"    
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "StoreGate/ReadHandle.h"
#include <TH1F.h>
#include <TTree.h>
#include <TBranch.h>


namespace Tracker
{

static const std::string moduleFailureReason{"ClusterFitAlg: Exceeds max clusters"};

// Constructor with parameters:
ClusterFitAlg::ClusterFitAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator), 
  AthHistogramming( name ),
  m_idHelper{nullptr},
  m_histSvc ( "THistSvc/THistSvc", name )
{
}

// Initialize method:
StatusCode ClusterFitAlg::initialize() {
  ATH_MSG_INFO("ClusterFitAlg::initialize()");
  ATH_MSG_ALWAYS("zCenter: " << m_zCenter[0] << " / " << m_zCenter[1] << " / " << m_zCenter[2] << " / " << m_zCenter[3]);

  ATH_CHECK(m_faserTriggerDataKey.initialize());
  ATH_CHECK(m_clusterContainerKey.initialize());
  ATH_CHECK(m_trackCollection.initialize());

  // Get the SCT ID helper
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));


  // Histograms
  m_chi2 = new TH1F("chi2", "Fit #chi^2;#chi^2;Events/bin", 100, 0, 50);

  // Tree
  m_tree = new TTree("residTree","Cosmics residuals");
  m_tree->Branch("id", &m_hash, "id/I");
  m_tree->Branch("residual", &m_residual, "residual/F");
  m_tree->Branch("pull", &m_pull, "pull/F");
  m_tree->Branch("chi2", &m_refitChi2, "chi2/F");

  ATH_CHECK(histSvc()->regHist("/HIST/chi2", m_chi2));
  ATH_CHECK(histSvc()->regTree("/HIST/residTree", m_tree));

  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode ClusterFitAlg::execute(const EventContext& ctx) const
{
  ++m_numberOfEvents;  
  setFilterPassed(false, ctx);

  SG::WriteHandle trackContainer{m_trackCollection, ctx};
  std::unique_ptr<TrackCollection> outputTracks = std::make_unique<TrackCollection>();

  SG::ReadHandle<xAOD::FaserTriggerData> triggerContainer{m_faserTriggerDataKey, ctx};
  ATH_CHECK(triggerContainer.isValid());
  uint32_t eventNumber = triggerContainer->eventId();
  uint8_t triggerBits = triggerContainer->tap();
//   if ((triggerBits & 0x4) == 0) 
  if (m_triggerMask != 0 && ((triggerBits & m_triggerMask) == 0))
//   if (eventNumber != 10841)
  {
      ATH_CHECK(trackContainer.record(std::move(outputTracks)));
      return StatusCode::SUCCESS;
  }
  ++m_numberOfTriggeredEvents;

  SG::ReadHandle<FaserSCT_ClusterContainer> clusterContainer{m_clusterContainerKey, ctx};
  ATH_CHECK(clusterContainer.isValid());

  // Tabulate cluster info by wafer

  std::map<IdentifierHash, std::pair<std::vector<clusterInfo*>, std::vector<clusterInfo*>>> clusterMap; 
  std::set<int> stations {};

  FaserSCT_ClusterContainer::const_iterator clusterCollections {clusterContainer->begin()};
  FaserSCT_ClusterContainer::const_iterator clusterCollectionsEnd {clusterContainer->end()};

  // Make a first pass and count clusters
  // Only if cluster limit is set to non-zero value
  if (m_clusterLimit > 0) {
    unsigned int n_clusters(0);
    for (; clusterCollections != clusterCollectionsEnd; ++clusterCollections) 
    {
      const Tracker::FaserSCT_ClusterCollection* cl{*clusterCollections};
      n_clusters += cl->size();
    }

    // Give up if too many clusters found
    if (n_clusters > m_clusterLimit) {
      m_numberOfSkippedEvents++;
      ATH_MSG_WARNING("Skipping event with " << n_clusters << " clusters!");
      ATH_CHECK(trackContainer.record(std::move(outputTracks)));
      return StatusCode::SUCCESS;
    }

    // Reset begin pointer and tabulate cluster info by wafer
    clusterCollections = clusterContainer->begin();
  }

  for (; clusterCollections != clusterCollectionsEnd; ++clusterCollections) 
  {
    ++m_numberOfClusterCollection;      
    const Tracker::FaserSCT_ClusterCollection* cl{*clusterCollections};
    m_numberOfCluster += cl->size();
    IdentifierHash hash = cl->identifyHash();
    clusterMap.emplace(std::make_pair(hash,std::make_pair(std::vector<clusterInfo*>{ }, std::vector<clusterInfo*> { })));

    FaserSCT_ClusterCollection::const_iterator clusters {cl->begin()};
    FaserSCT_ClusterCollection::const_iterator clustersEnd {cl->end()};
    for (; clusters != clustersEnd; ++clusters)
    {
        ATH_MSG_VERBOSE(**clusters);
        const TrackerDD::SiDetectorElement* elem = (*clusters)->detectorElement();
        if (elem != nullptr)
        {
            ATH_MSG_VERBOSE("( " << m_idHelper->wafer_hash(elem->identify()) << " = " << m_idHelper->station(elem->identify()) << "/" << m_idHelper->layer(elem->identify()) <<  "/" << m_idHelper->eta_module(elem->identify()) << "/" << m_idHelper->phi_module(elem->identify()) << "|" << m_idHelper->side(elem->identify()) <<
                         " ): isStereo? " << elem->isStereo() << " sinStereo: " << elem->sinStereo() << " Swap phi? " << elem->swapPhiReadoutDirection() << " Depth, Eta, Phi directions: " << 
                         elem->hitDepthDirection() << " : " << elem->hitEtaDirection() << " : " << elem->hitPhiDirection() << " / zGlobal = " << (*clusters)->globalPosition().z());
            Identifier id = elem->identify();
            stations.emplace(m_idHelper->station(id));
            clusterInfo* info = new clusterInfo { };
            info->cluster  = *clusters;
            double alpha = std::abs(asin(elem->sinStereo()));
            int layer = m_idHelper->layer(id);
            int eta = m_idHelper->eta_module(id); // -1 or +1
            int etaIndex = (eta + 1)/2; // 0 or 1
            int phi = m_idHelper->phi_module(id); // 0 to 3
            int moduleIndex = ((etaIndex + phi)%2 == 1 ? phi : phi + 4); // 0 to 7
            switch (moduleIndex)
            {
                case 0:
                case 2:
                {
                    // info->sinAlpha = sin(alpha);
                    // info->cosAlpha = -cos(alpha);
                    info->sinAlpha = -sin(alpha);
                    info->cosAlpha = cos(alpha);
                    break;
                }
                case 1:
                case 3:
                {
                    info->sinAlpha = -sin(alpha);
                    info->cosAlpha = cos(alpha);
                    break;
                }
                case 4:
                case 6:
                {
                    // info->sinAlpha = -sin(alpha);
                    // info->cosAlpha = -cos(alpha);
                    info->sinAlpha = sin(alpha);
                    info->cosAlpha = cos(alpha);
                    break;
                }
                case 5:
                case 7:
                {
                    info->sinAlpha = sin(alpha);
                    info->cosAlpha = cos(alpha);
                    break;
                }
            }
            int side = m_idHelper->side(id);
            if (side > 0) info->sinAlpha = - info->sinAlpha; // always reverse angle for back side wafer
            info->u        = (*clusters)->globalPosition().y() * info->cosAlpha + (*clusters)->globalPosition().x() * info->sinAlpha;
            info->z        = (*clusters)->globalPosition().z();
            ATH_MSG_VERBOSE("Event: " << eventNumber << " Adding cluster " << layer << "/" << eta << "/" << phi << "/" << side << ", u= " << info->u << ", z= " << info->z << ", sin= " << info->sinAlpha << ", cos= " << info->cosAlpha );
            info->sigmaSq  = (*clusters)->localCovariance()(0,0);
            if (info->sinAlpha * info->cosAlpha < 0)
            {
                clusterMap[hash/2].first.push_back(info);
            }
            else if (info->sinAlpha * info->cosAlpha > 0)
            {
                clusterMap[hash/2].second.push_back(info);
            }
            else
            {
                ATH_MSG_ERROR("Invalid stereo angle");
                delete info;
            }
        }
        else
        {
            ATH_MSG_VERBOSE("detelem missing");
        }
    }
    ATH_MSG_DEBUG("Cluster collection size=" << cl->size() << ", Hash=" << cl->identifyHash());
  }
  ATH_MSG_DEBUG("clusterContainer->numberOfCollections() " << clusterContainer->numberOfCollections());

  // Loop over stations
  if (!stations.empty())
  {
      for (int thisStation : stations)
      {
          bool layerGood[3] {false, false, false};
          int nGoodLayers = 0;
          std::vector<std::vector<layerCombo*>> layerCombos { std::vector<layerCombo*> {}, std::vector<layerCombo*> {}, std::vector<layerCombo*> {} };
          ATH_MSG_VERBOSE(" Processing station: " << thisStation);
          for (auto entry : clusterMap )
          {
              Identifier waferID = m_idHelper->wafer_id(2 * entry.first);
              if (m_idHelper->station( waferID ) != thisStation) continue;
              int layer = m_idHelper->layer( waferID );
              std::vector<clusterInfo*>& stereoMinus = entry.second.first;
              std::vector<clusterInfo*>& stereoPlus  = entry.second.second;
              ATH_MSG_VERBOSE("Module entry in layer " << layer << " has " << stereoMinus.size() << " negative and " << stereoPlus.size() << " positive stereo clusters");
              if (stereoMinus.size() > 0 && stereoPlus.size() > 0)
              {
                if (!layerGood[layer])
                {
                    nGoodLayers++;
                    layerGood[layer] = true;
                }
                for (clusterInfo* minus : stereoMinus)
                {
                    for (clusterInfo* plus : stereoPlus)
                    {
                        layerCombo* combo = new layerCombo { };
                        combo->initialize();
                        combo->addCluster(plus, m_zCenter[thisStation]);
                        combo->addCluster(minus, m_zCenter[thisStation]);
                        layerCombos[layer].push_back( combo );
                    }
                }
              }
          }
          ATH_MSG_VERBOSE( "Found " << nGoodLayers << " layers with stereo clusters in at least one module");
          if (nGoodLayers == 3)
          {
              Eigen::Matrix< double, 4, 1 > bestFit;
              Amg::MatrixX bestCov(4,4);
              std::vector<const clusterInfo*> bestClusters;
              double bestChi2 = -1;
              for (layerCombo* combo_0 : layerCombos[0])
              {
                  for (layerCombo* combo_1 : layerCombos[1])
                  {
                      for (layerCombo* combo_2 : layerCombos[2])
                      {
                          std::tuple<Eigen::Matrix<double, 4, 1>, 
                                     Eigen::Matrix<double, 4, 4>, 
                                     double> 
                                     fitResult = ClusterFit(combo_0, combo_1, combo_2);
                          if (bestChi2 < 0 || std::get<2>(fitResult) < bestChi2)
                          {
                            bestChi2 = std::get<2>(fitResult);
                            bestFit = std::get<0>(fitResult);
                            bestCov = std::get<1>(fitResult);
                            bestClusters.clear();
                            if (combo_0->stereoPlus != nullptr)
                              bestClusters.push_back(combo_0->stereoPlus);
                            if (combo_0->stereoMinus != nullptr)
                              bestClusters.push_back(combo_0->stereoMinus);
                            if (combo_1->stereoPlus != nullptr)
                              bestClusters.push_back(combo_1->stereoPlus);
                            if (combo_1->stereoMinus != nullptr)
                              bestClusters.push_back(combo_1->stereoMinus);
                            if (combo_2->stereoPlus != nullptr)
                              bestClusters.push_back(combo_2->stereoPlus);
                            if (combo_2->stereoMinus != nullptr)
                              bestClusters.push_back(combo_2->stereoMinus);
                          }
                       }
                  }
              }
              ATH_MSG_VERBOSE("Event #" << eventNumber << " best fit: (" << bestFit(0) << "+/-" << sqrt(bestCov(0,0)) << "," << 
                                               bestFit(1) << "+/-" << sqrt(bestCov(1,1)) << "," << 
                                               bestFit(2) << "+/-" << sqrt(bestCov(2,2)) << "," << 
                                               bestFit(3) << "+/-" << sqrt(bestCov(3,3)) << "); chi2 = " << bestChi2 );
            //   if (eventNumber == 10841) std::cout << "fit = ParametricPlot3D[{" << bestFit(0) << " + (" << bestFit(2) << ") z, " << bestFit(1) << " + ( " << bestFit(3) << ") z, z}, {z, -50, 50}, Boxed->False];" << std::endl;                                               
            //   int iC = 0;
              for (auto info : bestClusters)
              {
                  if (info->sinAlpha * info->cosAlpha < 0) continue;
                  double dz = info->z - m_zCenter[thisStation];
                  double xFit = bestFit(0) + bestFit(2) * dz;
                  double yFit = bestFit(1) + bestFit(3) * dz;
                  double uFit = yFit * info->cosAlpha + xFit * info->sinAlpha;
                  ATH_MSG_VERBOSE("Event: " << eventNumber << " z: " << info->z << " z-zC: " << dz<< " uFit: " << uFit << " u: " << info->u);
                //   auto stripEnds = info->cluster->detectorElement()->endsOfStrip(info->cluster->localPosition());
                //   if (eventNumber == 10841) std::cout << "l" << iC << " = Line[{{" << stripEnds.first.x() << ", " << stripEnds.first.y() << ", " << stripEnds.first.z() - m_zCenter[thisStation] << "}, {" << 
                //                                                       stripEnds.second.x() << ", " << stripEnds.second.y() << ", " << stripEnds.second.z() - m_zCenter[thisStation] << "}}];" << std::endl;
                //   iC++;
              }
              for (auto info : bestClusters)
              {
                  if (info->sinAlpha * info->cosAlpha > 0) continue;
                  double dz = info->z - m_zCenter[thisStation];
                  double xFit = bestFit(0) + bestFit(2) * dz;
                  double yFit = bestFit(1) + bestFit(3) * dz;
                  double uFit = yFit * info->cosAlpha + xFit * info->sinAlpha;
                  ATH_MSG_VERBOSE("Event: " << eventNumber << " z: " << info->z << " z-zC: " << dz<< " uFit: " << uFit << " u: " << info->u);
                //   auto stripEnds = info->cluster->detectorElement()->endsOfStrip(info->cluster->localPosition());
                //   if (eventNumber == 10841) std::cout << "l" << iC << " = Line[{{" << stripEnds.first.x() << ", " << stripEnds.first.y() << ", " << stripEnds.first.z() - m_zCenter[thisStation] << "}, {" << 
                //                                                       stripEnds.second.x() << ", " << stripEnds.second.y() << ", " << stripEnds.second.z() - m_zCenter[thisStation] << "}}];" << std::endl;
                //   iC++;
              }
            //   if (eventNumber == 10841) std::cout << "Show[Graphics3D[{l1, l2, l3, l4, l5, l6}, Boxed -> False], fit]" << std::endl;
              //Residuals(bestClusters);
              ATH_CHECK(AddTrack(outputTracks, bestFit, bestCov, bestClusters, bestChi2, bestClusters.size()-4));
              m_chi2->Fill(bestChi2);
              setFilterPassed(true, ctx);
              ++m_numberOfFits;
          }
          // clean-up
          for (auto layerEntry : layerCombos)
          {
              for (auto combo : layerEntry)
              {
                  if (combo != nullptr) delete combo;
              }
              layerEntry.clear();
          }
          layerCombos.clear();
      }
  }
  else
  {
      ATH_MSG_DEBUG("No stations with clusters found.");
  }

  ATH_CHECK(trackContainer.record(std::move(outputTracks)));

  // Clean up
  for (auto entry : clusterMap)
  {
      for (clusterInfo* info : entry.second.first)
      {
          if (info != nullptr) delete info;
      }
      entry.second.first.clear();
      for (clusterInfo* info : entry.second.second)
      {
          if (info != nullptr) delete info;
      }
      entry.second.second.clear();
  }
  clusterMap.clear();

  // Done
  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode ClusterFitAlg::finalize() 
{
  ATH_MSG_INFO("ClusterFitAlg::finalize()");
  ATH_MSG_INFO( m_numberOfEvents << " events processed" );
  ATH_MSG_INFO( m_numberOfTriggeredEvents << " triggered events processed" );
  ATH_MSG_INFO( m_numberOfSkippedEvents << " events skipped for too many clusters" );
  ATH_MSG_INFO( m_numberOfClusterCollection<< " cluster collections processed" );
  ATH_MSG_INFO( m_numberOfCluster<< " cluster processed" );
  ATH_MSG_INFO( m_numberOfFits << " fits performed" );

  return StatusCode::SUCCESS;
}

// Method to create and store Trk::Track from fit results
StatusCode 
ClusterFitAlg::AddTrack(std::unique_ptr<TrackCollection>& tracks, 
                        const Eigen::Matrix< double, 4, 1 >& fitResult, 
                        const Eigen::Matrix< double, 4, 4 >& fitCovariance,  
                        std::vector<const clusterInfo*>& fitClusters,
                        double chi2, int ndof) const
{
    Trk::TrackInfo i { Trk::TrackInfo::TrackFitter::Unknown, Trk::ParticleHypothesis::muon };
    auto q = std::make_unique<Trk::FitQuality>(chi2, ndof);
    auto s = std::make_unique<Trk::TrackStates>();

    int station = m_idHelper->station(fitClusters[0]->cluster->detectorElement()->identify());

    // translate parameters to nominal fit point
    s->push_back( GetState(fitResult, fitCovariance, nullptr, station) );

    for (const clusterInfo* cInfo : fitClusters)
    {
	      s->push_back( GetState(fitResult, fitCovariance, cInfo->cluster, station) );
    }

    // Create and store track
    // std::unique_ptr<DataVector<const Trk::TrackStateOnSurface>> sink(s);
    tracks->push_back(new Trk::Track(i, std::move(s) , std::move(q)));
    return StatusCode::SUCCESS;
}

Trk::TrackStateOnSurface*
ClusterFitAlg::GetState( const Eigen::Matrix< double, 4, 1 >& fitResult, 
                         const Eigen::Matrix< double, 4, 4 >& fitCovariance,  
                         const FaserSCT_Cluster* fitCluster, int station ) const
{
    // position of fit point:
    // int station = m_idHelper->station(fitCluster->detectorElement()->identify());
    double zFit = m_zCenter[station];
    if (fitCluster != nullptr) zFit = fitCluster->globalPosition()[2];
    Amg::Vector3D pos { fitResult[0] + fitResult[2] * (zFit - m_zCenter[station]), fitResult[1] + fitResult[3] * (zFit - m_zCenter[station]), zFit };
    double phi = atan2( fitResult[3], fitResult[2] );
    double theta = atan( sqrt(fitResult[2]*fitResult[2] + fitResult[3]*fitResult[3]) );
    double qoverp = 1.0/100000.0;

    Eigen::Matrix< double, 4, 4 > jacobian = Eigen::Matrix< double, 4, 4 >::Zero();
    jacobian << 1, 0, zFit - m_zCenter[station], 0, 
                0, 1, 0, zFit - m_zCenter[station],
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
        rot = std::make_unique<FaserSCT_ClusterOnTrack>( fitCluster, 
                                          std::move(*locpar), std::move(*locerr),
                                          m_idHelper->wafer_hash(fitCluster->detectorElement()->identify()));
    }
    std::unique_ptr<Trk::TrackParameters> p { new Trk::CurvilinearParameters { pos, phi, theta, qoverp, *(covPar5.release()) } };
    return new Trk::TrackStateOnSurface { std::move(rot), std::move(p) };
}

std::tuple<Eigen::Matrix<double, 4, 1>, 
           Eigen::Matrix<double, 4, 4>, 
           double> 
ClusterFitAlg::ClusterFit(layerCombo* c0, layerCombo* c1, layerCombo* c2) const
{ 
    double sums[15];
    for (size_t i = 0; i < 15; i++)
    {
        sums[i] = c0->sums[i] + c1->sums[i] + c2->sums[i];
    }
    Eigen::Matrix< double, 5, 5 > s;
    s << sums[1] , sums[2] , sums[4] , sums[5] , sums[10] ,
        sums[2] , sums[3] , sums[5] , sums[6] , sums[11] ,
        sums[4] , sums[5] , sums[7] , sums[8] , sums[12] ,
        sums[5] , sums[6] , sums[8] , sums[9] , sums[13] ,
        sums[10] , sums[11] , sums[12] , sums[13] , sums[14];
    Eigen::Matrix< double, 4, 4 > s4;
    s4 = s.block<4,4>(0,0);
    Eigen::Matrix< double, 4, 1 > v;
    v << sums[10] , sums[11] , sums[12] , sums[13];
    ATH_MSG_VERBOSE(s4);
    ATH_MSG_VERBOSE(v);
    ATH_MSG_VERBOSE(s4.inverse());
//    Eigen::Matrix< double, 4, 1 > x = s4.colPivHouseholderQr().solve(v);                
    Eigen::Matrix< double, 4, 1 > x = s4.fullPivLu().solve(v);       
    ATH_MSG_VERBOSE("Check solution");
    ATH_MSG_VERBOSE(s4*x);
    ATH_MSG_VERBOSE(v);
    Eigen::Matrix< double, 5, 1 > x5;
    x5 << x(0), x(1), x(2), x(3), -1;
    double chi2 = x5.transpose()*s*x5;  

    return std::make_tuple(x, s4.inverse(), chi2);
}

void
ClusterFitAlg::Residuals(std::vector<const clusterInfo*>& fitClusters) const
{
    for (const clusterInfo* cI : fitClusters)
    {
        double zFit = cI->z;
        layerCombo* zero = new layerCombo{ };
        layerCombo* one = new layerCombo{ };
        layerCombo* two = new layerCombo{ };
        zero->initialize();
        one->initialize();
        two->initialize();
        if (fitClusters[0]->z != zFit) zero->addCluster(fitClusters[0], zFit);
        if (fitClusters[1]->z != zFit) zero->addCluster(fitClusters[1], zFit);
        if (fitClusters[2]->z != zFit) one->addCluster(fitClusters[2], zFit);
        if (fitClusters[3]->z != zFit) one->addCluster(fitClusters[3], zFit);
        if (fitClusters[4]->z != zFit) two->addCluster(fitClusters[4], zFit);
        if (fitClusters[5]->z != zFit) two->addCluster(fitClusters[5], zFit);
        std::tuple<Eigen::Matrix<double, 4, 1>, Eigen::Matrix<double, 4, 4>, double> newFit = ClusterFit( zero, one, two);
        double deltaU = std::get<0>(newFit)[1] * cI->cosAlpha + std::get<0>(newFit)[0] * cI->sinAlpha - cI->u;
        double pullU = deltaU/sqrt(cI->sigmaSq + std::get<1>(newFit)(0,0)*cI->sinAlpha*cI->sinAlpha + std::get<1>(newFit)(1,1)*cI->cosAlpha*cI->cosAlpha);

        const TrackerDD::SiDetectorElement* elem = cI->cluster->detectorElement();
        IdentifierHash hash = m_idHelper->wafer_hash(elem->identify());
        ATH_MSG_VERBOSE("hash, residual, pull, n:" << hash << " : " << deltaU << " : " << pullU << " : " << zero->sums[0] + one->sums[0] + two->sums[0]);
        m_hash = hash;
        m_residual = deltaU;
        m_pull = pullU;
        m_refitChi2 = std::get<2>(newFit);
        m_tree->Fill();

        delete zero;
        delete one;
        delete two;
    }
}

}
