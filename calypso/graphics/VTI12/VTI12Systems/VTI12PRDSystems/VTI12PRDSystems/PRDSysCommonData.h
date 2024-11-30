/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS and FASER collaborations
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class PRDSysCommonData                    //
//                                                            //
//  Description: Common pointers, data, node-to-object maps   //
//               etc. for the prd system                      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef PRDSYSCOMMONDATA_H
#define PRDSYSCOMMONDATA_H

#include "VP1Base/VP1HelperClassBase.h"
class IVP13DSystem;
class HitsSoNodeManager;
class PRDSystemController;
class VP1SoMaterialMixer;
// class InDetProjHelper;
class PRDTrackSegmentHelper;
class SoTransform;
class SoPath;
class PRDHandleBase;
namespace Trk { class PrepRawData; }

class PRDSysCommonData : public VP1HelperClassBase {
public:

  PRDSysCommonData(IVP13DSystem *, PRDSystemController*);
  virtual ~PRDSysCommonData();

  IVP13DSystem * system() const;
  HitsSoNodeManager * nodeManager() const;
  PRDSystemController * controller() const;
  VP1SoMaterialMixer * materialMixer() const;
//   InDetProjHelper * indetProjHelper_SCT() const;
  PRDTrackSegmentHelper * trackAndSegmentHelper() const;

  void registerTransform2Handle(SoTransform*transform,PRDHandleBase*handle);
  void registerPRD2Handle(const Trk::PrepRawData*prd,PRDHandleBase*handle);
  //Access pick->handle association (and pop path):
  PRDHandleBase * pickedPathToHandle( SoPath*pickedPath );

  void clearEventData(); //Deletes AscObjSelectionManager

private:

  class Imp;
  Imp * m_d;

  IVP13DSystem * m_3dsystem;
  HitsSoNodeManager * m_nodeManager;
  PRDSystemController * m_controller;
  VP1SoMaterialMixer * m_materialMixer;
//   InDetProjHelper * m_indetProjHelper_SCT;

  PRDTrackSegmentHelper * m_trackAndSegmentHelper;

};

inline IVP13DSystem* PRDSysCommonData::system() const { return m_3dsystem; }
inline HitsSoNodeManager* PRDSysCommonData::nodeManager() const { return m_nodeManager; }
inline PRDSystemController * PRDSysCommonData::controller() const { return m_controller; }
inline VP1SoMaterialMixer * PRDSysCommonData::materialMixer() const { return m_materialMixer; }
// inline InDetProjHelper * PRDSysCommonData::indetProjHelper_SCT() const { return m_indetProjHelper_SCT; }
inline PRDTrackSegmentHelper * PRDSysCommonData::trackAndSegmentHelper() const { return m_trackAndSegmentHelper; }

#endif
