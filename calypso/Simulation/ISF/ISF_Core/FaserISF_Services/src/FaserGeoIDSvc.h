/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// FaserGeoIDSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FASERSERVICES_FASERGEOIDSVC_H
#define ISF_FASERSERVICES_FASERGEOIDSVC_H 1

// framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// STL includes
#include <vector>
#include <list>
#include <set>

// DetectorDescription
#include "FaserDetDescr/FaserRegion.h"

// ISF includes
#include "FaserISF_Interfaces/IFaserGeoIDSvc.h"

// Geant4
class G4Navigator;

namespace ISF {

  /** @class GeoIDSvc
  
      A fast Athena service identifying the FaserRegion a given position/particle is in.

      @author Elmar.Ritsch -at- cern.ch

  
     */
  class FaserGeoIDSvc : public extends<AthService, ISF::IFaserGeoIDSvc> {
    public: 
     /** Constructor with parameters */
     FaserGeoIDSvc(const std::string& name,ISvcLocator* svc);

     /** Destructor */
     ~FaserGeoIDSvc();

     // Athena algtool's Hooks
     StatusCode  initialize();
     StatusCode  finalize();

     /** A static filter that returns the SimGeoID of the given position */
     FaserDetDescr::FaserRegion  identifyGeoID(const Amg::Vector3D &pos) const;

     /** Checks if the given position (or ISFParticle) is inside a given SimGeoID */
     ISF::InsideType inside(const Amg::Vector3D &pos, FaserDetDescr::FaserRegion geoID) const;

     /** Find the SimGeoID that the particle will enter with its next infinitesimal step
         along the given direction */
     FaserDetDescr::FaserRegion identifyNextGeoID(const Amg::Vector3D &pos, const Amg::Vector3D &dir) const;

    private:
      Gaudi::Property<double>     m_tolerance { this, "Tolerance", 1.0e-5, "Tolerance for being on surface" };
      G4Navigator*                m_navigator;
   }; 
  
} // ISF namespace

#endif //> !ISF_FASERSERVICES_FASERGEOIDSVC_H
