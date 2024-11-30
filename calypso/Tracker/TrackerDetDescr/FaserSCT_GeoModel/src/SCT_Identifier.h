/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_GEOMODEL_SCT_IDENTIFIER_H
#define FASERSCT_GEOMODEL_SCT_IDENTIFIER_H

class Identifier;
class FaserSCT_ID;

class SCT_Identifier
{
public:

  SCT_Identifier(const FaserSCT_ID* idHelper,
                 int station = 0,
		             int layer = 0, 
		             int phiModule = 0,
		             int etaModule = 0,
		             int side = 0)
    : m_idHelper{idHelper},
      m_station{station},
      m_layer{layer},
      m_phiModule{phiModule},
      m_etaModule{etaModule},
      m_side{side}
  {};


  void setStation(int i) {m_station = i;}
  int  getStation() const {return m_station;}

  void setLayer(int i) {m_layer = i;}
  int  getLayer() const {return m_layer;}

  void setEtaModule(int i) {m_etaModule = i;}
  int  getEtaModule() const {return m_etaModule;}

  void setPhiModule(int i) {m_phiModule = i;}
  int  getPhiModule() const {return m_phiModule;}

  void setSide(int i) {m_side = i;}
  int  getSide() const {return m_side;}

  Identifier getWaferId();

  // For debugging purposes.
  void print();

private:
  const FaserSCT_ID* m_idHelper;
  int m_station;
  int m_layer;
  int m_phiModule;
  int m_etaModule;
  int m_side;
};

#endif // FASERSCT_GEOMODEL_SCT_IDENTIFIER_H
