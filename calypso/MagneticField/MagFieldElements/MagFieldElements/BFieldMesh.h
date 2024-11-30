/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldMesh.h
//
// Generic 3-d mesh representing a simple field map.
// The field type is templated - it may be short (for the toroid) or double (for the solenoid)
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDMESH_H
#define BFIELDMESH_H

#include <iostream>
#include <vector>
#include <cmath>
#include "MagFieldElements/BFieldVector.h"
#include "MagFieldElements/BFieldCache.h"

template <class T>
class BFieldMesh {
public:
    // constructor
    BFieldMesh() : m_scale(1.0) {;}
    BFieldMesh( double xmin, double xmax, double ymin, double ymax, double zmin, double zmax,
                double bscale )
        : m_scale(bscale), m_nomScale(bscale)
        { m_min[0] = xmin; m_max[0] = xmax; m_min[1] = ymin; m_max[1] = ymax; m_min[2] = zmin; m_max[2] = zmax; }
    // set ranges
    void setRange( double xmin, double xmax, double ymin, double ymax, double zmin, double zmax )
        { m_min[0] = xmin; m_max[0] = xmax; m_min[1] = ymin; m_max[1] = ymax; m_min[2] = zmin; m_max[2] = zmax; }
    // set bscale
    void setBscale( double bscale ) { m_scale = m_nomScale = bscale; }
    // scale bscale by a factor
    void scaleBscale( double factor ) { m_scale = factor*m_nomScale; }
    // allocate space to vectors
    void reserve( int nx, int ny, int nz, int nfield );
    void reserve( int nx, int ny, int nz ) { reserve( nx, ny, nz, nx*ny*nz ); }
    // add elements to vectors
    void appendMesh( int i, double mesh ) { m_mesh[i].push_back(mesh); }
    void appendField( const BFieldVector<T> & field ) { m_field.push_back(field); }
    // build LUT
    void buildLUT();
    // test if a point is inside this zone
    inline bool inside( double x, double y, double z ) const;
    // find the bin
    inline void getCache( double x, double y, double z, BFieldCache & cache, double scaleFactor = 1.0 ) const;
    // get the B field
    void getB( const double *xyz, double *B, double* deriv=nullptr ) const;
    // accessors
    double min( int i ) const { return m_min[i]; }
    double max( int i ) const { return m_max[i]; }
    double xmin() const { return m_min[0]; }
    double xmax() const { return m_max[0]; }
    double ymin() const { return m_min[1]; }
    double ymax() const { return m_max[1]; }
    double zmin() const { return m_min[2]; }
    double zmax() const { return m_max[2]; }
    unsigned nmesh( int i ) const { return m_mesh[i].size(); }
    double mesh( int i, int j ) const { return m_mesh[i][j]; }
    unsigned nfield() const { return m_field.size(); }
    const BFieldVector<T> & field( int i ) const { return m_field[i]; }
    double bscale() const { return m_scale; }
    int memSize() const;
protected:
    double m_min[3], m_max[3];
    std::vector<double> m_mesh[3];
private:
    std::vector< BFieldVector<T> > m_field;
    double m_scale;
    double m_nomScale;  // nominal m_scale from the map
    // look-up table and related variables
    std::vector<int> m_LUT[3];
    double m_invUnit[3];     // inverse unit size in the LUT
    int m_xoff, m_yoff;
};

//
// Implemnetation follows
//

//
// Reserve space in the vectors to avoid unnecessary memory re-allocations.
//
template <class T>
void BFieldMesh<T>::reserve( int nx, int ny, int nz, int nfield )
{
    m_mesh[0].reserve( nx );
    m_mesh[1].reserve( ny );
    m_mesh[2].reserve( nz );
    m_field.reserve( nfield );
}

//
// Test if a point (x,y,z) is inside this mesh region.
//
template <class T>
bool BFieldMesh<T>::inside( double x, double y, double z ) const
{
    return ( x >= xmin() && x <= xmax() &&
             z >= zmin() && z <= zmax() && y >= ymin() && y <= ymax() );
}

//
// Find and return the cache of the bin containing (x,y,z)
//
template <class T>
void BFieldMesh<T>::getCache( double x, double y, double z, BFieldCache & cache, double scaleFactor ) const
{
    // find the mesh, and relative location in the mesh
    // x
    const std::vector<double>& mx(m_mesh[0]);
    int ix = int((x-xmin())*m_invUnit[0]); // index to LUT
    ix = m_LUT[0][ix]; // tentative mesh index from LUT
    if ( x > mx[ix+1] ) ix++;
    // y
    const std::vector<double>& my(m_mesh[1]);
    int iy = int((y-ymin())*m_invUnit[1]); // index to LUT
    iy = m_LUT[1][iy]; // tentative mesh index from LUT
    if ( y > my[iy+1] ) iy++;
    // z
    const std::vector<double>& mz(m_mesh[2]);
    int iz = int((z-zmin())*m_invUnit[2]); // index to LUT
    iz = m_LUT[2][iz]; // tentative mesh index from LUT
    if ( z > mz[iz+1] ) iz++;
    // store the bin edges
    cache.setRange( mx[ix], mx[ix+1], my[iy], my[iy+1], mz[iz], mz[iz+1] );
    // store the B field at the 8 corners
    int im0 = ix*m_xoff+iy*m_yoff+iz; // index of the first corner
    // cache.setField( 0, m_field[im0              ] );  // z,r,phi  => x,y,z
    // cache.setField( 1, m_field[im0            +1] );  // z,r,phi+1 => x,y,z+1
    // cache.setField( 2, m_field[im0      +m_roff  ] ); // z,r+1,phi => x,y+1,z
    // cache.setField( 3, m_field[im0      +m_roff+1] ); // z,r+1,phi+1 => x,y+1,z+1
    // cache.setField( 4, m_field[im0+m_zoff        ] ); // z+1,r,phi => x+1,y,z
    // cache.setField( 5, m_field[im0+m_zoff      +1] ); // z+1,r,phi+1 => x+1,y,z+1
    // cache.setField( 6, m_field[im0+m_zoff+m_roff  ] );// z+1,r+1,phi => x+1,y+1,z
    // cache.setField( 7, m_field[im0+m_zoff+m_roff+1] );// z+1,r+1,phi+1 => x+1,y+1,z+1
    cache.setField( 0, m_field[im0                ], scaleFactor ); // z,r,phi  => x,y,z
    cache.setField( 1, m_field[im0              +1], scaleFactor ); // z,r,phi+1 => x,y,z+1
    cache.setField( 2, m_field[im0       +m_yoff  ], scaleFactor ); // z,r+1,phi => x,y+1,z
    cache.setField( 3, m_field[im0       +m_yoff+1], scaleFactor ); // z,r+1,phi+1 => x,y+1,z+1
    cache.setField( 4, m_field[im0+m_xoff         ], scaleFactor ); // z+1,r,phi => x+1,y,z
    cache.setField( 5, m_field[im0+m_xoff       +1], scaleFactor ); // z+1,r,phi+1 => x+1,y,z+1
    cache.setField( 6, m_field[im0+m_xoff+m_yoff  ], scaleFactor ); // z+1,r+1,phi => x+1,y+1,z
    cache.setField( 7, m_field[im0+m_xoff+m_yoff+1], scaleFactor ); // z+1,r+1,phi+1 => x+1,y+1,z+1
    // store the B scale
    cache.setBscale( m_scale );
}

//
// Compute the magnetic field (and the derivatives) without caching
//
template <class T>
void BFieldMesh<T>::getB( const double *xyz, double *B, double* deriv ) const
{
    // cylindrical coordinates
    double x = xyz[0];
    double y = xyz[1];
    double z = xyz[2];
    // is it inside this map?
    if ( ! inside( x, y, z ) ) { // no
        B[0] = B[1] = B[2] = 0.0;
        if ( deriv ) for ( int i = 0; i < 9; i++ ) deriv[i] = 0.0;
        return;
    }
    // find the bin
    // x
    const std::vector<double>& mx(m_mesh[0]);
    int ix = int((x-xmin())*m_invUnit[0]); // index to LUT
    ix = m_LUT[0][ix]; // tentative mesh index from LUT
    if ( x > mx[ix+1] ) ix++;
    // y
    const std::vector<double>& my(m_mesh[1]);
    int iy = int((y-ymin())*m_invUnit[1]); // index to LUT
    iy = m_LUT[1][iy]; // tentative mesh index from LUT
    if ( y > my[iy+1] ) iy++;
    // z
    const std::vector<double>& mz(m_mesh[2]);
    int iz = int((z-zmin())*m_invUnit[2]); // index to LUT
    iz = m_LUT[2][iz]; // tentative mesh index from LUT
    if ( z > mz[iz+1] ) iz++;
    // get the B field at the 8 corners
    int im0 = ix*m_xoff+iy*m_yoff+iz; // index of the first corner
    BFieldVector<T> field[8];
    field[0] = m_field[im0                ];
    field[1] = m_field[im0              +1];
    field[2] = m_field[im0       +m_yoff  ];
    field[3] = m_field[im0       +m_yoff+1];
    field[4] = m_field[im0+m_xoff         ];
    field[5] = m_field[im0+m_xoff       +1];
    field[6] = m_field[im0+m_xoff+m_yoff  ];
    field[7] = m_field[im0+m_xoff+m_yoff+1];
    // fractional position inside this mesh
    double fx = (x-mx[ix]) / (mx[ix+1]-mx[ix]);
    double gx = 1.0 - fx;
    double fy = (y-my[iy]) / (my[iy+1]-my[iy]);
    double gy = 1.0 - fy;
    double fz = (z-mz[iz]) / (mz[iz+1]-mz[iz]);
    double gz = 1.0 - fz;
    // interpolate field values in x, y, z
    for ( int i = 0; i < 3; i++ ) { // z, r, phi
        B[i] = m_scale*( gx*( gy*( gz*field[0][i] + fz*field[1][i] ) +
                              fy*( gz*field[2][i] + fz*field[3][i] ) ) +
                         fx*( gy*( gz*field[4][i] + fz*field[5][i] ) +
                              fy*( gz*field[6][i] + fz*field[7][i] ) ) );
    }
    // compute field derivatives if requested
    if ( deriv ) {
        double sx = m_scale/(mx[ix+1]-mx[ix]);
        double sy = m_scale/(my[iy+1]-my[iy]);
        double sz = m_scale/(mz[iz+1]-mz[iz]);
        double dBdx[3];
        double dBdy[3];
        double dBdz[3];
        for ( int j = 0; j < 3; j++ ) { // Bz, Br, Bphi components
            dBdx[j]   = sx*( gy*( gz*(field[4][j]-field[0][j]) + fz*(field[5][j]-field[1][j]) ) +
                             fy*( gz*(field[6][j]-field[2][j]) + fz*(field[7][j]-field[3][j]) ) );
            dBdy[j]   = sy*( gx*( gz*(field[2][j]-field[0][j]) + fz*(field[3][j]-field[1][j]) ) +
                             fx*( gz*(field[6][j]-field[4][j]) + fz*(field[7][j]-field[5][j]) ) );
            dBdz[j]   = sz*( gx*( gy*(field[1][j]-field[0][j]) + fy*(field[3][j]-field[2][j]) ) +
                             fx*( gy*(field[5][j]-field[4][j]) + fy*(field[7][j]-field[6][j]) ) );
        }
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

//
// Construct the look-up table to accelerate bin-finding.
//
template <class T>
void BFieldMesh<T>::buildLUT()
{
    for ( int j = 0; j < 3; j++ ) { // x, y, z
        // align the m_mesh edges to m_min/m_max
        m_mesh[j].front() = m_min[j];
        m_mesh[j].back()  = m_max[j];
        // determine the unit size, q, to be used in the LUTs
        double width = m_mesh[j].back() - m_mesh[j].front();
        double q(width);
        for ( unsigned i = 0; i < m_mesh[j].size()-1; i++ ) {
            q = std::min( q, m_mesh[j][i+1] - m_mesh[j][i] );
        }
        // find the number of units in the LUT
        int n = int(width/q) + 1;
        q = width/(n+0.5);
        m_invUnit[j] = 1.0/q; // new unit size
        n++;
        int m = 0; // mesh number
        for ( int i = 0; i < n; i++ ) { // LUT index
            if ( i*q + m_mesh[j].front() > m_mesh[j][m+1] ) m++;
            m_LUT[j].push_back(m);
        }
    }
    m_yoff = m_mesh[2].size();       // index offset for incrementing y by 1
    m_xoff = m_yoff*m_mesh[1].size(); // index offset for incrementing x by 1
}

template <class T>
int BFieldMesh<T>::memSize() const
{
    int size = 0;
    size += sizeof(double)*10;
    size += sizeof(int)*2;
    for ( int i = 0; i < 3; i++ ) {
        size += sizeof(double)*m_mesh[i].capacity();
        size += sizeof(int)*m_LUT[i].capacity();
    }
    size += sizeof(BFieldVector<T>)*m_field.capacity();
    return size;
}

#endif
