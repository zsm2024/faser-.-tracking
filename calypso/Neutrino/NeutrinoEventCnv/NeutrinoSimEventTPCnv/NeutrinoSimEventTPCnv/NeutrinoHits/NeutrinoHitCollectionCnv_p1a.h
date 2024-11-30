/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NEUTRINOHITCOLLECTIONCNV_P1A_H
#define NEUTRINOHITCOLLECTIONCNV_P1A_H

// NeutrinoHitCollectionCnv_p1a, T/P separation of Neutrino Hits
// author D.Costanzo <davide.costanzo@cern.ch>
// author O.Arnaez <olivier.arnaez@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "NeutrinoSimEvent/NeutrinoHitCollection.h"
#include "NeutrinoHitCollection_p1a.h"


class NeutrinoHitCollectionCnv_p1a : public T_AthenaPoolTPCnvBase<NeutrinoHitCollection, NeutrinoHitCollection_p1a>
{
 public:

  NeutrinoHitCollectionCnv_p1a()  {};

  virtual NeutrinoHitCollection* createTransient(const NeutrinoHitCollection_p1a* persObj, MsgStream &log);

  virtual void	persToTrans(const NeutrinoHitCollection_p1a* persCont,
                            NeutrinoHitCollection* transCont,
                            MsgStream &log) ;
  virtual void	transToPers(const NeutrinoHitCollection* transCont,
                            NeutrinoHitCollection_p1a* persCont,
                            MsgStream &log) ;

 private:

  static const double m_persEneUnit;
  static const double m_persLenUnit;
  static const double m_persAngUnit;
  static const double m_2bHalfMaximum;
  static const int m_2bMaximum;
};

#endif
