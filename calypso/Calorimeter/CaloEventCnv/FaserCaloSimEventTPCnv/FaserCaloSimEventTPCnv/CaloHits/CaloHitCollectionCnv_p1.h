/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOHITCOLLECTIONCNV_P1_H
#define CALOHITCOLLECTIONCNV_P1_H

// CaloHitCollectionCnv_p1, T/P separation of Calo Hits
// author D.Costanzo <davide.costanzo@cern.ch>
// author O.Arnaez <olivier.arnaez@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "FaserCaloSimEvent/CaloHitCollection.h"
#include "CaloHitCollection_p1.h"


class CaloHitCollectionCnv_p1 : public T_AthenaPoolTPCnvBase<CaloHitCollection, CaloHitCollection_p1>
{
 public:

  CaloHitCollectionCnv_p1()  {};

  virtual CaloHitCollection* createTransient(const CaloHitCollection_p1* persObj, MsgStream &log);

  virtual void	persToTrans(const CaloHitCollection_p1* persCont,
                            CaloHitCollection* transCont,
                            MsgStream &log) ;
  virtual void	transToPers(const CaloHitCollection* transCont,
                            CaloHitCollection_p1* persCont,
                            MsgStream &log) ;

 private:

  static const double m_persEneUnit;
  static const double m_persLenUnit;
  static const double m_persAngUnit;
  static const double m_2bHalfMaximum;
  static const int m_2bMaximum;
};

#endif
