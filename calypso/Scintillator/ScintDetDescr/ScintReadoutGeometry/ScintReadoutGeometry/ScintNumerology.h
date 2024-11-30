/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   ScintNumerology.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef SCINTREADOUTGEOMETRY_SCINTNUMEROLOGY_H
#define SCINTREADOUTGEOMETRY_SCINTNUMEROLOGY_H

#include <set>

namespace ScintDD {

  /** @class ScintNumerology
  
     Class to extract numerology for Veto, Trigger and Preshower. For example number of stations, plates, pmts, etc.

     @author Grant Gorfine
   */

  class ScintNumerology {


    public:
    
      /** Constructor: */
      ScintNumerology();
      
      // Accessors:
      
      /** Number of stations */
      int numStations() const;

      /** Number of plates in a station */
      int numPlatesPerStation() const;

      /** Number of pmts for plate */
      int numPmtsPerPlate() const;

      const std::set<int>& stationIds() const;

      // Check presence of station
      /** Check if station exists */
      bool useStation(int station) const;
      
           
      // Modifiers:
      void addStation(int id);
      void setNumPlatesPerStation(int plates);
      void setNumPmtsPerPlate(int pmts);

    private:
      
      int m_numPlatesPerStation;
      int m_numPmtsPerPlate;
      std::set<int> m_stationIds;
  };
  
}// End namespace

#include "ScintNumerology.icc"

#endif // SCINTREADOUTGEOMETRY_SCINTNUMEROLOGY_H
