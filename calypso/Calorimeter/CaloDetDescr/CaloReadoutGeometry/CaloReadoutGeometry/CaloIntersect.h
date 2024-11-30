/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   CaloIntersect.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CALOREADOUTGEOMETRY_CALOINTERSECT_H
#define CALOREADOUTGEOMETRY_CALOINTERSECT_H

namespace CaloDD {

  /** @class CaloIntersect
 
      class to run intersection tests

      @author Grant Gorfine
  */

  class CaloIntersect {

    public:

      enum IntersectState {OUT = 0, BOUNDARY = 1, IN = 2};

      CaloIntersect(IntersectState state = OUT);

      bool in() const; // Definitely in
      bool out() const; // Definitely out
      bool nearBoundary() const; // Near boundary within tolerences
      bool mayIntersect() const; // in() || nearBoundary()
      operator bool() const; // Equivalent to mayIntersect().

      void setIn();
      void setOut();
      void setNearBoundary();

    private:
      IntersectState m_state;

  };

inline CaloIntersect::CaloIntersect(IntersectState state) 
  : m_state(state)
{}

inline bool CaloIntersect::in() const 
{
  return (m_state == IN);
}

inline bool CaloIntersect::out() const 
{
  return (m_state == OUT);
}

inline bool CaloIntersect::nearBoundary() const 
{
  return (m_state == BOUNDARY);
}


inline bool CaloIntersect::mayIntersect() const 
{
  return (m_state == BOUNDARY || m_state == IN);
}

inline CaloIntersect::operator bool() const 
{
  return mayIntersect();
}

inline void CaloIntersect::setIn()
{
  m_state = IN;
}

inline void CaloIntersect::setOut()
{
  m_state = OUT;
}

inline void CaloIntersect::setNearBoundary()
{
  m_state = BOUNDARY;
}

} // namespace CaloDD

#endif  //CALOREADOUTGEOMETRY_CALOINTERSECT_H

