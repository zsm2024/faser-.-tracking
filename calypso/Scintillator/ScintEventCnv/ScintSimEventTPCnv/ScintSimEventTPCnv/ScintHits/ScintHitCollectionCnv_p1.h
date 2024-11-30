/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTHITCOLLECTIONCNV_P1_H
#define SCINTHITCOLLECTIONCNV_P1_H

// ScintHitCollectionCnv_p1, T/P separation of Scint Hits
// author D.Costanzo <davide.costanzo@cern.ch>
// author O.Arnaez <olivier.arnaez@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ScintSimEvent/ScintHitCollection.h"
#include "ScintHitCollection_p1.h"


class ScintHitCollectionCnv_p1 : public T_AthenaPoolTPCnvBase<ScintHitCollection, ScintHitCollection_p1>
{
 public:

  ScintHitCollectionCnv_p1()  {};

  virtual ScintHitCollection* createTransient(const ScintHitCollection_p1* persObj, MsgStream &log);

  virtual void	persToTrans(const ScintHitCollection_p1* persCont,
                            ScintHitCollection* transCont,
                            MsgStream &log) ;
  virtual void	transToPers(const ScintHitCollection* transCont,
                            ScintHitCollection_p1* persCont,
                            MsgStream &log) ;

 private:

  static const double m_persEneUnit;
  static const double m_persLenUnit;
  static const double m_persAngUnit;
  static const double m_2bHalfMaximum;
  static const int m_2bMaximum;
};

#endif
