/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTERCONTAINERCNV_H
#define FASERSCT_CLUSTERCONTAINERCNV_H

#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"



class StoreGateSvc;
class FaserSCT_ID;

#include "TrackerEventAthenaPool/FaserSCT_ClusterContainerCnv_p0.h"
#include "TrackerEventAthenaPool/FaserSCT_ClusterContainerCnv_p3.h"
#include "FaserSCT_ClusterContainerCnv_tlp1.h"


// the latest persistent representation type of DataCollection:
typedef  FaserSCT_ClusterContainer_p3  SCT_ClusterContainer_PERS;
typedef  T_AthenaPoolCustomCnv<FaserSCT_ClusterContainer, SCT_ClusterContainer_PERS >  SCT_ClusterContainerCnvBase;

/**
** Create derived converter to customize the saving of identifiable
** container
**/
class FaserSCT_ClusterContainerCnv : public SCT_ClusterContainerCnvBase
{
  friend class CnvFactory<FaserSCT_ClusterContainerCnv >;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local
  FaserSCT_ClusterContainerCnv_p3   m_converter_p3;
//  FaserSCT_ClusterContainerCnv_tlp1 m_TPConverter_p3;

  // Should not be needed at some point.
  StoreGateSvc*  m_storeGate;

protected:
public:
  FaserSCT_ClusterContainerCnv (ISvcLocator* svcloc);
protected:
  virtual SCT_ClusterContainer_PERS*   createPersistent (FaserSCT_ClusterContainer* transCont);
  virtual FaserSCT_ClusterContainer* createTransient ();

  // Must initialize ID helpers
  virtual StatusCode initialize();
 // virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter_p3; }
};


#endif
