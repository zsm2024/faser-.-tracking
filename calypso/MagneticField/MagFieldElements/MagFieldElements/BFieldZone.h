/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldZone.h
//
// A "zone" inside the field map
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDZONE_H
#define BFIELDZONE_H

#include <vector>
#include "MagFieldElements/BFieldMesh.h"

class BFieldZone : public BFieldMesh<short> {
public:
    // constructor
    BFieldZone( int id, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax,
                double scale )
        : BFieldMesh<short>(xmin,xmax,ymin,ymax,zmin,zmax,scale), m_id(id) {;}
    // scale B field by a multiplicative factor
    void scaleField( double factor )
    { scaleBscale(factor); /*for (unsigned i=0; i<ncond(); i++) { m_cond[i].scaleCurrent(factor); }*/ }
    // accessors
    int id() const { return m_id; }
    int memSize() const
    { return BFieldMesh<short>::memSize() /*+ sizeof(int) + sizeof(BFieldCond)*m_cond.capacity()*/; }
    // adjust the min/max edges to a new value
    void adjustMin( int i, double x ) { m_min[i] = x; m_mesh[i].front() = x; }
    void adjustMax( int i, double x ) { m_max[i] = x; m_mesh[i].back() = x; }
private:
    int m_id;          // zone ID number
};

#endif
