/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VETOGEOMODEL_VETOIDENTIFIER_H
#define VETOGEOMODEL_VETOIDENTIFIER_H

class Identifier;
class VetoID;

class VetoIdentifier
{
public:

  VetoIdentifier( const VetoID* idHelper,
                  int station = 0,
		          int plate = 0 )
    : m_idHelper{idHelper},
      m_station{station},
      m_plate{plate}
  {};


  void setStation(int i) {m_station = i;}
  int  getStation() const {return m_station;}

  void setPlate(int i) {m_plate = i;}
  int  getPlate() const {return m_plate;}

  Identifier getPlateId();

  // For debugging purposes.
  void print();

private:
  const VetoID* m_idHelper;
  int m_station;
  int m_plate;
};

#endif // VETOGEOMODEL_VETOIDENTIFIER_H
