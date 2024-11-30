/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERGEOMODEL_TRIGGERIDENTIFIER_H
#define TRIGGERGEOMODEL_TRIGGERIDENTIFIER_H

class Identifier;
class TriggerID;

class TriggerIdentifier
{
public:

  TriggerIdentifier( const TriggerID* idHelper,
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
  const TriggerID* m_idHelper;
  int m_station;
  int m_plate;
};

#endif // TRIGGERGEOMODEL_TRIGGERIDENTIFIER_H
