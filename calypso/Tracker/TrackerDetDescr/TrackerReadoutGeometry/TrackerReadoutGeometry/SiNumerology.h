/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   SiNumerology.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef TRACKERREADOUTGEOMETRY_SINUMEROLOGY_H
#define TRACKERREADOUTGEOMETRY_SINUMEROLOGY_H

#include <vector>

namespace TrackerDD {

  /** @class SiNumerology
  
     Class to extract numerology for SCT. For example number of layers, disks, sectors, rings, etc.
     See InnerDetector/InDetExample/InDetDetDescrExample/src/SiReadSiDetectorElements.cxx for example of usage.

     @author Grant Gorfine
   */

  class SiNumerology {


    public:
    
      /** Constructor: */
      SiNumerology();
      
      // Accessors:
      
      /** Number of barrels. Normally 4 if interface detector present, or 3 otherwise. */
      int numBarrels() const; 

      int numStations() const;
           
      // /** Barrel/endcap identifier for each barrel. Normally barrelId(0) = 0 if interface detector present or 1 otherwise  */
      int barrelId(int index) const;

      int stationId(int index) const;
           
      /** Number of layers */
      int numLayers() const; 
      
      /** Number of sectors in phi for a layer */
      int numPhiModulesForLayer(int layer) const;

      int numPhiModulesForLayer() const;
      
      /** Number of sectors in phi for a ring in a disk */
      int numEtaModulesForLayer(int layer) const;
      
      int numEtaModulesForLayer() const;

      /** First eta_module number for a layer */
      int beginEtaModuleForLayer(int layer) const;
      
      int beginEtaModuleForLayer() const;

      /** Last eta_module number + 1 */
      int endEtaModuleForLayer(int layer) const;
      
      int endEtaModuleForLayer() const;

      /** Check if eta_module=0 exists */
      bool skipEtaZeroForLayer(int layer) const;

      bool skipEtaZeroForLayer() const;
      
      // Check presence of layer/disk
      /** Check if layer exists */
      bool useLayer(int layer) const;
      
      // Maximums 
      /** Maximum number of modules in a barrel stave */
      int maxNumBarrelEta() const;
      
      /** Maximum number of strips. Same as maxNumPhiCells() */
      int maxNumStrips() const; 
      
      /** Maximum number of cells in phi direction. Same as maxNumStrips()  */
      int maxNumPhiCells() const;
      
      // Modifiers:
      void addBarrel(int id);
      void setNumLayers(int nLayers); 
      void setNumPhiModulesForLayer(int layer, int nPhiModules);
      void setNumEtaModulesForLayer(int layer, int nEtaModules);
      void setMaxNumEtaCells(int cells);
      void setMaxNumPhiCells(int cells);
      
    private:
      
      int m_numLayers;
      int m_numDisks;
      int m_maxPhiCells;
      int m_maxEtaCells;
      int m_maxNumBarrelEta;
      int m_maxNumEndcapRings;
      int m_maxNumBarrelPhiModules;
      int m_maxNumEndcapPhiModules;
    
      std::vector<int> m_barrelIds;
      std::vector<int> m_phiModulesForLayer;
      std::vector<int> m_etaModulesForLayer;

  };
  
}// End namespace

#include "SiNumerology.icc"

#endif // TRACKERREADOUTGEOMETRY_SINUMEROLOGY_H
