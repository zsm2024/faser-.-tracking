#include "SPSeedBasedInitialParameterTool.h"

#include "StoreGate/ReadHandle.h"
#include "HepMC3/GenVertex.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Definitions/Units.hpp"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include <random>
#include <cmath>


using namespace Acts::UnitLiterals;

SPSeedBasedInitialParameterTool::SPSeedBasedInitialParameterTool(
    const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent) {}


  StatusCode SPSeedBasedInitialParameterTool::initialize() {
    ATH_MSG_INFO("SPSeedBasedInitialParameterTool::initialize");
    ATH_CHECK(m_trackerSeedContainerKey.initialize());
    ATH_CHECK(detStore()->retrieve(m_idHelper,"FaserSCT_ID"));
    return StatusCode::SUCCESS;
  }


StatusCode SPSeedBasedInitialParameterTool::finalize() {
  ATH_MSG_INFO("SPSeedBasedInitialParameterTool::finalize");
  return StatusCode::SUCCESS;
}


std::vector<Acts::BoundTrackParameters> SPSeedBasedInitialParameterTool::getInitialParameters() const {
  std::vector<Acts::BoundTrackParameters> initialParameters;
  initialParameters.clear();
  SG::ReadHandle<Tracker::TrackerSeedCollection> trackerSeedCollection(m_trackerSeedContainerKey);
  if (!trackerSeedCollection.isValid()){
    msg(MSG::FATAL) << "Could not find the data object "<< trackerSeedCollection.name() << " !" << endmsg;                                             
    return initialParameters;
  }
  //  msg(MSG::INFO) << "found "<< trackerSeedCollection.size()<< " seeds" << endmsg;                                             

  double charge = 1;
  double B = 0.55;

  Tracker::TrackerSeedCollection::const_iterator it = trackerSeedCollection->begin();
  Tracker::TrackerSeedCollection::const_iterator itend = trackerSeedCollection->end();
  for (; it != itend; ++it){
    const Tracker::TrackerSeed *colNext=&(**it);
    auto sps=colNext->getSpacePoints();
    msg(MSG::DEBUG) << "found "<< sps.size()<< "spacepoints in the seed" << endmsg;                                             

    Acts::Vector3 pos1_0(0., 0., 0.);
    Acts::Vector3 pos1_1(0., 0., 0.);
    Acts::Vector3 pos1_2(0., 0., 0.);                                       
    Acts::Vector3 pos2_0(0., 0., 0.);
    Acts::Vector3 pos2_1(0., 0., 0.);
    Acts::Vector3 pos2_2(0., 0., 0.);
    for(auto isp : sps){
      //get the cluster ID
      const Identifier id = isp->clusterList().first->identify();
      Amg::Vector3D gloPos=isp->globalPosition();
      int station = m_idHelper->station(id);
      int plane = m_idHelper->layer(id);
      if (station==1 && plane==0) pos1_0 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
      if (station==1 && plane==1) pos1_1 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
      if (station==1 && plane==2) pos1_2 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
      if (station==2 && plane==0) pos2_0 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
      if (station==2 && plane==1) pos2_1 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
      if (station==2 && plane==2) pos2_2 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
    }
    if((pos1_0.z()==0)||(pos1_1.z()==0)||(pos1_2.z()==0)||(pos2_0.z()==0)||(pos2_1.z()==0)||(pos2_2.z()==0)){
      msg(MSG::WARNING) << "failed to find 2 triplets "<< endmsg;                                             
      continue;
    }

    //const Acts::Vector3 pos = pos1_0;
    const Acts::Vector3 pos(pos1_0.x(), pos1_0.y(), pos1_0.z());
    Acts::Vector3 d1 = pos1_2 - pos1_0;
    Acts::Vector3 d2 = pos2_2 - pos2_0;
    // the direction of momentum in the first station
    Acts::Vector3 direct1 = d1.normalized();
    // the direction of momentum in the second station
    Acts::Vector3 direct2 = d2.normalized();
    // the vector pointing from the center of circle to the particle at la     yer 2 in Y-Z plane
    double R1_z = charge * direct1.y() / std::sqrt(direct1.y()*direct1.y()      + direct1.z()*direct1.z());
    // double R1_y = -charge * direct1.z() / std::sqrt(direct1.y()*direct1     .y() + direct1.z()*direct1.z());
    double R2_z = charge * direct2.y() / std::sqrt(direct2.y()*direct2.y()      + direct2.z()*direct2.z());
    double R = (pos2_0.z() - pos1_2.z()) / (R2_z - R1_z);
    // the norm of momentum in Y-Z plane
    double p_yz = 0.3*B*R / 1000.0;  // R(mm), p(GeV), B(T)
    double p_z = p_yz * direct1.z() / std::sqrt(direct1.y()*direct1.y() +      direct1.z()*direct1.z());
    double p_y = p_yz * direct1.y() / std::sqrt(direct1.y()*direct1.y() +      direct1.z()*direct1.z());
    double p_x = direct1.x() * p_z / direct1.z();
    // total momentum at the layer 0
    const Acts::Vector3 mom(p_x, p_y, p_z);
    double p = mom.norm();
    msg(MSG::DEBUG)<<"!!!!!!!!!!!  InitTrack momentum on layer 0: ( "<<mom.x()*1000<<",  "<<mom.y()*1000<<",  "<<mom.z()*1000<<",  "<<p*1000<<")  "<<endmsg;
    // build the track covariance matrix using the smearing sigmas
    double sigmaU = 200_um;
    double sigmaV = 200_um;
    double sigmaPhi = 1_degree;
    double sigmaTheta = 1_degree;
    double sigmaQOverP = 0.01*p / (p*p);
    double sigmaT0 = 1_ns;
    Acts::BoundSquareMatrix cov = Acts::BoundSquareMatrix::Zero();
    cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = sigmaU * sigmaU;
    cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = sigmaV * sigmaV;
    cov(Acts::eBoundPhi, Acts::eBoundPhi) = sigmaPhi * sigmaPhi;
    cov(Acts::eBoundTheta, Acts::eBoundTheta) = sigmaTheta * sigmaTheta;
    cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = sigmaQOverP * sigmaQOverP;
    cov(Acts::eBoundTime, Acts::eBoundTime) = sigmaT0 * sigmaT0;
 

    const auto surface = Acts::Surface::makeShared<Acts::PlaneSurface>(
    Acts::Vector3 {0, 0, pos.z()}, Acts::Vector3{0, 0, -1});

    Acts::BoundVector params = Acts::BoundVector::Zero();
    params[Acts::eBoundLoc0] = pos.x();
    params[Acts::eBoundLoc1] = pos.y();
    params[Acts::eBoundPhi] = Acts::VectorHelpers::phi(mom.normalized());
    params[Acts::eBoundTheta] = Acts::VectorHelpers::theta(mom.normalized());
    params[Acts::eBoundQOverP] = charge/p;
    params[Acts::eBoundTime] = 0;

    //@todo: make the particle hypothesis configurable
    Acts::BoundTrackParameters InitTrackParam  = Acts::BoundTrackParameters(surface, params, cov, Acts::ParticleHypothesis::muon());

    initialParameters.push_back(InitTrackParam);
  }


  return initialParameters;
}
