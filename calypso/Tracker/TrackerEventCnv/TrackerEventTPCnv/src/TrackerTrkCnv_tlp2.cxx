/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerEventTPCnv/TrackerTrackCnv_tlp2.h"


TrackerTrackCnv_tlp2::TrackerTrackCnv_tlp2()
{
   // Add all converters defined in this top level converter:
   // never change the order of adding converters!  
//    addTPConverter( &m_pixelClustersOTCnv );   
   addTPConverter( &m_sctClustersOTCnv );     
//    addTPConverter( &m_trtDriftCirclesOTCnv ); 

//    addTPConverter( &m_pixelCrotCnv );     
//    addTPConverter( &m_sctCrotCnv ); 
//    addTPConverter( &m_trtCrotCnv );      
   
}



void TrackerTrackCnv_tlp2::setPStorage( Tracker::Track_tlp2 *storage )
{
//    m_pixelClustersOTCnv.    setPStorage( &storage->m_pixelClustersOT );
   m_sctClustersOTCnv.      setPStorage( &storage->m_sctClustersOT  );  
//    m_trtDriftCirclesOTCnv.  setPStorage( &storage->m_trtDriftCirclesOT );

//    m_pixelCrotCnv.          setPStorage( &storage->m_pixelCROTs  );  
//    m_sctCrotCnv.            setPStorage( &storage->m_sctCROTs );
//    m_trtCrotCnv.            setPStorage( &storage->m_trtCROTs );

}   
 
