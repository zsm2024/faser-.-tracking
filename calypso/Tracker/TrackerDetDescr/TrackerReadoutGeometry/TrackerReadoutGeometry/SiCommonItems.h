/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiCommonItems.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRACKERREADOUTGEOMETRY_SICOMMONITEMS_H
#define TRACKERREADOUTGEOMETRY_SICOMMONITEMS_H

class FaserDetectorID;

// Message Stream Member
#include "AthenaBaseComps/AthMessaging.h"
#include "CxxUtils/checker_macros.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/RCBase.h"


#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Geometry/Transform3D.h"

#include <mutex>

namespace TrackerDD {

    /** @class SiCommonItems
    
        Helper class to concentrate common items, such as the pointer to the IdHelper,
        the lorentzAngle service or the information about the solenoidal frame.
        
        To be used for SCT.
        
        @author: Grant Gorfine
        mondified & maintained: Nick Styles, Andreas Salzburger
        */

    class SiCommonItems: public RCBase, public AthMessaging 
    {
    
        public:
        
          SiCommonItems(const FaserDetectorID* const idHelper);
          
          const FaserDetectorID* getIdHelper() const;
          const HepGeom::Transform3D & solenoidFrame() const;
          void setSolenoidFrame(const HepGeom::Transform3D & transform) const; 
          void setLorentzAngleTool(const ISiLorentzAngleTool* lorentzAngleTool);
          const ISiLorentzAngleTool * lorentzAngleTool() const;

        private:
        
          const FaserDetectorID* m_idHelper; 
          mutable HepGeom::Transform3D m_solenoidFrame ATLAS_THREAD_SAFE; // Guarded by m_mutex
          const ISiLorentzAngleTool *m_lorentzAngleTool;

          mutable std::mutex m_mutex;
    };
    
    
    inline const FaserDetectorID* SiCommonItems::getIdHelper() const
    {
      return m_idHelper;
    }
    
    
    inline const HepGeom::Transform3D & SiCommonItems::solenoidFrame() const
    {
      std::lock_guard<std::mutex> lock{m_mutex};
      return m_solenoidFrame;
      // This reference might be changed by setSolenoidFrame.
      // However, it occurrs very rarely.
    }
    
    

} // End namespace InDetDD

#endif // TRACKERREADOUTGEOMETRY_SICOMMONITEMSS_H
