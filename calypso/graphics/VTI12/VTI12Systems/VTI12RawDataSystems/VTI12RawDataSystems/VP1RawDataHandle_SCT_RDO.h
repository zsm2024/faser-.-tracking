/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataHandle_SCT_RDO            //
//                                                            //
//  Description: Handle for SCT RDO's                         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008 (rewritten January 2009)      //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATAHANDLE_SCT_RDO_H
#define VP1RAWDATAHANDLE_SCT_RDO_H

#include "VTI12RawDataSystems/VP1RawDataHandleBase.h"
class FaserSCT_RDORawData;

namespace TrackerDD { class SiDetectorElement; }

class VP1RawDataHandle_SCT_RDO : public VP1RawDataHandleBase {
public:

  VP1RawDataHandle_SCT_RDO(VP1RawDataCollBase*,const FaserSCT_RDORawData*);
  virtual ~VP1RawDataHandle_SCT_RDO();
  QStringList clicked(bool verbose) const;

  VP1RawDataFlags::TrackerPartsFlags inTrackerParts() const;

protected:
  SoNode * buildShape();
  SoTransform * buildTransform();
  const FaserSCT_RDORawData* m_data;
  const TrackerDD::SiDetectorElement * element() const;//null in case of errors
};

#endif
