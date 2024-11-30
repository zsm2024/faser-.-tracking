/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author R.D.Schaffer -at- cern.ch
 * @date end 2019
 * @brief Local cache for magnetic field (based on MagFieldServices/FaserFieldSvcTLS.h)
 */

#ifndef MAGFIELDELEMENTS_FASERFIELDMAP_H
#define MAGFIELDELEMENTS_FASERFIELDMAP_H 1

// MagField includes
#include "MagFieldElements/BFieldCache.h"
// #include "MagFieldElements/BFieldCacheZR.h"
// #include "MagFieldElements/BFieldCond.h"
#include "MagFieldElements/BFieldZone.h"
// #include "MagFieldElements/BFieldMeshZR.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CxxUtils/restrict.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include<iostream>
#include <memory>

// forward declarations
class TFile;

namespace MagField {

    
/** @class FaserFieldMap
 *
 *  @brief Map for magnetic field 
 *
 *  @author R.D.Schaffer -at- cern.ch
 */
    class FaserFieldMap
    {
    public:
        FaserFieldMap();
        ~FaserFieldMap(); 

        // initialize map from root file
        bool initializeMap( TFile* rootfile );

        // Functions used by getField[ZR] in FaserFieldCache
        // search for a "zone" to which the point (z,r,phi) belongs
        const BFieldZone* findBFieldZone( double z, double r, double phi ) const;

        // fast 2d map (made of one zone)
        // const BFieldMeshZR* getBFieldMesh() const;

        /** status of the magnets */
        // bool solenoidOn() const { return solenoidCurrent() > 0.0; }
        // bool toroidOn() const   { return toroidCurrent()   > 0.0; }
        // magnet currents read with map - needed for scaling
        // float solenoidCurrent() const { return m_solenoidCurrent; }
        // float toroidCurrent() const   { return m_toroidCurrent;   }
        int   dipoleZoneId() const  { return m_dipoleZoneId;  }
    private:
    
        FaserFieldMap& operator= (FaserFieldMap&& other)      = delete;
        FaserFieldMap(const FaserFieldMap& other)             = delete;
        FaserFieldMap& operator= (const FaserFieldMap& other) = delete;
        FaserFieldMap(FaserFieldMap&& other)                  = delete;

        // slow zone search is used during initialization to build the LUT
        BFieldZone* findZoneSlow( double z, double r, double phi );

        // utility functions used by readMap
        int read_packed_data( std::istream& input, std::vector<int>& data ) const;
        int read_packed_int( std::istream& input, int &n ) const;
        void buildLUT();
        // void buildZR();

        /** approximate memory footprint in bytes */
        int memSize() const;

        /** Data Members **/

        // field map name
        std::string m_filename; 

        // currents read in with map
        // float m_solenoidCurrent{0}; // solenoid current in ampere
        // float m_toroidCurrent{0};   // toroid current in ampere
        int   m_dipoleZoneId{-1};      // dipole zone id

        // full 3d map (made of multiple zones)
        std::vector<BFieldZone>        m_zone;

        // fast 2d map (made of one zone)
        // BFieldMeshZR*                  m_meshZR{nullptr};

        // data members used in zone-finding
        std::vector<double>            m_edge[3];    // zone boundaries in z, r, phi
        std::vector<int>               m_edgeLUT[3]; // look-up table for zone edges
        double                         m_invq[3];    // 1/stepsize in m_edgeLUT
        std::vector<const BFieldZone*> m_zoneLUT; // look-up table for zones
        // more data members to speed up zone-finding
        double                         m_xmin{0};   // minimum x
        double                         m_xmax{0};   // maximum x
        int                            m_nx  {0};   // number of x bins in zoneLUT
        double                         m_ymin{0};   // minimum y
        double                         m_ymax{0};   // maximum y
        int                            m_ny  {0};   // number of r bins in zoneLUT
        double                         m_zmin{0};   // minimum z
        double                         m_zmax{0};   // maximum z
        int                            m_nz  {0};   // number of z bins in zoneLUT
        bool                           m_mapIsInitialized{false};
        
    };

}  // namespace MagField



#endif  // MAGFIELDELEMENTS_FASERFIELDMAP_H
