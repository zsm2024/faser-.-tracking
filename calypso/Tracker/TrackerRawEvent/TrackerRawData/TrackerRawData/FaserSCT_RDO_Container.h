/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 TrackerRawData package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: SCT_RDO_Container.h,v 1.6 2008-01-05 18:53:00 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef TRACKERRAWDATA_SCT_RDO_CONTAINER_H
#define TRACKERRAWDATA_SCT_RDO_CONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "TrackerRawData/TrackerRawDataContainer.h"
#include "TrackerRawData/TrackerRawDataCollection.h"
#include "TrackerRawData/FaserSCT_RDORawData.h"
#include "TrackerRawData/FaserSCT_RDO_Collection.h" // Needed to pick up CLID.

typedef TrackerRawDataContainer<TrackerRawDataCollection<FaserSCT_RDORawData> > 
	FaserSCT_RDO_Container; 

CLASS_DEF(FaserSCT_RDO_Container,1333462013,1)

typedef EventContainers::IdentifiableCache< TrackerRawDataCollection<FaserSCT_RDORawData> > FaserSCT_RDO_Cache;

CLASS_DEF( FaserSCT_RDO_Cache , 55085602 , 1 )

typedef EventContainers::IIdentifiableCont<TrackerRawDataCollection<FaserSCT_RDORawData>> ISCT_RDO_Container;//Interface only, do not place in storegate

#endif // TRACKERRAWDATA_FASERSCT_RDO_CONTAINER_H
