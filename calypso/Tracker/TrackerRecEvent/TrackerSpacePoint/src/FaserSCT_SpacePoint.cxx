/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkPrepRawData/PrepRawData.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

namespace Tracker
{
  //-------------------------------------------------------------

  FaserSCT_SpacePoint::FaserSCT_SpacePoint()
  {}

  //-------------------------------------------------------------

  FaserSCT_SpacePoint::FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
      const Amg::Vector3D* position,
      Amg::MatrixX* globalcov,
      const Amg::Vector2D* localpos,
      Amg::MatrixX* localcov,
      const ElementLink<Tracker::FaserSCT_ClusterContainer>& link1, const ElementLink<Tracker::FaserSCT_ClusterContainer>& link2): m_elelink1(link1),m_elelink2(link2)
  {
    m_elemIdList.first = elementIdList.first ;
    m_elemIdList.second = elementIdList.second ;
    const FaserSCT_Cluster* clus1=link1.cachedElement();
    const FaserSCT_Cluster* clus2=link2.cachedElement();
    std::cout<<*clus1<<std::endl;;
    std::cout<<clus1->identify()<<" "<<std::endl;
    m_clusList.first = clus1;
    m_clusList.second = clus2;
    m_position = *position ;
    m_globalCovariance = *globalcov;
    m_localParams = Trk::LocalParameters(*localpos) ;
    m_localCovariance = *localcov;
  }
  //-------------------------------------------------------------

  FaserSCT_SpacePoint::FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
				 const Amg::Vector3D* position,
				 const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>* clusList) 
  {
    setup(elementIdList,*position,clusList);
    setupLocalCovarianceSCT();
    setupGlobalFromLocalCovariance();
    m_elelink1.setElement(clusList->first);
    m_elelink2.setElement(clusList->second);
//    delete position;
  }

  //-------------------------------------------------------------

  FaserSCT_SpacePoint::FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
				 const Amg::Vector3D* position,
		     const ElementLink<Tracker::FaserSCT_ClusterContainer>& link1, const ElementLink<Tracker::FaserSCT_ClusterContainer>& link2): m_elelink1(link1),m_elelink2(link2)
  {
    const FaserSCT_Cluster* clus1=link1.cachedElement();
    const FaserSCT_Cluster* clus2=link2.cachedElement();
    const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*> clusList(clus1,clus2);
    setup(elementIdList,*position,&clusList);
    setupLocalCovarianceSCT();
    setupGlobalFromLocalCovariance();
    delete position;
  }

  //-------------------------------------------------------------

  FaserSCT_SpacePoint::FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
				 const Amg::Vector3D* position,
				 const Amg::MatrixX* loccov,
				 const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>* clusList) 
  {
    m_localCovariance = *loccov;
    setup(elementIdList,*position,clusList);
    setupGlobalFromLocalCovariance();
    m_elelink1.setElement(clusList->first);
    m_elelink2.setElement(clusList->second);
    delete loccov;
    delete position;
  }

  FaserSCT_SpacePoint::FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
				 const Amg::Vector3D& position,
				 const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>* clusList) 
  {
    setup(elementIdList,position,clusList);
    setupLocalCovarianceSCT();
    setupGlobalFromLocalCovariance();
    m_elelink1.setElement(clusList->first);
    m_elelink2.setElement(clusList->second);
  }

  //-------------------------------------------------------------

  FaserSCT_SpacePoint::FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
				 const Amg::Vector3D& position,
				 const Amg::MatrixX& loccov,
				 const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>* clusList) 
  {
    m_localCovariance = loccov;
    setup(elementIdList,position,clusList);
    setupGlobalFromLocalCovariance();
    m_elelink1.setElement(clusList->first);
    m_elelink2.setElement(clusList->second);
  }

  
  //-------------------------------------------------------------
  
  void FaserSCT_SpacePoint::setup(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
			     const Amg::Vector3D& position,
			     const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>* clusList)
  {
    m_clusList.first = clusList->first;
    m_clusList.second = clusList->second;
    m_position = position ;
    m_elemIdList.first = elementIdList.first ;
    m_elemIdList.second = elementIdList.second ;
    assert( (clusList->first!=0) && (clusList->second!=0) );
    assert(clusList->first->detectorElement()) ;
    std::optional<Amg::Vector2D> locpos{
     clusList->first->detectorElement()->surface().globalToLocal(position)};
    assert(locpos);
    m_localParams = Trk::LocalParameters(*locpos ) ;

  }

  //-------------------------------------------------------------

  //---------------------------------------------------------------

  void FaserSCT_SpacePoint::setupLocalCovarianceSCT()
  {

    /* For performance reason only, the error is assumed constant.
	numbers are taken from 
	Trigger/TrigTools/TrigOfflineSpacePointTool/OfflineSpacePointProviderTool */
    
    double deltaY = 0.0004;  // roughly pitch of SCT (80 mu) / sqrt(12)

    Amg::MatrixX cov(2,2);
    cov<<
      1.*deltaY,   25.*deltaY,
      25.*deltaY, 1600.*deltaY;
    
    m_localCovariance = cov;
  }
  
//-------------------------------------------------------------

MsgStream&    FaserSCT_SpacePoint::dump( MsgStream& out ) const
{
  out << "FaserSCT_SpacePoint  contains: " << std::endl;
  out << "Identifier Hashes ( " << int(this->elementIdList().first) << " , " ;
  out <<  int(this->elementIdList().second) << " ) " << std::endl ;
  out << "Global Position:  " << Amg::toString(this->globalPosition(),3) << std::endl;
  out << "Global Covariance Matrix " <<  Amg::toString(this->globCovariance(),3) << std::endl;
  out << "Local Parameters " << this->localParameters() << std::endl;
  out << "Local Covariance " << Amg::toString(this->localCovariance()) << std::endl; 
  out << "Cluster 1 :" << std::endl << (*this->clusterList().first) << std::endl;
  out << "Cluster 2 :" << std::endl << (*this->clusterList().second) << std::endl;

  return out;
}

//-------------------------------------------------------------


std::ostream& FaserSCT_SpacePoint::dump( std::ostream& out ) const
{
  out << "FaserSCT_SpacePoint  contains: " << std::endl;
  out << "Identifier Hashes ( " << int(this->elementIdList().first) << " , " ;
  out <<  int(this->elementIdList().second) << " ) " << std::endl ;
  out << "Global Position:  " << Amg::toString(this->globalPosition(),3) << std::endl;
  out << "Global Covariance Matrix " <<  Amg::toString(this->globCovariance(),3) << std::endl;
  out << "Local Parameters " << this->localParameters() << std::endl;
  out << "Local Covariance " << Amg::toString(this->localCovariance()) << std::endl; 
  out << "Cluster 1 :" << std::endl << (*this->clusterList().first) << std::endl;
  out << "Cluster 2 :" << std::endl << (*this->clusterList().second) << std::endl;
 
  return out;
}

// ------------------------------------------------------------------

//assignment operator
FaserSCT_SpacePoint& FaserSCT_SpacePoint::operator=(const FaserSCT_SpacePoint& o)
{
  if (&o !=this) 
  {
    FaserSCT_SpacePoint::operator=(o);
  }
  return *this;
}

/** set up the global covariance matrix by rotating the local one */

void FaserSCT_SpacePoint::setupGlobalFromLocalCovariance()
{
  const Amg::MatrixX& lc = this->localCovariance();

  Amg::MatrixX cov(3,3);

  cov<<
    lc(0,0),lc(0,1),0.,
    lc(1,0),lc(1,1),0.,
    // cppcheck-suppress constStatement
    0.     ,0.     ,0.;

  const Amg::RotationMatrix3D& R = associatedSurface().transform().rotation();
  m_globalCovariance = R*cov*R.transpose();

}

const Trk::Surface& FaserSCT_SpacePoint::associatedSurface() const
{ 
  assert(m_clusList.first->detectorElement());
  return m_clusList.first->detectorElement()->surface();
}

}
