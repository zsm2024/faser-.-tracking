/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserSCT_CLUSTERCONTAINERCNV_TLP1_H
#define FaserSCT_CLUSTERCONTAINERCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrackerEventAthenaPool/FaserSCT_ClusterContainer_tlp1.h"

#include "TrackerEventAthenaPool/FaserSCT_ClusterContainerCnv_p3.h"
#include "TrackerEventAthenaPool/FaserSCT_ClusterCnv_p3.h"
#include "TrackerEventAthenaPool/FaserSiWidthCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"


#include <iostream>

class FaserSCT_ClusterContainerCnv_tlp1
   : public AthenaPoolTopLevelTPConverter<FaserSCT_ClusterContainerCnv_p3, FaserSCT_ClusterContainer_tlp1 >
{
public:

  FaserSCT_ClusterContainerCnv_tlp1();
  virtual ~FaserSCT_ClusterContainerCnv_tlp1() = default;

//  virtual void	setPStorage( FaserSCT_ClusterContainer_tlp1 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

  void setUseDetectorElement(const bool useDetectorElement);

protected:

  //SiClusterCnv_p1		m_siClusterCnv;
  FaserSCT_ClusterCnv_p3		m_sctClusterCnv;
  FaserSiWidthCnv_p1			m_siWidthCnv;
//  PrepRawDataCnv_p1             m_prepDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;


};


#endif


