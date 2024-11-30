/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   NeutrinoIntersect.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef NEUTRINOREADOUTGEOMETRY_NEUTRINOINTERSECT_H
#define NEUTRINOREADOUTGEOMETRY_NEUTRINOINTERSECT_H

namespace NeutrinoDD {

  /** @class NeutrinoIntersect
 
      class to run intersection tests

      @author Grant Gorfine
  */

  class NeutrinoIntersect {

    public:

      enum IntersectState {OUT = 0, BOUNDARY = 1, IN = 2};

      NeutrinoIntersect(IntersectState state = OUT);

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

inline NeutrinoIntersect::NeutrinoIntersect(IntersectState state) 
  : m_state(state)
{}

inline bool NeutrinoIntersect::in() const 
{
  return (m_state == IN);
}

inline bool NeutrinoIntersect::out() const 
{
  return (m_state == OUT);
}

inline bool NeutrinoIntersect::nearBoundary() const 
{
  return (m_state == BOUNDARY);
}


inline bool NeutrinoIntersect::mayIntersect() const 
{
  return (m_state == BOUNDARY || m_state == IN);
}

inline NeutrinoIntersect::operator bool() const 
{
  return mayIntersect();
}

inline void NeutrinoIntersect::setIn()
{
  m_state = IN;
}

inline void NeutrinoIntersect::setOut()
{
  m_state = OUT;
}

inline void NeutrinoIntersect::setNearBoundary()
{
  m_state = BOUNDARY;
}

} // namespace NeutrinoDD

#endif  //NEUTRINOREADOUTGEOMETRY_NEUTRINOINTERSECT_H

