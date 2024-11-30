/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// 
// Implementation for ClusterMaker
//
//****************************************************************************

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrackerClusterMakerTool.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerReadoutGeometry/SiLocalPosition.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerPrepRawData/FaserSiWidth.h"

#include "EventPrimitives/EventPrimitives.h"

using CLHEP::micrometer;

namespace {
	inline double square(const double x){
		return x*x;
	}
	constexpr double ONE_TWELFTH = 1./12.;
}

namespace Tracker
{

// Constructor with parameters:
TrackerClusterMakerTool::TrackerClusterMakerTool(const std::string& t,
                                   const std::string& n,
                                   const IInterface* p) :
  AthAlgTool(t,n,p)
{ 
  declareInterface<TrackerClusterMakerTool>(this);
}

//================ Initialisation =============================================

StatusCode  TrackerClusterMakerTool::initialize(){
  // Code entered here will be executed once at program start.

   ATH_MSG_INFO ( name() << " initialize()" );

   if (not m_sctLorentzAngleTool.empty()) {
     ATH_CHECK(m_sctLorentzAngleTool.retrieve());
   } else {
     m_sctLorentzAngleTool.disable();
   }

//   ATH_CHECK(m_clusterErrorKey.initialize(SG::AllowEmpty));

   return StatusCode::SUCCESS;
}


// Computes global position and errors for SCT cluster.
  // Called by SCT Clustering tools
  // 
  // Input parameters
  // - the cluster Identifier 
  // - the position in local reference frame 
  // - the list of identifiers of the Raw Data Objects belonging to the cluster
  // - the width of the cluster
  // - the module the cluster belongs to  
  // - the error strategy, currently
  //    0: Cluster Width/sqrt(12.)
  //    1: Set to a different values for one and two-strip clusters (def.)

Tracker::FaserSCT_Cluster* TrackerClusterMakerTool::sctCluster(
                         const Identifier& clusterID,
			 const Amg::Vector2D& localPos,
                         const std::vector<Identifier>& rdoList,
                         const Tracker::FaserSiWidth& width,
                         const TrackerDD::SiDetectorElement* element,
                         int errorStrategy) const{

        // double shift = m_sctLorentzAngleTool->getLorentzShift(element->identifyHash());
		double shift = 0;
//        const InDetDD::SiLocalPosition& localPosition = 
//                        InDetDD::SiLocalPosition(localPos[Trk::locY),
//                                        localPos[Trk::locX)+shift,0);
        Amg::Vector2D locpos(localPos[Trk::locX]+shift, localPos[Trk::locY]);

	// error matrix
	const Amg::Vector2D& colRow = width.colRow();// made ref to avoid 
	// unnecessary copy EJWM

	auto errorMatrix = Amg::MatrixX(2,2);
	errorMatrix.setIdentity();

	// switches are more readable **OPT**
	// actually they're slower as well (so I'm told) so perhaps
	// this should be re-written at some point EJWM
    
	switch (errorStrategy){
	case 0:
	  errorMatrix.fillSymmetric(0,0,square(width.phiR())*ONE_TWELFTH);
	  errorMatrix.fillSymmetric(1,1,square(width.z())*ONE_TWELFTH);
	  break;
	case 1:
	  // mat(1,1) = pow(width.phiR()/colRow.x(),2)/12;
	  // single strip - resolution close to pitch/sqrt(12)
	  // two-strip hits: better resolution, approx. 40% lower
	  if(colRow.x() == 1){
	    errorMatrix.fillSymmetric(0,0,square(1.05*width.phiR())*ONE_TWELFTH);
	  }
	  else if(colRow.x() == 2){
	    errorMatrix.fillSymmetric(0,0,square(0.27*width.phiR())*ONE_TWELFTH);
	  }
	  else{
	    errorMatrix.fillSymmetric(0,0,square(width.phiR())*ONE_TWELFTH);
	  }
	  errorMatrix.fillSymmetric(1,1,square(width.z()/colRow.y())*ONE_TWELFTH);
	  break;
	default:
	  // single strip - resolution close to pitch/sqrt(12)
	  // two-strip hits: better resolution, approx. 40% lower
	  if(colRow.x() == 1){
	    errorMatrix.fillSymmetric(0,0,square(width.phiR())*ONE_TWELFTH);
	  }
	  else if(colRow.x() == 2){
	    errorMatrix.fillSymmetric(0,0,square(0.27*width.phiR())*ONE_TWELFTH);
	  }
	  else{
	    errorMatrix.fillSymmetric(0,0,square(width.phiR())*ONE_TWELFTH);
	  }
	  errorMatrix.fillSymmetric(1,1,square(width.z()/colRow.y())*ONE_TWELFTH);
	  break;
	}

	//        delete localPos;
	//	localPos=0;
	Tracker::FaserSCT_Cluster* newCluster = new Tracker::FaserSCT_Cluster(clusterID, locpos, rdoList , width, element, errorMatrix);
	return newCluster;

}

}