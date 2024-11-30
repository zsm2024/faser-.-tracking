/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file NeutrinoDetectorElement.cxx
 * Implementation file for class NeutrinoDetectorElement
 * @author Grant Gorfine
 * Based on version developed by David Calvet.
**/

#include "NeutrinoReadoutGeometry/NeutrinoDetectorElement.h"

#include "NeutrinoIdentifier/EmulsionID.h"

#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelFaserUtilities/GeoAlignmentStore.h"
#include "FaserDetDescr/FaserDetectorID.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/PhysicalConstants.h" // for M_PI
#include "CLHEP/Units/SystemOfUnits.h"

#include "NeutrinoReadoutGeometry/NeutrinoDetectorDesign.h"


#include "NeutrinoReadoutGeometry/NeutrinoCommonItems.h"

#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"

#include <cmath>
#include <cassert>
#include <limits>

namespace NeutrinoDD {
using Trk::distEta;
using Trk::distPhi;
// using Trk::distDepth;

// Constructor with parameters:
NeutrinoDetectorElement::NeutrinoDetectorElement(const Identifier &id,
                                     const NeutrinoDetectorDesign *design,
                                     const GeoVFullPhysVol *geophysvol,
                                     const NeutrinoCommonItems * commonItems,
                                     const GeoAlignmentStore* geoAlignStore) :
  TrkDetElementBase(geophysvol),
  m_id(id),
  m_idHash(64000),
  m_design(design),
  m_commonItems(commonItems),
  m_prevInZ(nullptr),
  m_nextInZ(nullptr),
  m_cacheValid(false),
  m_firstTime(true),
  m_isEmulsion(true),
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
  m_minX=defaultMin;
  m_maxX=defaultMax;
  m_minY=defaultMin;
  m_maxY=defaultMax;
  m_minZ=defaultMin;
  m_maxZ=defaultMax;
  m_hitEta = m_design->etaAxis();
  m_hitPhi = m_design->phiAxis();
  m_hitDepth = m_design->depthAxis();
  ///
  
 commonConstructor();
}

void
NeutrinoDetectorElement::commonConstructor() 
{
  if (!m_id.is_valid()) throw std::runtime_error("NeutrinoDetectorElement: Invalid identifier");

  // Set booleans for wether we are veto/trigger/preshower
  m_isEmulsion = getIdHelper()->is_emulsion(m_id);
  if (!m_isEmulsion)
  {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Element id is not for emulsion" << endmsg;
  }
  
  // // Set IdHash.
  const EmulsionID* emulsionId = dynamic_cast<const EmulsionID* >(getIdHelper());
  m_idHash = emulsionId->film_hash(m_id);
  
  if (!m_idHash.is_valid()) throw std::runtime_error("NeutrinoDetectorElement: Unable to set IdentifierHash");

  // Increase the reference count of the NeutrinoDetectorDesign objects.
  m_design->ref();

  // Increase the reference count of the NeutrinoCommonItems objects.
  m_commonItems->ref();

  // Should we reference count the geophysvol as well?

}


// Destructor:
NeutrinoDetectorElement::~NeutrinoDetectorElement()
{
  // The design is reference counted so that it will not be deleted until the last element is deleted.
  m_design->unref();

  m_commonItems->unref();
}

bool NeutrinoDetectorElement::isEmulsion() const
{
  return m_isEmulsion;
}


void 
NeutrinoDetectorElement::updateCache() const
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  bool firstTimeTmp = m_firstTime;
  m_firstTime = false;
  m_cacheValid = true;
  
  const GeoTrf::Transform3D & geoTransform = transformHit();
     
  HepGeom::Point3D<double> centerGeoModel(0., 0., 0.);
  m_centerCLHEP = Amg::EigenTransformToCLHEP(geoTransform) * centerGeoModel;
  m_center = Amg::Vector3D(m_centerCLHEP[0],m_centerCLHEP[1],m_centerCLHEP[2]);
  
  // (In ATLAS:)
  // Determine directions depth, eta and phi axis in reconstruction local frame
  // ie depth away from interaction point
  //    phi in direction of increasing phi
  //    eta in direction of increasing z in barrel, and increasing r in endcap
  //
  // depthAxis, phiAxis, and etaAxis are defined to be x,y,z respectively for all detectors for hit local frame.
  // depthAxis, phiAxis, and etaAxis are defined to be z,x,y respectively for all detectors for reco local frame.
  //
  // In FASER:
  // depthAxis, phiAxis and etaAxis are defined to be z,x,y respectively for all detectors in hit local AND reco local frames
  // This is accomplished simply by the fixed initialization of m_hitEta, m_hitPhi and m_hitDepth in NeutrinoDetectorDesign
  //

  static const HepGeom::Vector3D<double> localAxes[3] = {
    HepGeom::Vector3D<double>(1,0,0),
    HepGeom::Vector3D<double>(0,1,0),
    HepGeom::Vector3D<double>(0,0,1)
  };
  
  static const HepGeom::Vector3D<double> & localRecoPhiAxis = localAxes[distPhi];     // Defined to be same as x axis
  static const HepGeom::Vector3D<double> & localRecoEtaAxis = localAxes[distEta];     // Defined to be same as y axis
  static const HepGeom::Vector3D<double> & localRecoDepthAxis = localAxes[distDepth]; // Defined to be same as z axis
  
  // We only need to calculate the rough orientation once.
  //For it to change would require extreme unrealistic misalignment changes.
  if (firstTimeTmp) {
    // Determine the unit vectors in global frame
        
    const HepGeom::Vector3D<double> &geoModelPhiAxis = localAxes[m_hitPhi];
    const HepGeom::Vector3D<double> &geoModelEtaAxis = localAxes[m_hitEta];
    const HepGeom::Vector3D<double> &geoModelDepthAxis = localAxes[m_hitDepth];

    HepGeom::Vector3D<double> globalDepthAxis(Amg::EigenTransformToCLHEP(geoTransform) * geoModelDepthAxis);
    HepGeom::Vector3D<double> globalPhiAxis(Amg::EigenTransformToCLHEP(geoTransform) * geoModelPhiAxis);
    HepGeom::Vector3D<double> globalEtaAxis(Amg::EigenTransformToCLHEP(geoTransform) * geoModelEtaAxis);

    // unit radial vector
    // HepGeom::Vector3D<double> unitR(m_center.x(), m_center.y(), 0.);
     
    // unitR.setMag(1.);

    // HepGeom::Vector3D<double> nominalEta;
    // HepGeom::Vector3D<double> nominalNormal;
    // HepGeom::Vector3D<double> nominalPhi(-unitR.y(), unitR.x(), 0);
  
    // In Barrel like geometry, the etaAxis is along increasing z, and normal is in increasing radial direction.
    // In Endcap like geometry, the etaAxis is along increasing r, and normal is in decreasing z direction,
    // We base whether it is barrel like or endcap like by the orientation of the local z axis of the 
    // the element. This allows the use of endcap identifiers in a TB setup. A warning message is issued however if
    // the orientation and identifier are not consistent (ie a barrel like orientation with an endcap identifier).

    // bool barrelLike = true;
    // nominalEta.setZ(1);
    // if (std::abs(globalEtaAxis.dot(nominalEta)) < 0.5) { // Check that it is in roughly the right direction.
      // barrelLike = false;
    // }   

    // if (isBarrel() && !barrelLike) {      
    //   if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Element has endcap like orientation with barrel identifier."
    //                                               << endmsg;
    // } else if (!isBarrel() && barrelLike) {
    //   if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Element has barrel like orientation with endcap identifier."
    //                                               << endmsg;
    // }
    
    // if (barrelLike) {
    //   nominalEta.setZ(1);
    //   nominalNormal =  unitR;
    // } else { // endcap like
      // nominalNormal.setZ(-1);
      // nominalEta = unitR;
    // }

    // Determine if axes are to have their directions swapped.

    //
    // Depth axis.
    //
    // double depthDir = globalDepthAxis.dot(nominalNormal);
    // m_depthDirection = true;
    // if (depthDir < 0) {
      // if (m_design->depthSymmetric()) {
        // m_depthDirection = false;
      // } else {
      //   if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to swap local depth axis." << endmsg;
      // }
    // }
    // if (std::abs(depthDir) < 0.5) { // Check that it is in roughly the right direction.
    //   msg(MSG::ERROR) << "Orientation of local depth axis does not follow correct convention." << endmsg;
    //   // throw std::runtime_error("Orientation of local depth axis does not follow correct convention.");
    //   m_depthDirection = true; // Don't swap.
    // }
    
    //
    // Phi axis (unclear how to handle for FASER - never swap for now)
    //
    // double phiDir = globalPhiAxis.dot(nominalPhi);
    // m_phiDirection = true;
    // if (phiDir < 0) {
    //   if (m_design->phiSymmetric()) {
    //     m_phiDirection = false;
    //   } else {
    //     if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to swap local xPhi axis." << endmsg;
    //   }
    // }
    
    // if (std::abs(phiDir) < 0.5) { // Check that it is in roughly the right direction.
    //   msg(MSG::ERROR) << "Orientation of local xPhi axis does not follow correct convention." << endmsg;
    //   // throw std::runtime_error("Orientation of local xPhi axis does not follow correct convention.");
    //   m_phiDirection = true; // Don't swap.
    // }
    
    //
    // Eta axis (unclear how to handle for FASER - never swap for now)
    //
    // double etaDir = globalEtaAxis.dot(nominalEta);
    // m_etaDirection = true;
    // if (etaDir < 0) {
    //   if (m_design->etaSymmetric()) {
    //     m_etaDirection = false;
    //   } else {
    //     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to swap local xEta axis." << endmsg;
    //   }
    // }
    // if (std::abs(etaDir) < 0.5) { // Check that it is in roughly the right direction.
    //   msg(MSG::ERROR) << "Orientation of local xEta axis does not follow correct convention." << endmsg;
    //   // throw std::runtime_error("Orientation of local xEta axis does not follow correct convention.");
    //   m_etaDirection = true; // Don't swap
    // }   

  } // end if (firstTimeTmp)
  


  m_transformCLHEP = Amg::EigenTransformToCLHEP(geoTransform) * recoToHitTransform();
  //m_transform = m_commonItems->solenoidFrame() * geoTransform * recoToHitTransform();
  m_transform = Amg::CLHEPTransformToEigen(m_transformCLHEP);
  
  // Check that local frame is right-handed. (ie transform has no reflection)
  // This can be done by checking that the determinant is >0.
  // if (firstTimeTmp) { // Only need to check this once.
  //   HepGeom::Transform3D & t = m_transformCLHEP;
  //   double det = t(0,0) * (t(1,1)*t(2,2) - t(1,2)*t(2,1)) -
  //                t(0,1) * (t(1,0)*t(2,2) - t(1,2)*t(2,0)) +
  //                t(0,2) * (t(1,0)*t(2,1) - t(1,1)*t(2,0));
  //   if (det < 0) {
    //   if (m_design->depthSymmetric()) {
        // if (msgLvl(MSG::DEBUG))
        //   msg(MSG::DEBUG) << "Local frame is left-handed, Swapping depth axis to make it right handed."
        //                   << endmsg;
        // m_depthDirection = !m_depthDirection;
        // m_transformCLHEP = Amg::EigenTransformToCLHEP(geoTransform) * recoToHitTransform();
        // m_transform = Amg::CLHEPTransformToEigen(m_transformCLHEP);
        //m_transform = m_commonItems->solenoidFrame() * geoTransform * recoToHitTransform();
    //    } else {
    //    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Local frame is left-handed." << endmsg;
    //   }
    // }
  // }
  
  
  // Initialize various cached members
  // The unit vectors
  HepGeom::Vector3D<double> normalCLHEP = m_transformCLHEP * localRecoDepthAxis;
  m_normal = Amg::Vector3D( normalCLHEP[0], normalCLHEP[1], normalCLHEP[2]);
  

  m_phiAxisCLHEP = m_transformCLHEP * localRecoPhiAxis;
  m_etaAxisCLHEP = m_transformCLHEP * localRecoEtaAxis;

  m_phiAxis = Amg::Vector3D(m_phiAxisCLHEP[0],m_phiAxisCLHEP[1],m_phiAxisCLHEP[2]);
  m_etaAxis = Amg::Vector3D(m_etaAxisCLHEP[0],m_etaAxisCLHEP[1],m_etaAxisCLHEP[2]);

  getExtent(m_minX, m_maxX, m_minY, m_maxY, m_minZ, m_maxZ);


  // Determin isStereo
//   if (firstTimeTmp) {
//     if (isSCT() && m_otherSide) {
//       double sinStereoThis = std::abs(sinStereo());
//       double sinStereoOther = std::abs(m_otherSide->sinStereo());
//       if (sinStereoThis == sinStereoOther) {
//         // If they happend to be equal then set side0 as axial and side1 as stereo.
//         const SCT_ID* sctId = dynamic_cast<const SCT_ID *>(getIdHelper());
//         if (sctId) {
//           int side = sctId->side(m_id);
//           m_isStereo = (side == 1);
//         }
//       } else {
//         // set the stereo side as the one with largest absolute sinStereo.
//         m_isStereo = (sinStereoThis > sinStereoOther);
//       }
//     } else {
//       m_isStereo = false;
//     }
//   }    
}


const GeoTrf::Transform3D &
NeutrinoDetectorElement::transformHit() const
{
  if (m_geoAlignStore) {
    const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getAbsPosition(getMaterialGeom());
    if (ptrXf) return *ptrXf;
  }
  return getMaterialGeom()->getAbsoluteTransform();
}

const Amg::Transform3D &
NeutrinoDetectorElement::transform() const
{
  if (!m_cacheValid){                                                                                               
    std::lock_guard<std::recursive_mutex> lock(m_mutex);                                                            
    if (!m_cacheValid) updateCache();                                                                               
  }  
  return m_transform;
}

const HepGeom::Transform3D &
NeutrinoDetectorElement::transformCLHEP() const
{
  //stuff to get the CLHEP version of the local to global transform.
   if (!m_cacheValid){                                                                                               
    std::lock_guard<std::recursive_mutex> lock(m_mutex);                                                            
    if (!m_cacheValid) updateCache();                                                                               
  }   
  return m_transformCLHEP;
}

const HepGeom::Transform3D 
NeutrinoDetectorElement::defTransformCLHEP() const
{
  if (m_geoAlignStore) {
    const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getDefAbsPosition(getMaterialGeom());
    if (ptrXf) return Amg::EigenTransformToCLHEP(*ptrXf) * recoToHitTransform();
  }
  return Amg::EigenTransformToCLHEP(getMaterialGeom()->getDefAbsoluteTransform()) * recoToHitTransform();
}  
   
const Amg::Transform3D 
NeutrinoDetectorElement::defTransform() const
{
  HepGeom::Transform3D tmpTransform = defTransformCLHEP();
  return Amg::CLHEPTransformToEigen(tmpTransform);
}

const HepGeom::Transform3D 
NeutrinoDetectorElement::recoToHitTransform() const
{

  // Determine the reconstruction local (LocalPosition) to global transform.

  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  if (m_firstTime) updateCache();

  // global = transform * recoLocal
  //        = transformHit * hitLocal
  //        = transformHit * recoToHitTransform * recoLocal
  //
  // (In ATLAS:) recoToHitTransform takes recoLocal to hitLocal
  // x,y,z -> y,z,x 
  // equiv to a rotation around Y of 90 deg followed by a rotation around X of 90deg
  // 
  // recoToHit is static as it needs to be calculated once only.
  // We use the HepGeom::Transform3D constructor which takes one coordinates system to another where the
  // coordinate system is defined by it center and two axes.
  // distPhi, distEta are the reco local axes and hitPhi and hitEta are the hit local axes.
  // It assume phi, eta, depth makes a right handed system which is the case.
  static const HepGeom::Vector3D<double> localAxes[3] = {
    HepGeom::Vector3D<double>(1,0,0),
    HepGeom::Vector3D<double>(0,1,0),
    HepGeom::Vector3D<double>(0,0,1)
  };
  //static 
    
  const HepGeom::Transform3D recoToHit(HepGeom::Point3D<double>(0,0,0),localAxes[distPhi],localAxes[distEta],
                                       HepGeom::Point3D<double>(0,0,0),localAxes[m_hitPhi],localAxes[m_hitEta]);
  
  // Swap direction of axis as appropriate
  CLHEP::Hep3Vector scale(1,1,1);
  if (!m_phiDirection)   scale[distPhi]   = -1;
  if (!m_etaDirection)   scale[distEta]   = -1;
  if (!m_depthDirection) scale[distDepth] = -1;
  return recoToHit * HepGeom::Scale3D(scale[0],scale[1],scale[2]);
}

const Amg::Transform3D &
NeutrinoDetectorElement::filmTransform() const
{
  return transform();
  // return  (isModuleFrame()) ?  transform() : m_otherSide->transform();
}

  Amg::Transform3D
  NeutrinoDetectorElement::defFilmTransform() const
{
  return defTransform();
//  return  (isModuleFrame()) ? defTransform() : m_otherSide->defTransform();
}  


// Take a transform in the local reconstruction and return it in the module frame
// For a given transform l in frame A. The equivalent transform in frame B is
//  B.inverse() * A * l * A.inverse() * B
// Here A is the local to global transform of the element and B is the local to global
// transform of the module.
// If we are already in the module frame then there is nothing to do, we just return the
// transform that is input. Otherwise we use the above formula.
Amg::Transform3D 
NeutrinoDetectorElement::localToBaseFrame(const Amg::Transform3D & localTransform) const
{
  // if (isModuleFrame()) {
    return localTransform;
  // } else {
  //   return m_otherSide->transform().inverse() * transform() * localTransform * transform().inverse() *  m_otherSide->transform();
  // }
}

Amg::Transform3D 
NeutrinoDetectorElement::localToBaseTransform() const
{
  // if (isModuleFrame()) {
    return Amg::Transform3D(); // Identity
  // } else {
  //   return  m_otherSide->transform().inverse() * transform();
  // }
}
      

// bool 
// NeutrinoDetectorElement::isModuleFrame() const
// {
//   // The module frame is the axial side.
//   // NB isStereo returns false for the pixel and so
//   // isModuleFrame is always true for the pixel.

//   return !isStereo();
// }


const Amg::Vector3D & 
NeutrinoDetectorElement::center() const
{
  if (!m_cacheValid){                                                                                               
    std::lock_guard<std::recursive_mutex> lock(m_mutex);                                                            
    if (!m_cacheValid) updateCache();                                                                               
  }  
  return m_center;
}

const Amg::Vector3D & 
NeutrinoDetectorElement::normal() const
{
  if (!m_cacheValid){                                                                                               
    std::lock_guard<std::recursive_mutex> lock(m_mutex);                                                            
    if (!m_cacheValid) updateCache();                                                                               
  }  
  return m_normal;
}

const HepGeom::Vector3D<double> & 
NeutrinoDetectorElement::etaAxisCLHEP() const
{
  if (!m_cacheValid){                                                                                               
    std::lock_guard<std::recursive_mutex> lock(m_mutex);                                                            
    if (!m_cacheValid) updateCache();                                                                               
  }  
  return m_etaAxisCLHEP;
}

const HepGeom::Vector3D<double> & 
NeutrinoDetectorElement::phiAxisCLHEP() const
{
  if (!m_cacheValid){                                                                                               
    std::lock_guard<std::recursive_mutex> lock(m_mutex);                                                            
    if (!m_cacheValid) updateCache();                                                                               
  }  
  return m_phiAxisCLHEP;
}

const Amg::Vector3D & 
NeutrinoDetectorElement::etaAxis() const
{
  if (!m_cacheValid){                                                                                               
    std::lock_guard<std::recursive_mutex> lock(m_mutex);                                                            
    if (!m_cacheValid) updateCache();                                                                               
  }  
  return m_etaAxis;
}

const Amg::Vector3D & 
NeutrinoDetectorElement::phiAxis() const
{
  if (!m_cacheValid){                                                                                               
    std::lock_guard<std::recursive_mutex> lock(m_mutex);                                                            
    if (!m_cacheValid) updateCache();                                                                               
  }  
  return m_phiAxis;
}

Amg::Vector2D
NeutrinoDetectorElement::hitLocalToLocal(double xEta, double xPhi) const  // Will change order to phi,eta
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  if (!m_cacheValid) updateCache();
  if (!m_etaDirection) xEta = -xEta;
  if (!m_phiDirection) xPhi = -xPhi;
  return Amg::Vector2D(xPhi, xEta);
}

HepGeom::Point3D<double>
NeutrinoDetectorElement::hitLocalToLocal3D(const HepGeom::Point3D<double> & hitPosition) const
{
  // Equiv to transform().inverse * transformHit() * hitPosition
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  if (!m_cacheValid) updateCache();
  double xDepth = hitPosition[m_hitDepth];
  double xPhi = hitPosition[m_hitPhi];
  double xEta = hitPosition[m_hitEta];
  if (!m_depthDirection) xDepth = -xDepth;
  if (!m_phiDirection) xPhi = -xPhi;
  if (!m_etaDirection) xEta = -xEta;
  return HepGeom::Point3D<double>(xPhi, xEta, xDepth);
}

Trk::Surface & 
NeutrinoDetectorElement::surface()
{
  if (not m_surface) m_surface.set(std::make_unique<Trk::PlaneSurface>(*this));
  return *m_surface;
}
  
const Trk::Surface & 
NeutrinoDetectorElement::surface() const
{
  if (not m_surface) m_surface.set(std::make_unique<Trk::PlaneSurface>(*this));
  return *m_surface;
}
  
const std::vector<const Trk::Surface*>& NeutrinoDetectorElement::surfaces() const 
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (!m_surfaces.size()) {
        // get this surface
        m_surfaces.push_back(&surface());
        // get the other side surface
        // if (otherSide()) {
        //     m_surfaces.push_back(&(otherSide()->surface()));
        // }
    }
    // return the surfaces
    return m_surfaces;
}  
  
const Trk::SurfaceBounds & 
NeutrinoDetectorElement::bounds() const
{
  return m_design->bounds();
}
  
// Get min/max or r, z,and phi
void NeutrinoDetectorElement::getExtent(double &xMin, double &xMax, 
                                  double &yMin, double &yMax,
                                  double &zMin, double &zMax) const
{

  HepGeom::Point3D<double> corners[4];
  getCorners(corners);

  bool first = true;

  // double phiOffset = 0.;

  double radialShift = 0.;
  const HepGeom::Transform3D rShift = HepGeom::TranslateX3D(radialShift);//in local frame, radius is x

  for (int i = 0; i < 4; i++) {

    // if (testDesign) corners[i].transform(rShift);

    HepGeom::Point3D<double> globalPoint = globalPosition(corners[i]);

    // double rPoint = globalPoint.perp();
    double xPoint = globalPoint.x();
    double yPoint = globalPoint.y();
    double zPoint = globalPoint.z();
    // double phiPoint = globalPoint.phi();
    
    // Use first point to initializa min/max values.
    if (first) {

      // // Put phi in a range so that we are not near -180/+180 division.
      // // Do this by adding an offset if phi > 90 CLHEP::deg or < -90 CLHEP::deg. 
      // // This offset is later removed.
      // if (phiPoint < -0.5 * M_PI) {
      //   phiOffset = -0.5 * M_PI;
      // } else if  (phiPoint > 0.5 * M_PI) {
      //   phiOffset = 0.5 * M_PI;
      // }
      // phiMin = phiMax = phiPoint - phiOffset;
      // rMin = rMax = rPoint;
      xMin = xMax = xPoint;
      yMin = yMax = yPoint;
      zMin = zMax = zPoint;

    } else {
      // phiPoint -= phiOffset;
      // // put phi back in -M_PI < phi < +M_PI range
      // if (phiPoint < -M_PI) phiPoint += 2. * M_PI;
      // if (phiPoint > M_PI)  phiPoint -= 2. * M_PI;
      // phiMin = std::min(phiMin, phiPoint);
      // phiMax = std::max(phiMax, phiPoint);
      // rMin = std::min(rMin, rPoint);
      // rMax = std::max(rMax, rPoint);
      xMin = std::min(xMin, xPoint);
      xMax = std::max(xMax, xPoint);
      yMin = std::min(yMin, yPoint);
      yMax = std::max(yMax, yPoint);      
      zMin = std::min(zMin, zPoint);
      zMax = std::max(zMax, zPoint);
    }
    first = false;
  }

  // // put phi back in -M_PI < phi < +M_PI range
  // phiMin += phiOffset;
  // phiMax += phiOffset;
  // if (phiMin < -M_PI) phiMin += 2. * M_PI;
  // if (phiMin > M_PI)  phiMin -= 2. * M_PI;
  // if (phiMax < -M_PI) phiMax += 2. * M_PI;
  // if (phiMax > M_PI)  phiMax -= 2. * M_PI;

}



// Get eta/phi extent. Returns min/max eta and phi and r (for barrel)
// or z (for endcap) Takes as input the vertex spread in z (+-deltaZ).
// Gets 4 corners of the sensor and calculates eta phi for each corner
// for both +/- vertex spread.  The returned phi is between -M_PI and M_PI
// with the direction minPhi to maxPhi always in the positive sense,
// so if the element extends across the -180/180 boundary then minPhi will
// be greater than maxPhi.
// void NeutrinoDetectorElement::getEtaPhiRegion(double deltaZ, double &etaMin, double &etaMax, double &phiMin, 
//                                         double &phiMax, double &rz) const
// {
//   std::lock_guard<std::recursive_mutex> lock(m_mutex);
//   if (!m_cacheValid) updateCache();

//   HepGeom::Point3D<double> corners[4];
//   getCorners(corners);

//   bool first = true;

//   double phiOffset = 0.;

//   for (int i = 0; i < 4; i++) {
//     double etaMinPoint;
//     double etaMaxPoint;
//     double phiPoint;

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
//       if (phiPoint > M_PI)  phiPoint -= 2. * M_PI;
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
//   if (phiMin > M_PI)  phiMin -= 2. * M_PI;
//   if (phiMax < -M_PI) phiMax += 2. * M_PI;
//   if (phiMax > M_PI)  phiMax -= 2. * M_PI;

//   // Calculate rz = r (barrel) or z (endcap)
//   // Use center of sensor ((0,0,0) in local coordinates) for determining this.
//   //  HepGeom::Point3D<double> globalCenter = globalPosition(HepGeom::Point3D<double>(0,0,0));
//   if (isBarrel()) {
//     rz = center().perp(); // r
//   } else {
//     rz = center().z();  // z
//   }

// }

// Gets eta phi for a point given in local coordinates. deltaZ is specified to
// account for the vertex spread. phi is independent of this vertex
// spread. etaMax will correspond to zMin (-deltaZ) and etaMin will
// correspond to zMax (+deltaZ).
// void NeutrinoDetectorElement::getEtaPhiPoint(const HepGeom::Point3D<double> & point, double deltaZ, 
//                                        double &etaMin, double &etaMax, double &phi) const
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

void NeutrinoDetectorElement::getCorners(HepGeom::Point3D<double> *corners) const
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  if (!m_cacheValid) updateCache();

  // This makes the assumption that the forward SCT detectors are orientated such that 
  // the positive etaAxis corresponds to the top of the detector where the width is largest.
  // This is currently always the case.
  // For the SCT barrel and pixel detectors minWidth and maxWidth are the same and so should 
  // work for all orientations.

  // double minWidth = m_design->minWidth();
  // double maxWidth = m_design->maxWidth();
  double width    = m_design->width();
  double height   = m_design->height();
  
  // Lower left
  corners[0][distPhi] = -0.5 * width;
  corners[0][distEta] = -0.5 * height;
  corners[0][distDepth] = 0.;

  // Lower right
  corners[1][distPhi] =  0.5 * width;
  corners[1][distEta] = -0.5 * height;
  corners[1][distDepth] = 0.;

  // Upper Right
  corners[2][distPhi] = 0.5 * width;
  corners[2][distEta] = 0.5 * height;
  corners[2][distDepth] = 0.;

  // Upper left
  corners[3][distPhi] = -0.5 * width;
  corners[3][distEta] =  0.5 * height;
  corners[3][distDepth] = 0.;
}

NeutrinoIntersect
NeutrinoDetectorElement::inDetector(const Amg::Vector2D & localPosition, 
                              double phiTol, double etaTol) const
{
  return m_design->inDetector(localPosition, phiTol, etaTol);
}

  
NeutrinoIntersect 
NeutrinoDetectorElement::inDetector(const HepGeom::Point3D<double> & globalPosition, double phiTol, double etaTol) const
{
  return m_design->inDetector(localPosition(globalPosition), phiTol, etaTol);
}

// bool 
// NeutrinoDetectorElement::nearBondGap(Amg::Vector2D localPosition, double etaTol) const
// {
//   return m_design->nearBondGap(localPosition, etaTol);
// }

// bool
// NeutrinoDetectorElement::nearBondGap(HepGeom::Point3D<double> globalPosition, double etaTol) const
// {
//   return m_design->nearBondGap(localPosition(globalPosition), etaTol);
// }  

// Amg::Vector2D
// NeutrinoDetectorElement::rawLocalPositionOfCell(const SiCellId &cellId) const
// {
//   return m_design->localPositionOfCell(cellId);
// }

// Amg::Vector2D
// NeutrinoDetectorElement::rawLocalPositionOfCell(const Identifier & id) const
// {
//   SiCellId cellId = cellIdFromIdentifier(id);
//   return m_design->localPositionOfCell(cellId);
// }

// int 
// NeutrinoDetectorElement::numberOfConnectedCells(const SiCellId cellId) const
// {
//   SiReadoutCellId readoutId = m_design->readoutIdOfCell(cellId);
//   return m_design->numberOfConnectedCells(readoutId);
// }

// SiCellId 
// NeutrinoDetectorElement::connectedCell(const SiCellId cellId, int number) const
// {
//   SiReadoutCellId readoutId = m_design->readoutIdOfCell(cellId);
//   return m_design->connectedCell(readoutId, number);
// }


// SiCellId 
// NeutrinoDetectorElement::cellIdOfPosition(const Amg::Vector2D &localPosition) const
// {
//   return m_design->cellIdOfPosition(localPosition);
// }

// Identifier
// NeutrinoDetectorElement::identifierOfPosition(const Amg::Vector2D &localPosition) const
// {
//   SiCellId cellId = m_design->cellIdOfPosition(localPosition);
//   return identifierFromCellId(cellId);
// }

// Identifier 
// NeutrinoDetectorElement::identifierFromCellId(const SiCellId & cellId) const
// {
//   Identifier id; // Will be initialized in an invalid state.

//   // If something fails it returns the id in an invalid state.

//   if (cellId.isValid()) {

//     if (isPixel()) {
//       const PixelID * pixelIdHelper = static_cast<const PixelID *>(getIdHelper());
//       if (pixelIdHelper) {
//         id = pixelIdHelper->pixel_id(m_id, cellId.phiIndex(), cellId.etaIndex());
//       }
//     } else {
//       const SCT_ID * sctIdHelper = static_cast<const SCT_ID *>(getIdHelper());
//       if (sctIdHelper) {
//         id = sctIdHelper->strip_id(m_id, cellId.strip());
//       }
//     }
//   }

//   return id;
// }

// SiCellId   
// NeutrinoDetectorElement::cellIdFromIdentifier(const Identifier & identifier) const
// {
//   SiCellId cellId; // Initialized in invalid state.

//   // If something fails it returns the cellId in an invalid state.

//   if (identifier.is_valid()) {
  
//     if (isPixel()) {
//       const PixelID * pixelIdHelper = static_cast<const PixelID *>(getIdHelper());
//       if (pixelIdHelper) {
//         cellId = SiCellId(pixelIdHelper->phi_index(identifier), pixelIdHelper->eta_index(identifier));
//       }
//     } else {
//       const SCT_ID * sctIdHelper = static_cast<const SCT_ID *>(getIdHelper());
//       if (sctIdHelper) {
//         cellId =  SiCellId(sctIdHelper->strip(identifier));
//       }
//     }
//   }
  
//   return cellId;
// }

} // namespace NeutrinoDD
