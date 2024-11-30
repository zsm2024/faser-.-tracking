/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerEventTPCnv/FaserSCT_ClusterContainerCnv_tlp1.h"


FaserSCT_ClusterContainerCnv_tlp1::FaserSCT_ClusterContainerCnv_tlp1()
{
   // Add all converters defined in this top level converter:
   // never change the order of adding converters!  
   addMainTPConverter();
   addTPConverter( &m_sctClusterCnv );   
   addTPConverter( &m_siClusterCnv );   
   addTPConverter( &m_siWidthCnv );   
   addTPConverter( &m_prepDataCnv);
   addTPConverter( &m_locPosCnv);
   addTPConverter( &m_errorMatCnv);


}



void FaserSCT_ClusterContainerCnv_tlp1::setPStorage( Tracker::FaserSCT_ClusterContainer_tlp1 *storage )
{
 setMainCnvPStorage( &storage->m_sctDCCont );

 m_sctClusterCnv.setPStorage( &storage->m_sctClusters );
 m_siClusterCnv.setPStorage( &storage->m_siClusters );
 m_siWidthCnv.setPStorage( &storage->m_siWidths );
 m_prepDataCnv.setPStorage( &storage->m_prepData );
 m_locPosCnv.setPStorage( &storage->m_locPos );
 m_errorMatCnv.setPStorage( &storage->m_errorMat );
}   

// Method for test/SCT_ClusterCnv_p1_test.cxx
void FaserSCT_ClusterContainerCnv_tlp1::setUseDetectorElement(const bool useDetectorElement) {
  m_mainConverter.setUseDetectorElement(useDetectorElement);
}
