/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerSimData/TrackerSimData.h"
#include "MsgUtil.h"

// Persistent class and converter header file
#include "TrackerEventAthenaPool/TrackerSimData_p2.h"
#include "TrackerSimDataCnv_p2.h"
#include "AthenaBaseComps/AthMessaging.h"


typedef std::vector<TrackerSimData::Deposit>::const_iterator depositIterator;

TrackerSimDataCnv_p2::TrackerSimDataCnv_p2()
  : m_sg(nullptr)
{
}


void
TrackerSimDataCnv_p2::persToTrans(const TrackerSimData_p2* persObj, TrackerSimData* transObj, MsgStream& log)
{
  // MSG_VERBOSE(log,"TrackerSimDataCnv_p2::persToTrans called ");
  std::vector<TrackerSimData::Deposit> deposits;
  deposits.reserve( persObj->m_enDeposits.size() );
  for (unsigned int icount=0; icount < persObj->m_enDeposits.size(); icount++) {
    HepMcParticleLink mcLink (m_sg);
    HepMcPLCnv.persToTrans(&(persObj->m_links[icount]),&mcLink, log);
    deposits.emplace_back (mcLink, persObj->m_enDeposits[icount]);
  }

  *transObj = TrackerSimData (std::move(deposits),
                              persObj->m_word);
}

void
TrackerSimDataCnv_p2::transToPers(const TrackerSimData* transObj, TrackerSimData_p2* persObj, MsgStream &log)
{
  //  MSG_VERBOSE(log,"TrackerSimDataCnv_p2::transToPers called ");
   HepMcParticleLinkCnv_p2 HepMcPLCnv;

   persObj->m_word = transObj->word();
   const std::vector<TrackerSimData::Deposit> &dep(transObj->getdeposits());
   persObj->m_links.resize(dep.size() );
   persObj->m_enDeposits.resize(dep.size() );
   depositIterator it=dep.begin();
   for (int icount=0; it != dep.end(); it++, icount++) {
     HepMcPLCnv.transToPers(&(dep[icount].first), &(persObj->m_links[icount]), log);
     persObj->m_enDeposits[icount] = dep[icount].second;
   }
}

void TrackerSimDataCnv_p2::setCurrentStore (IProxyDict* store)
{
  m_sg = store;
}