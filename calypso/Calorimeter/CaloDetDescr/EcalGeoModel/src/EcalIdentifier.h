/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ECALGEOMODEL_ECALIDENTIFIER_H
#define ECALGEOMODEL_ECALIDENTIFIER_H

class Identifier;
class EcalID;

class EcalIdentifier
{
public:

  EcalIdentifier( const EcalID* idHelper,
                  int row = 0,
		              int module = 0 )
    : m_idHelper{idHelper},
      m_row{row},
      m_module{module}
  {};


  void setRow(int i) {m_row = i;}
  int  getRow() const {return m_row;}

  void setModule(int i) {m_module = i;}
  int  getModule() const {return m_module;}

  Identifier getModuleId();

  // For debugging purposes.
  void print();

private:
  const EcalID* m_idHelper;
  int m_row;
  int m_module;
};

#endif // ECALGEOMODEL_ECALIDENTIFIER_H
