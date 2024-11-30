/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NeutrinoSimEvent/NeutrinoHit.h"
#include "Identifier/Identifier.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

#include "NeutrinoSimEventTPCnv/NeutrinoHits/NeutrinoHit_p1.h"
#include "NeutrinoSimEventTPCnv/NeutrinoHits/NeutrinoHitCnv_p1.h"


void
NeutrinoHitCnv_p1::persToTrans(const NeutrinoHit_p1* persObj, NeutrinoHit* transObj, MsgStream &log)
{
  HepMcParticleLinkCnv_p2 HepMcPLCnv;
  HepMcParticleLink link;
  HepMcPLCnv.persToTrans(&(persObj->m_partLink),&link, log);

   *transObj = NeutrinoHit (HepGeom::Point3D<double> (persObj->m_stX,
                                                   persObj->m_stY,
                                                   persObj->m_stZ),
                         HepGeom::Point3D<double> (persObj->m_enX,
                                                   persObj->m_enY,
                                                   persObj->m_enZ),
                         persObj->m_energyLoss,
                         persObj->m_meanTime,
                         link,
                         persObj->m_ID
                      );
}


void
NeutrinoHitCnv_p1::transToPers(const NeutrinoHit* transObj, NeutrinoHit_p1* persObj, MsgStream &log)
{
//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "NeutrinoHitCnv_p1::transToPers called " << endmsg;
   HepMcParticleLinkCnv_p2 HepMcPLCnv;

   HepGeom::Point3D<double> st = transObj->localStartPosition();
   persObj->m_stX         = st.x();
   persObj->m_stY         = st.y();
   persObj->m_stZ         = st.z();

   HepGeom::Point3D<double> en = transObj->localEndPosition();
   persObj->m_enX         = en.x();
   persObj->m_enY         = en.y();
   persObj->m_enZ         = en.z();

   persObj->m_energyLoss  = transObj->energyLoss();
   persObj->m_meanTime    = transObj->meanTime();
   persObj->m_ID          = transObj->identify();
   HepMcPLCnv.transToPers(&(transObj->particleLink()),&(persObj->m_partLink), log);
}
