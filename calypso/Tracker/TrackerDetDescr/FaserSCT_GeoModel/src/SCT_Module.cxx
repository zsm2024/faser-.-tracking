/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Module.h"

#include "SCT_Identifier.h"
#include "SCT_MaterialManager.h"

#include "SCT_GeometryManager.h"
#include "SCT_BarrelModuleParameters.h"
#include "SCT_GeneralParameters.h"

#include "SCT_Sensor.h"
#include "SCT_Hybrid.h"
#include "SCT_Pigtail.h"
#include "SCT_InnerSide.h"
#include "SCT_OuterSide.h"
#include "SCT_BaseBoard.h"

#include "TrackerReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"
// 8th Aug 2005 S.Mima modified.
#include "GeoModelKernel/GeoShapeSubtraction.h"

#include <cmath>

SCT_Module::SCT_Module(const std::string & name,
                       TrackerDD::SCT_DetectorManager* detectorManager,
                       const SCT_GeometryManager* geometryManager,
                       SCT_MaterialManager* materials)
: SCT_UniqueComponentFactory(name, detectorManager, geometryManager, materials),
    m_innerSide(0),
    m_outerSide(0),
    m_baseBoard(0)

{
  getParameters();
  m_logVolume = preBuild();
}


SCT_Module::~SCT_Module()
{
  // Clean up
  delete m_baseBoardPos; // 6th Apr 2005 S.Mima modified.
  delete m_innerSidePos;
  delete m_outerSidePos;

  delete m_innerSide;
  delete m_outerSide;
  delete m_baseBoard;

  delete m_env1RefPointVector;
  delete m_env2RefPointVector;
  delete m_env3RefPointVector;
}


void
SCT_Module::getParameters()
{
  const SCT_BarrelModuleParameters       * parameters = m_geometryManager->barrelModuleParameters();
  const SCT_GeneralParameters     * generalParameters = m_geometryManager->generalParameters();

  m_safety       = generalParameters->safety();

  m_sensorGap    = parameters->moduleSensorToSensorGap();
  
  m_stereoAngle = parameters->moduleStereoAngle();
  int      sign = parameters->moduleStereoUpperSign();
  m_upperSide   = parameters->moduleUpperSideNumber();
  m_stereoInner = -0.5*sign*m_stereoAngle;
  m_stereoOuter =  0.5*sign*m_stereoAngle;

  m_baseBoardOffsetY = parameters->baseBoardOffsetY();
  m_baseBoardOffsetZ = parameters->baseBoardOffsetZ();
}


const GeoLogVol * 
SCT_Module::preBuild()
{
  // Create child components
  m_outerSide = new SCT_OuterSide("OuterSide", m_detectorManager, m_geometryManager, m_materials);
  m_baseBoard = new SCT_BaseBoard("BaseBoard", m_detectorManager, m_geometryManager, m_materials);
  m_innerSide = new SCT_InnerSide("InnerSide", m_detectorManager, m_geometryManager, m_materials);

  //
  // We have 2 envelopes.
  // 1. It contains two sensors and baseBoard.
  // 2. It contains hybrid and pigtail.
  //
  // See "doc/cornerName.ps".
  //

  // We need to take into account the safety incorporated in the inner and outer side 
  // envelopes. 
  double sensorWidth   =  m_innerSide->sensor()->width() + m_safety;
  double sensorLength  =  m_innerSide->sensor()->length() + m_safety;
  double innerSideHybridWidth   =  m_innerSide->hybrid()->width() + m_safety;
  double innerSideHybridLength  =  m_innerSide->hybrid()->length() + m_safety;
  double outerSideHybridWidth   =  m_outerSide->hybrid()->width() + m_safety;
  double outerSideHybridLength  =  m_outerSide->hybrid()->length() + m_safety;
  double baseBoardWidth         =  m_baseBoard->width() + m_safety;
  double baseBoardLength        =  m_baseBoard->length() + m_safety;
  double outerSidePigtailLength =  m_outerSide->pigtail()->length() + m_safety;


  // Define constants for convenience.
  // for corner of outer side sensor.
  GeoTrf::Vector3D a(0.0, 0.5 * sensorWidth, 0.5 * sensorLength);
  GeoTrf::Vector3D b(0.0, -0.5 * sensorWidth, a.z());
  GeoTrf::Vector3D c(0.0, b.y(),-0.5 * sensorLength);
  GeoTrf::Vector3D d(0.0, a.y(), c.z());

  // for corner of inner side sensor.
  GeoTrf::Vector3D e(0.0, a.y(), a.z());
  GeoTrf::Vector3D f(0.0, b.y(), b.z());
  GeoTrf::Vector3D g(0.0, c.y(), c.z());
  GeoTrf::Vector3D h(0.0, d.y(), d.z());

  // for corner of base board.
  GeoTrf::Vector3D u(0.0,
                      m_baseBoardOffsetY + 0.5*baseBoardWidth,
                      m_baseBoardOffsetZ + 0.5*baseBoardLength);
  GeoTrf::Vector3D v(0.0, m_baseBoardOffsetY - 0.5*baseBoardWidth, u.z());
  GeoTrf::Vector3D w(0.0, v.y(), m_baseBoardOffsetZ - 0.5*baseBoardLength);
  GeoTrf::Vector3D x(0.0, u.y(),w.z());
  
  // for corner of hybrid, connector outer and pigtail of outer side.
  GeoTrf::Vector3D i(0.0, 
                      0.5*outerSideHybridWidth,
                      m_outerSide->hybridOffsetZ() + 0.5*outerSidePigtailLength);
  GeoTrf::Vector3D l(0.0,
                      -0.5*outerSideHybridWidth - m_outerSide->pigtail()->width(), i.z());
  GeoTrf::Vector3D m(0.0, l.y(),
                      m_outerSide->hybridOffsetZ() - 0.5*outerSidePigtailLength);
  GeoTrf::Vector3D p(0.0, i.y(),
                      m.z());



  // for corner of hybrid and interConnect of inner side.
  GeoTrf::Vector3D q(0.0, 0.5*outerSideHybridWidth, m_outerSide->hybridOffsetZ() + 0.5*outerSideHybridLength );
  GeoTrf::Vector3D r(0.0, -0.5*innerSideHybridWidth, q.z());
  GeoTrf::Vector3D s(0.0, r.y(), m_innerSide->hybridOffsetZ() - 0.5*innerSideHybridLength);
  GeoTrf::Vector3D t(0.0, q.y(), s.z());

  // All points turn +-20 mGaudi::Units::rad around physical center of module.
  a = GeoTrf::RotateX3D(m_stereoOuter/Gaudi::Units::radian)*a;
  b = GeoTrf::RotateX3D(m_stereoOuter/Gaudi::Units::radian)*b;
  c = GeoTrf::RotateX3D(m_stereoOuter/Gaudi::Units::radian)*c;
  d = GeoTrf::RotateX3D(m_stereoOuter/Gaudi::Units::radian)*d;

  e = GeoTrf::RotateX3D(m_stereoInner/Gaudi::Units::radian)*e;
  f = GeoTrf::RotateX3D(m_stereoInner/Gaudi::Units::radian)*f;
  g = GeoTrf::RotateX3D(m_stereoInner/Gaudi::Units::radian)*g;
  h = GeoTrf::RotateX3D(m_stereoInner/Gaudi::Units::radian)*h;

  i = GeoTrf::RotateX3D(m_stereoOuter/Gaudi::Units::radian)*i;
  //k.rotateX(m_stereoOuter/Gaudi::Units::radian);
  l = GeoTrf::RotateX3D(m_stereoOuter/Gaudi::Units::radian)*l;
  m = GeoTrf::RotateX3D(m_stereoOuter/Gaudi::Units::radian)*m;
  //n.rotateX(m_stereoOuter/Gaudi::Units::radian);
  p = GeoTrf::RotateX3D(m_stereoOuter/Gaudi::Units::radian)*p;

  q = GeoTrf::RotateX3D(m_stereoInner/Gaudi::Units::radian)*q;
  r = GeoTrf::RotateX3D(m_stereoInner/Gaudi::Units::radian)*r;
  s = GeoTrf::RotateX3D(m_stereoInner/Gaudi::Units::radian)*s;
  t = GeoTrf::RotateX3D(m_stereoInner/Gaudi::Units::radian)*t;

  // Calculate demension of envelope1.
  const double z_ab = std::max(a.z(), b.z());
  const double z_ef = std::max(e.z(), f.z());
  const double z_cd = std::min(c.z(), d.z());
  const double z_gh = std::min(g.z(), h.z());

  const double y_ad = std::max(a.y(), d.y());
  const double y_eh = std::max(e.y(), h.y());
  const double y_bc = std::min(b.y(), c.y());
  const double y_fg = std::min(f.y(), g.y());

  const double zmaxEnv1 = std::max(z_ab, z_ef);
  const double zminEnv1 = std::min(z_cd, z_gh);
  const double zmaxEnv3 = u.z();
  const double zminEnv3 = w.z();

  const double ypre_maxEnv1 = std::max(y_ad, y_eh);
  const double ypre_minEnv1 = std::min(y_bc, y_fg);

  // const double ymaxEnv1 = std::max(ypre_maxEnv1, u.y());
  // const double yminEnv1 = std::min(ypre_minEnv1, v.y());
  const double ymaxEnv1 = ypre_maxEnv1;
  const double yminEnv1 = ypre_minEnv1;
  const double ymaxEnv3 = std::max(ypre_maxEnv1, u.y());
  const double yminEnv3 = std::min(ypre_minEnv1, v.y());

  // some temporary value.
  const double thicknessEnv1 = m_sensorGap + 2.0*m_outerSide->sensor()->thickness();
  const double lengthEnv1 = zmaxEnv1 - zminEnv1;
  const double widthEnv1  = ymaxEnv1 - yminEnv1;

  const double thicknessEnv3 = thicknessEnv1;
  const double lengthEnv3 = zmaxEnv3 - zminEnv3;
  const double widthEnv3 = ymaxEnv3 - yminEnv3;

  // Center of evnelope1.
  const double xCenterEnv1 = 0.0;
  const double yCenterEnv1 = yminEnv1 + 0.5*widthEnv1;
  const double zCenterEnv1 = zmaxEnv1 - 0.5*lengthEnv1;

  m_env1RefPointVector = new GeoTrf::Vector3D(-xCenterEnv1, -yCenterEnv1, -zCenterEnv1);

  // Center of envelope3
  const double xCenterEnv3 = 0.0;
  const double yCenterEnv3 = yminEnv3 + 0.5*widthEnv3;
  const double zCenterEnv3 = zmaxEnv3 - 0.5*lengthEnv3;

  m_env3RefPointVector = new GeoTrf::Vector3D(-xCenterEnv3, -yCenterEnv3, -zCenterEnv3);

  // Calculate demension of envelope2.
  const double z_ikl = std::max(i.z(), l.z());
  const double z_qr = std::max(q.z(), r.z());
  const double z_mnp = std::min(m.z(), p.z());
  const double z_st = std::min(s.z(), t.z());

  const double y_ip = std::max(i.y(), p.y());
  const double y_qt = std::max(q.y(), t.y());
  const double y_lm = std::min(l.y(), m.y());
  const double y_rs = std::min(r.y(), s.y());

  const double zmaxEnv2 = std::max(z_ikl, z_qr);
  const double zminEnv2 = std::min(z_mnp, z_st);

  const double ymaxEnv2 = std::max(y_ip, y_qt);
  const double yminEnv2 = std::min(y_lm, y_rs);

  const double thicknessEnv2 = 2.0*(m_outerSide->hybridOffsetX() + 
                                    0.5*m_innerSide->sensor()->thickness() + 
                                    0.5*m_sensorGap + 
                                    0.5*m_outerSide->hybrid()->thickness());
  const double lengthEnv2 = zmaxEnv2 - zminEnv2;
  const double widthEnv2  = ymaxEnv2 - yminEnv2;

  // Center of envelope2.
  const double xCenterEnv2 = 0.0;
  const double yCenterEnv2 = ymaxEnv2 - 0.5*widthEnv2;
  const double zCenterEnv2 = zmaxEnv2 - 0.5*lengthEnv2;

  m_env2RefPointVector = new GeoTrf::Vector3D(-xCenterEnv2, -yCenterEnv2, -zCenterEnv2);

  // 8th Aug 2005 S.Mima modified.
  // Calculate dimension of subbox 
  const double xmaxSubBox = - 0.5*m_baseBoard->thickness() - m_safety;
  const double xminSubBox = - 0.5*thicknessEnv2 - 2.0*m_safety;

  const double ymaxSubBox = std::min(r.y(), s.y()) - m_safety;
  const double yminSubBox = yminEnv2 - 2.0*m_safety;

  const double zmaxSubBox = zmaxEnv1 + 2.0*m_safety;
  const double zminSubBox = zminEnv1 - 2.0*m_safety; 
  
  const double thicknessSubBox = xmaxSubBox - xminSubBox;
  const double widthSubBox = ymaxSubBox - yminSubBox;
  const double lengthSubBox = zmaxSubBox - zminSubBox;

  // Center of subbox.
  const double xCenterSubBox = xmaxSubBox - 0.5*thicknessSubBox;
  const double yCenterSubBox = ymaxSubBox - 0.5*widthSubBox;
  const double zCenterSubBox = zmaxSubBox - 0.5*lengthSubBox;

  // const double xmaxSubBox1a = - 0.5*m_baseBoard->thickness() - m_safety;
  // const double xminSubBox1a = - 0.5*thicknessEnv2 - 2.0*m_safety;

  // const double yminSubBox1a = std::max(r.y(), s.y()) + m_safety;
  // const double ymaxSubBox1a = std::min(y_bc, y_fg) - 2.0*m_safety;

  // const double zmaxSubBox1a = z_st - m_safety;
  // const double zminSubBox1a = zminEnv1 - 2.0*m_safety; 
  
  // const double thicknessSubBox1a = xmaxSubBox1a - xminSubBox1a;
  // const double widthSubBox1a = ymaxSubBox1a - yminSubBox1a;
  // const double lengthSubBox1a = zmaxSubBox1a - zminSubBox1a;

  // // Center of subbox.
  // const double xCenterSubBox1a = xmaxSubBox1a - 0.5*thicknessSubBox1a;
  // const double yCenterSubBox1a = ymaxSubBox1a - 0.5*widthSubBox1a;
  // const double zCenterSubBox1a = zmaxSubBox1a - 0.5*lengthSubBox1a;


  // const double xmaxSubBox1b = xmaxSubBox1a;
  // const double xminSubBox1b = xminSubBox1a;

  // const double yminSubBox1b = yminSubBox1a;
  // const double ymaxSubBox1b = ymaxSubBox1a;

  // const double zminSubBox1b = z_qr + m_safety;
  // const double zmaxSubBox1b = zmaxEnv1 + 2.0*m_safety; 
  
  // const double thicknessSubBox1b = xmaxSubBox1b - xminSubBox1b;
  // const double widthSubBox1b = ymaxSubBox1b - yminSubBox1b;
  // const double lengthSubBox1b = zmaxSubBox1b - zminSubBox1b;

  // // Center of subbox.
  // const double xCenterSubBox1b = xmaxSubBox1b - 0.5*thicknessSubBox1b;
  // const double yCenterSubBox1b = ymaxSubBox1b - 0.5*widthSubBox1b;
  // const double zCenterSubBox1b = zmaxSubBox1b - 0.5*lengthSubBox1b;


// Second subbox
  const double xmaxSubBox2 = - 0.5*thicknessEnv1 - m_safety;
  const double xminSubBox2 = xminSubBox;

  const double ymaxSubBox2 = ymaxEnv2 + 2.0*m_safety;
  const double yminSubBox2 = yminEnv2 - 2.0*m_safety;

  const double zmaxSubBox2 = std::max(z_ab, z_ef) + 2.0*m_safety;
  const double zminSubBox2 = z_qr + 2.0*m_safety;

  const double thicknessSubBox2 = xmaxSubBox2 - xminSubBox2;
  const double widthSubBox2 = ymaxSubBox2 - yminSubBox2;
  const double lengthSubBox2 = zmaxSubBox2 - zminSubBox2;

  const double xCenterSubBox2 = xmaxSubBox2 - 0.5*thicknessSubBox2;
  const double yCenterSubBox2 = ymaxSubBox2 - 0.5*widthSubBox2;
  const double zCenterSubBox2 = zmaxSubBox2 - 0.5*lengthSubBox2;

  // Third subbox
  const double xmaxSubBox3 = xmaxSubBox2;
  const double xminSubBox3 = xminSubBox2;

  const double ymaxSubBox3 = ymaxSubBox2;
  const double yminSubBox3 = yminSubBox2;

  const double zmaxSubBox3 = z_st - 2.0*m_safety;
  const double zminSubBox3 = std::min(z_cd, z_gh) - 2.0*m_safety;

  const double thicknessSubBox3 = xmaxSubBox3 - xminSubBox3;
  const double widthSubBox3 = ymaxSubBox3 - yminSubBox3;
  const double lengthSubBox3 = zmaxSubBox3 - zminSubBox3;

  const double xCenterSubBox3 = xmaxSubBox3 - 0.5*thicknessSubBox3;
  const double yCenterSubBox3 = ymaxSubBox3 - 0.5*widthSubBox3;
  const double zCenterSubBox3 = zmaxSubBox3 - 0.5*lengthSubBox3;

  // Fourth subbox
  const double xmaxSubBox4 = - 0.5*m_baseBoard->thickness() - m_safety;
  const double xminSubBox4 = - 0.5*thicknessEnv2 - 10.0*m_safety;

  const double ymaxSubBox4 = std::min(f.y(), g.y()) - m_safety;
  const double yminSubBox4 = yminEnv2 - 10.0*m_safety;

  const double zmaxSubBox4 = zmaxEnv1 + 10.0*m_safety;
  const double zminSubBox4 = std::max(q.z(), r.z()) + 2.0*m_safety; 
  
  const double thicknessSubBox4 = xmaxSubBox4 - xminSubBox4;
  const double widthSubBox4 = ymaxSubBox4 - yminSubBox4;
  const double lengthSubBox4 = zmaxSubBox4 - zminSubBox4;

  // Center of subbox.
  const double xCenterSubBox4 = xmaxSubBox4 - 0.5*thicknessSubBox4;
  const double yCenterSubBox4 = ymaxSubBox4 - 0.5*widthSubBox4;
  const double zCenterSubBox4 = zmaxSubBox4 - 0.5*lengthSubBox4;

  // Fifth subbox
  const double xmaxSubBox5 = - 0.5*m_baseBoard->thickness() - m_safety;
  const double xminSubBox5 = - 0.5*thicknessEnv2 - 10.0*m_safety;

  const double ymaxSubBox5 = std::min(f.y(), g.y()) - m_safety;
  const double yminSubBox5 = yminEnv2 - 10.0*m_safety;

  const double zminSubBox5 = zminEnv1 - 10.0*m_safety;
  const double zmaxSubBox5 = std::min(s.z(), t.z()) - 2.0*m_safety; 
  
  const double thicknessSubBox5 = xmaxSubBox5 - xminSubBox5;
  const double widthSubBox5 = ymaxSubBox5 - yminSubBox5;
  const double lengthSubBox5 = zmaxSubBox5 - zminSubBox5;

  // Center of subbox.
  const double xCenterSubBox5 = xmaxSubBox5 - 0.5*thicknessSubBox5;
  const double yCenterSubBox5 = ymaxSubBox5 - 0.5*widthSubBox5;
  const double zCenterSubBox5 = zmaxSubBox5 - 0.5*lengthSubBox5;

  // Sixth subbox
  const double xmaxSubBox6 = - 0.5*m_baseBoard->thickness() - m_safety;
  const double xminSubBox6 = - 0.5*thicknessEnv2 - 10.0*m_safety;

  const double yminSubBox6 = std::max(e.y(), h.y()) + m_safety;
  const double ymaxSubBox6 = ymaxEnv2 + 10.0*m_safety;

  const double zmaxSubBox6 = zmaxEnv1 + 10.0*m_safety;
  const double zminSubBox6 = std::max(q.z(), r.z()) + 2.0*m_safety; 
  
  const double thicknessSubBox6 = xmaxSubBox6 - xminSubBox6;
  const double widthSubBox6 = ymaxSubBox6 - yminSubBox6;
  const double lengthSubBox6 = zmaxSubBox6 - zminSubBox6;

  // Center of subbox.
  const double xCenterSubBox6 = xmaxSubBox6 - 0.5*thicknessSubBox6;
  const double yCenterSubBox6 = ymaxSubBox6 - 0.5*widthSubBox6;
  const double zCenterSubBox6 = zmaxSubBox6 - 0.5*lengthSubBox6;

  // Seventh subbox
  const double xmaxSubBox7 = - 0.5*m_baseBoard->thickness() - m_safety;
  const double xminSubBox7 = - 0.5*thicknessEnv2 - 10.0*m_safety;

  const double yminSubBox7 = std::max(e.y(), h.y()) + m_safety;
  const double ymaxSubBox7 = ymaxEnv2 + 10.0*m_safety;

  const double zminSubBox7 = zminEnv1 - 10.0*m_safety;
  const double zmaxSubBox7 = std::min(s.z(), t.z()) - 2.0*m_safety; 
  
  const double thicknessSubBox7 = xmaxSubBox7 - xminSubBox7;
  const double widthSubBox7 = ymaxSubBox7 - yminSubBox7;
  const double lengthSubBox7 = zmaxSubBox7 - zminSubBox7;

  // Center of subbox.
  const double xCenterSubBox7 = xmaxSubBox7 - 0.5*thicknessSubBox7;
  const double yCenterSubBox7 = ymaxSubBox7 - 0.5*widthSubBox7;
  const double zCenterSubBox7 = zmaxSubBox7 - 0.5*lengthSubBox7;


  m_env1Thickness = thicknessEnv1 + 2*m_safety;
  m_env1Width     = widthEnv1     + 2*m_safety;
  m_env1Length    = lengthEnv1    + 2*m_safety;

  m_env2Thickness = thicknessEnv2 + 2*m_safety;
  m_env2Width     = widthEnv2     + 2*m_safety;
  m_env2Length    = lengthEnv2    + 2*m_safety;

  m_env3Thickness = thicknessEnv3 + 2*m_safety;
  m_env3Width     = widthEnv3     + 2*m_safety;
  m_env3Length    = lengthEnv3    + 2*m_safety;

  // Envelope 2 defines the overall thickness. NB. The ski assumes the 
  // the envelope "thickness" is centered on x, which it is.
  m_thickness   = m_env2Thickness;
  m_activeWidth = m_env2Width;
  m_width       = m_env2Width;
  m_length      = m_env1Length;

  // Calculate a few things.
  const double OSPosX =   0.5*m_sensorGap + 0.5*m_innerSide->sensor()->thickness();
  const double ISPosX = -(0.5*m_sensorGap + 0.5*m_innerSide->sensor()->thickness());

  //
  // Make an envelope for the whole module.
  //
  // 6th Feb 2005 D.Naito modified.
  const GeoBox * envelope1 = new GeoBox(0.5*m_env1Thickness, 0.5*m_env1Width, 0.5*m_env1Length);
  const GeoBox * envelope2 = new GeoBox(0.5*m_env2Thickness, 0.5*m_env2Width, 0.5*m_env2Length);
  const GeoBox * envelope3 = new GeoBox(0.5*m_env3Thickness, 0.5*m_env3Width, 0.5*m_env3Length);

  // 8th Aug 2005 S.Mima modified.
  const GeoBox * subBox  = new GeoBox(0.5*thicknessSubBox, 0.5*widthSubBox, 0.6*lengthSubBox);
  // const GeoBox * subBox1a  = new GeoBox(0.5*thicknessSubBox1a, 0.5*widthSubBox1a, 0.5*lengthSubBox1a);
  // const GeoBox * subBox1b  = new GeoBox(0.5*thicknessSubBox1b, 0.5*widthSubBox1b, 0.5*lengthSubBox1b);
  const GeoBox * subBox2 = new GeoBox(0.5*thicknessSubBox2, 0.5*widthSubBox2, 0.5*lengthSubBox2);
  const GeoBox * subBox3 = new GeoBox(0.5*thicknessSubBox3, 0.5*widthSubBox3, 0.5*lengthSubBox3);
  const GeoBox * subBox4 = new GeoBox(0.5*thicknessSubBox4, 0.5*widthSubBox4, 0.5*lengthSubBox4);
  const GeoBox * subBox5 = new GeoBox(0.5*thicknessSubBox5, 0.5*widthSubBox5, 0.5*lengthSubBox5);
  const GeoBox * subBox6 = new GeoBox(0.5*thicknessSubBox6, 0.5*widthSubBox6, 0.5*lengthSubBox6);
  const GeoBox * subBox7 = new GeoBox(0.5*thicknessSubBox7, 0.5*widthSubBox7, 0.5*lengthSubBox7);

  // In the following, envelope1 and envelope2 are added and SUBBOX is pulled. 
  const GeoShape & moduleEnvelope = (*envelope1 << GeoTrf::Translate3D(xCenterEnv1, yCenterEnv1, zCenterEnv1)).
    add(*envelope3 << GeoTrf::Translate3D(xCenterEnv3, yCenterEnv3, zCenterEnv3)).
    add(*envelope2 << GeoTrf::Translate3D(xCenterEnv2, yCenterEnv2, zCenterEnv2)).
    subtract(*subBox4 << GeoTrf::Translate3D(xCenterSubBox4, yCenterSubBox4, zCenterSubBox4)).
    subtract(*subBox5 << GeoTrf::Translate3D(xCenterSubBox5, yCenterSubBox5, zCenterSubBox5)).
    subtract(*subBox6 << GeoTrf::Translate3D(xCenterSubBox6, yCenterSubBox6, zCenterSubBox6)).
    subtract(*subBox7 << GeoTrf::Translate3D(xCenterSubBox7, yCenterSubBox7, zCenterSubBox7)).
    subtract(*subBox << GeoTrf::Translate3D(xCenterSubBox, yCenterSubBox, zCenterSubBox)).
    subtract(*subBox2 << GeoTrf::Translate3D(xCenterSubBox2, yCenterSubBox2, zCenterSubBox2)).
    subtract(*subBox3 << GeoTrf::Translate3D(xCenterSubBox3, yCenterSubBox3, zCenterSubBox3));
    // subtract(*subBox1a << GeoTrf::Translate3D(xCenterSubBox1a, yCenterSubBox1a, zCenterSubBox1a)).
    // subtract(*subBox1b << GeoTrf::Translate3D(xCenterSubBox1b, yCenterSubBox1b, zCenterSubBox1b)).

  const GeoLogVol * moduleLog           = new GeoLogVol(getName(), &moduleEnvelope, m_materials->gasMaterial());
  
  //
  // inner side
  //
  GeoTrf::Transform3D rotInner = GeoTrf::RotateX3D(m_stereoInner) * GeoTrf::RotateZ3D(180*Gaudi::Units::deg);
  m_innerSidePos = new GeoTrf::Transform3D(GeoTrf::Transform3D(GeoTrf::Translation3D(ISPosX, 0.0, 0.0)*rotInner));

  //
  // outer side
  //
  GeoTrf::RotateX3D rotOuter(m_stereoOuter);
  m_outerSidePos = new GeoTrf::Transform3D(GeoTrf::Transform3D(GeoTrf::Translation3D(OSPosX, 0.0, 0.0)*rotOuter));

  //
  // base board
  // 6th Apr 2005 S.Mima modified.
  //
  const double baseBoardPosY = m_baseBoardOffsetY;
  const double baseBoardPosZ = m_baseBoardOffsetZ;
  m_baseBoardPos = new GeoTrf::Translate3D(0.0, baseBoardPosY, baseBoardPosZ);


  return moduleLog;
}


GeoVPhysVol * 
SCT_Module::build(SCT_Identifier id)
{
  GeoFullPhysVol * module = new GeoFullPhysVol(m_logVolume); 

  // We make these fullPhysVols for the alignment code.
  // We probably should make the transform for the sensor 
  // alignable rather than the "side" to save making an extra full phys volume.

  //
  // Build the module
  //
  // Add Baseboard
  GeoTransform * baseBoardTransform = new GeoTransform(*m_baseBoardPos);
  module->add(baseBoardTransform);
  module->add(m_baseBoard->getVolume());

  // Add innerside
  GeoAlignableTransform * innerTransform = new GeoAlignableTransform(*m_innerSidePos);
  module->add(innerTransform);
  int innerSideNumber = (m_upperSide) ? 0 : 1;
  module->add(new GeoNameTag("Side#"+intToString(innerSideNumber))); // Identifier side=0
  module->add(new GeoIdentifierTag(innerSideNumber));
  id.setSide(innerSideNumber);
  Identifier innerId = id.getWaferId();
  GeoVPhysVol * innerSide = m_innerSide->build(id);
  module->add(innerSide);  
  // Store alignable transform
  m_detectorManager->addAlignableTransform(0, innerId, innerTransform, innerSide);
     
  // Add outerside
  GeoAlignableTransform * outerTransform = new GeoAlignableTransform(*m_outerSidePos);
  module->add(outerTransform);
  int outerSideNumber = m_upperSide;
  module->add(new GeoNameTag("Side#"+intToString(outerSideNumber))); // Identifier side=1
  module->add(new GeoIdentifierTag(outerSideNumber));
  id.setSide(outerSideNumber);
  Identifier outerId = id.getWaferId();
  GeoVPhysVol * outerSide = m_outerSide->build(id);
  module->add(outerSide);
  // Store alignable transform
  m_detectorManager->addAlignableTransform(0, outerId, outerTransform, outerSide);
  
  return module;
}
