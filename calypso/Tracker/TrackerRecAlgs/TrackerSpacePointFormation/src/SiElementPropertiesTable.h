/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Stores properties that are needed in SiSpacePointFinder.
 --------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: SiElementPropertiesTable.h,v 1.3 2009-01-07 17:50:36 markuse Exp $

#ifndef TRACKERSPACEPOINTMAKERALG_SIELEMENTPROPERTIESTABLE_H
#define TRACKERSPACEPOINTMAKERALG_SIELEMENTPROPERTIESTABLE_H

#include "SiElementProperties.h"

#include "Identifier/IdentifierHash.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"

#include <vector>


namespace TrackerDD{
class SiElementPropertiesTable final 
{
public:
    
    SiElementPropertiesTable(const FaserSCT_ID&					idHelper,
			     const TrackerDD::SiDetectorElementCollection&	elements, 
			     float						epsilonWidth); 
    ~SiElementPropertiesTable();

    const std::vector<IdentifierHash>*	neighbours(const IdentifierHash& waferID) const;
    float				halfWidth(IdentifierHash hashID) const;
    
private:
    std::vector<SiElementProperties*>		m_properties;
    
};


inline const std::vector<IdentifierHash>*
SiElementPropertiesTable::neighbours(const IdentifierHash& waferID) const
{
    return (m_properties[(unsigned int)waferID])->neighbours();
}

inline float
SiElementPropertiesTable::halfWidth(IdentifierHash waferID) const
{
    return (m_properties[(unsigned int)waferID])->halfWidth();
}

}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrackerDD::SiElementPropertiesTable , 175393198, 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrackerDD::SiElementPropertiesTable , 247356960);

#endif // TRACKERSPACEPOINTMAKERALG_SIELEMENTPROPERTIESTABLE_H
