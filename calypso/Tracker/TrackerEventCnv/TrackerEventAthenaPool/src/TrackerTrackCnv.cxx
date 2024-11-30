/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "TrackerTrackCnv.h"
#include "TrkTrack/TrackCollection.h"

#include <iostream>
#include <stdexcept>

TrackerTrackCnv::TrackerTrackCnv(ISvcLocator* svcloc):
    TrackerTrackCnvBase(svcloc)
{
}


StatusCode TrackerTrackCnv::initialize()
{
   if( !TrackerTrackCnvBase::initialize().isSuccess() )
      return StatusCode::FAILURE;

   
   // std::cout << " ------->>---- In TrackerTrackCnv::initialize()" << std::endl;

   return StatusCode::SUCCESS;
}


// this method just reads the persistent object - no TP conversion here
void 
TrackerTrackCnv::readObjectFromPool( const std::string& token )
{
    static const pool::Guid p2_guid( "27491695-F64A-43EE-859D-73F33AF3317D" );
    // static const pool::Guid p2_guid( "4E3778E2-1497-4F10-8746-AA02319FAC83" );
    // static const pool::Guid p1_guid( "8380F7AC-4A8F-4382-95A5-1234E43D3B08" );

   // set the POOL token which will be used for reading from POOL
   setToken( token );

   // select the object type based on its GUID 
   if( compareClassGuid( p2_guid ) )     {
      // read the object using the main TP converter
      poolReadObject< TrackerTrack_PERS >( m_TPConverter2 );
//    }else if( compareClassGuid( p1_guid ) )     {
//       poolReadObject< InDet::Track_tlp1 >( m_TPConverter1 );
   }
   else
      throw std::runtime_error( "Unsupported version of TrackerTrack_PERS (unknown GUID)" );
}


