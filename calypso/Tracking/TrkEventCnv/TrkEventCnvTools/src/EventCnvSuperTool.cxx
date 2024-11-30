/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventCnvTools/EventCnvSuperTool.h"
#include "FaserDetDescr/FaserDetectorID.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventCnvTools/ITrkEventCnvTool.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include <typeinfo>

Trk::EventCnvSuperTool::EventCnvSuperTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
    :
    base_class(t,n,p),
    m_detID(0),
    m_haveTrackerCnvTool(false),   // Will be set to true on retrieval
    m_errCount(0),
    m_maxErrCount(10)
{
    declareProperty("MaxErrorCount", m_maxErrCount, "Maximum number of errors that will be reported");
}

Trk::EventCnvSuperTool::~EventCnvSuperTool(){
  if (m_errCount>m_maxErrCount) ATH_MSG_WARNING("Suppressed "<<(m_errCount-m_maxErrCount)<<" WARNING or ERROR messages");
}

StatusCode
Trk::EventCnvSuperTool::initialize(){   
    // Try to get FaserID
    StatusCode sc = detStore()->retrieve( m_detID, "FaserID" );
    if( sc.isFailure() ) {
        msg(MSG::WARNING) << "Could not get FaserDetectorID " << endmsg;
    }
    
    //Now try to get the tools
    if ( !m_trackerCnvTool.empty() ) {
        if (m_trackerCnvTool.retrieve().isFailure() ) 
        {
            msg(MSG::DEBUG) << "Failed to retrieve Tracker helper tool "<< m_trackerCnvTool 
                  <<". Will not be able to recreate Tracker Surfaces / Det Elements."<<endmsg;
        } else {
            msg(MSG::VERBOSE) << "Retrieved tool " << m_trackerCnvTool << endmsg;
            m_haveTrackerCnvTool=true;
        }
    } else {
      m_trackerCnvTool.setTypeAndName("");
    }

    if (!m_haveTrackerCnvTool){
        msg(MSG::WARNING) << "Failed to retrieve Tracker tool. Will not be able to recreate surfaces / detector elements."<< endmsg;
        m_maxErrCount=0; // No point in further WARNINGs
    }
    
    return StatusCode::SUCCESS;
}

StatusCode
Trk::EventCnvSuperTool::finalize(){
    msg()<< "Finalize().";
    if (m_errCount>0) msg()<<" Tried to print "<<m_errCount<<" ERROR/WARNING messages (with maximum permissable = "<<m_maxErrCount<<")";
    msg()<<endmsg;
    return StatusCode::SUCCESS;
}

const Trk::ITrkEventCnvTool*    
Trk::EventCnvSuperTool::getCnvTool(const Identifier& id) const {
    if (m_detID==0) return 0;

    if (m_haveTrackerCnvTool)
    {
        return &(*m_trackerCnvTool);
    }
    else
    {
        if ( (m_errCount++)<m_maxErrCount) msg(MSG::WARNING)
                << "Tracker RIO_OnTrack, but have no Tracker cnv tool!"
                << endmsg;
        return 0;
    }


    // if(m_detID->is_indet(id))
    // {
    //     //std::cout<<"Trk::EventCnvSuperTool::getCnvTool = ID"<<std::endl;
    //     if (m_haveIdCnvTool )
    //     {
    //         return &(*m_idCnvTool);
    //     }else{
    //         if ( (m_errCount++)<m_maxErrCount) msg(MSG::WARNING)
    //             << "ID RIO_OnTrack, but have no ID cnv tool!"
    //             << endmsg;
    //         return 0;
    //     }
    // }else{
    //     if(m_detID->is_muon(id) )
    //     {
    //         //std::cout<<"Trk::EventCnvSuperTool::getCnvTool = Muon"<<std::endl;
    //         if (m_haveMuonCnvTool)
    //         {
    //             return &(*m_muonCnvTool);
    //         }else{
    //             if ( (m_errCount++)<m_maxErrCount) msg(MSG::WARNING)
    //                 << "Muon RIO_OnTrack, but have no muon cnv tool. Cannot set check RoT."
    //                 << endmsg;
    //             return 0;
    //         }
    //     }
    // }
    
    if ( (m_errCount++)<m_maxErrCount){
        std::string ident = m_detID->show_to_string(id);        
        msg(MSG::WARNING)
        << "Unknown Identifier: ("<< ident<<"), that is ("<<id<<")"
        << endmsg;
    }
    return 0;
    
}

const Trk::Surface* 
Trk::EventCnvSuperTool::getSurface(const Identifier& id) const {
    const Surface* surface = 0;
    const Trk::ITrkEventCnvTool* cnvTool = getCnvTool(id);
    if (cnvTool!=0) {
        const TrkDetElementBase* detEl = cnvTool->getDetectorElement( id );
        if (detEl!=0)
            surface = &(detEl->surface(id));
        else
            if ( (m_errCount++)<m_maxErrCount) msg(MSG::WARNING)<< "getSurface: could not get detector element from id:"<<id<<" Returning 0." << endmsg;            
    } else {
        if ( (m_errCount++)<m_maxErrCount) msg(MSG::WARNING)<< "getSurface: could not get cnv tool for Identifier:"<<id<< endmsg;
    }
    return surface;
}

void
Trk::EventCnvSuperTool::recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const
{
    using namespace std;
    const Trk::ITrkEventCnvTool* cnvTool = getCnvTool(RoT->identify());
    if (cnvTool!=0) {
        cnvTool->recreateRIO_OnTrack( RoT );
    } else {
        const type_info& info = typeid(*RoT);
        if ( (m_errCount++)<m_maxErrCount) 
            msg(MSG::WARNING)<< "recreateRIO_OnTrack: could not get cnv tool. Returning without correctly filling ROT of type: "<< info.name()<< endmsg;
    }
    return;
}

void
Trk::EventCnvSuperTool::prepareRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const
{
    const Trk::ITrkEventCnvTool* cnvTool = getCnvTool(RoT->identify());
    if (cnvTool!=0) {
        cnvTool->prepareRIO_OnTrack( RoT );
    } else {
        if ( (m_errCount++)<m_maxErrCount) msg()<< "prepareRIO_OnTrack could not find appropriate tool to prepare: "<<*RoT<<std::endl; 
    }
    return;
}

void
Trk::EventCnvSuperTool::prepareRIO_OnTrackLink ( const Trk::RIO_OnTrack *RoT,
                                                 ELKey_t& key,
                                                 ELIndex_t& index ) const
{
    const Trk::ITrkEventCnvTool* cnvTool = getCnvTool(RoT->identify());
    if (cnvTool!=0) {
        cnvTool->prepareRIO_OnTrackLink ( RoT, key, index );
    } else {
        if ( (m_errCount++)<m_maxErrCount) msg()<< "prepareRIO_OnTrack could not find appropriate tool to prepare: "<<*RoT<<std::endl; 
    }
    return;
}
