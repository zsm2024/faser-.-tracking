/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserTrackBarcodeInfo_H
#define FaserTrackBarcodeInfo_H

#include "FaserVTrackInformation.h"

namespace ISF {
  class ISFParticle;
}

class FaserTrackBarcodeInfo: public FaserVTrackInformation {
public:
  FaserTrackBarcodeInfo(int uid, int bc, ISF::FaserISFParticle* baseIsp=0);
  virtual int GetParticleBarcode() const override {return m_barcode;}  // TODO Drop this once UniqueID and Status are used instead
  virtual int GetParticleUniqueID() const override {return m_uniqueID;}
  virtual int GetParticleStatus() const override {return m_status;}
  virtual const ISF::FaserISFParticle *GetBaseISFParticle() const override {return m_theBaseISFParticle;}
  virtual ISF::FaserISFParticle *GetBaseISFParticle() override {return m_theBaseISFParticle;}

  virtual void SetBaseISFParticle(ISF::FaserISFParticle*) override;
  virtual void SetReturnedToISF(bool returned) override;
  virtual bool GetReturnedToISF() const override {return m_returnedToISF;}

private:
  ISF::FaserISFParticle *m_theBaseISFParticle;
  int m_barcode;  // TODO Drop this once UniqueID and Status are used instead
  int m_uniqueID;
  int m_status{0}; //FIXME
  bool m_returnedToISF;
};


#endif // TrackBarcodeInfo_H

