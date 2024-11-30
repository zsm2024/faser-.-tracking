/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//  Update: Riccardo Maria BIANCHI rbianchi@cern.ch Feb 2014                   //

#ifndef BUILDVP1LIGHT

#include "VTI12Utils/HitToSoNode.h"
#include "VTI12Utils/HitsSoNodeManager.h"
#include "VTI12Utils/VP1LinAlgUtils.h"
#include "VTI12Utils/VP1DetInfo.h"
#include "VP1Base/VP1Msg.h"
#include "VP1HEPVis/nodes/SoTransparency.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>

#include "TrkSurfaces/Surface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

// #include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

#include "ScintReadoutGeometry/ScintDetectorElement.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

#include <sstream>
#include <cmath>


#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

class HitToSoNode::Imp {
public:
  Imp() : theclass(0), driftdischeight(0.15) {}
  HitToSoNode * theclass;
  const double driftdischeight;
  HitsSoNodeManager nodeManager;
};

HitToSoNode::HitToSoNode(IVP1System * sys)
    : VP1HelperClassBase(sys,"HitToSoNode"), m_d(new Imp)
{
    m_d->theclass = this;
}

//____________________________________________________________________
HitToSoNode::~HitToSoNode()
{
    messageVerbose("destructor begin");
    delete m_d;
    messageVerbose("destructor end");
}

//____________________________________________________________________
SoTransform * HitToSoNode::createTransform(const Trk::RIO_OnTrack& rio, bool useSurfacePositionOnly) const
{  
    const Trk::Surface& theSurface = rio.detectorElement()->surface(rio.identify());
    SoTransform * theHitTransform = VP1LinAlgUtils::toSoTransform(theSurface.transform());

  // const Amg::Vector3D* theHitGPos;
    if ( useSurfacePositionOnly ) {
    // if (verbose) VP1Msg::message("useSurfacePositionOnly");
        theHitTransform->translation.setValue(theSurface.center()[0], theSurface.center()[1], theSurface.center()[2]);

    } else {

    // for strips, clusters or short tubes, use position of hit.
    // FIXME - for DCs, shouldn't this be the d0=0 position, i.e. on the wire?
        theHitTransform->translation.setValue(rio.globalPosition()[0], rio.globalPosition()[1], rio.globalPosition()[2]);
    }
    return theHitTransform;
}

// void HitToSoNode::buildTubeShapes(const Trk::RIO_OnTrack& rio, SoSeparator*&shape_simple, SoSeparator*&shape_detailed, bool /**blockGP*/, double length, bool doProjection)
// {
//     const Trk::CylinderBounds* ccbo = dynamic_cast<const Trk::CylinderBounds*>(&(rio.detectorElement()->surface(rio.identify()).bounds()));
//     assert(ccbo!=0);
//     if (!ccbo) {
//       VP1Msg::message("HitToSoNode::buildTubeShapes ERROR: Trk::CylinderBounds conversion failed");
//       return;
//     }

//     double radius = fabs(rio.localParameters().get(Trk::locR));
//     if (radius<0.15)
//         radius = 0.0;//radius is so small it is better to collapse to line
        
//     double halflength(ccbo->halflengthZ());
    
//     if (doProjection && dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(&rio)){

//       double strawLength=2.0*halflength;
//       if (strawLength<160.0)
//         strawLength = 2*349.3150-strawLength;//Yeah, we hardcode a lot here... but dimensions of TRT barrel straws are not likely to change.

//       SoPointSet       * scatPointSet    = new SoPointSet;
//       SoVertexProperty * scatVtxProperty = new SoVertexProperty;
//       scatVtxProperty->vertex.set1Value(0,0.0f,0.0f,strawLength);
//       scatPointSet->numPoints=1;
//       scatPointSet->vertexProperty.setValue(scatVtxProperty);

//       shape_detailed->addChild(scatPointSet);
//       shape_simple->addChild(scatPointSet);

//     } else {
//       if (length!=0.0) {
//         halflength = length/2.0;
//         // FIXME! translate to z position here instead of in AscObj_TSOS, for symmetry with buildStripShapes
//       }
//       SoNode * simpleShape = m_d->nodeManager.getShapeNode_DriftTube( halflength, 0.0 );
//       shape_simple->addChild(simpleShape);
//          //Detailed shape uses tube (unless negligible radius):
//     if (radius==0.0)
//         shape_detailed->addChild(simpleShape);
//     else
//         shape_detailed->addChild(m_d->nodeManager.getShapeNode_DriftTube( halflength, radius ));
//     }
// }

void HitToSoNode::buildStripShapes(const Trk::RIO_OnTrack& rio, SoSeparator*&shape_simple, SoSeparator*&shape_detailed, bool blockGP)
{
    // assuming starting with position being that of centre of surface i.e. always working in local coord system


    double stripLength =100.0, stripWidth = 10.0, stripThickness=1.0;
    std::optional<Amg::Vector2D> localposROT=std::nullopt;
    Amg::Vector2D* localposStrip=0;
    Identifier id=rio.identify();
    fillValues(id, rio.detectorElement(), stripLength, stripWidth, stripThickness, localposStrip);

  //std::cout<<"Got back: "<<stripLength<<","<<stripWidth<<","<<stripThickness<<std::endl;
  //std::cout<<"LocalposStrip="<<*localposStrip<<std::endl;

  // if (static_cast<PRDCollHandle_TGC*>(collHandle())->project())
  //   striplength += 300.0;//Fixme: Rough extension for now
    double tolerance = 3.;
    localposROT = rio.detectorElement()->surface( rio.identify() ).globalToLocal(rio.globalPosition(), tolerance);

    if( !localposROT )
    {
        localposROT = Amg::Vector2D{};
        VP1Msg::message("Warning: Local hit position was NULL");
    }

    if (!blockGP){
      SoSeparator * gpSep = new SoSeparator;
        SoTranslation * localtransGP = new SoTranslation;
        localtransGP->translation.setValue(static_cast<float>( (*localposROT)[Trk::locX]),
            static_cast<float>( (*localposROT)[Trk::locY]),
            0.0f);
            
        gpSep->addChild(localtransGP);
        gpSep->addChild(m_d->nodeManager.getShapeNode_Cross(10));
        shape_detailed->addChild(gpSep);
    }


    SoTranslation * localtrans0 = new SoTranslation;
    localtrans0->translation.setValue(static_cast<float>( (*localposStrip)[Trk::locX]),
        static_cast<float>( (*localposStrip)[Trk::locY]),
        0.0f);


    shape_simple->addChild(localtrans0);
    shape_simple->addChild( m_d->nodeManager.getShapeNode_Strip(stripLength));

    shape_detailed->addChild(localtrans0);
    shape_detailed->addChild(m_d->nodeManager.getShapeNode_Strip(stripLength,stripWidth,stripThickness));
    // Transform back to centre of Surface
    SoTranslation * localtrans1 = new SoTranslation;
    localtrans1->translation.setValue(static_cast<float>( -(*localposStrip)[Trk::locX]),
        static_cast<float>( -(*localposStrip)[Trk::locY]),
        0.0f);
    shape_simple->addChild(localtrans1);
    shape_detailed->addChild(localtrans1);

    delete localposStrip;
}

void HitToSoNode::fillValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness, Amg::Vector2D*& localposStrip){

    const FaserDetectorID * idhelper = VP1DetInfo::faserIDHelper();
    if (!idhelper) return;
    if (idhelper->is_sct(id)) { 
        fillSiValues(id, baseDetEl, striplength, stripWidth, stripThickness, localposStrip); return;
    }
    VP1Msg::message("Warning: HitToSoNode::fillValues(...) unknown technology.");
    return;
}


void HitToSoNode::fillSiValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness,  Amg::Vector2D*& localposStrip){
    const TrackerDD::SiDetectorElement* detEl =
        dynamic_cast<const TrackerDD::SiDetectorElement*>(baseDetEl);
    if ( !detEl){
        VP1Msg::message("Could not get Si det element");
        localposStrip = new Amg::Vector2D;
        return;
    }

    localposStrip = new Amg::Vector2D(detEl->rawLocalPositionOfCell( id ));
    //    (*localPosStrip)[Trk::distPhi] += (idhelper->is_pixel(id) ? m_pixelLorentzAngleTool : m_sctLorentzAngleTool)->getLorentzShift(detEl->identifyHash());
    // SiLorentzAngleTool cannot be used here because HitToSoNode is not a tool nor algorithm

    striplength    = detEl->etaPitch() ;
    stripWidth     = detEl->phiPitch( *localposStrip );
    stripThickness = detEl->thickness()/10.0;
    
}

#endif // BUILDVP1LIGHT
