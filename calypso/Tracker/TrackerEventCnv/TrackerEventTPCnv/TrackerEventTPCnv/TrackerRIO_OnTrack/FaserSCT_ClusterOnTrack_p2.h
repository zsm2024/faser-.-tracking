/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTER_ON_TRACK_p2_TRK_H
#define FASERSCT_CLUSTER_ON_TRACK_p2_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"
#include "DataModelAthenaPool/ElementLink_p1.h"

namespace Tracker
{
    class FaserSCT_ClusterOnTrack_p2
    {
    public:
    FaserSCT_ClusterOnTrack_p2():m_id{0}, m_idDE{0}, m_isbroad(false), m_positionAlongStrip{0.} {}


        unsigned long long            m_id;
        TPObjRef                      m_localParams;
        TPObjRef                      m_localErrMat;

        IdentifierHash::value_type     m_idDE;
        bool                           m_isbroad;

        ElementLinkInt_p1  m_prdLink;

//element allowing for a calculation of global position     
        float m_positionAlongStrip;
    };
}

#endif 
