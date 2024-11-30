/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 
 -------------------
 ATLAS Collaboration
 ***************************************************************************/

#include "Identifier/Identifier.h"
#include "SiElementProperties.h" 
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h" 

namespace TrackerDD{
SiElementProperties::SiElementProperties(const IdentifierHash&			idHash, 
					 const FaserSCT_ID&			idHelper,
					 const TrackerDD::SiDetectorElement&	element,
					 float					epsilonWidth) : m_neighbours(),m_halfWidth(0)

{
    // construct vector of neighbours of this wafer.
    Identifier compact;
    IdContext idCntxt(idHelper.wafer_context());
    
    int res = idHelper.get_id(idHash,compact, &idCntxt);
    if (res) return;      // cannot convert
       
    //if (idHelper.side(compact)==0) return;
    // if it is the stereo side do nothing
    if (element.isStereo()) return;
    IdentifierHash otherHash;
    IdentifierHash neighbourHash;

    res = idHelper.get_other_side(idHash, otherHash);
    if (res==0) m_neighbours.push_back(otherHash);
    res  = idHelper.get_prev_in_phi(otherHash, neighbourHash);
    if (res==0) m_neighbours.push_back(neighbourHash);
    res = idHelper.get_next_in_phi(otherHash, neighbourHash);
    if (res==0) m_neighbours.push_back(neighbourHash);
 
    // These neighbours are only defined for barrel, and may fail
    // because wafer is at one of the ends
    //if (idHelper.is_barrel(compact))
    {
	res = idHelper.get_prev_in_eta(otherHash, neighbourHash);
	if (res ==0)
	    m_neighbours.push_back(neighbourHash);
	res  = idHelper.get_next_in_eta(otherHash, neighbourHash );
	if (res ==0)
	    m_neighbours.push_back(neighbourHash);
    }

    // Find half width of wafer at centre

    m_halfWidth = (element.design().minWidth()+element.design().maxWidth())/4 
		  + epsilonWidth;        // add a bit for safety.
}

//-------------------------------------------------------------------------
SiElementProperties::~SiElementProperties()
{}

}
//----------------------------------------------------------------------------

