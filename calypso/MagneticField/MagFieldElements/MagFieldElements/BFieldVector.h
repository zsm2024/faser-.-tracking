/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldVector.h
//
// Magnetic field value stored in the map.
// It may be either short (toroid) or double (solenoid).
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDVECTOR_H
#define BFIELDVECTOR_H

template <class T>
class BFieldVector {
public:
    // constructor
    BFieldVector() {;}
    BFieldVector( T Bx, T By, T Bz ) { m_B[0] = Bx; m_B[1] = By; m_B[2] = Bz; }
    // setter
    void set( T Bx, T By, T Bz ) { m_B[0] = Bx; m_B[1] = By; m_B[2] = Bz; }
    // accessors
    T x() const { return m_B[0]; }
    T y() const { return m_B[1]; }
    T z() const { return m_B[2]; }
    // array-like accessor
    T operator[]( int i ) const { return m_B[i]; }
private:
    T m_B[3];
};

#endif
