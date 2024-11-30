/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMULSIONGEOMODEL_EMULSIONIDENTIFIER_H
#define EMULSIONGEOMODEL_EMULSIONIDENTIFIER_H

class Identifier;
class EmulsionID;

class EmulsionIdentifier
{
public:

  EmulsionIdentifier( const EmulsionID* idHelper,
                      int module = 0,
		                  int base = 0,
                      int film = 0 )
    : m_idHelper{idHelper},
      m_module{module},
      m_base{base},
      m_film{film}
  {};


  void setModule(int i) {m_module = i;}
  int  getModule() const {return m_module;}

  void setBase(int i) {m_base = i;}
  int  getBase() const {return m_base;}

  void setFilm(int i) {m_film = i;}
  int  getFilm() const {return m_film;}


  Identifier getFilmId();

  // For debugging purposes.
  void print();

private:
  const EmulsionID* m_idHelper;
  int m_module;
  int m_base;
  int m_film;
};

#endif // EMULSIONGEOMODEL_EMULSIONIDENTIFIER_H
