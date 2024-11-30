/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserFieldSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MAGFIELDSERVICES_FASERFIELDSVC_H
#define MAGFIELDSERVICES_FASERFIELDSVC_H 1

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

// MagField includes
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "FaserFieldSvcTLS.h"
#include "MagFieldElements/BFieldCache.h"
#include "MagFieldElements/BFieldZone.h"
// // #include "MagFieldInterfaces/IMagFieldManipulator.h"

// STL includes
#include <vector>
#include <iostream>

#include "CxxUtils/checker_macros.h"

// forward declarations
// class CondAttrListCollection;
class BFieldZone;
class TFile;
class Incident;

namespace MagField {

  /** @class FaserFieldSvc
    */

  class FaserFieldSvc : public extends<AthService, IMagFieldSvc, IIncidentListener> {
    public:

      //** Constructor with parameters */
      FaserFieldSvc( const std::string& name, ISvcLocator* pSvcLocator ) ATLAS_CTORDTOR_NOT_THREAD_SAFE;

      /** Destructor */
      virtual ~FaserFieldSvc() ATLAS_CTORDTOR_NOT_THREAD_SAFE;

      /** Athena algorithm's interface methods */
      virtual StatusCode  initialize ATLAS_NOT_THREAD_SAFE () override; 
      virtual StatusCode  finalize() override;

      /** Read **/
      virtual void handle(const Incident& runIncident) override;

      /** Call back for possible magnet filename update **/
    //   StatusCode updateMapFilenames(IOVSVC_CALLBACK_ARGS);

      /** get B field value at given position */
      /** xyz[3] is in mm, bxyz[3] is in kT */
      /** if deriv[9] is given, field derivatives are returned in kT/mm */
      virtual void getField(const double *xyz, double *bxyz, double *deriv = nullptr) const override final;
      virtual void getFieldZR(const double *xyz, double *bxyz, double *deriv = nullptr) const override final;

    private:
      /** Retrieve, initialize and return a thread-local storage object */
      inline struct FaserFieldSvcTLS &getFaserFieldSvcTLS() const;

      // Functions used by getField[ZR]
      // search for a "zone" to which the point (z,r,phi) belongs
      inline const BFieldZone* findZone( double x, double y, double z ) const;
      // slow search is used during initialization to build the LUT
      BFieldZone* findZoneSlow( double x, double y, double z );
      // fill the cache. return true if successful
      // return false if the position is outside the valid map volume
      inline bool fillFieldCache(double x, double y, double z, FaserFieldSvcTLS &tls) const;
    //   inline bool fillFieldCacheZR(double z, double r, FaserFieldSvcTLS &tls) const;

      // set currents from when DCS is not read
    //   StatusCode importCurrents(FaserFieldSvcTLS &tls);
        StatusCode startOfRunInitialization(FaserFieldSvcTLS &tls);
      // initialize map
      StatusCode initializeMap(FaserFieldSvcTLS &tls);
      // read the field map from an ASCII or ROOT file
      StatusCode readMap( const char* filename );
    //   StatusCode readMap( std::istream& input );
      StatusCode readMap( TFile* rootfile );
      // write the field map to a ROOT file
    //   void writeMap( TFile* rootfile ) const;
      // clear the field map
      void clearMap(FaserFieldSvcTLS &tls);

      // utility functions used by readMap
    //   int read_packed_data( std::istream& input, std::vector<int>& data ) const;
    //   int read_packed_int( std::istream& input, int &n ) const;
      void buildLUT();
    //   void buildZR();

      // scale field according to the current
    //   void scaleField();

      /** approximate memory footprint in bytes */
      int memSize() const;

      /** Data Members **/

      // field map names
      std::string m_fullMapFilename; // all magnets on
    //   std::string m_soleMapFilename; // solenoid on / toroid off
    //   std::string m_toroMapFilename; // toroid on / solenoid off
      // current associated with the map
    //   double m_mapSoleCurrent; // solenoid current in A
    //   double m_mapToroCurrent; // toroid current in A
      // threshold below which currents are considered zero
    //   double m_soleMinCurrent; // minimum solenoid current to be considered ON
    //   double m_toroMinCurrent; // minimum toroid current to be considered ON
      // flag to use magnet current from DCS in COOL
    //   bool m_useDCS;
      // COOL folder name containing current information
    //   std::string m_coolCurrentsFolderName;
      // flag to read magnet map filenames from COOL
    //   bool m_useMapsFromCOOL;
      // COOL folder name containing field maps
    //   std::string m_coolMapsFolderName;
      // actual current if DCS is not in use
    //   double m_useSoleCurrent; // solenoid current in A
    //   double m_useToroCurrent; // toroid current in A
      // flag to skip current rescale and use map currents as they are
    //   bool m_lockMapCurrents;

      // handle for COOL field map filenames
    //   const DataHandle<CondAttrListCollection> m_mapHandle;
      // handle for COOL currents
    //   const DataHandle<CondAttrListCollection> m_currentHandle;

      // full 3d map (made of multiple zones)
      std::vector<BFieldZone>        m_zone;

      // fast 2d map (made of one zone)
    //   BFieldMeshZR                  *m_meshZR;

      // data members used in zone-finding
      std::vector<double>            m_edge[3];    // zone boundaries in x, y, z
      std::vector<int>               m_edgeLUT[3]; // look-up table for zone edges
      double                         m_invq[3];    // 1/stepsize in m_edgeLUT
      std::vector<const BFieldZone*> m_zoneLUT; // look-up table for zones
      // more data members to speed up zone-finding
      double                         m_xmin;   // minimum x
      double                         m_xmax;   // maximum x
      int                            m_nx;     // number of x bins in zoneLUT
      double                         m_ymin;   // maximum y
      double                         m_ymax;   // maximum y
      int                            m_ny;     // number of y bins in zoneLUT
      double                         m_zmin;   // maximum z
      double                         m_zmax;   // maximum z
      int                            m_nz;   // number of z bins in zoneLUT

      /* // handle for field manipulator, if any
	 bool m_doManipulation;
	 ToolHandle<IMagFieldManipulator> m_manipulator; */

  };
}

// inline functions

//
// Initialize and return a thread-local storage object
//
struct MagField::FaserFieldSvcTLS&
MagField::FaserFieldSvc::getFaserFieldSvcTLS() const {
  static thread_local FaserFieldSvcTLS tls = FaserFieldSvcTLS();
  // return thread-local object
  return tls;
}

//
// Search for the zone that contains a point (z, r, phi)
// Fast version utilizing the LUT.
//
const BFieldZone*
MagField::FaserFieldSvc::findZone( double x, double y, double z ) const
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

/** fill given magnetic field zone */
bool
MagField::FaserFieldSvc::fillFieldCache(double x, double y, double z, FaserFieldSvcTLS &tls) const
{
  // search for the zone
  const BFieldZone* zone = findZone( x, y, z );
  if ( zone == nullptr ) {
      // outsize all zones
      return false;
  }
  // fill the cache
  zone->getCache( x, y, z, tls.cache );

  // pointer to the conductors in the zone
//   tls.cond = zone->condVector();

  // set a flag that the thread-local storage is initialized
  tls.isInitialized = true;

  return true;
}

/** fill Z-R cache for solenoid */
// bool
// MagField::AtlasFieldSvc::fillFieldCacheZR(double z, double r, AtlasFieldSvcTLS &tls) const
// {
//   // is it inside the solenoid zone?
//   if ( m_meshZR && m_meshZR->inside( z, r ) ) {
//     // fill the cache
//     m_meshZR->getCache( z, r, tls.cacheZR );
//   } else {
//     // outside solenoid
//     return false;
//   }

//   return true;
// }

#endif //> !MAGFIELDSERVICES_FASERFIELDSVC_H
