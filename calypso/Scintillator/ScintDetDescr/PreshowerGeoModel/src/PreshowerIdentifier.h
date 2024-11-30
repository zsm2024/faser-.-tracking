/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRESHOWERGEOMODEL_PRESHOWERIDENTIFIER_H
#define PRESHOWERGEOMODEL_PRESHOWERIDENTIFIER_H

class Identifier;
class PreshowerID;

class PreshowerIdentifier
{
public:

  PreshowerIdentifier( const PreshowerID* idHelper,
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
  const PreshowerID* m_idHelper;
  int m_station;
  int m_plate;
};

#endif // PRESHOWERGEOMODEL_PRESHOWERIDENTIFIER_H
