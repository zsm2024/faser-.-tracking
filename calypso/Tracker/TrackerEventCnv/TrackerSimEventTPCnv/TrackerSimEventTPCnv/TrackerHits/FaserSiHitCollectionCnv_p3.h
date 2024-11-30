/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSIHITCOLLECTIONCNV_P3_H
#define FASERSIHITCOLLECTIONCNV_P3_H

// FaserSiHitCollectionCnv_p3, T/P separation of FaserSi Hits
// author D.Costanzo <davide.costanzo@cern.ch>
// author O.Arnaez <olivier.arnaez@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "FaserSiHitCollection_p3.h"


class FaserSiHitCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<FaserSiHitCollection, FaserSiHitCollection_p3>
{
 public:

  FaserSiHitCollectionCnv_p3()  {};

  virtual FaserSiHitCollection* createTransient(const FaserSiHitCollection_p3* persObj, MsgStream &log);

  virtual void	persToTrans(const FaserSiHitCollection_p3* persCont,
                            FaserSiHitCollection* transCont,
                            MsgStream &log) ;
  virtual void	transToPers(const FaserSiHitCollection* transCont,
                            FaserSiHitCollection_p3* persCont,
                            MsgStream &log) ;

 private:

  static const double m_persEneUnit;
  static const double m_persLenUnit;
  static const double m_persAngUnit;
  static const double m_2bHalfMaximum;
  static const int m_2bMaximum;
};

#endif
