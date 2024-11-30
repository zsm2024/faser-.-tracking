/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserFieldMap.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// ISF_Services include
#include "MagFieldElements/FaserFieldMap.h"

// PathResolver
#include "PathResolver/PathResolver.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// ROOT
#include "TFile.h"
#include "TTree.h"

/** Constructor **/
MagField::FaserFieldMap::FaserFieldMap() 
{}


MagField::FaserFieldMap::~FaserFieldMap()
{}

// Search for the zone that contains a point (z, r, phi)
// Fast version utilizing the LUT.
//
const BFieldZone*
MagField::FaserFieldMap::findBFieldZone( double x, double y, double z ) const
{
  // make sure it's inside the largest zone
  // NB: the sign of the logic is chosen in order to return 0 on NaN inputs
  if ( x >= m_xmin && x <= m_xmax && y >= m_ymin && y <= m_ymax && z >= m_zmin && z <= m_zmax) {
    // find the edges of the zone
    // x
    const std::vector<double>& edgex(m_edge[0]);
    int ix = int(m_invq[0]*(x-m_xmin)); // index to LUT
    ix = m_edgeLUT[0][ix]; // tentative index from LUT
    if ( x > edgex[ix+1] ) ix++;
    // y
    const std::vector<double>& edgey(m_edge[1]);
    int iy = int(m_invq[1]*(y-m_ymin)); // index to LUT
    iy = m_edgeLUT[1][iy]; // tentative index from LUT
    if ( y > edgey[iy+1] ) iy++;
    // z
    const std::vector<double>& edgez(m_edge[2]);
    int iz = int(m_invq[2]*(z-m_zmin)); // index to LUT
    iz = m_edgeLUT[2][iz]; // tentative index from LUT
    if ( z > edgez[iz+1] ) iz++;
    // use LUT to get the zone
    return m_zoneLUT[(ix*m_ny+iy)*m_nz+iz];
  } else {
    return nullptr;
  }
}


//
// read the map from a ROOT file.
// returns true if successful.
//
bool
MagField::FaserFieldMap::initializeMap( TFile* rootfile )
{
    // open the tree
    TTree* tree = (TTree*)rootfile->Get("BFieldMap");

    int id;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    double bscale;
    int nmeshx;
    int nmeshy;
    int nmeshz;
    double* meshx;
    double* meshy;
    double* meshz;
    int nfield;
    short* fieldx;
    short* fieldy;
    short* fieldz;
    // define the fixed-sized branches first
    tree->SetBranchAddress( "id", &id );
    tree->SetBranchAddress( "xmin", &xmin );
    tree->SetBranchAddress( "xmax", &xmax );
    tree->SetBranchAddress( "ymin", &ymin );
    tree->SetBranchAddress( "ymax", &ymax );
    tree->SetBranchAddress( "zmin", &zmin );
    tree->SetBranchAddress( "zmax", &zmax );
    tree->SetBranchAddress( "bscale", &bscale );
    // tree->SetBranchAddress( "ncond", &ncond );
    tree->SetBranchAddress( "nmeshx", &nmeshx );
    tree->SetBranchAddress( "nmeshy", &nmeshy );
    tree->SetBranchAddress( "nmeshz", &nmeshz );
    tree->SetBranchAddress( "nfield", &nfield );
    // prepare arrays - need to know the maximum sizes
    // open the tree of buffer sizes (may not exist in old maps)
    unsigned /*maxcond(0),*/ maxmeshx(0), maxmeshy(0), maxmeshz(0), maxfield(0);
    TTree* tmax = (TTree*)rootfile->Get("BFieldMapSize");
    if ( tmax != 0 ) {
        // tmax->SetBranchAddress( "maxcond", &maxcond );
        tmax->SetBranchAddress( "maxmeshx", &maxmeshx );
        tmax->SetBranchAddress( "maxmeshy", &maxmeshy );
        tmax->SetBranchAddress( "maxmeshz", &maxmeshz );
        tmax->SetBranchAddress( "maxfield", &maxfield );
        tmax->GetEntry(0);
    } else { // "BFieldMapSize" tree does not exist
        for ( int i = 0; i < tree->GetEntries(); i++ ) {
            tree->GetEntry(i);
            // maxcond = std::max( maxcond, (unsigned)ncond );
            maxmeshx = std::max( maxmeshx, (unsigned)nmeshx );
            maxmeshy = std::max( maxmeshy, (unsigned)nmeshy );
            maxmeshz = std::max( maxmeshz, (unsigned)nmeshz );
            maxfield = std::max( maxfield, (unsigned)nfield );
        }
    }
    meshx = new double[maxmeshx];
    meshy = new double[maxmeshy];
    meshz = new double[maxmeshz];
    fieldx = new short[maxfield];
    fieldy = new short[maxfield];
    fieldz = new short[maxfield];
    // define the variable length branches
    tree->SetBranchAddress( "meshx", meshx );
    tree->SetBranchAddress( "meshy", meshy );
    tree->SetBranchAddress( "meshz", meshz );
    tree->SetBranchAddress( "fieldx", fieldx );
    tree->SetBranchAddress( "fieldy", fieldy );
    tree->SetBranchAddress( "fieldz", fieldz );

    // reserve the space for m_zone so that it won't move as the vector grows
    m_zone.reserve( tree->GetEntries() );
    // read all tree and store
    for ( int i = 0; i < tree->GetEntries(); i++ ) {
        tree->GetEntry(i);
        BFieldZone z( id, xmin, xmax, ymin, ymax, zmin, zmax, bscale );
        m_zone.push_back(z);
        m_zone.back().reserve( nmeshx, nmeshy, nmeshz );
        for ( int j = 0; j < nmeshx; j++ ) {
            m_zone.back().appendMesh( 0, meshx[j] );
        }
        for ( int j = 0; j < nmeshy; j++ ) {
            m_zone.back().appendMesh( 1, meshy[j] );
        }
        for ( int j = 0; j < nmeshz; j++ ) {
            m_zone.back().appendMesh( 2, meshz[j] );
        }
        for ( int j = 0; j < nfield; j++ ) {
            BFieldVector<short> field( fieldx[j], fieldy[j], fieldz[j] );  // z should change most rapidly, then y, then x
            m_zone.back().appendField( field );
        }
    }
    // clean up
    tree->Delete();
    delete[] meshx;
    delete[] meshy;
    delete[] meshz;
    delete[] fieldx;
    delete[] fieldy;
    delete[] fieldz;
    // build the LUTs
    buildLUT();
    // buildZR();

    return true;
}

//
// Search for the zone that contains a point (x, y, z)
// This is a linear-search version, used only to construct the LUT.
//
BFieldZone* MagField::FaserFieldMap::findZoneSlow( double x, double y, double z )
{
    for ( int j = m_zone.size()-1; j >= 0; --j ) {
        if ( m_zone[j].inside( x, y, z ) ) return &m_zone[j];
    }
    return nullptr;
}

//
// Build the look-up table used by FindZone().
// Called by readMap()
// It also calls buildLUT() for all zones.
//
void MagField::FaserFieldMap::buildLUT()
{
    // make lists of (x,y,z) edges of all zones
    for ( int j = 0; j < 3; j++ ) { // x, y, z
        for ( unsigned i = 0; i < m_zone.size(); i++ ) {
            double e[2];
            e[0] = m_zone[i].min(j);
            e[1] = m_zone[i].max(j);
            for ( int k = 0; k < 2; k++ ) {
                m_edge[j].push_back(e[k]);
            }
        }
        // add -pi and +pi to phi, just in case
        // m_edge[2].push_back(-M_PI);
        // m_edge[2].push_back(M_PI);
        // sort
        sort( m_edge[j].begin(), m_edge[j].end() );
        // remove duplicates
        // must do this by hand to allow small differences due to rounding in phi
        int i = 0;
        for ( unsigned k = 1; k < m_edge[j].size(); k++ ) {
            if ( fabs(m_edge[j][i] - m_edge[j][k]) < 1.0e-6 ) continue;
            m_edge[j][++i] = m_edge[j][k];
        }
        m_edge[j].resize( i+1 );
        // because of the small differences allowed in the comparison above,
        // m_edge[][] values do not exactly match the m_zone[] boundaries.
        // we have to fix this up in order to avoid invalid field values
        // very close to the zone boundaries.
        for ( unsigned i = 0; i < m_zone.size(); i++ ) {
            for ( unsigned k = 0; k < m_edge[j].size(); k++ ) {
                if ( fabs(m_zone[i].min(j) - m_edge[j][k]) < 1.0e-6 ) {
                    m_zone[i].adjustMin(j,m_edge[j][k]);
                }
                if ( fabs(m_zone[i].max(j) - m_edge[j][k]) < 1.0e-6 ) {
                    m_zone[i].adjustMax(j,m_edge[j][k]);
                }
            }
        }
    }
    // build LUT for edge finding
    for ( int j = 0; j < 3; j++ ) { // x, y, z
        // find the size of the smallest interval
        double width = m_edge[j].back() - m_edge[j].front();
        double q(width);
        for ( unsigned i = 0; i < m_edge[j].size()-1; i++ ) {
            q = std::min( q, m_edge[j][i+1] - m_edge[j][i] );
        }
        // find the number of cells in the LUT
        int n = int(width/q) + 1;
        q = width/(n+0.5);
        m_invq[j] = 1.0/q;
        n++;
        // fill the LUT
        int m = 0;
        for ( int i = 0; i < n; i++ ) { // index of LUT
            if ( q*i+m_edge[j].front() > m_edge[j][m+1] ) m++;
            m_edgeLUT[j].push_back(m);
        }
    }
    // store min/max for speedup
    m_xmin=m_edge[0].front();
    m_xmax=m_edge[0].back();
    m_ymin=m_edge[1].front();
    m_ymax=m_edge[1].back();
    m_zmin=m_edge[2].front();
    m_zmax=m_edge[2].back();
    // build LUT for zone finding
    m_nx = m_edge[0].size() - 1;
    m_ny = m_edge[1].size() - 1;
    m_nz = m_edge[2].size() - 1;
    m_zoneLUT.reserve( m_nx*m_ny*m_nz );
    for ( int ix = 0; ix < m_nx; ix++ ) {
        double x = 0.5*(m_edge[0][ix]+m_edge[0][ix+1]);
        for ( int iy = 0; iy < m_ny; iy++ ) {
            double y = 0.5*(m_edge[1][iy]+m_edge[1][iy+1]);
            for ( int iz = 0; iz < m_nz; iz++ ) {
                double z = 0.5*(m_edge[2][iz]+m_edge[2][iz+1]);
                const BFieldZone* zone = findZoneSlow( x, y, z );
                m_zoneLUT.push_back( zone );
            }
        }
    }
    // build LUT in each zone
    for ( unsigned i = 0; i < m_zone.size(); i++ ) {
        m_zone[i].buildLUT();
    }
}

//
// Approximate memory footprint
//
int MagField::FaserFieldMap::memSize() const
{
    int size = 0;
    size += sizeof(BFieldCache);
    // size += sizeof(BFieldCacheZR);
    for ( unsigned i = 0; i < m_zone.size(); i++ ) {
        size += m_zone[i].memSize();
    }
    for ( int i = 0; i < 3; i++ ) {
        size += sizeof(double)*m_edge[i].capacity();
        size += sizeof(int)*m_edgeLUT[i].capacity();
    }
    size += sizeof(BFieldZone*)*m_zoneLUT.capacity();
    return size;
}


