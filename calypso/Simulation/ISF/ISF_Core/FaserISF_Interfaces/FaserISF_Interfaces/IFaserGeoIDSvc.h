/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IFaserGeoIDSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_IFASERGEOIDSVC_H
#define ISF_INTERFACES_IFASERGEOIDSVC_H 1

// Gaudi
#include "GaudiKernel/IInterface.h"

// Amg
#include "GeoPrimitives/GeoPrimitives.h"

// DetectorDescription
#include "FaserDetDescr/FaserRegion.h"

// ISF Simulation includes
#include "FaserISF_Event/FaserISFParticle.h" 
 
namespace ISF {

  enum InsideType {
    fOutside        = 0,
    fSurface        = 1,
    fInside         = 2,
   // to be used to allocate arrays
    fNumInsideTypes = 3
  };

  /**
   @class IFaserGeoIDSvc

   The interface to chose between the sub geometry services,
   realized as an AlgTool since it does not have to be dynamically created
   
   The IFaserGeoIDSvc registers the nextGeoSvcID to the ISFParticle.
       
   @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
   */
     
  class IFaserGeoIDSvc : virtual public IInterface {
     public:
     
       /** Virtual destructor */
       virtual ~IFaserGeoIDSvc(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(IFaserGeoIDSvc, 1, 0);

       /** Athena algtool's Hooks */
       virtual StatusCode  initialize() = 0;
       virtual StatusCode  finalize() = 0;

       /** Checks if the given position (ISFParticle) is inside/outside/onsurface a given FaserRegion */
       virtual ISF::InsideType inside(const Amg::Vector3D &pos, FaserDetDescr::FaserRegion geoID) const = 0;
       inline  ISF::InsideType inside(const FaserISFParticle& sp, FaserDetDescr::FaserRegion geoID) const;
       inline  ISF::InsideType inside(double x, double y, double z, FaserDetDescr::FaserRegion geoID) const;

       /** A static filter that returns the FaserRegion of the given ISFParticle (position)
            -> returns ISF::fUndefinedGeoID if particle is on surface */
       virtual FaserDetDescr::FaserRegion identifyGeoID(const Amg::Vector3D &pos) const = 0;
       inline  FaserDetDescr::FaserRegion identifyGeoID(const FaserISFParticle& sp) const;
       inline  FaserDetDescr::FaserRegion identifyGeoID(double x, double y, double z) const;

       /** Find the FaserRegion that the particle will enter with its next infinitesimal step
           along the given direction */
       virtual FaserDetDescr::FaserRegion identifyNextGeoID(const Amg::Vector3D &pos, const Amg::Vector3D &dir) const = 0;
       inline  FaserDetDescr::FaserRegion identifyNextGeoID(const FaserISFParticle& sp) const;
       inline  FaserDetDescr::FaserRegion identifyNextGeoID(double x, double y, double z,
                                               double dx, double dy, double dz) const;

       /** Find the FaserRegion that the particle will enter with its next infinitesimal step
           along the given direction, and register this geoID to the particle */
       inline  FaserDetDescr::FaserRegion identifyAndRegNextGeoID(FaserISFParticle& sp) const;
  };


  // inline methods (basically wrapper for ISFParticle/coord. -> HepGeom::Point3D<double>)
  //
  // inside() wrappers
  inline ISF::InsideType IFaserGeoIDSvc::inside(const FaserISFParticle& sp, FaserDetDescr::FaserRegion geoID) const {
    return inside( sp.position(), geoID);
  }
  inline ISF::InsideType IFaserGeoIDSvc::inside(double x, double y, double z, FaserDetDescr::FaserRegion geoID) const {
    const Amg::Vector3D pos(x, y, z);
    return inside( pos, geoID);
  }

  // identifyGeoID() wrappers
  inline FaserDetDescr::FaserRegion IFaserGeoIDSvc::identifyGeoID(const FaserISFParticle &sp) const { 
    return identifyGeoID( sp.position());
  }
  inline FaserDetDescr::FaserRegion IFaserGeoIDSvc::identifyGeoID(double x, double y, double z) const { 
    const Amg::Vector3D pos(x, y, z);
    return identifyGeoID( pos);
  }

  // identifyNextGeoID() wrappers
  inline FaserDetDescr::FaserRegion IFaserGeoIDSvc::identifyNextGeoID(const FaserISFParticle& sp) const {
    return identifyNextGeoID( sp.position(), sp.momentum());
  }
  inline FaserDetDescr::FaserRegion IFaserGeoIDSvc::identifyNextGeoID(double x, double y, double z,
                                                    double dx, double dy, double dz) const {
    const Amg::Vector3D pos(x, y, z);
    const Amg::Vector3D dir(dx, dy, dz);
    return identifyNextGeoID( pos, dir);
  }

  // identifyAndRegNextGeoID() wrapper
  inline  FaserDetDescr::FaserRegion IFaserGeoIDSvc::identifyAndRegNextGeoID( FaserISFParticle& part) const {
    FaserDetDescr::FaserRegion geoID = identifyNextGeoID( part.position(), part.momentum());
    part.setNextGeoID(geoID);
    return geoID;
  }

} // end of namespace

#endif // ISF_INTERFACES_IFASERGEOIDSVC_H 
