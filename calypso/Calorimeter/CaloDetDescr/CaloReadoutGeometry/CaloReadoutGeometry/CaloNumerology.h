/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   CaloNumerology.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef CALOREADOUTGEOMETRY_CALONUMEROLOGY_H
#define CALOREADOUTGEOMETRY_CALONUMEROLOGY_H

#include <set>

namespace CaloDD {

  /** @class CaloNumerology
  
     Class to extract numerology for Ecal. For example number of rows, modules, pmts, etc.

     @author Grant Gorfine
   */

  class CaloNumerology {


    public:
    
      /** Constructor: */
      CaloNumerology();
      
      // Accessors:
      
      /** Number of rows */
      int numRows() const;

      /** Number of modules in a row */
      int numModulesPerRow() const;

      /** Number of pmts for module */
      int numPmtsPerModule() const;

      const std::set<int>& rowIds() const;

      // Check presence of row
      /** Check if row exists */
      bool useRow(int row) const;
      
           
      // Modifiers:
      void addRow(int id);
      void setNumModulesPerRow(int modules);
      void setNumPmtsPerModule(int pmts);

    private:
      
      int m_numModulesPerRow;
      int m_numPmtsPerModule;
      std::set<int> m_rowIds;
  };
  
}// End namespace

#include "CaloNumerology.icc"

#endif // CALOREADOUTGEOMETRY_CALONUMEROLOGY_H
