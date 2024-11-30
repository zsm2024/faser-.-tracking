/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class SimHitHandle_NeutrinoHit             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VTI12TrackSystems/SimHitHandle_NeutrinoHit.h"
#include "VP1Base/VP1Msg.h"
#include "VTI12Utils/VP1DetInfo.h"
#include "VTI12Utils/VP1ParticleData.h"

#include "NeutrinoSimEvent/NeutrinoHit.h"
#include "NeutrinoReadoutGeometry/NeutrinoDetectorElement.h"
#include "NeutrinoReadoutGeometry/EmulsionDetectorManager.h"
#include "NeutrinoIdentifier/EmulsionID.h"
#include "VTI12TrackSystems/VP1TrackSanity.h"

//____________________________________________________________________
class SimHitHandle_NeutrinoHit::Imp {
public:
  Imp( const NeutrinoHit * h ) : thehit(h),detelem(0) {}
  const NeutrinoHit * thehit;
  mutable const NeutrinoDD::NeutrinoDetectorElement * detelem;
  bool ensureDetElemInit() const;
  Amg::Vector3D localToGlobal(const HepGeom::Point3D<double>&) const;
};


//____________________________________________________________________
SimHitHandle_NeutrinoHit::SimHitHandle_NeutrinoHit(const NeutrinoHit * h)
  : SimHitHandleBase(), m_d(new Imp(h))
{
  if (!h)
    VP1Msg::message("SimHitHandle_NeutrinoHit constructor ERROR: Received null hit pointer");
}

//____________________________________________________________________
SimHitHandle_NeutrinoHit::~SimHitHandle_NeutrinoHit()
{
  delete m_d;
}

//____________________________________________________________________
bool SimHitHandle_NeutrinoHit::Imp::ensureDetElemInit() const
{
  if (detelem)
    return true;
  int module = thehit->getModule();
  int base   = thehit->getBase();
  int film   = thehit->getFilm();

  Identifier id = VP1DetInfo::emulsionIDHelper()->film_id( module, base, film);
    //fixme: id is_valid ?
  detelem = VP1DetInfo::emulsionDetMgr()->getDetectorElement(id);
  //Fixme : Handle case where detelem can not be found gracefully. And check pointers from VP1DetInfo!!
  if (!detelem)
    VP1Msg::messageDebug("SimHitHandle_NeutrinoHit ERROR: Could not get detector element for hit!");
  return detelem!=0;
}

//____________________________________________________________________
Amg::Vector3D SimHitHandle_NeutrinoHit::momentumDirection() const
{
  if (VP1Msg::verbose()&&m_d->thehit->localEndPosition()==m_d->thehit->localStartPosition())
    VP1Msg::messageVerbose("SimHitHandle_NeutrinoHit::momentumDirection() ERROR: posStart()==posEnd()");
  return (posEnd()-posStart()).unit();
}

//____________________________________________________________________
Amg::Vector3D SimHitHandle_NeutrinoHit::Imp::localToGlobal( const HepGeom::Point3D<double>& local ) const
{
  if (!ensureDetElemInit())
    return Amg::Vector3D(0,0,0);
  return Amg::Vector3D(Amg::EigenTransformToCLHEP(detelem->transformHit()) * local);
}

//____________________________________________________________________
Amg::Vector3D SimHitHandle_NeutrinoHit::posStart() const
{
  return m_d->localToGlobal(m_d->thehit->localStartPosition());
}

//____________________________________________________________________
Amg::Vector3D SimHitHandle_NeutrinoHit::posEnd() const
{
  return m_d->localToGlobal(m_d->thehit->localEndPosition());
}

//____________________________________________________________________
double SimHitHandle_NeutrinoHit::hitTime() const
{
  return m_d->thehit->meanTime();
}

//____________________________________________________________________
const HepMcParticleLink& SimHitHandle_NeutrinoHit::particleLink() const
{
  return m_d->thehit->particleLink();
}

//____________________________________________________________________
Trk::TrackParameters * SimHitHandle_NeutrinoHit::createTrackParameters() const
{
  //Find charge and magnitude of momentum:
  double c;
  if ( !hasCharge() ) {
    bool ok;
    c = VP1ParticleData::particleCharge(pdg(),ok);
    if (!ok) {
      VP1Msg::message("SimHitHandle_NeutrinoHit::createTrackParameters ERROR: Could not find particle charge (pdg="
		      +QString::number(pdg())+"). Assuming charge=+1.");
      c = +1.0;
    } else {
      if (VP1Msg::verbose())
        VP1Msg::messageVerbose("Looked up particle charge for scintillator simhit with pdg code "+VP1Msg::str(pdg())+": "+VP1Msg::str(c));
    }
    const_cast<SimHitHandle_NeutrinoHit*>(this)->setCharge(c);
  } else {
    c = charge();
  }

  double mom = momentum();
  if (mom<=0) {
    VP1Msg::message("SimHitHandle_NeutrinoHit::createTrackParameters ERROR: Unknown momentum. Using 1 GeV");
    mom = 1*CLHEP::GeV;
  }

  ////We could in principle get a surface like this:
  //   if (!m_d->ensureDetElemInit()) {
  //     VP1Msg::messageDebug("SimHitHandle_NeutrinoHit WARNING: Could not get detector element!");
  //     return 0;
  //   }
  //   const Trk::PlaneSurface * surf
  //     = dynamic_cast<const Trk::PlaneSurface *>( &(m_d->detelem->surface()));
  //   if (!surf) {
  //     VP1Msg::message("SimHitHandle_NeutrinoHit::createTrackParameters ERROR: could not get Trk::PlaneSurface");
  //     return 0;
  //   }
  ////And then proceed to construct a new AtaPlane parameter with that
  ////surface. However, that gives some problems, so instead we simply
  ////create a perigee:

  const Amg::Vector3D globpos = posStart();

//  const Trk::GlobalMomentum u(momentumDirection());
  const Amg::Vector3D u(momentumDirection());

  return new Trk::Perigee(0, 0, u.phi(), u.theta(), c/mom, globpos);
}
