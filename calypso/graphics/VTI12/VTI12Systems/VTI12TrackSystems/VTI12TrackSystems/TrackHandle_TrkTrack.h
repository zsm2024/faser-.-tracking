/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackHandle_TrkTrack                //
//                                                            //
//  Description: TrackHandle for Trk::Track's                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKHANDLE_TRKTRACK_H
#define TRACKHANDLE_TRKTRACK_H

#include "VTI12TrackSystems/TrackHandleBase.h"
#include "VTI12TrackSystems/MeasurementToSoNode.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
  class Track;
  class MeasurementBase;
}

class TrackHandle_TrkTrack : public TrackHandleBase {
public:

  TrackHandle_TrkTrack(TrackCollHandleBase*,const Trk::Track*);
  virtual ~TrackHandle_TrkTrack();

  virtual QStringList clicked() const;

  virtual Amg::Vector3D momentum() const;
  const Trk::Track * provide_pathInfoTrkTrack() const { return m_trk; };

  const Trk::Track* trkTrackPointer() const { return m_trk; }

  virtual bool hasSubSystemHitInfo() const { ensureInitSubSysHitInfo(); return m_hitInfoStatus; }
  virtual unsigned getNSCTHits() const { return m_nhits_sct; }
  virtual const Trk::FitQuality* getFitQuality() const ;

  virtual void fillObjectBrowser(QList<QTreeWidgetItem *>& list); 
  void updateObjectBrowser();
  
  SoNode* zoomToTSOS(unsigned int index); //!< Depending on controller configuration attempt to zoom to the corresponding TSOS & returned detailed node matching it
  
  virtual QString shortInfo() const; //!< returns mom and hit information about track
  virtual QString type() const { return QString("Track"); } //!< return very short word with type (maybe link with collection type?)
  
private:
  void visibleStateChanged();
  void currentMaterialChanged();
  mutable unsigned m_nhits_sct;
  mutable int m_hitInfoStatus;//!< -1: uninitialized, 0: not present, 1: present.
  void ensureInitSubSysHitInfo() const;

protected:
  const Trk::Track* m_trk;
  Trk::MeasurementToSoNode m_measToSoNode; //!< Just used to get types, so probably better to hive that functionality off. @FIXME
};

#endif
