/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutrinoDetectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef NEUTRINOREADOUTGEOMETRY_NEUTRINODETECTORMANAGER_H
#define NEUTRINOREADOUTGEOMETRY_NEUTRINODETECTORMANAGER_H

#include "NeutrinoReadoutGeometry/NeutrinoDetectorManagerBase.h"

#include "NeutrinoReadoutGeometry/NeutrinoDetectorElementCollection.h"
#include "NeutrinoReadoutGeometry/NeutrinoDD_Defs.h"
#include "NeutrinoReadoutGeometry/NeutrinoNumerology.h"

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

namespace NeutrinoDD {

class NeutrinoDetectorElement;
class NeutrinoDetectorDesign;
class ExtendedAlignableTransform;
class NeutrinoNumerology;

  /** @class NeutrinoDetectorManager
  
        Base class for Neutrino Detector managers.
        
        The Detector manager has methods to retrieve the Identifier
        helper and methods to retrieve the detector elements.  It also
        manages the alignment with methods to register the call backs
        and infrastructure to associate the alignment transforms with
        the appropriate alignable transform in GeoModel.
  
       @author: Grant Gorfine
       - modified and maintained by Nick Styles & Andreas Salzburger 
       */

    class NeutrinoDetectorManager : public NeutrinoDetectorManagerBase  {
    
    
    public:
    
      // Constructor
      NeutrinoDetectorManager(StoreGateSvc * detStore, const std::string & name);
     
      // Destructor
      virtual ~NeutrinoDetectorManager() {};
    
    
      //
      // Access Readout Elements
      //
    
      /** access to individual elements using Identifier or IdentiferHash */
      virtual NeutrinoDetectorElement * getDetectorElement(const Identifier &id) const = 0;
      virtual NeutrinoDetectorElement * getDetectorElement(const IdentifierHash &idHash) const = 0;
    
      /** access to whole collectiom */
      virtual const NeutrinoDetectorElementCollection * getDetectorElementCollection() const = 0;
      virtual NeutrinoDetectorElementCollection::const_iterator getDetectorElementBegin() const = 0;
      virtual NeutrinoDetectorElementCollection::const_iterator getDetectorElementEnd() const = 0;
    
    
      /** Add elememts */
      virtual void addDetectorElement(NeutrinoDetectorElement * element) = 0;
    
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
    
      void setDesign(const NeutrinoDetectorDesign*);
      const NeutrinoDetectorDesign* getDesign() const;

      /** Access Numerology */
      const NeutrinoNumerology & numerology() const {return m_numerology;}
      NeutrinoNumerology & numerology() {return m_numerology;}
    
    private:
      //** Prevent copy and assignment */
      const NeutrinoDetectorManager & operator=(const NeutrinoDetectorManager &right);
      NeutrinoDetectorManager(const NeutrinoDetectorManager &right); 
    
      /** This method is called by the NeutrinoDetectorManagerBase */
      virtual bool setAlignableTransformDelta(int level, 
                                              const Identifier & id, 
                                              const Amg::Transform3D & delta,
                                              FrameType frame,
                                              GeoVAlignmentStore* alignStore) const = 0;
    
    
    
      std::string                               m_tag;
      NeutrinoNumerology                           m_numerology;
      const NeutrinoDetectorDesign *               m_design;
    
    };


} // namespace NeutrinoDD

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h"

CLASS_DEF(NeutrinoDD::NeutrinoDetectorManager, 203251628, 1)
#endif

#endif // NEUTRINOREADOUTGEOMETRY_NEUTRINODETECTORMANAGER_H
