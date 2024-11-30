/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VTI12TrackSystems/MeasurementToSoNode.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include "VP1HEPVis/nodes/SoTubs.h"
#include "VP1HEPVis/nodes/SoGenericBox.h"
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoRotationXYZ.h>

#include "VTI12Utils/VP1LinAlgUtils.h"
#include "VTI12Utils/HitsSoNodeManager.h"
#include "VP1Base/VP1Msg.h"



// Amg Eigen objects
#include "GeoPrimitives/GeoPrimitives.h"

//MeasurementBase
#include "TrkMeasurementBase/MeasurementBase.h"

//ROTs
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
// #include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
// #include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
// #include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
// #include "InDetRIO_OnTrack/SiClusterOnTrack.h"

//CompetingROTs
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

//Pseudo Measurements
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

//Segments
#include "TrkSegment/Segment.h"

// PRDs
// #include "InDetPrepRawData/PixelCluster.h"
// #include "InDetPrepRawData/SCT_Cluster.h"
// #include "InDetPrepRawData/TRT_DriftCircle.h"

//IdHelpers
Trk::MeasurementToSoNode::MeasurementToSoNode()
    :
    m_stripThickness(0.8),
    m_clusDrawMode(Trk::MeasurementToSoNode::SingleStrip),
    m_dtLength(10.0)
{
  SoTubs::initClass();
  SoGenericBox::initClass();
  // Have simple view on by default
  for (int i=0; i<TrkObjToString::Unknown ; i++) m_simpleView.push_back(true);
}

std::pair<SoSeparator*,  std::vector<SoNode*> >
Trk::MeasurementToSoNode::createMeasSep(  const Trk::MeasurementBase& meas)
{

    VP1Msg::messageVerbose("Trk::MeasurementToSoNode::createMeasSep()");

    // Vector to hold all the nodes associated with this measurement
    std::vector<SoNode*> measurements;
    SoTransform*   theHitTransform=0;
    TrkObjToString::MeasurementType detType = m_objToType.type(&meas);
    // Find the correct type and call that method.

    switch (detType)
    {
        case TrkObjToString::SCT:
        {
            const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(&meas);
            if (rot){
              measurements = toSoNodes( *rot );
              theHitTransform=createTransform(*rot, detType);
            }
            break;
        }
        case TrkObjToString::CompetingROT:
        {
            const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(&meas);
            if (crot){
              measurements = toSoNodes(*crot);
              const Trk::RIO_OnTrack* mostProbROT =  &(crot->rioOnTrack( crot->indexOfMaxAssignProb ()));
              theHitTransform=createTransform( static_cast<const Trk::MeasurementBase&>(*mostProbROT), TrkObjToString::type( mostProbROT ));
            }
            break;
        }
        case TrkObjToString::PseudoMeasurement:
        {
            const Trk::PseudoMeasurementOnTrack* pseudo = dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(&meas) ;
            if (pseudo){
              measurements = toSoNodes(*pseudo);
              theHitTransform=createTransform( *pseudo, detType);
            }
            break;
        }
        case TrkObjToString::Segment:
        case TrkObjToString::Hole:
        case TrkObjToString::Unknown:
        {
            std::cerr<< "MeasurementToSoNode::createMeasSep: WARNING - Unknown detector type!"<<std::endl;
            std::cerr<<" Dumping MeasurementBase: "<<meas<<std::endl;
        }

    }
    // Add all the above to the separator
    SoSeparator *theHitMarkerSep = new SoSeparator;
    theHitMarkerSep->addChild(theHitTransform);

    std::vector<SoNode*>::const_iterator it, itEnd=measurements.end();
    for ( it = measurements.begin(); it!=itEnd; it++)
    {
        theHitMarkerSep->addChild(*it);
    }
    //std::cout<<"MeasurementToSoNode::createMeasSep - done"<<std::endl;

    return std::make_pair(theHitMarkerSep, measurements);
}

std::vector<SoNode*>
Trk::MeasurementToSoNode::toSoNodes(const Trk::RIO_OnTrack& rot)
{
    //std::cout<<"toSoNodes"<<std::endl;

  //TODO Implement modes properly
  std::vector<SoNode*> nodes ;
  switch (m_clusDrawMode)
  {
  case SingleStrip:
  case MultipleStrips:
  case Errors: 
  {
    double coord1 = rot.localParameters().get(Trk::loc1);
    double coord2 = 0.0;
    if (rot.localParameters().dimension()>1) coord2 = rot.localParameters().get(Trk::loc2);
//    nodes.push_back( toSoNode(rot.detectorElement(), rot.identify(), m_objToType.type(&rot), Trk::LocalPosition(coord1,coord2) ) );
    nodes.push_back( toSoNode(rot.detectorElement(), rot.identify(), m_objToType.type(&rot), Amg::Vector2D(coord1,coord2) ) );
    break;
  }
  default:
    break;
  }
  return nodes;
}

std::vector<SoNode*>
Trk::MeasurementToSoNode::toSoNodes(const Trk::CompetingRIOsOnTrack& crot)
{
    VP1Msg::messageVerbose("Trk::MeasurementToSoNode::CompetingRIOsOnTrack()");

    //
    // ++++++++++++++++++++++ TO DO! +++++++++++++++++++++++++
    //
    const Trk::RIO_OnTrack* mostProbROT =  &(crot.rioOnTrack( crot.indexOfMaxAssignProb ()));
    std::vector<SoNode*> nodes = toSoNodes( *mostProbROT ); // Temporary hack - should draw all competing ROTs and show them linked somehow.
    return nodes;
}

std::vector<SoNode*>
Trk::MeasurementToSoNode::toSoNodes(const Trk::PseudoMeasurementOnTrack& /*rot*/)
{
    //
    // ++++++++++++++++++++++ TO DO! +++++++++++++++++++++++++
    //
	VP1Msg::messageVerbose("Trk::MeasurementToSoNode::PseudoMeasurementOnTrack()");

    std::vector<SoNode*> nodes ;
    nodes.push_back( m_nodeManager.getShapeNode_Strip(10.0, 10.0, 10.0) );
    return nodes;
}

SoNode*
Trk::MeasurementToSoNode::toSoNode(const Trk::TrkDetElementBase* /*baseDetEl*/, Identifier /*id*/, TrkObjToString::MeasurementType prdType, Amg::Vector2D /*locPos*/)
{
	VP1Msg::messageVerbose("Trk::MeasurementToSoNode::toSoNode(Trk)");

  SoNode *theHitMarker = 0;
  switch (prdType)
  {
  // case TrkObjToString::SCT:
  //   {
  //     const InDetDD::SiDetectorElement* detEl = dynamic_cast<const InDetDD::SiDetectorElement*>(baseDetEl);
  //     if(detEl){ // by definition this should never fail
  //       theHitMarker = toSoNode(detEl, id, prdType, locPos);
  //     }
  //     break;
  //   }
  default:
    //message( "Specific Hit type could not be identified.");
    theHitMarker = m_nodeManager.getShapeNode_Strip(10.0, 10.0, 10.0);
    break;
  }
  return theHitMarker;
}


// SoNode*
// Trk::MeasurementToSoNode::toSoNode( const InDetDD::SiDetectorElement* detEl, Identifier /*id*/, TrkObjToString::MeasurementType detType, Amg::Vector2D& locPos)
// {
// 	VP1Msg::messageVerbose("Trk::MeasurementToSoNode::toSoNode(InDetDD)");

//   SoNode *theHitMarker = 0;
//   if ( isSimpleView(TrkObjToString::Pixel) && detType==TrkObjToString::Pixel)
//   {
//     // theHitMarker = createPointMarker(); // Simple Pixel
//     theHitMarker = m_nodeManager.getShapeNode_Point(); // Simple Pixel
//   }
//   else
//   {

//     double stripLength=detEl->etaPitch() ;
//     if (isSimpleView(TrkObjToString::SCT) && detType==TrkObjToString::SCT)
//     {
//       theHitMarker = createLineMarker(stripLength/2.0); // Simple SCT
//     }
//     else
//     {
//       double stripWidth=detEl->phiPitch( locPos );
//       theHitMarker = m_nodeManager.getShapeNode_Strip(stripLength, stripWidth, m_stripThickness); // Detailed SCT/Pixel
//     }
//   }
//   return theHitMarker;
// }

SoNode*
Trk::MeasurementToSoNode::createLineMarker( double halfLength, bool isTube )
{
  //////std::cout<<"createLineMarker. halfLength="<<halfLength<<std::endl;
  SoVertexProperty * scatVtxProperty = new SoVertexProperty();
  if (isTube)
  {
    scatVtxProperty->vertex.set1Value(0, 0.0,0.0,-halfLength);
    scatVtxProperty->vertex.set1Value(1, 0.0,0.0, halfLength);
  }
  else
  {
    scatVtxProperty->vertex.set1Value(0, 0.0,-halfLength,0.0);
    scatVtxProperty->vertex.set1Value(1, 0.0,halfLength,0.0);
  }
  SoLineSet * line = new SoLineSet();
  line->numVertices = 2;
  line->vertexProperty = scatVtxProperty;
  return line;
}

SoTransform*
Trk::MeasurementToSoNode::createTransform(  const Trk::MeasurementBase& mb, TrkObjToString::MeasurementType detType)
{
  const Trk::Surface& theSurface = mb.associatedSurface();

  // We need to handle various cases:
  // -> full tubes - use the surface position (or equivalent, global position of straw)
  // -> short tubes/strips - use the global position of the ROT (but this is now done inside AscoObj_TSOS)
  // -> long strips - use the global position of centre of strip.

  SoTransform*   theHitTransform = VP1LinAlgUtils::toSoTransform(theSurface.transform());

  Amg::Vector3D theHitGPos={-999.0,-999.0,-999.0} ;
  // so, strips
  switch (detType) {
      // case TrkObjToString::SCT:      
      // {        // For strips we should use the centre of the strip - this is a bit of a hack - should use detector elements (add above)
      //   const Amg::Vector3D* tempHitGPos = theSurface.localToGlobal( mb.localParameters() );
      //   theHitGPos=*tempHitGPos;
      //   delete tempHitGPos;
      //   break;
      // }
      default:
        // shouldn't ever get in here!
     	 // initializations to avoid warnings
     	 theHitGPos[0]=-999.;
     	 theHitGPos[1]=-999.;
    	 theHitGPos[2]=-999.;
       break;
  }
  
  theHitTransform->translation.setValue(theHitGPos[0], theHitGPos[1], theHitGPos[2]);
  return theHitTransform;
}
