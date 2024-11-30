/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldCache.h
//
// Cashe of one bin of the magnetic field map.
// Defined by ranges in z, r, phi, and the B vectors at the 8 corners of the "bin".
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDCACHE_H
#define BFIELDCACHE_H

#include <iostream>
#include <cmath>
#include "MagFieldElements/BFieldVector.h"

class BFieldCache {
public:
    // default constructor sets unphysical boundaries, so that inside() will fail
    BFieldCache() : m_xmin(0.0), m_xmax(0.0), m_ymin(0.0), m_ymax(0.0), m_zmin(0.0), m_zmax(-1.0) {;}
    // make this cache invalid, so that inside() will fail
    void invalidate() { m_zmin = 0.0; m_zmax = -1.0; }
    // set the z, r, phi range that defines the bin
    void setRange( double xmin, double xmax, double ymin, double ymax, double zmin, double zmax )
    { m_xmin = xmin; m_xmax = xmax; m_ymin = ymin; m_ymax = ymax; m_zmin = zmin; m_zmax = zmax;
      m_invx = 1.0/(xmax-xmin); m_invy = 1.0/(ymax-ymin); m_invz = 1.0/(zmax-zmin); }
    // set the field values at each corner (rescale for current scale factor)
    void setField( int i, const BFieldVector<double> &field, double scaleFactor = 1.0 )
        { for(int j=0; j<3; j++) m_field[j][i] = scaleFactor * field[j]; }
    void setField( int i, const BFieldVector<short> &field, double scaleFactor = 1.0 )
        { for(int j=0; j<3; j++) m_field[j][i] = scaleFactor * field[j]; }
    // set the multiplicative factor for the field vectors
    void setBscale( double bscale ) { m_scale = bscale; }
    float bscale() { return m_scale; }
    // test if (z, r, phi) is inside this bin
    bool inside( double x, double y, double z ) const
    { return ( z >= m_zmin && z <= m_zmax && x >= m_xmin && x <= m_xmax && y >= m_ymin && y <= m_ymax ); }
    // interpolate the field and return B[3].
    // also compute field derivatives if deriv[9] is given.
    inline void getB( const double *xyz, double *B, double *deriv=nullptr ) const;
private:
    double m_xmin, m_xmax; // bin range in z
    double m_ymin, m_ymax; // bin range in r
    double m_zmin, m_zmax; // bin range in phi
    float m_invx, m_invy, m_invz; // 1/(bin size) in x, y, z
    float m_field[3][8]; // (Bx,By,Bz) at 8 corners of the bin
    float m_scale; // unit of m_field in kT
};

// inline methods

void
BFieldCache::getB( const double *xyz, double *B, double *deriv ) const
{
    const double &x(xyz[0]);
    const double &y(xyz[1]);
    const double &z(xyz[2]);
    // fractional position inside this bin
    float fx = (x-m_xmin)*m_invx;
    float gx = 1.0 - fx;
    float fy = (y-m_ymin)*m_invy;
    float gy = 1.0 - fy;
    float fz = (z-m_zmin)*m_invz;
    float gz = 1.0 - fz;
    // interpolate field values in x, y, z
    // float Bzrphi[3];
    for ( int i = 0; i < 3; i++ ) { // z, r, phi components
        const float *field = m_field[i];
        B[i] = m_scale*( gx*( gy*( gz*field[0] + fz*field[1] ) +
                              fy*( gz*field[2] + fz*field[3] ) ) +
                         fx*( gy*( gz*field[4] + fz*field[5] ) +
                              fy*( gz*field[6] + fz*field[7] ) ) );
    }
    // compute field derivatives if requested
    if ( deriv ) {
        float sx = m_scale*m_invx;
        float sy = m_scale*m_invy;
        float sz = m_scale*m_invz;
        float dBdx[3];
        float dBdy[3];
        float dBdz[3];
        for ( int j = 0; j < 3; j++ ) { // Bz, Br, Bphi components
            const float *field = m_field[j];
            dBdx[j]   = sx*( gy*( gz*(field[4]-field[0]) + fz*(field[5]-field[1]) ) +
                             fy*( gz*(field[6]-field[2]) + fz*(field[7]-field[3]) ) );
            dBdy[j]   = sy*( gx*( gz*(field[2]-field[0]) + fz*(field[3]-field[1]) ) +
                             fx*( gz*(field[6]-field[4]) + fz*(field[7]-field[5]) ) );
            dBdz[j]   = sz*( gx*( gy*(field[1]-field[0]) + fy*(field[3]-field[2]) ) +
                             fx*( gy*(field[5]-field[4]) + fy*(field[7]-field[6]) ) );
        }
        // convert to cartesian coordinates
        deriv[0] = dBdx[0];
        deriv[1] = dBdy[0];
        deriv[2] = dBdz[0];
        deriv[3] = dBdx[1];
        deriv[4] = dBdy[1];
        deriv[5] = dBdz[1];
        deriv[6] = dBdx[2];
        deriv[7] = dBdy[2];
        deriv[8] = dBdz[2];
    }
}

#endif
