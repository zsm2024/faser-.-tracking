/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataHandle_SCT_RDO          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008 (rewritten January 2009)      //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VTI12RawDataSystems/VP1RawDataHandle_SCT_RDO.h"
#include "VTI12RawDataSystems/VP1RawDataCommonData.h"
#include "VTI12Utils/HitsSoNodeManager.h"
#include "VTI12Utils/VP1DetInfo.h"
#include "VTI12Utils/VP1LinAlgUtils.h"

#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>

#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

#include "TrackerReadoutGeometry/SCT_ModuleSideDesign.h"   // new CMake

#include "TrackerRawData/FaserSCT_RDORawData.h"
#include "TrackerIdentifier/FaserSCT_ID.h"

#include "GeoPrimitives/GeoPrimitives.h"


//____________________________________________________________________
VP1RawDataHandle_SCT_RDO::VP1RawDataHandle_SCT_RDO(VP1RawDataCollBase* coll,const FaserSCT_RDORawData*data)
  : VP1RawDataHandleBase(coll), m_data(data)
{
}

//____________________________________________________________________
VP1RawDataHandle_SCT_RDO::~VP1RawDataHandle_SCT_RDO()
{
}

//____________________________________________________________________
QStringList VP1RawDataHandle_SCT_RDO::clicked(bool verbose) const
{
  static const FaserSCT_ID * idhelper = VP1DetInfo::sctIDHelper();
  QStringList l;
  l << " ===> FaserSCT_RDORawData";
  l << "   Data word: "+unsignedToHex(m_data->getWord());
  if (verbose) {
    if (idhelper) {
      Identifier id(m_data->identify());
      if (idhelper->station(id) == 0 )      l << "   Part: Interface Station";
      else if (idhelper->station(id) == 1 ) l << "   Part: Upstream Station";
      else if (idhelper->station(id) == 2 ) l << "   Part: Central Station";
      else                                  l << "   Part: Downstream Station";
    //   bool barrel(false);
    //   if (idhelper->barrel_ec(id)==-2) l << "   Part: End Cap C";
    //   else if (idhelper->barrel_ec(id)==2) l << "   Part: End Cap A";
    //   else { l << "   Part: Barrel"; barrel = true; }
    //   l << "   "+QString(barrel?"Layer":"Disk")+": "+QString::number(idhelper->layer_disk(id));
      l << "   Plane: "+QString::number(idhelper->layer(id));
      l << "   Phi module: "+QString::number(idhelper->phi_module(id));
      l << "   Eta module: "+QString::number(idhelper->eta_module(id));
      l << "   Side: "+QString(idhelper->side(id)==0 ? "0":"1");
      l << "   Group Size: "+QString::number(m_data->getGroupSize());
      if (m_data->getGroupSize()>1)
        l << "  Strips: "+QString::number(idhelper->strip(id))+".."+QString::number(idhelper->strip(id)+m_data->getGroupSize()-1);
      else
        l << "  Strip: "+QString::number(idhelper->strip(id));
    }
    //Fixme: Try dynamic cast to SCT3_RawData, and print more information if it succeeds.
  }
  return l;
}

//____________________________________________________________________
SoNode * VP1RawDataHandle_SCT_RDO::buildShape()
{
  static const FaserSCT_ID * idhelper = VP1DetInfo::sctIDHelper();
  const TrackerDD::SiDetectorElement * elem = element();
  const int ngroup = m_data->getGroupSize();
  if (!elem||ngroup<1)
    return common()->nodeManager()->getShapeNode_Point();//fixme: warn

  const TrackerDD::SCT_ModuleSideDesign& design = dynamic_cast<const TrackerDD::SCT_ModuleSideDesign&>(elem->design());

  SoLineSet * line = new SoLineSet;
  SoVertexProperty * vertices = new SoVertexProperty;
  line->vertexProperty = vertices;

  Amg::Transform3D invTransform(elem->transform().inverse());
  const int ifirststrip(idhelper ? idhelper->strip(m_data->identify()) : 0);
  int iver(0);
  std::pair<TrackerDD::SiLocalPosition,TrackerDD::SiLocalPosition> localEnds;
  for (int i=0;i<ngroup;++i) {
    std::pair< Amg::Vector3D, Amg::Vector3D > globalEnds = elem->endsOfStrip(design.positionFromStrip(ifirststrip+i));
    Amg::Vector3D localA(invTransform*globalEnds.first);
    Amg::Vector3D localB(invTransform*globalEnds.second);
    localEnds = design.endsOfStrip(design.positionFromStrip(ifirststrip+i));
    vertices->vertex.set1Value(iver++,localA.x(),localA.y(),0);
    vertices->vertex.set1Value(iver++,localB.x(),localB.y(),0);
    line->numVertices.set1Value(i,2);
  }
  return line;
}

//____________________________________________________________________
SoTransform * VP1RawDataHandle_SCT_RDO::buildTransform()
{
  const TrackerDD::SiDetectorElement * elem = element();
  if (!elem)
    return common()->nodeManager()->getUnitTransform();//fixme: warn
  return VP1LinAlgUtils::toSoTransform(elem->transform());
}

//____________________________________________________________________
const TrackerDD::SiDetectorElement * VP1RawDataHandle_SCT_RDO::element() const
{
  const TrackerDD::SCT_DetectorManager * detmgr = VP1DetInfo::sctDetMgr();
  if (!detmgr)
    return 0;
  return detmgr->getDetectorElement(m_data->identify());
}

// //____________________________________________________________________
// SoMaterial * VP1RawDataHandle_SCT_RDO::determineMaterial() const
// {
//   return common()->sctMaterial();
// }

//____________________________________________________________________
VP1RawDataFlags::TrackerPartsFlags VP1RawDataHandle_SCT_RDO::inTrackerParts() const
{
  const TrackerDD::SiDetectorElement * elem = element();
  if (!elem)
    return VP1RawDataFlags::All;

  static const FaserSCT_ID * idhelper = VP1DetInfo::sctIDHelper();
  Identifier id(m_data->identify());
  if (idhelper->station(id) == 0 ) return VP1RawDataFlags::Interface;
  if (idhelper->station(id) == 1 ) return VP1RawDataFlags::Upstream;
  if (idhelper->station(id) == 2 ) return VP1RawDataFlags::Central;
  if (idhelper->station(id) == 3 ) return VP1RawDataFlags::Downstream;
  return VP1RawDataFlags::All;

}
