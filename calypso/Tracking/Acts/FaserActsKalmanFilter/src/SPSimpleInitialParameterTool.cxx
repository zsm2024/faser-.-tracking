#include "SPSimpleInitialParameterTool.h"


#include "StoreGate/ReadHandle.h"
#include "HepMC3/GenVertex.h"
#include "TrackerIdentifier/FaserSCT_ID.h"

#include "Acts/Surfaces/PerigeeSurface.hpp"

#include <random>
#include <cmath>


using namespace Acts::UnitLiterals;

SPSimpleInitialParameterTool::SPSimpleInitialParameterTool(
    const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent) {}


  StatusCode SPSimpleInitialParameterTool::initialize() {
    ATH_MSG_INFO("SPSimpleInitialParameterTool::initialize");
    return StatusCode::SUCCESS;
  }


StatusCode SPSimpleInitialParameterTool::finalize() {
  ATH_MSG_INFO("SPSimpleInitialParameterTool::finalize");
  return StatusCode::SUCCESS;
}


Acts::BoundTrackParameters SPSimpleInitialParameterTool::getInitialParameters(Acts::Vector3 pos1, Acts::Vector3 pos2, Acts::Vector3 pos3) const {

  double charge = pos1.y()+pos3.y()>2.*pos2.y()?1:-1;
  double B = 0.55;

  Acts::Vector3 amc=pos3-pos1;
  Acts::Vector3 amb=pos2-pos1;
  Acts::Vector3 bmc=pos3-pos2;
  Acts::Vector3 apc=(pos3+pos1)/2.;
  double tmpa=sqrt(amc.y()*amc.y()+amc.z()*amc.z())/2.;
  double tmpb=sqrt((pos2.y()-apc.y())*(pos2.y()-apc.y())+(pos2.z()-apc.z())*(pos2.z()-apc.z()));
  double R=(tmpa*tmpa+tmpb*tmpb)/2./tmpb;
  double p_yz = 0.3*B*R / 1000.0;  // R(mm), p(GeV), B(T)

  Acts::Vector3 d1=amc.normalized()+amb.normalized()-bmc.normalized();
  Acts::Vector3 d2=(pos3+pos2)/2.-pos1;
  double p_z=p_yz*d1.z()/sqrt(d1.z()*d1.z()+d1.y()*d1.y());
  double p_y=p_z*d1.y()/d1.z();
  double p_x=p_z*d2.x()/d2.z();

  const Acts::Vector3 mom(p_x, p_y, p_z);
  double p = mom.norm();
  double theta = Acts::VectorHelpers::theta(mom.normalized());
  double phi = Acts::VectorHelpers::phi(mom.normalized());
  msg(MSG::DEBUG)<<"!!!!!!!!!!!  InitTrack momentum on layer 0: ( "<<mom.x()*1000<<",  "<<mom.y()*1000<<",  "<<mom.z()*1000<<",  "<<p*1000<<")  "<<endmsg;
  // build the track covariance matrix using the smearing sigmas
  double sigmaU = 10000_um;
  double sigmaV = 10000_um;
  double sigmaPhi = 20_degree;
  double sigmaTheta = 10_degree;
  double sigmaQOverP = 10.00*p / (p*p);
  //double sigmaQOverP = 0.02*p / (p*p);
  double sigmaT0 = 1_ns;
  Acts::BoundSquareMatrix cov = Acts::BoundSquareMatrix::Zero();
  cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = sigmaU * sigmaU;
  cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = sigmaV * sigmaV;
  cov(Acts::eBoundPhi, Acts::eBoundPhi) = sigmaPhi * sigmaPhi;
  cov(Acts::eBoundTheta, Acts::eBoundTheta) = sigmaTheta * sigmaTheta;
  cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = sigmaQOverP * sigmaQOverP;
  cov(Acts::eBoundTime, Acts::eBoundTime) = sigmaT0 * sigmaT0;
  double time =0;

  Acts::BoundVector params = Acts::BoundVector::Zero();
  params[Acts::eBoundLoc0] = 0;
  params[Acts::eBoundLoc1] = 0;
  params[Acts::eBoundTime] = time;
  params[Acts::eBoundPhi] = phi;
  params[Acts::eBoundTheta] = theta;
  //params[Acts::eBoundQOverP] = particleHypothesis.qOverP(p, charge);
  params[Acts::eBoundQOverP] = charge/p;

  //@todo: use the first plane as the reference surface
  auto perigee = Acts::Surface::makeShared<const Acts::PerigeeSurface>(pos1);

 //@todo make the particle hypothesis configurable
  return  Acts::BoundTrackParameters(perigee, params, cov, Acts::ParticleHypothesis::muon());

}

Acts::BoundTrackParameters SPSimpleInitialParameterTool::getInitialParameters_2stations(Acts::Vector3 pos1, Acts::Vector3 pos2, Acts::Vector3 dir1) const {

  Acts::Vector3 dir2=(pos2-pos1).normalized();
  dir1*=1./sqrt(dir1.y()*dir1.y()+dir1.z()*dir1.z());
  dir2*=1./sqrt(dir2.y()*dir2.y()+dir2.z()*dir2.z());
  Acts::Vector3 dir1p2=(dir1+dir2)/2.;
  Acts::Vector3 dir1m2=(dir1-dir2)/2.;
  double charge = dir2.y()>dir1.y()?1:-1;
  double B = 0.55;
  double st=2.*sqrt(dir1p2.y()*dir1p2.y()+dir1p2.z()*dir1p2.z())*sqrt(dir1m2.y()*dir1m2.y()+dir1m2.z()*dir1m2.z());
  dir2=(pos2-pos1)/2.;
  double R=sqrt(dir2.y()*dir2.y()+dir2.z()*dir2.z())/st;

  double p_yz = 0.3*B*R / 1000.0;  // R(mm), p(GeV), B(T)
  double p_z=p_yz*dir1.z()/sqrt(dir1.y()*dir1.y()+dir1.z()*dir1.z());
  double p_y=p_z*dir1.y()/dir1.z();
  double p_x=p_z*dir1.x()/dir1.z();

  const Acts::Vector3 mom(p_x, p_y, p_z);
  double p = mom.norm();
  double theta = Acts::VectorHelpers::theta(mom.normalized());
  double phi = Acts::VectorHelpers::phi(mom.normalized());
  msg(MSG::DEBUG)<<"!!!!!!!!!!!  InitTrack momentum on layer 0: ( "<<mom.x()*1000<<",  "<<mom.y()*1000<<",  "<<mom.z()*1000<<",  "<<p*1000<<")  "<<st<<" "<<charge<<endmsg;
  // build the track covariance matrix using the smearing sigmas
  double sigmaU = 1000_um;
  double sigmaV = 1000_um;
  double sigmaPhi = 10_degree;
  double sigmaTheta = 2_degree;
  double sigmaQOverP = 0.20*p / (p*p);
  //double sigmaQOverP = 0.02*p / (p*p);
  double sigmaT0 = 1_ns;
  Acts::BoundSquareMatrix cov = Acts::BoundSquareMatrix::Zero();
  cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = sigmaU * sigmaU;
  cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = sigmaV * sigmaV;
  cov(Acts::eBoundPhi, Acts::eBoundPhi) = sigmaPhi * sigmaPhi;
  cov(Acts::eBoundTheta, Acts::eBoundTheta) = sigmaTheta * sigmaTheta;
  cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = sigmaQOverP * sigmaQOverP;
  cov(Acts::eBoundTime, Acts::eBoundTime) = sigmaT0 * sigmaT0;
  double time =0;

  Acts::BoundVector params = Acts::BoundVector::Zero();
  params[Acts::eBoundLoc0] = 0;
  params[Acts::eBoundLoc1] = 0;
  params[Acts::eBoundTime] = time;
  params[Acts::eBoundPhi] = phi;
  params[Acts::eBoundTheta] = theta;
  //params[Acts::eBoundQOverP] = particleHypothesis.qOverP(p, charge);
  params[Acts::eBoundQOverP] = charge/p;

  //@todo: use the first plane as the reference surface
  auto perigee = Acts::Surface::makeShared<const Acts::PerigeeSurface>(pos1);

  //@todo make the particle hypothesis configurable
  return Acts::BoundTrackParameters(perigee, params, cov, Acts::ParticleHypothesis::muon()); 
}

std::vector<Acts::BoundTrackParameters> SPSimpleInitialParameterTool::getInitialParameters_1station(std::vector<Acts::Vector3> pos1, std::vector<Acts::Vector3> pos2, std::vector<Acts::Vector3> pos3) const {
  std::vector<Acts::BoundTrackParameters> InitTrackParams;
  InitTrackParams.clear();

  for(std::size_t i1=0;i1<pos1.size();i1++){
    for(std::size_t i2=0;i2<pos2.size();i2++){
      for(std::size_t i3=0;i3<pos3.size();i3++){
	if(fabs(pos3[i3].y()-pos2[i2].y())>1.5*fabs(pos2[i2].y()-pos1[i1].y()))continue;
	if(fabs(pos3[i3].x()-pos2[i2].x())>1.5*fabs(pos2[i2].x()-pos1[i1].x()))continue;
	Acts::Vector3 pstart=pos1[i1];
	Acts::Vector3 pend=(pos2[i2]+pos3[i3])/2.;
	Acts::Vector3 dir=pend-pstart;

	double charge = 1;
	//  double B = 0.55;

	double p_yz = 100.;
	//double p_yz = 0.3*B*R / 1000.0;  // R(mm), p(GeV), B(T)
	double p_z=p_yz*dir.y()/sqrt(dir.y()*dir.y()+dir.z()*dir.z());
	if(p_z<0)continue;
	double p_y=p_z*dir.y()/dir.z();
	double p_x=p_z*dir.x()/dir.z();


	const Acts::Vector3 mom(p_x, p_y, p_z);
	double p = mom.norm();
        double theta = Acts::VectorHelpers::theta(mom.normalized());
        double phi = Acts::VectorHelpers::phi(mom.normalized());
	msg(MSG::DEBUG)<<"!!!!!!!!!!!  InitTrack momentum on layer 0: ( "<<mom.x()*1000<<",  "<<mom.y()*1000<<",  "<<mom.z()*1000<<",  "<<p*1000<<")  "<<endmsg;
	// build the track covariance matrix using the smearing sigmas
	double sigmaU = 2000_um;
	double sigmaV = 2000_um;
	double sigmaPhi = 30_degree;
	double sigmaTheta = 10_degree;
	double sigmaQOverP = 1.20*p / (p*p);
	//double sigmaQOverP = 0.02*p / (p*p);
	double sigmaT0 = 1_ns;
	Acts::BoundSquareMatrix cov = Acts::BoundSquareMatrix::Zero();
	cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = sigmaU * sigmaU;
	cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = sigmaV * sigmaV;
	cov(Acts::eBoundPhi, Acts::eBoundPhi) = sigmaPhi * sigmaPhi;
	cov(Acts::eBoundTheta, Acts::eBoundTheta) = sigmaTheta * sigmaTheta;
	cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = sigmaQOverP * sigmaQOverP;
	cov(Acts::eBoundTime, Acts::eBoundTime) = sigmaT0 * sigmaT0;
	double time =0;
	//const Acts::Vector4 postime(pstart.x(), pstart.y(), pstart.z(),time);

        Acts::BoundVector params = Acts::BoundVector::Zero();
        params[Acts::eBoundLoc0] = 0;
        params[Acts::eBoundLoc1] = 0;
        params[Acts::eBoundTime] = time;
        params[Acts::eBoundPhi] = phi;
        params[Acts::eBoundTheta] = theta;
        //params[Acts::eBoundQOverP] = particleHypothesis.qOverP(p, charge);
        params[Acts::eBoundQOverP] = charge/p;
       
        //@todo: use the first plane as the reference surface
        //@todo make the particle hypothesis configurable
        auto perigee = Acts::Surface::makeShared<const Acts::PerigeeSurface>(pstart);

	InitTrackParams.emplace_back(perigee, params, cov, Acts::ParticleHypothesis::muon());
      }
    }
  }

  return InitTrackParams;
}
