/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLHANDLE_SCT_H
#define PRDCOLLHANDLE_SCT_H

#include "VTI12PRDSystems/PRDCollHandleBase.h"
#include "VTI12PRDSystems/PRDCommonFlags.h"

class PRDCollHandle_SCT : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_SCT(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_SCT();

public slots:
  void setPartsFlags(PRDCommonFlags::TrackerPartsFlags);//BarrelA, BarrelC, EndcapA, EndcapC
  void setExcludeIsolatedClusters(bool);

protected:
  virtual PRDHandleBase * addPRD(const Trk::PrepRawData*) override;
  virtual bool cut(PRDHandleBase*) override;

  virtual void eraseEventDataSpecific() override;
  virtual void postLoadInitialisation() override;
  virtual void setupSettingsFromControllerSpecific(PRDSystemController*) override;
  virtual float lodArea() const override { return 800.0f*800.0f; }
  virtual QColor defaultColor() const override;

private:

  class Imp;
  Imp * m_d;

};

#endif
