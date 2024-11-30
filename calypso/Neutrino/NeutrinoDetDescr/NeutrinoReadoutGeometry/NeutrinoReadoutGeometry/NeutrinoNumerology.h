/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   NeutrinoNumerology.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef NEUTRINOREADOUTGEOMETRY_NEUTRINONUMEROLOGY_H
#define NEUTRINOREADOUTGEOMETRY_NEUTRINONUMEROLOGY_H

#include <set>

namespace NeutrinoDD {

  /** @class NeutrinoNumerology
  
     Class to extract numerology for emulsion. For example number of modules, bases, films, etc.

     @author Grant Gorfine
   */

  class NeutrinoNumerology {


    public:
    
      /** Constructor: */
      NeutrinoNumerology();
      
      // Accessors:
      
      /** Number of modules */
      int numModules() const;

      /** Number of bases in a module */
      int numBasesPerModule() const;

      /** Number of films per base */
      int numFilmsPerBase() const;

      const std::set<int>& moduleIds() const;

      // Check presence of station
      /** Check if station exists */
      bool useModule(int module) const;
      
           
      // Modifiers:
      void addModule(int id);
      void setNumBasesPerModule(int bases);
      void setNumFilmsPerBase(int films);

    private:
      
      int m_numBasesPerModule;
      int m_numFilmsPerBase;
      std::set<int> m_moduleIds;
  };
  
}// End namespace

#include "NeutrinoNumerology.icc"

#endif // NEUTRINOREADOUTGEOMETRY_NEUTRINONUMEROLOGY_H
