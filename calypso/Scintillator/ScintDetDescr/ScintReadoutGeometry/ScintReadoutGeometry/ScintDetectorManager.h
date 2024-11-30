/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ScintDetectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SCINTREADOUTGEOMETRY_SCINTDETECTORMANAGER_H
#define SCINTREADOUTGEOMETRY_SCINTDETECTORMANAGER_H

#include "ScintReadoutGeometry/ScintDetectorManagerBase.h"

#include "ScintReadoutGeometry/ScintDetectorElementCollection.h"
#include "ScintReadoutGeometry/ScintDD_Defs.h"
#include "ScintReadoutGeometry/ScintNumerology.h"

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

namespace ScintDD {

class ScintDetectorElement;
class ScintDetectorDesign;
class ExtendedAlignableTransform;
class ScintNumerology;

  /** @class ScintDetectorManager
  
        Base class for Scintillator Detector managers.
        
        The Detector manager has methods to retrieve the Identifier
        helper and methods to retrieve the detector elements.  It also
        manages the alignment with methods to register the call backs
        and infrastructure to associate the alignment transforms with
        the appropriate alignable transform in GeoModel.
  
       @author: Grant Gorfine
       - modified and maintained by Nick Styles & Andreas Salzburger 
       */

    class ScintDetectorManager : public ScintDetectorManagerBase  {
    
    
    public:
    
      // Constructor
      ScintDetectorManager(StoreGateSvc * detStore, const std::string & name);
     
      // Destructor
      virtual ~ScintDetectorManager() {};
    
    
      //
      // Access Readout Elements
      //
    
      /** access to individual elements using Identifier or IdentiferHash */
      virtual ScintDetectorElement * getDetectorElement(const Identifier &id) const = 0;
      virtual ScintDetectorElement * getDetectorElement(const IdentifierHash &idHash) const = 0;
    
      /** access to whole collectiom */
      virtual const ScintDetectorElementCollection * getDetectorElementCollection() const = 0;
      virtual ScintDetectorElementCollection::const_iterator getDetectorElementBegin() const = 0;
      virtual ScintDetectorElementCollection::const_iterator getDetectorElementEnd() const = 0;
    
    
      /** Add elememts */
      virtual void addDetectorElement(ScintDetectorElement * element) = 0;
    
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
    
      void setDesign(const ScintDetectorDesign*);
      const ScintDetectorDesign* getDesign() const;

      /** Access Numerology */
      const ScintNumerology & numerology() const {return m_numerology;}
      ScintNumerology & numerology() {return m_numerology;}
    
    private:
      //** Prevent copy and assignment */
      const ScintDetectorManager & operator=(const ScintDetectorManager &right);
      ScintDetectorManager(const ScintDetectorManager &right); 
    
      /** This method is called by the ScintDetectorManagerBase */
      virtual bool setAlignableTransformDelta(int level, 
                                              const Identifier & id, 
                                              const Amg::Transform3D & delta,
                                              FrameType frame,
                                              GeoVAlignmentStore* alignStore) const = 0;
    
    
    
      std::string                               m_tag;
      ScintNumerology                           m_numerology;
      const ScintDetectorDesign *               m_design;
    
    };


} // namespace ScintDD

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h"

CLASS_DEF(ScintDD::ScintDetectorManager, 209997176, 1)
#endif

#endif // SCINTREADOUTGEOMETRY_SCINTDETECTORMANAGER_H
