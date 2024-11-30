/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ScintDetectorElement.h
**/

#ifndef SCINTREADOUTGEOMETRY_SCINTDETECTORELEMENT_H
#define SCINTREADOUTGEOMETRY_SCINTDETECTORELEMENT_H

// Base class.
#include "TrkDetElementBase/TrkDetElementBase.h"

// Data member classes
#include "CxxUtils/CachedUniquePtr.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "ScintReadoutGeometry/ScintDetectorDesign.h"
#include "ScintReadoutGeometry/ScintLocalPosition.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "ScintReadoutGeometry/ScintIntersect.h"
#include "ScintReadoutGeometry/ScintCommonItems.h"
#include "ScintReadoutGeometry/ScintDD_Defs.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include "CLHEP/Geometry/Point3D.h"

#include <atomic>
#include <mutex>

class FaserDetectorID;
class GeoVFullPhysVol;
class GeoAlignmentStore;

namespace Trk{
 class Surface;
 class SurfaceBounds;
 constexpr DetectorElemType Scintillator = DetectorElemType::TRT;
}

namespace ScintDD {

  /**
  
   @class ScintDetectorElement
  
   Class to hold geometrical description of a scintillator detector element. 
   A detector element is a single scintillator plate.
    
   @par Coordinate Frames.
  
   The following coordinate frames are used in these elements.
  
    - Global frame:\n
         Currently global frame in G4/GeoModel. Probably eventually
         will be global frame most suitable for reconstruction 
         (eg solenoid axis).
  
    - Local hit frame:\n 
         Local frame for hits. It is the same as local frame in G4 and GeoModel. 
         I also refer to this as the local simulation frame. 
         In FASER:
            - hitDepth = local z = global z (always beam direction)
            - hitEta   = local y = global y (always vertical up)
            - hitPhi   = local x (right-handed wrt y cross z, hence same as global x; left of beam direction)
          (hitPhi, hitEta, hitDepth) is right-handed
          In veto/preshower, photon direction is +hitEta = +y
          In trigger, pmt number increases in +hitPhi direction = +x
         In ATLAS (retained for reference):
         By convention elements are orientated such that:
            - hitDepth = local x
            - hitPhi   = local y
            - hitEta   = local z
         Directions of these correspond to the physical wafer. Consequently hitDepth and hitPhi axes go in 
         different directions depending on the orientation of the module.
         The readout side is determined from design()->readoutSide(). 
  
   - Local reconstruction frame:\n 
            - distPhi  = local x (= global x in FASER)
            - distEta  = local y (= global y in FASER)
            - distDepth = local z (= global z in FASER)
            .
         (In ATLAS:)
         The directions of the axes are defined as
            - distPhi in direction of increasing phi
            - distEta in direction of increasing z in barrel and increasing r in endcap.
            - distDepth (normal) choosen to give right-handed coordinate. 
               =>  away from intersection point for barrel, decreasing z for endcap
     
   @par Overview of Methods 
  
   Methods are grouped into the the following categories
  
      -  Identification
      -  Navigation
      -  Transformation/Orientation
      -  Module Frame
      -  Element Extent
      -  Design methods
      -  Intersection Tests
      -  Lorentz Correction
      -  Readout cell id
      -  Miscellaneous
      -  Cache handling.
  
  
   @author Grant Gorfine
   - modified & maintained: Nick Styles, Andreas Salzburger
   - modified Nigel Hessey: get directions from the design instead of hard-wiring them   
   - modified for Faser: Dave Casper

  */  

  class ScintDetectorElement : public Trk::TrkDetElementBase {

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
    public:
    
    
      /// Constructor:
      ScintDetectorElement(const Identifier &id, 
                           const ScintDetectorDesign *design,
                           const GeoVFullPhysVol *geophysvol,
                           const ScintCommonItems * commonItems,
                           const GeoAlignmentStore* geoAlignStore=nullptr);
    
      /// Destructor:
      virtual ~ScintDetectorElement();
    
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Identification
      /// Methods to identify the element and identifier manipulation.
      // 
      ///////////////////////////////////////////////////////////////////
    
      //@{
    
      /// identifier of this detector element:
      virtual Identifier identify() const override final;      
      // Identifier identify() const;
    
      /// identifier hash
      virtual IdentifierHash identifyHash() const override final;      
      // IdentifierHash identifyHash() const;
    
      /// Returns the id helper
      const FaserDetectorID* getIdHelper() const;
    
      bool isVeto() const;
      bool isTrigger() const;
      bool isPreshower() const;
      bool isVetoNu() const;

      // Identifier <-> pmt 
    
      /// Identifier from pmt 
      Identifier identifierFromCellId(const int& cellId) const;
    
      /// pmt from Identifier
      int   cellIdFromIdentifier(const Identifier & identifier) const;
      
      //@}
    
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Navigation
      /// Methods to access neighbours. 
      //
      ///////////////////////////////////////////////////////////////////
    
      //@{
        const ScintDetectorElement* nextInZ() const;
        const ScintDetectorElement* prevInZ() const;
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Transformation/Orientation
      //
      ///////////////////////////////////////////////////////////////////
    
      //@{
      // Position 
      /// Local (simulation/hit frame) to global transform
      virtual const GeoTrf::Transform3D & transformHit() const;
      /// Local (reconstruction frame) to global transform
      virtual const Amg::Transform3D & transform() const override;
      /// Default Local (reconstruction frame) to global transform
      /// ie with no misalignment. 
      const HepGeom::Transform3D defTransformCLHEP() const;
      const Amg::Transform3D defTransform() const;
      /// Center in global coordinates
      virtual const Amg::Vector3D & center() const override;
    
      const HepGeom::Transform3D & transformCLHEP() const;
    
      /// Simulation/Hit local frame to reconstruction local frame. 2D.
      //  TODO: Will change order of parameters at some point.
      Amg::Vector2D hitLocalToLocal(double xEta, double xPhi) const;
      /// Same as previuos method but 3D.
      HepGeom::Point3D<double> hitLocalToLocal3D(const HepGeom::Point3D<double> & hitPosition) const;
    
      /// Transform to go from local reconstruction frame to local hit frame.
      const HepGeom::Transform3D recoToHitTransform() const;
    
      /// Directions of hit depth,phi,eta axes relative to reconstruction local position
      /// axes (LocalPosition). Returns +/-1.
      double hitDepthDirection() const;
      /// See previous method.
      double hitPhiDirection() const;
      /// See previous method.
      double hitEtaDirection() const;
    
      // To determine if readout direction between online and offline needs swapping, see methods
      // swapPhiReadoutDirection() and swapEtaReadoutDirection() below in "Readout Cell id" section
    
      // Orientation. 
      // Directions.
      //  phiAxis - in same direction as increasing phi and identifier phi_index/strip. 
      //            NB. This requires some flipping of axes with repsect to the hits.  
      //  etaAxis - in direction of increasing z in the barrel and increasing r in the endcap. 
      //  normal  - choosen to give right-handed coordinate frame (x=normal,y=phiAxis,z=etaAxis)
      //            NB. This requires some flipping of axes with repsect to the hits.  
      
      /// Get reconstruction local x axes in global frame. 
      const Amg::Vector3D & phiAxis() const;
      const HepGeom::Vector3D<double> & phiAxisCLHEP() const;
      /// Get reconstruction local y axes in global frame. 
      const Amg::Vector3D & etaAxis() const;
      const HepGeom::Vector3D<double> & etaAxisCLHEP() const;
      /// Get reconstruction local normal axes in global frame. Choosen to give right-handed coordinate frame.
      virtual const Amg::Vector3D & normal() const override;
     
      /// transform a hit local position into a global position:
      HepGeom::Point3D<double> globalPositionHit(const HepGeom::Point3D<double> &simulationLocalPos) const;
      Amg::Vector3D globalPositionHit(const Amg::Vector3D &simulationLocalPos) const;
      
      /// transform a reconstruction local position into a global position:
      HepGeom::Point3D<double> globalPosition(const HepGeom::Point3D<double> &localPos) const;
      Amg::Vector3D globalPosition(const Amg::Vector3D &localPos) const;

      /// as in previous method but for 2D local position
      HepGeom::Point3D<double> globalPositionCLHEP(const Amg::Vector2D &localPos) const;
      
      Amg::Vector3D globalPosition(const Amg::Vector2D &localPos) const;
    
      /// transform a global position into a 2D local position (reconstruction frame)
      Amg::Vector2D localPosition(const HepGeom::Point3D<double> & globalPosition) const;

      Amg::Vector2D localPosition(const Amg::Vector3D& globalPosition) const;
     
      /// Element Surface
      virtual Trk::Surface & surface();
      virtual const Trk::Surface & surface() const override;
    
      //@}

      /** Returns the full list of surfaces associated to this detector element */
      virtual const std::vector<const Trk::Surface*>& surfaces() const;

      /**
      
      @name Plate Frame 
      Methods to help work with the module frame. 
      This is mainly of of use in the SCT where the module frame can
      in general be different from the element frame. It is actully
      defined as the frame of one of the sides (currently the axial
      side), but using these methods one does not need to make any
      assumptions about what the actual frame is.  In the following
      methods the local element frame is the local reconstruction
      frame of this element.
      */
      
      //@{
    
      /// Plate to global frame transform. 
      /// Includes misalignment. 
      //const HepGeom::Transform3D & moduleTransform() const;
      const Amg::Transform3D & plateTransform() const;
    
      /// Default plate to global frame transform, ie with no misalignment. 
      Amg::Transform3D defPlateTransform() const;

      /// Take a transform of the local element frame and return its equivalent in the module frame.
      //HepGeom::Transform3D localToModuleFrame(const HepGeom::Transform3D & localTransform) const;
      Amg::Transform3D localToModuleFrame(const Amg::Transform3D & localTransform) const;
    

      /// Transformation from local element to module frame.  This can be
      /// used to take a local position in the element frame and transform
      /// it to a position in the module frame. If one is already in the
      /// module frame it will return the Identity transform.
      //HepGeom::Transform3D localToModuleTransform() const;
      Amg::Transform3D localToModuleTransform() const;


      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Element Extent
      /// Methods to get extent of element in x, y and z.  
      ///////////////////////////////////////////////////////////////////
    
      //@{
      // Extent in x,y and z
      double xMin() const;
      double xMax() const;
      double yMin() const;
      double yMax() const;
      double zMin() const;
      double zMax() const;
    
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Design methods
      //
      ///////////////////////////////////////////////////////////////////
      //@{
    
      /// access to the local description:
      const ScintDetectorDesign &design() const;
    
      // Methods from design
      double width() const; // Width in x direction.
      double length() const; // Length in y direction 
      double thickness() const; // Thickness in z direction
    
      virtual const Trk::SurfaceBounds & bounds() const override;
    
      // Test that it is in the active region
      // Intersect has 3 states
      // bool SiIntersect::in() const // definitely in
      // bool SiIntersect::out() const // definitely out
      // bool SiIntersect::nearBoundary() const // near a boundary within the tolerances 
      // bool SiIntersect::mayIntersect() const // in() OR nearBoundary()
      ScintIntersect inDetector(const Amg::Vector2D & localPosition, double phiTol, double etaTol) const;
      ScintIntersect inDetector(const HepGeom::Point3D<double> & globalPosition, double phiTol, double etaTol) const;

      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Cache handling.
      //
      ///////////////////////////////////////////////////////////////////
      //@{.
      //   - Methods to handle invalidating and updating caches. The cached values include values that are affected by alignment
      //     Surface are only created on demand.  The method updateAllCaches also creates the surfaces as well as calling updateCache.
      //     Conditions cache contains Lorentz angle related quantities.
     
      /// Signal that cached values are no longer valid.
      /// Invalidate general cache
      // void invalidate ATLAS_NOT_THREAD_SAFE () const;
      void invalidate();
    
      ///Set/calculate cache values 
      void setCache(){
        updateCache();
      } 
     ///Set/calculate all cache values including  surfaces.  
      void setAllCaches(){
        updateAllCaches();
      } 
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Methods to satisfy TrkDetElementBase interface
      //
      ///////////////////////////////////////////////////////////////////
      //{@
      virtual const Amg::Transform3D & transform(const Identifier&) const {return transform();}
      virtual const Trk::Surface& surface (const Identifier&) const {return surface();}
      virtual const Amg::Vector3D& center (const Identifier&) const {return center();}
      virtual const Amg::Vector3D& normal (const Identifier&) const {return normal();}
      virtual const Trk::SurfaceBounds & bounds(const Identifier&) const {return bounds();}
      //@}
    
      //////////////////////////////////////////////////////////////////////////////////////

      void setNextInZ(const ScintDetectorElement* element);
      void setPrevInZ(const ScintDetectorElement* element);

      //////////////////////////////////////////////////////////////////////////////////////
    
    public:
    
      const ScintCommonItems* getCommonItems() const;

    /** TrkDetElementBase interface detectorType
     */
     virtual Trk::DetectorElemType detectorType() const override final;

      ///////////////////////////////////////////////////////////////////
      // Private methods:
      ///////////////////////////////////////////////////////////////////
    
    private:
      /// Recalculate  cached values. 
      void updateCache() const;
   
      /// Update all caches including surfaces.
      void updateAllCaches() const;
    

      // Common code for constructors.
      void commonConstructor();
    
      // Calculate extent in x, y and z. The values are cached and there
      // are xMin(), xMax etc methods.
      void getExtent(double &xMin, double &xMax,
                     double &yMin, double &yMax,
                     double &zMin, double &zMax) const;
    
      // Return the four corners of an element in local coordinates.
      // Pass it an array of length 4.
      // This function is used by getEtaPhiRegion()
      void getCorners(HepGeom::Point3D<double> *corners) const;
    
      //Declaring the Message method for further use
      MsgStream& msg (MSG::Level lvl) const { return m_commonItems->msg(lvl);}
    
      //Declaring the Method providing Verbosity Level
      bool msgLvl (MSG::Level lvl) const { return m_commonItems->msgLvl(lvl);}
    
    
      ///////////////////////////////////////////////////////////////////
      // Private methods:
      ///////////////////////////////////////////////////////////////////
    private:
      // Don't allow copying.
      ScintDetectorElement();
      ScintDetectorElement(const ScintDetectorElement&);
      ScintDetectorElement &operator=(const ScintDetectorElement&);

      ///////////////////////////////////////////////////////////////////
      // Protected data:
      ///////////////////////////////////////////////////////////////////
    protected:
      Identifier m_id; // identifier of this detector element
      IdentifierHash m_idHash; // hash id
      const ScintDetectorDesign *m_design; // local description of this detector element
      const ScintCommonItems * m_commonItems;
    
      const ScintDetectorElement* m_prevInZ;
      const ScintDetectorElement* m_nextInZ;

      bool m_isVeto;
      bool m_isTrigger;
      bool m_isPreshower;
      bool m_isVetoNu;
      
      //
      // Cached values.
      //
      // Axes
      ScintDetectorDesign::Axis m_hitEta;
      ScintDetectorDesign::Axis m_hitPhi;
      ScintDetectorDesign::Axis m_hitDepth;

      // Directions of axes. These are true if the hit/simulation and reconstruction local frames are
      // in the same direction and false if they are opposite.
      mutable bool m_depthDirection ATLAS_THREAD_SAFE; // Guarded by m_mutex // Direction of depth axis. 
                             // Also direction of readout implant (n+ for pixel, p+ for SCT).
      mutable bool m_phiDirection ATLAS_THREAD_SAFE;     //
      mutable bool m_etaDirection ATLAS_THREAD_SAFE;     //

      mutable std::atomic_bool m_cacheValid; // Alignment associated quatities.
      mutable std::atomic_bool m_firstTime;

      mutable std::recursive_mutex m_mutex;

      mutable Amg::Transform3D m_transform ATLAS_THREAD_SAFE; // Guarded by m_mutex
      mutable HepGeom::Transform3D m_transformCLHEP ATLAS_THREAD_SAFE; // Guarded by m_mutex

      mutable Amg::Vector3D m_normal ATLAS_THREAD_SAFE; // Guarded by m_mutex
      mutable Amg::Vector3D m_etaAxis ATLAS_THREAD_SAFE; // Guarded by m_mutex
      mutable HepGeom::Vector3D<double> m_etaAxisCLHEP ATLAS_THREAD_SAFE; // Guarded by m_mutex
      mutable Amg::Vector3D m_phiAxis ATLAS_THREAD_SAFE; // Guarded by m_mutex
      mutable HepGeom::Vector3D<double> m_phiAxisCLHEP ATLAS_THREAD_SAFE; // Guarded by m_mutex
      mutable Amg::Vector3D m_center ATLAS_THREAD_SAFE; // Guarded by m_mutex
      mutable HepGeom::Vector3D<double> m_centerCLHEP ATLAS_THREAD_SAFE; // Guarded by m_mutex

      mutable double m_minX   ATLAS_THREAD_SAFE;// Guarded by m_mutex
      mutable double m_maxX   ATLAS_THREAD_SAFE;// Guarded by m_mutex
      mutable double m_minY   ATLAS_THREAD_SAFE;// Guarded by m_mutex
      mutable double m_maxY   ATLAS_THREAD_SAFE;// Guarded by m_mutex
      mutable double m_minZ   ATLAS_THREAD_SAFE;// Guarded by m_mutex
      mutable double m_maxZ   ATLAS_THREAD_SAFE;// Guarded by m_mutex

      CxxUtils::CachedUniquePtrT<Trk::Surface> m_surface;
      mutable std::vector<const Trk::Surface*> m_surfaces ATLAS_THREAD_SAFE; // Guarded by m_mutex

      const GeoAlignmentStore* m_geoAlignStore{};
    };
    
    ///////////////////////////////////////////////////////////////////
    // Inline methods:
    ///////////////////////////////////////////////////////////////////
       

    inline Trk::DetectorElemType ScintDetectorElement::detectorType() const{
      return Trk::Scintillator;
    }



    inline HepGeom::Point3D<double> ScintDetectorElement::globalPositionHit(const HepGeom::Point3D<double> &localPos) const
    {
      return Amg::EigenTransformToCLHEP(transformHit())*localPos;
    }
    
    inline Amg::Vector3D ScintDetectorElement::globalPosition(const Amg::Vector2D &localPos) const
    {
      if (!m_cacheValid) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        if (!m_cacheValid) updateCache();
      }
      return m_center + localPos[Trk::distEta] * m_etaAxis + localPos[Trk::distPhi] * m_phiAxis;
    }

    inline Amg::Vector3D ScintDetectorElement::globalPositionHit(const Amg::Vector3D &localPos) const
    {
      return transformHit() * localPos;
    }
    
     inline HepGeom::Point3D<double> ScintDetectorElement::globalPositionCLHEP(const Amg::Vector2D &localPos) const
    {
      if (!m_cacheValid) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        if (!m_cacheValid) updateCache();
      }
      return m_centerCLHEP + localPos[Trk::distEta] * m_etaAxisCLHEP + localPos[Trk::distPhi] * m_phiAxisCLHEP;
    }
     //here
     inline Amg::Vector3D ScintDetectorElement::globalPosition(const Amg::Vector3D &localPos) const
    {
      return transform() * localPos;
    }
    
     inline HepGeom::Point3D<double> ScintDetectorElement::globalPosition(const HepGeom::Point3D<double> &localPos) const
    {
      return transformCLHEP() * localPos;
    }
    
    inline Amg::Vector2D ScintDetectorElement::localPosition(const HepGeom::Point3D<double> & globalPosition) const
    {
      if (!m_cacheValid){
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        if (!m_cacheValid) updateCache();
      }
      HepGeom::Vector3D<double> relativePos = globalPosition - m_centerCLHEP;
      return Amg::Vector2D(relativePos.dot(m_phiAxisCLHEP), relativePos.dot(m_etaAxisCLHEP));
    }

    inline Amg::Vector2D ScintDetectorElement::localPosition(const Amg::Vector3D & globalPosition) const
    {
      if (!m_cacheValid){
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        if (!m_cacheValid) updateCache();
      }
      Amg::Vector3D relativePos = globalPosition - m_center;
      return Amg::Vector2D(relativePos.dot(m_phiAxis), relativePos.dot(m_etaAxis));
    }

    inline const ScintDetectorDesign &ScintDetectorElement::design() const
    {
      return *m_design;
    }
    
    inline const FaserDetectorID* ScintDetectorElement::getIdHelper() const
    {
      return m_commonItems->getIdHelper();
    }
    
    inline Identifier ScintDetectorElement::identify() const
    {
      return m_id;
    }
    
    inline IdentifierHash ScintDetectorElement::identifyHash() const
    {
      return m_idHash;
    }
    
    inline double ScintDetectorElement::hitDepthDirection() const
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
      return (m_depthDirection) ? 1. : -1.;
    }
    
    inline double ScintDetectorElement::hitPhiDirection() const
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
      return (m_phiDirection) ? 1. : -1.;
    }
    
    inline double ScintDetectorElement::hitEtaDirection() const
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
      return (m_etaDirection) ? 1. : -1.;
    }
       
    inline void ScintDetectorElement::invalidate()
    {
      m_cacheValid = false;
    }

    inline void ScintDetectorElement::updateAllCaches() const
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
      if (not m_surface) surface();
    }
    
    
    inline double ScintDetectorElement::xMin() const 
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
      return m_minX;
    }
    
    inline double ScintDetectorElement::xMax() const 
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
      return m_maxX;
    }
    
    inline double ScintDetectorElement::yMin() const 
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
      return m_minY;
    }
    
    inline double ScintDetectorElement::yMax() const 
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
      return m_maxY;
    }
    
    inline double ScintDetectorElement::zMin() const 
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
      return m_minZ;
    }
    
    inline double ScintDetectorElement::zMax() const 
    {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
      return m_maxZ;
    }
    
    inline double ScintDetectorElement::width() const
    {
      return m_design->width();
    }
       
    inline double ScintDetectorElement::length() const
    {
      return m_design->length();
    }
    
    inline double ScintDetectorElement::thickness() const
    {
      return m_design->thickness();
    }
    
    inline const ScintCommonItems* ScintDetectorElement::getCommonItems() const
    {
      return m_commonItems;
    }

    inline const ScintDetectorElement* ScintDetectorElement::prevInZ() const
    {
      return m_prevInZ;
    }

    inline const ScintDetectorElement* ScintDetectorElement::nextInZ() const
    {
      return m_nextInZ;
    }

    inline void ScintDetectorElement::setPrevInZ(const ScintDetectorElement* element)
    {
      m_prevInZ = element;
    }

    inline void ScintDetectorElement::setNextInZ(const ScintDetectorElement* element)
    {
      m_nextInZ = element;
    }
        
} // namespace ScintDD

#endif // SCINTREADOUTGEOMETRY_SCINTDETECTORELEMENT_H

