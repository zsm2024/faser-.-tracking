/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

#include "FaserSiSpacePointTool/TrackerSpacePointMakerTool.h"

// Cluster and space point collections
#include "TrackerSpacePoint/FaserSCT_SpacePointCollection.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointOverlapCollection.h"

// For processing clusters
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SiLocalPosition.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"

// Space points
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"

namespace Tracker
{

static const InterfaceID IID_ITrackerSpacePointMakerTool
("TrackerSpacePointMakerTool", 252891434, 0);

const InterfaceID& TrackerSpacePointMakerTool::interfaceID() { 
  return IID_ITrackerSpacePointMakerTool; 
}

// Constructor with parameters:
TrackerSpacePointMakerTool::TrackerSpacePointMakerTool(const std::string& type,
    const std::string& name,
    const IInterface* parent) :
  AthAlgTool(type, name, parent) {
    declareInterface< TrackerSpacePointMakerTool>(this);
  }
//--------------------------------------------------------------------------
StatusCode TrackerSpacePointMakerTool::initialize()  {
  // Get the SCT Helper
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  m_SCTgapParameter = fabs(m_SCTgapParameter);
  if (m_SCTgapParameter > 0.002) m_SCTgapParameter = 0.002;
  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------------
StatusCode TrackerSpacePointMakerTool::finalize() {
  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------------
void TrackerSpacePointMakerTool::newEvent() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  std::lock_guard<std::mutex> lock{m_mutex};
  CacheEntry* ent{m_cache.get(ctx)};
  if (ent->m_evt!=ctx.evt()) { // New event in this slot
    ent->clear();
    ent->m_evt = ctx.evt();
  } else {
    ent->m_elementOLD = nullptr;
  }
}    
//--------------------------------------------------------------------------
FaserSCT_SpacePoint* TrackerSpacePointMakerTool::makeSCT_SpacePoint(const Tracker::FaserSCT_Cluster& cluster1, 
    const Tracker::FaserSCT_Cluster& cluster2,
    const Amg::Vector3D& vertexVec, 
    const TrackerDD::SiDetectorElement* element1,
    const TrackerDD::SiDetectorElement* element2,
    double stripLengthGapTolerance) const {

  // Find intersection of a line through a cluster on one sct detector and
  // a line through a cluster on its stereo pair. Return zero if lines 
  // don't intersect.

  // A general point on the line joining point a to point b is
  // x, where 2*x=(1+m)*a + (1-m)*b. Similarly for 2*y=(1+n)*c + (1-n)*d.
  // Suppose that v is the vertex, which we take as (0,0,0); this could
  // an input parameter later, if required. Requiring that the two 'general
  // points' lie on a straight through v means that the vector x-v is a 
  // multiple of y-v. This condition fixes the parameters m and n.
  // We then return the 'space-point' x, supposed to be the phi-layer. 
  // We require that -1<m<1, otherwise x lies 
  // outside the segment a to b; and similarly for n.

  Amg::Vector2D locpos = cluster1.localPosition();  
  Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
  std::pair<Amg::Vector3D, Amg::Vector3D> 
    ends1(element1->endsOfStrip(TrackerDD::SiLocalPosition(localPos.y(), localPos.x(), 0.))); 

  locpos = cluster2.localPosition();  
  localPos = Amg::Vector2D(locpos[0], locpos[1]);
  std::pair<Amg::Vector3D, Amg::Vector3D>
    ends2(element2->endsOfStrip(TrackerDD::SiLocalPosition(localPos.y(), localPos.x(), 0.))); 

  Amg::Vector3D a(ends1.first);   // Top end, first cluster
  Amg::Vector3D b(ends1.second);  // Bottom end, first cluster
  Amg::Vector3D c(ends2.first);   // Top end, second cluster
  Amg::Vector3D d(ends2.second);  // Bottom end, second cluster
  Amg::Vector3D q(a-b);           // vector joining ends of line
  Amg::Vector3D r(c-d);           // vector joining ends of line

  Amg::Vector3D point;

  bool ok(true);  
  if (m_usePerpProj) {
    /* a simple hack for the case the origin of the particle is completely unknown:
       The closest approach of element1 to element2 is used (perpendicular projection)
       to determine the position of the SpacePoint on element 1. 
       This option is especially aimed at the use with cosmics data.
       */
    double det = Amg::Vector3D(q.cross(r)).z();
    if (fabs(det)>10e-7)
    {
       Amg::Vector3D s(a+b);
       Amg::Vector3D t(c+d);
       double lambda0 = r.cross(Amg::Vector3D(s-t)).z()/det;
       double lambda1 = q.cross(Amg::Vector3D(s-t)).z()/det;
       point = (s + lambda0 * q)/2;
       ATH_MSG_VERBOSE( "Endpoints 1 : ( " <<  a.x() << " , " << a.y() << " , " << a.z() << " )   to   (" << b.x() << " , " << b.y() << " , " << b.z() << " ) " );
       ATH_MSG_VERBOSE( "Endpoints 2 : ( " <<  c.x() << " , " << c.y() << " , " << c.z() << " )   to   (" << d.x() << " , " << d.y() << " , " << d.z() << " )  " );
       ATH_MSG_VERBOSE( "Intersection: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   " );
       // require that the point is within the bounds of at least one of the two strips
       if (fabs(lambda0) > 1 + m_stripLengthTolerance)
       {
         if (fabs(lambda1) > 1 + m_stripLengthTolerance)
         {
           ATH_MSG_DEBUG("Intersection lies outside the bounds of both strips");
           ok = false;
         }
       }
    }
    else
    {
      ATH_MSG_WARNING("Intersection failed");
      ok = false;
    }
  } else {   
    Amg::Vector3D s(a+b-2.*vertexVec);  // twice the vector from vertex to midpoint
    Amg::Vector3D t(c+d-2.*vertexVec);  // twice the vector from vertex to midpoint
    Amg::Vector3D qs(q.cross(s));  
    Amg::Vector3D rt(r.cross(t));  
    double m(-s.dot(rt)/q.dot(rt));    // ratio for first  line
    double n(0.);                      // ratio for second line

    // We increase the length of the strip by 1%. This a fudge which allows
    // us to recover space-points from tracks pointing back to an interaction
    // point up to around z = +- 20 cm

    double limit = 1. + m_stripLengthTolerance;

    if      (fabs(            m             ) > limit) ok = false;
    else if (fabs((n=-(t.dot(qs)/r.dot(qs)))) > limit) ok = false;

    if ((not ok) and (stripLengthGapTolerance != 0.)) {

      double qm     = q.mag()                             ;
      double limitn = limit+(stripLengthGapTolerance/qm);

      if (fabs(m) <= limitn)  {

	if (n==0.) n = -(t.dot(qs)/r.dot(qs));

	if (fabs(n) <= limitn) {
	  double mn  = q.dot(r)/(qm*qm);
	  if ((m >  1.) or (n >  1.)) {
	    double dm = (m-1.)   ;
	    double dn = (n-1.)*mn;
	    double sm = dm;
	    if (dm < dn) sm = dn;
	    m -= sm;
	    n -= (sm*mn);
	  } else if ((m < -1.) or (n < -1.)) {
	    double dm = -(m+1.)   ;
	    double dn = -(n+1.)*mn;
	    double sm = dm;
	    if (dm < dn) sm = dn;
	    m += sm;
	    n += (sm*mn);
	  }
	  if (fabs(m) < limit and fabs(n) < limit) ok = true;
	}
      }
    }
    if (ok) point = 0.5*(a + b + m*q);
  }
  if (ok) {
    ATH_MSG_VERBOSE( "SpacePoint generated at: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   " );       
    std::pair<IdentifierHash,IdentifierHash> elementIdList( element1->identifyHash() , element2->identifyHash() ); 
    std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*> clusList {&cluster1, &cluster2 };
    return new FaserSCT_SpacePoint(elementIdList, point, &clusList);
  }

  return nullptr;
}

//--------------------------------------------------------------------------
void TrackerSpacePointMakerTool::fillSCT_SpacePointCollection(const Tracker::FaserSCT_ClusterCollection* clusters1, 
    const Tracker::FaserSCT_ClusterCollection* clusters2,
    double min, double max, bool allClusters, 
    const Amg::Vector3D& vertexVec,
    const TrackerDD::SiDetectorElementCollection* elements,
    FaserSCT_SpacePointCollection* spacepointCollection) const {
  double stripLengthGapTolerance = 0.;

  // Try all combinations of clusters for space points
  FaserSCT_ClusterCollection::const_iterator clusters1Next = clusters1->begin();
  FaserSCT_ClusterCollection::const_iterator clusters1Finish = clusters1->end();

  // -ME fixme- get first element
  const TrackerDD::SiDetectorElement* element1 = nullptr;

  if ((*clusters1Next) and (clusters1Next != clusters1Finish)) {
    element1 = elements->getDetectorElement(clusters1->identifyHash());
  }

  if (element1==nullptr) {
    ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters1Next)->identify()));
    return;
  }

  //tmpSpacePoints changed to local variable to enable rentrancy
  std::vector<FaserSCT_SpacePoint*> tmpSpacePoints;

  for (; clusters1Next!=clusters1Finish; ++clusters1Next){   
    Amg::Vector2D locpos = (*clusters1Next)->localPosition();
    Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
    double xPhi1 = TrackerDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi();    
    FaserSCT_ClusterCollection::const_iterator clusters2Next=(*clusters2).begin();
    FaserSCT_ClusterCollection::const_iterator clusters2Finish=(*clusters2).end();

    // -ME fixme- get first element
    const TrackerDD::SiDetectorElement* element2 = nullptr;
    if (*clusters2Next and (clusters2Next != clusters2Finish)) {
      element2= elements->getDetectorElement(clusters2->identifyHash());
    }

    if (element2==nullptr) {
      ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters2Next)->identify()));
      break;
    } 

    if (m_SCTgapParameter != 0.) {
      double dm = offset(element1, element2, stripLengthGapTolerance);
      min -= dm;
      max += dm;
    }

    for (; clusters2Next!=clusters2Finish; ++clusters2Next){
      Amg::Vector2D locpos = (*clusters2Next)->localPosition();
      Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
      double diff = TrackerDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi() - xPhi1;
      if (((min <= diff) and (diff <= max)) or allClusters) {
	FaserSCT_SpacePoint* sp =
	  makeSCT_SpacePoint(**clusters1Next, **clusters2Next, vertexVec, element1, element2, stripLengthGapTolerance);
	if (sp) {
	  tmpSpacePoints.push_back(sp);
	}
      }
    }
  }

  spacepointCollection->reserve(spacepointCollection->size() + tmpSpacePoints.size());
  for (FaserSCT_SpacePoint* sp: tmpSpacePoints) {
    spacepointCollection->push_back(sp);
  }
}



//--------------------------------------------------------------------------
void TrackerSpacePointMakerTool::fillSCT_SpacePointEtaOverlapCollection(const Tracker::FaserSCT_ClusterCollection* clusters1, 
    const Tracker::FaserSCT_ClusterCollection* clusters2,
    double min, double max, bool allClusters, 
    const Amg::Vector3D& vertexVec,
    const TrackerDD::SiDetectorElementCollection* elements,
    FaserSCT_SpacePointOverlapCollection* spacepointoverlapCollection) const {

  double stripLengthGapTolerance = 0.; 

  // Require that (xPhi2 - xPhi1) lie in the range specified.
  // Used eta modules
  // Try all combinations of clusters for space points
  Tracker::FaserSCT_ClusterCollection::const_iterator clusters1Next = clusters1->begin();
  Tracker::FaserSCT_ClusterCollection::const_iterator clusters1Finish = clusters1->end();

  const TrackerDD::SiDetectorElement* element1 = nullptr;

  if ((*clusters1Next) and (clusters1Next != clusters1Finish)) {
    element1 = elements->getDetectorElement(clusters1->identifyHash());
  }
  if (element1==nullptr) {
    ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters1Next)->identify()));
    return;
  } 
  for (; clusters1Next!=clusters1Finish; ++clusters1Next) {

    Amg::Vector2D locpos = (*clusters1Next)->localPosition();
    Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
    double xPhi1 = TrackerDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi();

    Tracker::FaserSCT_ClusterCollection::const_iterator clusters2Next = (*clusters2).begin();
    Tracker::FaserSCT_ClusterCollection::const_iterator clusters2Finish = (*clusters2).end();

    const TrackerDD::SiDetectorElement* element2 = nullptr;
    if (*clusters2Next and (clusters2Next != clusters2Finish)) {
      element2 = elements->getDetectorElement(clusters2->identifyHash());
    }
    if (element2==nullptr) {
      ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters2Next)->identify()));
      break;
    } 
    if (m_SCTgapParameter != 0.) {
      double dm = offset(element1, element2, stripLengthGapTolerance);
      min -= dm;
      max += dm;
    }

    for (; clusters2Next!=clusters2Finish; ++clusters2Next){
      Amg::Vector2D locpos = (*clusters2Next)->localPosition();
      Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
      double diff = TrackerDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi() - xPhi1;
      if (((min <= diff) and (diff <= max)) or allClusters){
	FaserSCT_SpacePoint* sp =
	  makeSCT_SpacePoint(**clusters1Next, **clusters2Next, vertexVec, element1, element2, stripLengthGapTolerance);
	if (sp) {
	  spacepointoverlapCollection->push_back(sp);
	}
      }
    }
  }

}


//--------------------------------------------------------------------------
void TrackerSpacePointMakerTool::fillSCT_SpacePointPhiOverlapCollection(const Tracker::FaserSCT_ClusterCollection* clusters1, 
    const Tracker::FaserSCT_ClusterCollection* clusters2,
    double min1, double max1, double min2, double max2, 
    bool allClusters,
    const Amg::Vector3D& vertexVec,
    const TrackerDD::SiDetectorElementCollection* elements,
    FaserSCT_SpacePointOverlapCollection* spacepointoverlapCollection) const {

  double stripLengthGapTolerance = 0.;
  if (m_SCTgapParameter != 0.) {
    min1 -= 20.;
    max1 += 20.;
  }

  // Clus1 must lie
  // within min1 and max1 and clus between min2 and max2. Used for phi
  // overlaps.

  // Try all combinations of clusters for space points
  Tracker::FaserSCT_ClusterCollection::const_iterator clusters1Next = clusters1->begin();
  Tracker::FaserSCT_ClusterCollection::const_iterator clusters1Finish = clusters1->end();

  const TrackerDD::SiDetectorElement* element1 = nullptr;
  if ((*clusters1Next) and (clusters1Next != clusters1Finish)) {
    element1 = elements->getDetectorElement(clusters1->identifyHash());
  }
  if (element1==nullptr) {
    ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters1Next)->identify()));
    return;
  } 

  for (; clusters1Next!=clusters1Finish; ++clusters1Next){
    Amg::Vector2D locpos = (*clusters1Next)->localPosition();
    Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
    double xPhi1 = TrackerDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi();
    if (((min1 <= xPhi1) and (xPhi1 <= max1)) or allClusters) {
      Tracker::FaserSCT_ClusterCollection::const_iterator clusters2Next = (*clusters2).begin();
      Tracker::FaserSCT_ClusterCollection::const_iterator clusters2Finish = (*clusters2).end();

      const TrackerDD::SiDetectorElement* element2 = nullptr;
      if (*clusters2Next and (clusters2Next != clusters2Finish)) {
	element2 = elements->getDetectorElement(clusters2->identifyHash());
      }
      if (element2==nullptr) {
	ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters2Next)->identify()));
	break;
      }

      if (m_SCTgapParameter != 0.) {
	double dm = offset(element1, element2, stripLengthGapTolerance);
	min2 -= dm;
	max2 += dm;
      }

      for (; clusters2Next!=clusters2Finish; ++clusters2Next) {
	Amg::Vector2D locpos = (*clusters2Next)->localPosition(); 
	Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
	double xPhi2 = TrackerDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi();
	if (((min2<= xPhi2) and (xPhi2 <= max2)) or allClusters) {
	  FaserSCT_SpacePoint* sp =
	    makeSCT_SpacePoint(**clusters1Next, **clusters2Next, vertexVec, element1, element2, stripLengthGapTolerance);
	  if (sp) {
	    spacepointoverlapCollection->push_back(sp);
	  }
	}
      }
    }   
  }
}

///////////////////////////////////////////////////////////////////
// Possible offset estimation in Z or R direction due to gap size 
///////////////////////////////////////////////////////////////////

double TrackerSpacePointMakerTool::offset
(const TrackerDD::SiDetectorElement* element1, const TrackerDD::SiDetectorElement* element2, double& stripLengthGapTolerance) const
{
  const Amg::Transform3D& T1 = element1->transform();
  const Amg::Transform3D& T2 = element2->transform();

  double x12 = T1(0,0)*T2(0,0)+T1(1,0)*T2(1,0)+T1(2,0)*T2(2,0)                              ;
  double r   = sqrt(T1(0,3)*T1(0,3)+T1(1,3)*T1(1,3))                                        ;
  double s   = (T1(0,3)-T2(0,3))*T1(0,2)+(T1(1,3)-T2(1,3))*T1(1,2)+(T1(2,3)-T2(2,3))*T1(2,2);

  double dm  = (m_SCTgapParameter*r)*fabs(s*x12);
  double d   = dm/sqrt((1.-x12)*(1.+x12));

  if (fabs(T1(2,2)) > 0.7) d*=(r/fabs(T1(2,3))); // endcap d = d*R/Z

  stripLengthGapTolerance = d; 
  return dm;
}

void TrackerSpacePointMakerTool::offset(double& stripLengthGapTolerance,
    const TrackerDD::SiDetectorElement* element1,
    const TrackerDD::SiDetectorElement* element2) const {
  const Amg::Transform3D& T1 = element1->transform();
  const Amg::Transform3D& T2 = element2->transform();
  Amg::Vector3D           C  = element1->center()   ;

  double r   = sqrt(C[0]*C[0]+C[1]*C[1])                                                    ;
  double x12 = T1(0,0)*T2(0,0) + T1(1,0)*T2(1,0) + T1(2,0)*T2(2,0)                              ;
  double s   = (T1(0,3)-T2(0,3))*T1(0,2) + (T1(1,3)-T2(1,3))*T1(1,2) + (T1(2,3)-T2(2,3))*T1(2,2);

  double dm  = (m_SCTgapParameter*r)*fabs(s*x12);

  double d   = 0.;
  if (element1->design().shape() == TrackerDD::Trapezoid || element1->design().shape() == TrackerDD::Annulus) {
    d = dm*(1./0.04);
  } else {
    d = dm/sqrt((1.-x12)*(1.+x12));
  }

  if (fabs(T1(2,2)) > 0.7) d *= (r/fabs(T1(2,3))); // endcap d = d*R/Z
  stripLengthGapTolerance = d;
}

///////////////////////////////////////////////////////////////////
// Compare SCT strips and space points production
///////////////////////////////////////////////////////////////////

void TrackerSpacePointMakerTool::makeSCT_SpacePoints(const double stripLengthGapTolerance) const {
  // Find intersection of a line through a cluster on one sct detector and
  // a line through a cluster on its stereo pair. Return zero if lines
  // don't intersect.

  // A general point on the line joining point a to point b is
  // x, where 2*x=(1+m)*a + (1-m)*b. Similarly for 2*y=(1+n)*c + (1-n)*d.
  // Suppose that v is the vertex, which we take as (0,0,0); this could
  // an input parameter later, if required. Requiring that the two 'general
  // points' lie on a straight through v means that the vector x-v is a
  // multiple of y-v. This condition fixes the parameters m and n.
  // We then return the 'space-point' x, supposed to be the phi-layer.
  // We require that -1<m<1, otherwise x lies
  // outside the segment a to b; and similarly for n.

  const EventContext& ctx{Gaudi::Hive::currentContext()};
  std::lock_guard<std::mutex> lock{m_mutex};
  CacheEntry* ent{m_cache.get(ctx)};
  if (ent->m_evt!=ctx.evt()) { // New event in this slot
    ent->clear();
    ent->m_evt = ctx.evt();
  } else {
    if (ent->m_tmpSpacePoints.size()) {
      for (FaserSCT_SpacePoint* sp : ent->m_tmpSpacePoints) {
	delete sp;
      }
    }
    ent->m_tmpSpacePoints.clear();
  }

  std::vector<FaserSCTinformation>::iterator I    = ent->m_SCT0.begin(), IE = ent->m_SCT0.end();
  std::vector<FaserSCTinformation>::iterator J,JB = ent->m_SCT1.begin(), JE = ent->m_SCT1.end();

  double limit = 1. + m_stripLengthTolerance;

  for (; I!=IE; ++I) {
    double qm     = (*I).qm();
    for (J=JB; J!=JE; ++J) {
      double limitm = limit+(stripLengthGapTolerance*qm);

      double a =-(*I).s().dot((*J).qs());
      double b = (*I).q().dot((*J).qs());
      if (fabs(a) > fabs(b)*limitm) continue;

      double qn     = (*J).qm();
      double limitn = limit+(stripLengthGapTolerance*qn);

      double c =-(*J).s().dot((*I).qs());
      double d = (*J).q().dot((*I).qs());
      if (fabs(c) > fabs(d)*limitn) continue;

      double m = a/b;
      double n = c/d;

      if (m >  limit or n >  limit) {

	double cs  = (*I).q().dot((*J).q())*(qm*qm);
	double dm  = (m-1.);
	double dmn = (n-1.)*cs;
	if (dmn > dm) dm = dmn;
	m-=dm; n-=(dm/cs);

      } else if (m < -limit or n < -limit) {

	double cs = (*I).q().dot((*J).q())*(qm*qm);
	double dm  = -(1.+m);
	double dmn = -(1.+n)*cs;
	if (dmn > dm) dm = dmn;
	m+=dm; n+=(dm/cs);

      }

      if (fabs(m) > limit or fabs(n) > limit) continue;

      Amg::Vector3D point((*I).position(m));

      ATH_MSG_VERBOSE("SpacePoint generated at: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   ");
      const std::pair<IdentifierHash, IdentifierHash> elementIdList(ent->m_element0->identifyHash(), ent->m_element1->identifyHash());
      const std::pair<const FaserSCT_Cluster*, const FaserSCT_Cluster*> clusList {(*I).cluster(), (*J).cluster()};
      ent->m_tmpSpacePoints.push_back(new FaserSCT_SpacePoint(elementIdList, point, &clusList));
    }
  }
}

}
