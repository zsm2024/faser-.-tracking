/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiDetectorElement.cxx
 * Implementation file for class SiDetectorElement
 * @author Grant Gorfine
 * Based on version developed by David Calvet.
 **/

#include "TrackerReadoutGeometry/SiDetectorElement.h"

#include "FaserDetDescr/FaserDetectorID.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelFaserUtilities/GeoAlignmentStore.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_ModuleSideDesign.h"
#include "TrackerReadoutGeometry/SiReadoutCellId.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/PhysicalConstants.h" // for M_PI
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/ThreeVector.h"

#include <cassert>
#include <cmath>
#include <limits>

namespace TrackerDD {
  using Trk::distPhi;
  using Trk::distEta;
  using Trk::distDepth;

  // Constructor with parameters:
  SiDetectorElement::SiDetectorElement(const Identifier& id,
                                       const SiDetectorDesign* design,
                                       const GeoVFullPhysVol* geophysvol,
                                       const SiCommonItems* commonItems,
                                       const GeoAlignmentStore* geoAlignStore) :
    TrkDetElementBase(geophysvol),
    m_id(id),
    m_design(design),
    m_commonItems(commonItems),
    m_nextInEta(nullptr),
    m_prevInEta(nullptr),
    m_nextInPhi(nullptr),
    m_prevInPhi(nullptr),
    m_otherSide(nullptr),
    m_cacheValid(false),
    m_firstTime(true),
    m_stereoCacheValid(false),
    m_isStereo(false),
    m_mutex(),
    m_surface{},
    m_surfaces{},
    m_geoAlignStore(geoAlignStore)
    {
      //The following are fixes for coverity bug 11955, uninitialized scalars:
      const bool boolDefault(true);
      m_depthDirection=boolDefault;
      m_phiDirection=boolDefault;
      m_etaDirection=boolDefault;
      const double defaultMin(std::numeric_limits<double>::max());
      const double defaultMax(std::numeric_limits<double>::lowest());
      m_minZ=defaultMin;
      m_maxZ=defaultMax;
      m_minR=defaultMin;
      m_maxR=defaultMax;
      m_minPhi=defaultMin;
      m_maxPhi=defaultMax;

      m_hitEta = m_design->etaAxis();
      m_hitPhi = m_design->phiAxis();
      m_hitDepth = m_design->depthAxis();
      ///
  
      commonConstructor();
    }

  void
  SiDetectorElement::commonConstructor() 
  {
    if (!m_id.is_valid()) throw std::runtime_error("SiDetectorElement: Invalid identifier");
    const FaserSCT_ID* sctId = dynamic_cast<const FaserSCT_ID*>(getIdHelper());
    m_idHash = sctId->wafer_hash(m_id);
    if (!m_idHash.is_valid()) throw std::runtime_error("SiDetectorElement: Unable to set IdentifierHash");

    // Increase the reference count of the SiDetectorDesign objects.
    m_design->ref();

    // Increase the reference count of the SiCommonItems objects.
    m_commonItems->ref();

    // Should we reference count the geophysvol as well?

  }


  // Destructor:
  SiDetectorElement::~SiDetectorElement()
  {
    // The design is reference counted so that it will not be deleted until the last element is deleted.
    m_design->unref();

    m_commonItems->unref();
  }

  /* 
   * update cache 
   * This is supposed to be called inside a block
   * like
   * std::lock_guard< ... > 
   * if (!cacheValid) {
   *  updateCache()
   * }
   */
  void 
  SiDetectorElement::updateCache() const
  {
    const GeoTrf::Transform3D& geoTransform = transformHit();
    const GeoTrf::Transform3D& geoModuleTransform = transformModule();
    // ATH_MSG_ALWAYS("Wafer transform: " << geoTransform.translation() << "//" << geoTransform.rotation() );
    // ATH_MSG_ALWAYS("Module transform: " << geoModuleTransform.translation() << "//" << geoModuleTransform.rotation() );

    double radialShift = 0.;
      
    HepGeom::Point3D<double> centerGeoModel(radialShift, 0., 0.);
    m_centerCLHEP = Amg::EigenTransformToCLHEP(geoTransform) * centerGeoModel;
    m_center = Amg::Vector3D(m_centerCLHEP[0], m_centerCLHEP[1], m_centerCLHEP[2]);

    //
    // Determine directions depth, eta and phi axis in reconstruction local frame
    // ie depth away from interaction point
    //    phi in direction of increasing phi
    //    eta in direction of increasing z in barrel, and increasing r in endcap
    //
   
    // depthAxis, phiAxis, and etaAxis are defined to be x,y,z respectively for all detectors for hit local frame.
    // depthAxis, phiAxis, and etaAxis are defined to be z,x,y respectively for all detectors for reco local frame.
    static const HepGeom::Vector3D<double> localAxes[3] = {
      HepGeom::Vector3D<double>(1., 0., 0.),
      HepGeom::Vector3D<double>(0., 1., 0.),
      HepGeom::Vector3D<double>(0., 0., 1.)
    };

    static const HepGeom::Vector3D<double>& localRecoPhiAxis = localAxes[distPhi];     // Defined to be same as x axis
    static const HepGeom::Vector3D<double>& localRecoEtaAxis = localAxes[distEta];     // Defined to be same as y axis
    static const HepGeom::Vector3D<double>& localRecoDepthAxis = localAxes[distDepth]; // Defined to be same as z axis

    // We only need to calculate the rough orientation once.
    //For it to change would require extreme unrealistic misalignment changes.
    if (m_firstTime) {
      // Determine the unit vectors in global frame

      const HepGeom::Vector3D<double>& geoModelPhiAxis = localAxes[m_hitPhi];
      const HepGeom::Vector3D<double>& geoModelEtaAxis = localAxes[m_hitEta];
      const HepGeom::Vector3D<double>& geoModelDepthAxis = localAxes[m_hitDepth];

      HepGeom::Vector3D<double> globalDepthAxis(Amg::EigenTransformToCLHEP(geoTransform) * geoModelDepthAxis);
      HepGeom::Vector3D<double> globalPhiAxis(Amg::EigenTransformToCLHEP(geoTransform) * geoModelPhiAxis);
      HepGeom::Vector3D<double> globalEtaAxis(Amg::EigenTransformToCLHEP(geoTransform) * geoModelEtaAxis);

      HepGeom::Vector3D<double> nominalEta { 1.0, 0.0, 0.0 };
      HepGeom::Vector3D<double> nominalNormal { 0.0, 0.0, 1.0 };
      HepGeom::Vector3D<double> nominalPhi { 0.0, 1.0, 0.0 };
  
      // In Faser, the etaAxis is along increasing x, and normal is in increasing z direction

      // Determine if axes are to have their directions swapped.

      //
      // Depth axis.
      //
      double depthDir = globalDepthAxis.dot(nominalNormal);
      m_depthDirection = true;
      if (depthDir < 0.) {
          m_depthDirection = false;
      }
      if (std::abs(depthDir) < 0.5) { // Check that it is in roughly the right direction.
        ATH_MSG_ERROR("Orientation of local depth axis does not follow correct convention.");
        // throw std::runtime_error("Orientation of local depth axis does not follow correct convention.");
        m_depthDirection = true; // Don't swap.
      }
    
      //
      // Phi axis
      //
      double phiDir = globalPhiAxis.dot(nominalPhi);
      m_phiDirection = true;
      if (phiDir < 0.) {
          m_phiDirection = false;
      }    
      if (std::abs(phiDir) < 0.5) { // Check that it is in roughly the right direction.
        ATH_MSG_ERROR("Orientation of local xPhi axis does not follow correct convention.");
        // throw std::runtime_error("Orientation of local xPhi axis does not follow correct convention.");
        m_phiDirection = true; // Don't swap.
      }
    
      //
      // Eta axis
      //
      double etaDir = globalEtaAxis.dot(nominalEta);
      m_etaDirection = true;
      if (etaDir < 0.) {
          m_etaDirection = false;
      }
      if (std::abs(etaDir) < 0.5) { // Check that it is in roughly the right direction.
        ATH_MSG_ERROR("Orientation of local xEta axis does not follow correct convention.");
        // throw std::runtime_error("Orientation of local xEta axis does not follow correct convention.");
        m_etaDirection = true; // Don't swap
      }   
    } // end if (m_firstTime)
  
    m_transformCLHEP = Amg::EigenTransformToCLHEP(geoTransform) * recoToHitTransformImpl();
    m_transform = Amg::CLHEPTransformToEigen(m_transformCLHEP);
  
    m_moduleTransformCLHEP = Amg::EigenTransformToCLHEP(geoModuleTransform) * recoToHitTransformImpl();
    m_moduleTransform = Amg::CLHEPTransformToEigen(m_moduleTransformCLHEP);

    // Check that local frame is right-handed. (ie transform has no reflection)
    // This can be done by checking that the determinant is >0.
    if (m_firstTime) { // Only need to check this once.
      HepGeom::Transform3D& t = m_transformCLHEP;
      double det = t(0,0) * (t(1,1)*t(2,2) - t(1,2)*t(2,1)) -
                   t(0,1) * (t(1,0)*t(2,2) - t(1,2)*t(2,0)) +
                   t(0,2) * (t(1,0)*t(2,1) - t(1,1)*t(2,0));
      if (det < 0.) {
          ATH_MSG_DEBUG("Local frame is left-handed, Swapping depth axis to make it right handed.");
          m_depthDirection = !m_depthDirection;
          m_transformCLHEP = Amg::EigenTransformToCLHEP(geoTransform) * recoToHitTransformImpl();
          m_transform = Amg::CLHEPTransformToEigen(m_transformCLHEP);
      }
    } 
  
    // Initialize various cached members
    // The unit vectors
    HepGeom::Vector3D<double> normalCLHEP = m_transformCLHEP * localRecoDepthAxis;
    m_normal = Amg::Vector3D(normalCLHEP[0], normalCLHEP[1], normalCLHEP[2]);
  
    m_phiAxisCLHEP = m_transformCLHEP * localRecoPhiAxis;
    m_etaAxisCLHEP = m_transformCLHEP * localRecoEtaAxis;

    m_phiAxis = Amg::Vector3D(m_phiAxisCLHEP[0], m_phiAxisCLHEP[1], m_phiAxisCLHEP[2]);
    m_etaAxis = Amg::Vector3D(m_etaAxisCLHEP[0], m_etaAxisCLHEP[1], m_etaAxisCLHEP[2]);

    getExtent(m_minR, m_maxR, m_minZ, m_maxZ, m_minPhi, m_maxPhi);

    //Install the cache at the end
    m_cacheValid.store(true);
    if (m_firstTime) m_firstTime.store(false);
  }

  void
  SiDetectorElement::determineStereo() const
  {
    // Assume m_mutex is already locked.

    if (m_firstTime) updateCache();

    if (m_otherSide) {
      double sinStereoThis = std::abs(sinStereoImpl()); //Call the private impl method
      double sinStereoOther = std::abs(m_otherSide->sinStereo());
      if (sinStereoThis == sinStereoOther) {
        // If they happend to be equal then set side0 as axial and side1 as stereo.
        const FaserSCT_ID* sctId = dynamic_cast<const FaserSCT_ID*>(getIdHelper());
        if (sctId) {
          int side = sctId->side(m_id);
          m_isStereo = (side == 1);
        }
      } else {
        // set the stereo side as the one with largest absolute sinStereo.
        m_isStereo = (sinStereoThis > sinStereoOther);
      }
    } else {
      m_isStereo = false;
    }

    m_stereoCacheValid.store(true);
  }

  const GeoTrf::Transform3D&
  SiDetectorElement::transformHit() const
  {
    if (m_geoAlignStore) {
      const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getAbsPosition(getMaterialGeom());
      if (ptrXf) return *ptrXf;
    }
    return getMaterialGeom()->getAbsoluteTransform();
  }

  const GeoTrf::Transform3D&
  SiDetectorElement::transformModule() const
  {
    PVConstLink parent = getMaterialGeom()->getParent()->getParent();
    const GeoVFullPhysVol* fullParent = dynamic_cast<const GeoVFullPhysVol*>(&(*parent));
    if (fullParent == nullptr)
    {
      ATH_MSG_FATAL("Unable to reach parent module volume");
      if (m_geoAlignStore) {
        const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getAbsPosition(getMaterialGeom());
        if (ptrXf) return *ptrXf;
      }
      return getMaterialGeom()->getAbsoluteTransform();
    }
    // ATH_MSG_ALWAYS("Found full parent named: " << fullParent->getLogVol()->getName() << " with id " << fullParent->getId());
    if (m_geoAlignStore) {
      const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getAbsPosition(fullParent);
      if (ptrXf) return *ptrXf;
    }
    return fullParent->getAbsoluteTransform();
  }

  const Amg::Transform3D&
  SiDetectorElement::transform() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_transform;
  }

  const HepGeom::Transform3D&
  SiDetectorElement::transformCLHEP() const
  {
    //stuff to get the CLHEP version of the local to global transform.
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_transformCLHEP;
  }

  const HepGeom::Transform3D 
  SiDetectorElement::defTransformCLHEP() const
  {
    if (m_geoAlignStore) {
      const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getDefAbsPosition(getMaterialGeom());
      if (ptrXf) return Amg::EigenTransformToCLHEP(*ptrXf) * recoToHitTransform();
    }
    return Amg::EigenTransformToCLHEP(getMaterialGeom()->getDefAbsoluteTransform()) * recoToHitTransform();
  }  
   
  const Amg::Transform3D 
  SiDetectorElement::defTransform() const
  {
    HepGeom::Transform3D tmpTransform = defTransformCLHEP();
    return Amg::CLHEPTransformToEigen(tmpTransform);
  }

  const HepGeom::Transform3D 
  SiDetectorElement::recoToHitTransform() const
  {
    // Determine the reconstruction local (LocalPosition) to global transform.
    if (m_firstTime) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (m_firstTime) updateCache();
    }

    return recoToHitTransformImpl();
  }

  const HepGeom::Transform3D 
  SiDetectorElement::recoToHitTransformImpl() const
  {
    //        = transfromHit * hitLocal
    //        = transformHit * recoToHitTransform * recoLocal
    // recoToHitTransform takes recoLocal to hitLocal
    // x,y,z -> y,z,x 
    // equiv to a rotation around Y of 90 deg followed by a rotation around X of 90deg
    // 
    // recoToHit is static as it needs to be calculated once only.
    // We use the HepGeom::Transform3D constructor which takes one coordinates system to another where the
    // coordinate system is defined by it center and two axes.
    // distPhi, distEta are the reco local axes and hitPhi and hitEta are the hit local axes.
    // It assume phi, eta, depth makes a right handed system which is the case.
    static const HepGeom::Vector3D<double> localAxes[3] = {
      HepGeom::Vector3D<double>(1., 0., 0.),
      HepGeom::Vector3D<double>(0., 1., 0.),
      HepGeom::Vector3D<double>(0., 0., 1.)
    };
    //static 
    
    const HepGeom::Transform3D recoToHit(HepGeom::Point3D<double>(0., 0., 0.),
                                         localAxes[distPhi],
                                         localAxes[distEta],
                                         HepGeom::Point3D<double>(0., 0., 0.),
                                         localAxes[m_hitPhi],
                                         localAxes[m_hitEta]);
  
    // Swap direction of axis as appropriate
    CLHEP::Hep3Vector scale(1., 1., 1.);
    if (!m_phiDirection)   scale[distPhi]   = -1.;
    if (!m_etaDirection)   scale[distEta]   = -1.;
    if (!m_depthDirection) scale[distDepth] = -1.;
    return recoToHit * HepGeom::Scale3D(scale[0], scale[1], scale[2]);
  }

  const Amg::Transform3D&
  SiDetectorElement::moduleTransform() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_moduleTransform;
  }

  const HepGeom::Transform3D&
  SiDetectorElement::moduleTransformCLHEP() const
  {
    //stuff to get the CLHEP version of the local to global transform.
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_moduleTransformCLHEP;
  }

  Amg::Transform3D
  SiDetectorElement::defModuleTransform() const
  {
    HepGeom::Transform3D tmpTransform = defModuleTransformCLHEP();
    return Amg::CLHEPTransformToEigen(tmpTransform);
  }  

  const HepGeom::Transform3D 
  SiDetectorElement::defModuleTransformCLHEP() const
  {
    PVConstLink parent = getMaterialGeom()->getParent()->getParent();
    const GeoVFullPhysVol* fullParent = dynamic_cast<const GeoVFullPhysVol*>(&(*parent));
    if (fullParent == nullptr)
    {
      ATH_MSG_FATAL("Unable to reach parent module volume");
      if (m_geoAlignStore) {
        const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getDefAbsPosition(getMaterialGeom());
        if (ptrXf) return Amg::EigenTransformToCLHEP(*ptrXf) * recoToHitTransform();
      }
      return Amg::EigenTransformToCLHEP(getMaterialGeom()->getDefAbsoluteTransform()) * recoToHitTransform();
    }
    // ATH_MSG_ALWAYS("Found full parent named: " << fullParent->getLogVol()->getName() << " with id " << fullParent->getId());
    if (m_geoAlignStore) {
      const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getDefAbsPosition(fullParent);
      if (ptrXf) return Amg::EigenTransformToCLHEP(*ptrXf) * recoToHitTransform();
    }
    return Amg::EigenTransformToCLHEP(fullParent->getDefAbsoluteTransform()) * recoToHitTransform();
  }  


  // Take a transform in the local reconstruction and return it in the module frame
  // For a given transform l in frame A. The equivalent transform in frame B is
  //  B.inverse() * A * l * A.inverse() * B
  // Here A is the local to global transform of the element and B is the local to global
  // transform of the module.
  Amg::Transform3D 
  SiDetectorElement::localToModuleFrame(const Amg::Transform3D& localTransform) const
  {
    return moduleTransform().inverse() * transform() * localTransform * transform().inverse() * moduleTransform();
    // if (isModuleFrame()) {
    //   return localTransform;
    // } else {
    //   return m_otherSide->transform().inverse() * transform() * localTransform * transform().inverse() * m_otherSide->transform();
    // }
  }

  Amg::Transform3D 
  SiDetectorElement::localToModuleTransform() const
  {
    return transform().inverse() * moduleTransform();
    // if (isModuleFrame()) {
    //   return Amg::Transform3D(); // Identity
    // } else {
    //   return m_otherSide->transform().inverse() * transform();
    // }
  }

  bool 
  SiDetectorElement::isModuleFrame() const
  {
    // The module frame is the axial side.
    // NB isStereo returns false for the pixel and so
    // isModuleFrame is always true for the pixel.

    return !isStereo();
  }


  const Amg::Vector3D& 
  SiDetectorElement::center() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_center;
  }

  const Amg::Vector3D&  
  SiDetectorElement::normal() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_normal;
  }

  const HepGeom::Vector3D<double>& 
  SiDetectorElement::etaAxisCLHEP() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_etaAxisCLHEP;
  }

  const HepGeom::Vector3D<double>& 
  SiDetectorElement::phiAxisCLHEP() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_phiAxisCLHEP;
  }

  const Amg::Vector3D& 
  SiDetectorElement::etaAxis() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_etaAxis;
  }

  const Amg::Vector3D& 
  SiDetectorElement::phiAxis() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_phiAxis;
  }

  Amg::Vector2D
  SiDetectorElement::hitLocalToLocal(double xEta, double xPhi) const  // Will change order to phi,eta
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    if (!m_etaDirection) xEta = -xEta;
    if (!m_phiDirection) xPhi = -xPhi;
    return Amg::Vector2D(xPhi, xEta);
  }

  HepGeom::Point3D<double>
  SiDetectorElement::hitLocalToLocal3D(const HepGeom::Point3D<double>& hitPosition) const
  {
    // Equiv to transform().inverse * transformHit() * hitPosition
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    double xDepth = hitPosition[m_hitDepth];
    double xPhi = hitPosition[m_hitPhi];
    double xEta = hitPosition[m_hitEta];
    if (!m_depthDirection) xDepth = -xDepth;
    if (!m_phiDirection) xPhi = -xPhi;
    if (!m_etaDirection) xEta = -xEta;
    return HepGeom::Point3D<double>(xPhi, xEta, xDepth);
  }

  // compute sin(tilt angle) at center:
  double SiDetectorElement::sinTilt() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    // Tilt is defined as the angle between a refVector and the sensor normal.
    // In barrel refVector = unit vector radial.
    // in endcap it is assumed there is no tilt.
    // sinTilt = (refVector cross normal) . z

    // Angle between normal and radial vector.
    // HepGeom::Vector3D<double> refVector(m_center.x(), m_center.y(), 0);
    // return (refVector.cross(m_normal)).z()/refVector.mag();
    // or the equivalent
    return (m_center.x() * m_normal.y() - m_center.y() * m_normal.x()) / m_center.perp();
  }

  double SiDetectorElement::sinTilt(const Amg::Vector2D& localPos) const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    HepGeom::Point3D<double> point = globalPositionCLHEP(localPos);
    return sinTilt(point);
  }

  double SiDetectorElement::sinTilt(const HepGeom::Point3D<double>& globalPos) const
  { 
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    // It is assumed that the global position is already in the plane of the element.

    // Angle between normal and radial vector.
    //HepGeom::Vector3D<double> refVector(globalPos.x(), globalPos.y(), 0);
    //return (refVector.cross(m_normal)).z()/refVector.mag();
    // or the equivalent
    return (globalPos.x() * m_normal.y() - globalPos.y() * m_normal.x()) / globalPos.perp();
  }

  double SiDetectorElement::sinStereo() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return sinStereoImpl();
  }

  double SiDetectorElement::sinStereoImpl() const
  {
    // Stereo is the angle between a refVector and a vector along the strip/pixel in eta direction.
    // I'm not sure how the sign should be defined. I've defined it here
    // with rotation sense respect to normal,
    // where normal is away from IP in barrel and in -ve z direction in endcap

    // In Barrel refVector = unit vector along z axis,
    // in endcap refVector = unit vector radial.
    //
    // sinStereo = (refVector cross stripAxis) . normal
    //           = (refVector cross etaAxis) . normal
    //           = refVector . (etaAxis cross normal)
    //           = refVector . phiAxis  
    //
    // in Barrel we use
    // sinStereo = refVector . phiAxis
    //           = phiAxis.z()
    //
    // in endcap we use
    // sinStereo = (refVector cross etaAxis) . normal
    //           = -(center cross etaAxis) . zAxis
    //           = (etaAxis cross center). z() 
    double sinStereo = 0.;
    sinStereo = -m_etaAxis.y();
    return sinStereo;
  }

  double SiDetectorElement::sinStereo(const Amg::Vector2D& localPos) const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    HepGeom::Point3D<double> point=globalPositionCLHEP(localPos);
    return sinStereoImpl(point);
  }

  double SiDetectorElement::sinStereo(const HepGeom::Point3D<double>& globalPos) const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return sinStereoImpl(globalPos);
  } 

  double SiDetectorElement::sinStereoImpl(const HepGeom::Point3D<double>& globalPos) const
  {
    //
    // sinStereo =  (refVector cross stripAxis) . normal
    //
    double sinStereo = 0.;
    if (m_design->shape() != TrackerDD::Trapezoid) {
      sinStereo = -m_etaAxis.y();
    } else { // trapezoid
      assert (minWidth() != maxWidth());
      double radius = width() * length() / (maxWidth() - minWidth());
      HepGeom::Vector3D<double> stripAxis = radius * m_etaAxisCLHEP + globalPos - m_centerCLHEP;
      sinStereo = (stripAxis.x() * m_normal.y() - stripAxis.y() * m_normal.x()) / stripAxis.mag();
    }
    return sinStereo;
  }

  bool 
  SiDetectorElement::isStereo() const 
  {
    if (!m_stereoCacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_stereoCacheValid) {
        determineStereo();
      }
    }

    return m_isStereo;
  }


  double
  SiDetectorElement::sinStereoLocal(const Amg::Vector2D& localPos) const
  {
    // The equation below will work for rectangle detectors as well in which 
    // case it will return 0. But we return zero immediately as there is no point doing the calculation.
    if (m_design->shape() == TrackerDD::Box) return 0.;
    double oneOverRadius = (maxWidth() - minWidth()) / (width() * length());
    double x = localPos[distPhi];
    double y = localPos[distEta];
    return -x*oneOverRadius / sqrt( (1+y*oneOverRadius)*(1+y*oneOverRadius) + x*oneOverRadius*x*oneOverRadius );
  }

  double
  SiDetectorElement::sinStereoLocal(const HepGeom::Point3D<double>& globalPos) const
  {
    return sinStereoLocal(localPosition(globalPos));
  }

  // Special method for SCT to retrieve the two ends of a "strip"
  std::pair<Amg::Vector3D,Amg::Vector3D> SiDetectorElement::endsOfStrip(const Amg::Vector2D& position) const
  {
    const std::pair<Amg::Vector2D,Amg::Vector2D> localEnds=
      m_design->endsOfStrip(position);
    return std::pair<Amg::Vector3D,Amg::Vector3D >(globalPosition(localEnds.first),
                                                   globalPosition(localEnds.second));
  }

  Trk::Surface& 
  SiDetectorElement::surface()
  {
    if (not m_surface) m_surface.set(std::make_unique<Trk::PlaneSurface>(*this));
    return *m_surface;
  }
  
  const Trk::Surface& 
  SiDetectorElement::surface() const
  {
    if (not m_surface) m_surface.set(std::make_unique<Trk::PlaneSurface>(*this));
    return *m_surface;
  }
  
  const std::vector<const Trk::Surface*>& SiDetectorElement::surfaces() const 
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_surfaces.size()) {
      // get this surface
      m_surfaces.push_back(&surface());
      // get the other side surface
      if (otherSide()) {
        m_surfaces.push_back(&(otherSide()->surface()));
      }
    }
    // return the surfaces
    return m_surfaces;
  }  
  
  const Trk::SurfaceBounds& 
  SiDetectorElement::bounds() const
  {
    return m_design->bounds();
  }
  
  // Get min/max or r, z,and phi
  // helper method only to be used for the cache construction
  // i.e inside updateCache
  void SiDetectorElement::getExtent(double& rMin, double& rMax, 
                                    double& zMin, double& zMax,
                                    double& phiMin, double& phiMax) const
  {
    HepGeom::Point3D<double> corners[4];
    getCorners(corners);

    bool first = true;

    double phiOffset = 0.;

    double radialShift = 0.;
    const HepGeom::Transform3D rShift = HepGeom::TranslateX3D(radialShift);//in local frame, radius is x

    for (int i = 0; i < 4; i++) {

      // m_tranform is already there as  part of the cache construction
      // This method seems to be used only as a helper for updateCache
      HepGeom::Point3D<double> globalPoint = m_transformCLHEP * corners[i];

      double rPoint = globalPoint.perp();
      double zPoint = globalPoint.z();
      double phiPoint = globalPoint.phi();
    
      // Use first point to initializa min/max values.
      if (first) {

        // Put phi in a range so that we are not near -180/+180 division.
        // Do this by adding an offset if phi > 90 CLHEP::deg or < -90 CLHEP::deg. 
        // This offset is later removed.
        if (phiPoint < -0.5 * M_PI) {
          phiOffset = -0.5 * M_PI;
        } else if (phiPoint > 0.5 * M_PI) {
          phiOffset = 0.5 * M_PI;
        }
        phiMin = phiMax = phiPoint - phiOffset;
        rMin = rMax = rPoint;
        zMin = zMax = zPoint;

      } else {
        phiPoint -= phiOffset;
        // put phi back in -M_PI < phi < +M_PI range
        if (phiPoint < -M_PI) phiPoint += 2. * M_PI;
        if (phiPoint > M_PI)  phiPoint -= 2. * M_PI;
        phiMin = std::min(phiMin, phiPoint);
        phiMax = std::max(phiMax, phiPoint);
        rMin = std::min(rMin, rPoint);
        rMax = std::max(rMax, rPoint);
        zMin = std::min(zMin, zPoint);
        zMax = std::max(zMax, zPoint);
      }
      first = false;
    }

    // put phi back in -M_PI < phi < +M_PI range
    phiMin += phiOffset;
    phiMax += phiOffset;
    if (phiMin < -M_PI) phiMin += 2. * M_PI;
    if (phiMin >  M_PI) phiMin -= 2. * M_PI;
    if (phiMax < -M_PI) phiMax += 2. * M_PI;
    if (phiMax >  M_PI) phiMax -= 2. * M_PI;

  }

  // Get eta/phi extent. Returns min/max eta and phi and r (for barrel)
  // or z (for endcap) Takes as input the vertex spread in z (+-deltaZ).
  // Gets 4 corners of the sensor and calculates eta phi for each corner
  // for both +/- vertex spread.  The returned phi is between -M_PI and M_PI
  // with the direction minPhi to maxPhi always in the positive sense,
  // so if the element extends across the -180/180 boundary then minPhi will
  // be greater than maxPhi.
  // void SiDetectorElement::getEtaPhiRegion(double deltaZ, double& etaMin, double& etaMax, double& phiMin, 
  //                                         double& phiMax, double& rz) const
  // {
  //   if (!m_cacheValid) {
  //     std::lock_guard<std::mutex> lock(m_mutex);
  //     if (!m_cacheValid) updateCache();
  //   }

  //   HepGeom::Point3D<double> corners[4];
  //   getCorners(corners);

  //   bool first = true;

  //   double phiOffset = 0.;

  //   for (int i = 0; i < 4; i++) {
  //     double etaMinPoint = 0.;
  //     double etaMaxPoint = 0.;
  //     double phiPoint = 0.;

  //     // Get the eta phi value for this corner.
  //     getEtaPhiPoint(corners[i], deltaZ, etaMinPoint, etaMaxPoint, phiPoint);

  //     if (first) { // Use the first point to initialize the min/max values.

  //       // Put phi in a range so that we are not near -180/+180 division.
  //       // Do this by adding an offset if phi > 90 CLHEP::deg or < -90 CLHEP::deg. 
  //       // This offset is later removed.
  //       if (phiPoint < -0.5 * M_PI) {
  //         phiOffset = -0.5 * M_PI;
  //       } else if  (phiPoint > 0.5 * M_PI) {
  //         phiOffset = 0.5 * M_PI;
  //       }
  //       phiMin = phiMax = phiPoint - phiOffset;
  //       etaMin = etaMinPoint;
  //       etaMax = etaMaxPoint;
  //     } else {
  //       phiPoint -= phiOffset;
  //       // put phi back in -M_PI < phi < +M_PI range
  //       if (phiPoint < -M_PI) phiPoint += 2. * M_PI;
  //       if (phiPoint >  M_PI) phiPoint -= 2. * M_PI;
  //       phiMin = std::min(phiMin, phiPoint);
  //       phiMax = std::max(phiMax, phiPoint);
  //       etaMin = std::min(etaMin, etaMinPoint);
  //       etaMax = std::max(etaMax, etaMaxPoint);
  //     }
  //     first = false;
  //   }

  //   // put phi back in -M_PI < phi < +M_PI range
  //   phiMin += phiOffset;
  //   phiMax += phiOffset;
  //   if (phiMin < -M_PI) phiMin += 2. * M_PI;
  //   if (phiMin >  M_PI) phiMin -= 2. * M_PI;
  //   if (phiMax < -M_PI) phiMax += 2. * M_PI;
  //   if (phiMax >  M_PI) phiMax -= 2. * M_PI;

  //   // Calculate rz = r (barrel) or z (endcap)
  //   // Use center of sensor ((0,0,0) in local coordinates) for determining this.
  //   //  HepGeom::Point3D<double> globalCenter = globalPosition(HepGeom::Point3D<double>(0,0,0));
  //   rz = center().perp(); // r
  // }

  // // Gets eta phi for a point given in local coordinates. deltaZ is specified to
  // // account for the vertex spread. phi is independent of this vertex
  // // spread. etaMax will correspond to zMin (-deltaZ) and etaMin will
  // // correspond to zMax (+deltaZ).
  // void SiDetectorElement::getEtaPhiPoint(const HepGeom::Point3D<double>&  point, double deltaZ, 
  //                                        double& etaMin, double& etaMax, double& phi) const
  // {
  //   // Get the point in global coordinates.
  //   HepGeom::Point3D<double> globalPoint = globalPosition(point);

  //   double r = globalPoint.perp();
  //   double z = globalPoint.z();
  
  //   double thetaMin = atan2(r,(z + deltaZ));
  //   etaMax = -log(tan(0.5 * thetaMin));
  //   double thetaMax = atan2(r,(z - deltaZ));
  //   etaMin = -log(tan(0.5 * thetaMax));
  
  //   phi = globalPoint.phi();
  // }

  void SiDetectorElement::getCorners(HepGeom::Point3D<double>* corners) const
  {
    // This makes the assumption that the forward SCT detectors are orientated such that 
    // the positive etaAxis corresponds to the top of the detector where the width is largest.
    // This is currently always the case.
    // For the SCT barrel and pixel detectors minWidth and maxWidth are the same and so should 
    // work for all orientations.

    double minWidth = m_design->minWidth();
    double maxWidth = m_design->maxWidth();
    double length   = m_design->length();
  
    // Lower left
    corners[0][distPhi] = -0.5 * minWidth;
    corners[0][distEta] = -0.5 * length;
    corners[0][distDepth] = 0.;

    // Lower right
    corners[1][distPhi] =  0.5 * minWidth;
    corners[1][distEta] = -0.5 * length;
    corners[1][distDepth] = 0.;

    // Upper Right
    corners[2][distPhi] = 0.5 * maxWidth;
    corners[2][distEta] = 0.5 * length;
    corners[2][distDepth] = 0.;

    // Upper left
    corners[3][distPhi] = -0.5 * maxWidth;
    corners[3][distEta] =  0.5 * length;
    corners[3][distDepth] = 0.;
  }

  SiIntersect
  SiDetectorElement::inDetector(const Amg::Vector2D& localPosition, 
                                double phiTol, double etaTol) const
  {
    return m_design->inDetector(localPosition, phiTol, etaTol);
  }

  
  SiIntersect 
  SiDetectorElement::inDetector(const HepGeom::Point3D<double>& globalPosition, double phiTol, double etaTol) const
  {
    return m_design->inDetector(localPosition(globalPosition), phiTol, etaTol);
  }

  bool 
  SiDetectorElement::nearBondGap(const Amg::Vector2D& localPosition, double etaTol) const
  {
    return m_design->nearBondGap(localPosition, etaTol);
  }

  bool
  SiDetectorElement::nearBondGap(const HepGeom::Point3D<double>& globalPosition, double etaTol) const
  {
    return m_design->nearBondGap(localPosition(globalPosition), etaTol);
  }  

  Amg::Vector2D
  SiDetectorElement::rawLocalPositionOfCell(const SiCellId& cellId) const
  {
    return m_design->localPositionOfCell(cellId);
  }

  Amg::Vector2D
  SiDetectorElement::rawLocalPositionOfCell(const Identifier& id) const
  {
    SiCellId cellId = cellIdFromIdentifier(id);
    return m_design->localPositionOfCell(cellId);
  }

  int 
  SiDetectorElement::numberOfConnectedCells(const SiCellId cellId) const
  {
    SiReadoutCellId readoutId = m_design->readoutIdOfCell(cellId);
    return m_design->numberOfConnectedCells(readoutId);
  }

  SiCellId 
  SiDetectorElement::connectedCell(const SiCellId cellId, int number) const
  {
    SiReadoutCellId readoutId = m_design->readoutIdOfCell(cellId);
    return m_design->connectedCell(readoutId, number);
  }


  SiCellId 
  SiDetectorElement::cellIdOfPosition(const Amg::Vector2D& localPosition) const
  {
    return m_design->cellIdOfPosition(localPosition);
  }

  Identifier
  SiDetectorElement::identifierOfPosition(const Amg::Vector2D& localPosition) const
  {
    SiCellId cellId = m_design->cellIdOfPosition(localPosition);
    return identifierFromCellId(cellId);
  }

  Identifier 
  SiDetectorElement::identifierFromCellId(const SiCellId& cellId) const
  {
    Identifier id; // Will be initialized in an invalid state.

    // If something fails it returns the id in an invalid state.

    if (cellId.isValid()) {

      const FaserSCT_ID* sctIdHelper = dynamic_cast<const FaserSCT_ID*>(getIdHelper());
      if (sctIdHelper) {
        id = sctIdHelper->strip_id(m_id, cellId.strip());
      }
    }
    return id;
  }

  SiCellId   
  SiDetectorElement::cellIdFromIdentifier(const Identifier& identifier) const
  {
    SiCellId cellId; // Initialized in invalid state.

    // If something fails it returns the cellId in an invalid state.

    if (identifier.is_valid()) {
  
      const FaserSCT_ID* sctIdHelper = dynamic_cast<const FaserSCT_ID*>(getIdHelper());
      if (sctIdHelper) {
        cellId =  SiCellId(sctIdHelper->strip(identifier));
      }
    }  
    return cellId;
  }

  bool
  SiDetectorElement::isInterface() const
  {
    const FaserSCT_ID* sctId = dynamic_cast<const FaserSCT_ID*>(getIdHelper());
    if (nullptr == sctId) return false;
    return ( (sctId->is_sct(m_id)) && (sctId->station(m_id) == 0) );
  }

  bool
  SiDetectorElement::isUpstream() const
  {
    const FaserSCT_ID* sctId = dynamic_cast<const FaserSCT_ID*>(getIdHelper());
    if (nullptr == sctId) return false;
    return ( (sctId->is_sct(m_id)) && (sctId->station(m_id) == 1) );
  }

  bool
  SiDetectorElement::isCentral() const
  {
    const FaserSCT_ID* sctId = dynamic_cast<const FaserSCT_ID*>(getIdHelper());
    if (nullptr == sctId) return false;
    return ( (sctId->is_sct(m_id)) && (sctId->station(m_id) == 2) );
  }

  bool
  SiDetectorElement::isDownstream() const
  {
    const FaserSCT_ID* sctId = dynamic_cast<const FaserSCT_ID*>(getIdHelper());
    if (nullptr == sctId) return false;
    return ( (sctId->is_sct(m_id)) && (sctId->station(m_id) == 3) );
  }


} // namespace TrackerDD
