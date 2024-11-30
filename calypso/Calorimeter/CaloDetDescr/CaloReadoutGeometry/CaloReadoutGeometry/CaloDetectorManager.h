/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloDetectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CALOREADOUTGEOMETRY_CALODETECTORMANAGER_H
#define CALOREADOUTGEOMETRY_CALODETECTORMANAGER_H

#include "CaloReadoutGeometry/CaloDetectorManagerBase.h"

#include "CaloReadoutGeometry/CaloDetectorElementCollection.h"
#include "CaloReadoutGeometry/CaloDD_Defs.h"
#include "CaloReadoutGeometry/CaloNumerology.h"

// Amg stuff
#include "GeoPrimitives/GeoPrimitives.h"

#include "CLHEP/Geometry/Transform3D.h"

#include <string>
#include <map>
  
class StoreGateSvc;
class Identifier; 
class IdentifierHash;
class FaserDetectorID;
class GeoAlignableTransform;
class GeoVAlignmentStore;

namespace CaloDD {

class CaloDetectorElement;
class CaloDetectorDesign;
class ExtendedAlignableTransform;
class CaloNumerology;

  /** @class CaloDetectorManager
  
        Base class for Calorimeter Detector managers.
        
        The Detector manager has methods to retrieve the Identifier
        helper and methods to retrieve the detector elements.  It also
        manages the alignment with methods to register the call backs
        and infrastructure to associate the alignment transforms with
        the appropriate alignable transform in GeoModel.
  
       @author: Grant Gorfine
       - modified and maintained by Nick Styles & Andreas Salzburger 
       */

    class CaloDetectorManager : public CaloDetectorManagerBase  {
    
    
    public:
    
      // Constructor
      CaloDetectorManager(StoreGateSvc * detStore, const std::string & name);
     
      // Destructor
      virtual ~CaloDetectorManager() {};
    
    
      //
      // Access Readout Elements
      //
    
      /** access to individual elements using Identifier or IdentiferHash */
      virtual CaloDetectorElement * getDetectorElement(const Identifier &id) const = 0;
      virtual CaloDetectorElement * getDetectorElement(const IdentifierHash &idHash) const = 0;
    
      /** access to whole collectiom */
      virtual const CaloDetectorElementCollection * getDetectorElementCollection() const = 0;
      virtual CaloDetectorElementCollection::const_iterator getDetectorElementBegin() const = 0;
      virtual CaloDetectorElementCollection::const_iterator getDetectorElementEnd() const = 0;
    
    
      /** Add elememts */
      virtual void addDetectorElement(CaloDetectorElement * element) = 0;
    
      /** Initialize the neighbours. This can only be done when all elements are built */
      virtual void initNeighbours() = 0;
    
      /** Get tag used in dictionary */
      const std::string & tag() const; 
    
      /** Invalidate cache for all detector elements */
      virtual void invalidateAll() const;
    
      /** Update all caches */
      virtual void updateAll() const;
    
      
      /** Helper method to set delta transform from a global delta - Amg interface*/
      bool setAlignableTransformGlobalDelta(ExtendedAlignableTransform * extXF, 
                                            const Amg::Transform3D & delta,
                                            GeoVAlignmentStore* alignStore=nullptr) const;
    
      /** Helper method to set delta transform from a local delta - Amg interface */
      bool setAlignableTransformLocalDelta(ExtendedAlignableTransform * extXF, 
                                           const Amg::Transform3D & localToGlobalXF,
                                           const Amg::Transform3D & delta,
                                           GeoVAlignmentStore* alignStore=nullptr) const;
    
      /** Access to module design */
    
      void setDesign(const CaloDetectorDesign*);
      const CaloDetectorDesign* getDesign() const;

      /** Access Numerology */
      const CaloNumerology & numerology() const {return m_numerology;}
      CaloNumerology & numerology() {return m_numerology;}
    
    private:
      //** Prevent copy and assignment */
      const CaloDetectorManager & operator=(const CaloDetectorManager &right);
      CaloDetectorManager(const CaloDetectorManager &right); 
    
      /** This method is called by the CaloDetectorManagerBase */
      virtual bool setAlignableTransformDelta(int level, 
                                              const Identifier & id, 
                                              const Amg::Transform3D & delta,
                                              FrameType frame,
                                              GeoVAlignmentStore* alignStore) const = 0;
    
      std::string                              m_tag;
      CaloNumerology                           m_numerology;
      const CaloDetectorDesign *               m_design;
    
    };


} // namespace CaloDD

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h"

CLASS_DEF(CaloDD::CaloDetectorManager, 231427264, 1)
#endif

#endif // CALOREADOUTGEOMETRY_CALODETECTORMANAGER_H
