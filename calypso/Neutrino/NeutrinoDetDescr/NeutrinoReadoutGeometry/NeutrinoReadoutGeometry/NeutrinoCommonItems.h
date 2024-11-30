/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutrinoCommonItems.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef NEUTRINOREADOUTGEOMETRY_NEUTRINOCOMMONITEMS_H
#define NEUTRINOREADOUTGEOMETRY_NEUTRINOCOMMONITEMS_H

class FaserDetectorID;

// Message Stream Member
#include "AthenaBaseComps/AthMessaging.h"
#include "CxxUtils/checker_macros.h"
// #include "InDetCondTools/ISiLorentzAngleTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/RCBase.h"


#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Geometry/Transform3D.h"

#include <mutex>

namespace NeutrinoDD {

    /** @class NeutrinoCommonItems
    
        Helper class to concentrate common items, such as the pointer to the IdHelper,
        
        To be used for Emulsion
        
        @author: Grant Gorfine
        mondified & maintained: Nick Styles, Andreas Salzburger
        modified: Dave Casper
        */

    class NeutrinoCommonItems: public RCBase, public AthMessaging
    {
    
        public:
        
          NeutrinoCommonItems(const FaserDetectorID* const idHelper);
          
          const FaserDetectorID* getIdHelper() const;
        //   const HepGeom::Transform3D & solenoidFrame() const;
        //   void setSolenoidFrame(const HepGeom::Transform3D & transform) const; 

        private:
        
          const FaserDetectorID* m_idHelper; 
        //   mutable HepGeom::Transform3D m_solenoidFrame ATLAS_THREAD_SAFE; // Guarded by m_mutex

          mutable std::mutex m_mutex;
    };
    
    
    inline const FaserDetectorID* NeutrinoCommonItems::getIdHelper() const
    {
      return m_idHelper;
    }
    
    
    // inline const HepGeom::Transform3D & SiCommonItems::solenoidFrame() const
    // {
    //   std::lock_guard<std::mutex> lock{m_mutex};
    //   return m_solenoidFrame;
    //   // This reference might be changed by setSolenoidFrame.
    //   // However, it occurrs very rarely.
    // }
    
    

} // End namespace ScintDD

#endif // NEUTRINOREADOUTGEOMETRY_NEUTRINOCOMMONITEMSS_H
