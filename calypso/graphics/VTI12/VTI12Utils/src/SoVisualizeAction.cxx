/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VTI12Utils/SoVisualizeAction.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTessellatedSolid.h"
#include "GeoModelKernel/GeoFacet.h"
#include "GeoModelKernel/GeoGenericTrap.h"
#include "VP1HEPVis/nodes/SoTubs.h"
#include "VP1HEPVis/nodes/SoCons.h"
#include "VP1HEPVis/nodes/SoGenericBox.h"
#include "VP1HEPVis/nodes/SoPcons.h"
#include "VP1HEPVis/nodes/SoTessellated.h"
#include "VP1HEPVis/SbPolyhedron.h"
#include "VP1HEPVis/nodes/SoPolyhedron.h"
#include "VP1HEPVis/VP1HEPVisUtils.h"
#include "VTI12Utils/SbPolyhedrizeAction.h"

// System of units
#ifdef BUILDVP1LIGHT
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // --> 'GeoModelKernelUnits::cm'
#else
  #include "GaudiKernel/SystemOfUnits.h"
  #define SYSTEM_OF_UNITS Gaudi::Units // --> 'Gaudi::Units::cm'
#endif

#include <iostream>

SoVisualizeAction::SoVisualizeAction()
  : m_shape(0)
{
  VP1HEPVisUtils::initAllCustomClasses();
  setDepthLimit(0);
}

SoVisualizeAction::~SoVisualizeAction()
{
  // Don't delete.  Let ref count take care of the memory.
}

void SoVisualizeAction::handleShape(const GeoShape *shape)
{
  //qDebug() << "SoVisualizeAction::handleShape";
  // We don't recognize it.  Try to polyhedrize it!
  SbPolyhedrizeAction a;
  shape->exec(&a);
  const SbPolyhedron *poly =a.getPolyhedron();
  if (poly) {
    SoPolyhedron *myPoly = new SoPolyhedron(poly);
    m_shape=myPoly;
  }

}

void SoVisualizeAction::handleBox(const GeoBox *box)
{
  //qDebug() << "SoVisualizeAction::handleBox";
  SoGenericBox * gb = new SoGenericBox;
  gb->setParametersForBox( box->getXHalfLength(),box->getYHalfLength(),box->getZHalfLength() );
  m_shape=gb;
}

void SoVisualizeAction::handleCons(const GeoCons *cons)
{
  //qDebug() << "SoVisualizeAction::handleCons";
  SoCons::initClass();
  SoCons *socons= new SoCons;
  socons->fRmin1 =cons->getRMin1();
  socons->fRmin2 =cons->getRMin2();
  socons->fRmax1 =cons->getRMax1();
  socons->fRmax2 =cons->getRMax2();
  socons->fDz    =cons->getDZ();
  socons->fSPhi  =cons->getSPhi();
  socons->fDPhi  =cons->getDPhi();

  m_shape=socons;
}

void SoVisualizeAction::handlePcon(const GeoPcon *pcon)
{

  //qDebug() << "SoVisualizeAction::handlePcon";

  //Set up temporary data arrays for profile:
  float *z  = new float[pcon->getNPlanes()];
  float *rmn= new float[pcon->getNPlanes()];
  float *rmx= new float[pcon->getNPlanes()];

  for (unsigned s=0;s<pcon->getNPlanes();++s) {
    z[s]=pcon->getZPlane(s);
    rmn[s]=pcon->getRMinPlane(s);
    rmx[s]=pcon->getRMaxPlane(s);
  }

  SoPcons::initClass();
  SoPcons *sopcons = new SoPcons();
  sopcons->fSPhi = pcon->getSPhi();
  sopcons->fDPhi = pcon->getDPhi();
  sopcons->fRmin.setValues(0,pcon->getNPlanes(),rmn);
  sopcons->fRmax.setValues(0,pcon->getNPlanes(),rmx);
  sopcons->fDz.setValues  (0,pcon->getNPlanes(),z);

  //Delete temporary arrays:
  delete  [] z;
  delete  [] rmn;
  delete  [] rmx;

  m_shape=sopcons;

}

void SoVisualizeAction::handleTrap(const GeoTrap *trap)
{
  //qDebug() << "SoVisualizeAction::handleTrap";
  SoGenericBox * gb = new SoGenericBox;
  gb->setParametersForTrapezoid(trap->getZHalfLength(), trap->getTheta(), trap->getPhi(),
				trap->getDydzn(), trap->getDxdyndzn(), trap->getDxdypdzn(),
				trap->getDydzp(), trap->getDxdyndzp(), trap->getDxdypdzp(),
				trap->getAngleydzn(), trap->getAngleydzp());
  m_shape=gb;
}

void SoVisualizeAction::handleTrd(const GeoTrd *trd)
{
  //qDebug() << "SoVisualizeAction::handleTrd";
  SoGenericBox * gb = new SoGenericBox;
  gb->setParametersForTrd( trd->getXHalfLength1(), trd->getXHalfLength2(),
			   trd->getYHalfLength1(), trd->getYHalfLength2(),
			   trd->getZHalfLength() );
  m_shape=gb;
}

void SoVisualizeAction::handleTube(const GeoTube *tube)
{
  //qDebug() << "SoVisualizeAction::handleTube";
  SoTubs *sotubs= new SoTubs;
  sotubs->pRMin= tube->getRMin();
  sotubs->pRMax= tube->getRMax();
  sotubs->pDz  = tube->getZHalfLength();
  sotubs->pSPhi= 0;
  sotubs->pDPhi= 2*M_PI;
  m_shape=sotubs;
}

void SoVisualizeAction::handleTubs(const GeoTubs *tubs)
{
  //qDebug() << "SoVisualizeAction::handleTubs";
  SoTubs *sotubs= new SoTubs;
  sotubs->pRMin= tubs->getRMin();
  sotubs->pRMax= tubs->getRMax();
  sotubs->pDz  = tubs->getZHalfLength();
  sotubs->pSPhi= tubs->getSPhi();
  sotubs->pDPhi= tubs->getDPhi();
  m_shape=sotubs;
}

void SoVisualizeAction::handleSimplePolygonBrep(const GeoSimplePolygonBrep *brep)
{
  //qDebug() << "SoVisualizeAction::handleSimplePolygonBrep";
  //Fixme: Detect if order of vertices is the wrong way around... and reorder if necessary.

  double dz = brep->getDZ();
  std::vector<double> x, y;
  for(unsigned int i=0; i<brep->getNVertices(); ++i)
  {
    x.push_back(brep->getXVertex(i));
    y.push_back(brep->getYVertex(i));
  }

  SbPolyhedronPolygonXSect sbPoly(x,y,dz);
  SoPolyhedron* soPoly = new SoPolyhedron(sbPoly);
  m_shape = soPoly;
}

void SoVisualizeAction::handleTessellatedSolid (const GeoTessellatedSolid* geoTessellated)
{
  //qDebug() << "SoVisualizeAction::handleTessellatedSolid";

  SoTessellated * soTessellated = new SoTessellated;
  for(size_t i=0; i<geoTessellated->getNumberOfFacets();++i) {
    GeoFacet* facet = geoTessellated->getFacet(i);
    if(facet->getNumberOfVertices()==3) {
      if(facet->getVertexType()==GeoFacet::ABSOLUTE)
	soTessellated->addTriangularFacet(facet->getVertex(0).x(),facet->getVertex(0).y(),facet->getVertex(0).z(),
					  facet->getVertex(1).x(),facet->getVertex(1).y(),facet->getVertex(1).z(),
					  facet->getVertex(2).x(),facet->getVertex(2).y(),facet->getVertex(2).z());
      else
	soTessellated->addTriangularFacet(facet->getVertex(0).x(),facet->getVertex(0).y(),facet->getVertex(0).z(),
					  facet->getVertex(0).x()+facet->getVertex(1).x(),
					  facet->getVertex(0).y()+facet->getVertex(1).y(),
					  facet->getVertex(0).z()+facet->getVertex(1).z(),
					  facet->getVertex(0).x()+facet->getVertex(2).x(),
					  facet->getVertex(0).y()+facet->getVertex(2).y(),
					  facet->getVertex(0).z()+facet->getVertex(2).z());
    }
    else {
      if(facet->getVertexType()==GeoFacet::ABSOLUTE)
	soTessellated->addQuadrangularFacet(facet->getVertex(0).x(),facet->getVertex(0).y(),facet->getVertex(0).z(),
					    facet->getVertex(1).x(),facet->getVertex(1).y(),facet->getVertex(1).z(),
					    facet->getVertex(2).x(),facet->getVertex(2).y(),facet->getVertex(2).z(),
					    facet->getVertex(3).x(),facet->getVertex(3).y(),facet->getVertex(3).z());
      else
	soTessellated->addQuadrangularFacet(facet->getVertex(0).x(),facet->getVertex(0).y(),facet->getVertex(0).z(),
					    facet->getVertex(0).x()+facet->getVertex(1).x(),
					    facet->getVertex(0).y()+facet->getVertex(1).y(),
					    facet->getVertex(0).z()+facet->getVertex(1).z(),
					    facet->getVertex(0).x()+facet->getVertex(2).x(),
					    facet->getVertex(0).y()+facet->getVertex(2).y(),
					    facet->getVertex(0).z()+facet->getVertex(2).z(),
					    facet->getVertex(0).x()+facet->getVertex(3).x(),
					    facet->getVertex(0).y()+facet->getVertex(3).y(),
					    facet->getVertex(0).z()+facet->getVertex(3).z());

    }
  }
  soTessellated->finalize();
  m_shape=soTessellated;
}

void SoVisualizeAction::handleGenericTrap(const GeoGenericTrap *gentrap)
{
  //qDebug() << "SoVisualizeAction::handleGenericTrap";
  SoGenericBox * gb = new SoGenericBox;
  const GeoGenericTrapVertices& trapVertices = gentrap->getVertices();
  double dZ = gentrap->getZHalfLength();
  gb->setGenericParameters(trapVertices[0].x(),trapVertices[0].y(),-dZ,
			   trapVertices[1].x(),trapVertices[1].y(),-dZ,
			   trapVertices[2].x(),trapVertices[2].y(),-dZ,
			   trapVertices[3].x(),trapVertices[3].y(),-dZ,
			   trapVertices[4].x(),trapVertices[4].y(),dZ,
			   trapVertices[5].x(),trapVertices[5].y(),dZ,
			   trapVertices[6].x(),trapVertices[6].y(),dZ,
			   trapVertices[7].x(),trapVertices[7].y(),dZ);
  m_shape=gb;
}
