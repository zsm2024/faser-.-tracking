/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   ScintIntersect.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SCINTREADOUTGEOMETRY_SCINTINTERSECT_H
#define SCINTREADOUTGEOMETRY_SCINTINTERSECT_H

namespace ScintDD {

  /** @class ScintIntersect
 
      class to run intersection tests

      @author Grant Gorfine
  */

  class ScintIntersect {

    public:

      enum IntersectState {OUT = 0, BOUNDARY = 1, IN = 2};

      ScintIntersect(IntersectState state = OUT);

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

inline ScintIntersect::ScintIntersect(IntersectState state) 
  : m_state(state)
{}

inline bool ScintIntersect::in() const 
{
  return (m_state == IN);
}

inline bool ScintIntersect::out() const 
{
  return (m_state == OUT);
}

inline bool ScintIntersect::nearBoundary() const 
{
  return (m_state == BOUNDARY);
}


inline bool ScintIntersect::mayIntersect() const 
{
  return (m_state == BOUNDARY || m_state == IN);
}

inline ScintIntersect::operator bool() const 
{
  return mayIntersect();
}

inline void ScintIntersect::setIn()
{
  m_state = IN;
}

inline void ScintIntersect::setOut()
{
  m_state = OUT;
}

inline void ScintIntersect::setNearBoundary()
{
  m_state = BOUNDARY;
}

} // namespace ScintDD

#endif  //SCINTREADOUTGEOMETRY_SCINTINTERSECT_H

