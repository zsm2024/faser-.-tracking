/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

///////////////////////////////////////////////////////////////////
// SCT_SpacePoint.h
//   Header file for class SCT_SpacePoint
///////////////////////////////////////////////////////////////////
// Class to handle SPs for SCT
///////////////////////////////////////////////////////////////////
// Version 1.0   12/12/2005 Martin Siebel
///////////////////////////////////////////////////////////////////
#ifndef TRACKERSPACEPOINT_SCTSPACEPOINT_H
#define TRACKERSPACEPOINT_SCTSPACEPOINT_H

#include "AthLinks/ElementLink.h"
#include "Identifier/IdentifierHash.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"

class FaserSCT_SpacePointContainerCnv;
class FaserSCT_SpacePointContainerCnv_p0;
class FaserSCT_SpacePointCnv_p0;

namespace Tracker
{
  class TrackerCluster;
}

namespace Trk{
  class LocalParameters;
  class ErrorMatrix;
  class Surface;
}

namespace Tracker
{
  /**
   * @class SCT_SpacePoint
   * An SCT_SpacePoint is created from two SCT_Cluster's from two different wafers.
   */

  class FaserSCT_SpacePoint 
  {
    friend class FaserSCT_SpacePointContainerCnv;
    friend class FaserSCT_SpacePointContainerCnv_p0;
    friend class FaserSCT_SpacePointCnv_p0;

    /////////////////////////////`//////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

    public:

    /** Default constructor */
    FaserSCT_SpacePoint() ;

    /**
     * @name Parametrised constructors
     * In order to ensure initialisation, the global Position has to be
     * on the surface associated to the FIRST member of the PRD-pair clusList.
     */
    //@{
    FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList, 
	const Amg::Vector3D* position, 
	const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>* clusList) ;

    FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList, 
	const Amg::Vector3D* position,
	const Amg::MatrixX* loccov,//assumes ownership of loccov
	const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>* clusList) ;

    FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList, 
	const Amg::Vector3D& position, 
	const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>* clusList) ;
    FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList, 
	const Amg::Vector3D* position, 
	Amg::MatrixX* globallcov,
	const Amg::Vector2D* localpos,
	Amg::MatrixX* localcov,
	const ElementLink<Tracker::FaserSCT_ClusterContainer>& link1, const ElementLink<Tracker::FaserSCT_ClusterContainer>& link2) ;

    FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList, 
	const Amg::Vector3D& position,
	const Amg::MatrixX& loccov,//assumes ownership of loccov
	const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>* clusList) ;
    FaserSCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList, 
	const Amg::Vector3D* position, 
	const ElementLink<Tracker::FaserSCT_ClusterContainer>& link1, const ElementLink<Tracker::FaserSCT_ClusterContainer>& link2) ;
    //@}

    /** Copy Constructor */
    //      FaserSCT_SpacePoint(const FaserSCT_SpacePoint &) ;

    /** Destructor */
    ~FaserSCT_SpacePoint() = default;

    /** Overloading Assignment Operator */
    //      FaserSCT_SpacePoint &operator=(const FaserSCT_SpacePoint &);

    /** Clones */
    FaserSCT_SpacePoint* clone() const ;       
    /** return the pair of Ids of the element by reference*/
    const std::pair<IdentifierHash, IdentifierHash>& elementIdList() const;

    /** return the pair of cluster pointers by reference */
    const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>& clusterList() const;

    /** return the error matrix by reference 
      The Matrix is calculated from the local Covariance Matrix when demanded and the cached */
    const Amg::MatrixX& globCovariance() const;

    /** calculate eta (not cached), needs z0 info */
    double eta(double z0=0) const;

    /**returns the r value of the SpacePoint's position (in cylindrical coordinates). 
      This is calculated once in the ctor and then cached*/
    double r() const ;

    /**returns the phi value of the SpacePoint's position (in cylindrical coordinates).
      0070        This is calculated once in the ctor and then cached*/
    double phi() const ;


    /**Interface method for output, to be overloaded by child classes* */
    MsgStream&    dump( MsgStream& out ) const ;  

    /**Interface method for output, to be overloaded by child classes* */
    std::ostream& dump( std::ostream& out ) const ;

    /**Interface method to get the LocalParameters*/
    const Trk::LocalParameters& localParameters() const;

    /**Interface method to get the localError*/
    const Amg::MatrixX& localCovariance() const;

    /**Interface method to get the associated Surface*/
    const Trk::Surface& associatedSurface() const ;

    /**Interface method to get the global Position*/
    const Amg::Vector3D& globalPosition() const ;

    // Here to help implement the relevant semantics for 
    // the derived classes
    FaserSCT_SpacePoint(const FaserSCT_SpacePoint&) = default;
    FaserSCT_SpacePoint(FaserSCT_SpacePoint&&) noexcept = default;
    FaserSCT_SpacePoint& operator=(const FaserSCT_SpacePoint&);
    FaserSCT_SpacePoint& operator=(FaserSCT_SpacePoint&&) noexcept = default;

    //Element link to FaserSCT_Cluster
    ElementLink<Tracker::FaserSCT_ClusterContainer>* getElementLink1(){return (&m_elelink1);}
    ElementLink<Tracker::FaserSCT_ClusterContainer>* getElementLink2(){return (&m_elelink2);}
    const ElementLink<Tracker::FaserSCT_ClusterContainer>* getElementLink1()const {return (&m_elelink1);}
    const ElementLink<Tracker::FaserSCT_ClusterContainer>* getElementLink2()const {return (&m_elelink2);}
    const Tracker::FaserSCT_Cluster* cluster1() const{if(m_elelink1.isValid())return m_elelink1.cachedElement();else return 0;}
    const Tracker::FaserSCT_Cluster* cluster2() const{if(m_elelink2.isValid())return m_elelink2.cachedElement();else return 0;}

    void setLocalParameters(Amg::Vector2D& loc){m_localParams = Trk::LocalParameters(loc);};
    void setGlobalParameters(Amg::Vector3D& glo){m_position=glo;};
    void setGlobalCovariance(Amg::MatrixX& glo){m_globalCovariance=glo;};
    void setLocalCovariance(Amg::MatrixX& loc){m_localCovariance=loc;};
    void setClusList(std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*> list){m_clusList=list; m_elelink1.setElement(list.first);
      m_elelink2.setElement(list.second);
    };
    void setElemIdList(std::pair<IdentifierHash, IdentifierHash> list){m_elemIdList=list;};

    protected:

    Trk::LocalParameters m_localParams;
    Amg::MatrixX m_localCovariance;
    std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*> m_clusList;
    std::pair<IdentifierHash, IdentifierHash> m_elemIdList;
    Amg::Vector3D m_position; 
    Amg::MatrixX  m_globalCovariance;

    private:


    ElementLink<Tracker::FaserSCT_ClusterContainer> m_elelink1, m_elelink2;
    void setupGlobalFromLocalCovariance();
    /** method to set up the local Covariance Matrix. */
    void setupLocalCovarianceSCT() ;

    /** common method used in constructors. */
    void setup(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
	const Amg::Vector3D& position,
	const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>* clusList);
  };

  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////

  inline FaserSCT_SpacePoint* FaserSCT_SpacePoint::clone() const
  {  return new FaserSCT_SpacePoint(*this) ;  }

  inline const Trk::LocalParameters&
    FaserSCT_SpacePoint::localParameters() const
    {
      return m_localParams;
    }

  inline const Amg::MatrixX&
    FaserSCT_SpacePoint::localCovariance() const
    {
      return m_localCovariance;
    }

  /**Overload of << operator for MsgStream for debug output*/
  inline MsgStream&
    operator<<(MsgStream& sl, const FaserSCT_SpacePoint& mbase)
    {
      return mbase.dump(sl);
    }

  /**Overload of << operator for std::ostream for debug output*/
  inline std::ostream&
    operator<<(std::ostream& sl, const FaserSCT_SpacePoint& mbase)
    {
      return mbase.dump(sl);
    }
  inline const std::pair<IdentifierHash, IdentifierHash>& FaserSCT_SpacePoint::elementIdList() const
  { return m_elemIdList; }


  inline const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*>& FaserSCT_SpacePoint::clusterList() const
  {
    return m_clusList;
  }

  inline double FaserSCT_SpacePoint::eta(double z0) const
  {
    //  double zr = (m_z-z0)/m_r; 
    double zr = (m_position.z() - z0)/std::hypot(m_position.x(), m_position.y());
    return std::log(zr+std::sqrt(1.+zr*zr));
  }

  inline double FaserSCT_SpacePoint::r() const
  { return std::hypot(m_position.x(), m_position.y()); }

  inline double FaserSCT_SpacePoint::phi() const 
  {  return std::atan2(m_position[1], m_position[0]); }

  // The methods required by MeasurementBase
  inline const Amg::Vector3D& FaserSCT_SpacePoint::globalPosition() const
  {
    return m_position;
  }
  inline const Amg::MatrixX& FaserSCT_SpacePoint::globCovariance() const {
    return  m_globalCovariance;
  }

}
#endif // TRACKERSPACEPOINT_SCTSPACEPOINT_H
