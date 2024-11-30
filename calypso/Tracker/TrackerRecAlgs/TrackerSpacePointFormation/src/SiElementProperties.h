/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 This class contains some element properties that are used in 
SiSpacePointFormation
 : ie the wafer widths and wafer neighbours. These are cached during 
 SiSpacePointFormation initialization for the sake of efficiency.
 
 ATLAS Collaboration
 ***************************************************************************/

#ifndef TRACKERSPACEPOINTMAKERALG_SIELEMENTPROPERTIES_H
#define TRACKERSPACEPOINTMAKERALG_SIELEMENTPROPERTIES_H

#include <vector>
#include "Identifier/IdentifierHash.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h" 

class FaserSCT_ID; 


namespace TrackerDD{
class SiElementProperties

{
public:

    SiElementProperties(const IdentifierHash&			idHash, 
			const FaserSCT_ID&			idHelper,
			const TrackerDD::SiDetectorElement&	element,
			float					epsilonWidth); 

    ~SiElementProperties();

    const std::vector<IdentifierHash>*	neighbours (void);
    float				halfWidth (void);
    
private:
    std::vector<IdentifierHash>		m_neighbours;
    float				m_halfWidth;
    
};



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>


//--------------------------------------------------------------------------
inline const std::vector<IdentifierHash>*
SiElementProperties::neighbours()
{
    return &m_neighbours;
}

//----------------------------------------------------------------------------
inline float
SiElementProperties::halfWidth()
{
    return m_halfWidth;
}

//----------------------------------------------------------------------------
    
}
#endif // TRACKERSPACEPOINTMAKERALG_SIELEMENTPROPERTIES_H
