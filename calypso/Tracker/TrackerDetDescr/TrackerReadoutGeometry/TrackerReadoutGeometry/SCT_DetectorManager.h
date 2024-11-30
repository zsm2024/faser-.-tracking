/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_DetectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRACKERREADOUTGEOMETRY_SCT_DETECTORMANAGER_H
#define TRACKERREADOUTGEOMETRY_SCT_DETECTORMANAGER_H

#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoVPhysVol.h"

#include "TrackerReadoutGeometry/SiDetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "TrackerReadoutGeometry/TrackerDD_Defs.h"

#include "TrackerIdentifier/FaserSCT_ID.h"
  
class StoreGateSvc;
class Identifier; 
class IdentifierHash;
class GeoAlignableTransform;
class GeoVFullPhysVol;
class GeoVPhysVol;
class GeoVAlignmentStore;
class CondAttrListCollection;

namespace TrackerDD {

  class SiDetectorElement;
  class ExtendedAlignableTransform;
  class SCT_ModuleSideDesign;

  /** @class SCT_DetectorManager
  
      Dedicated detector manager extending the functionality of the SiDetectorManager
      with dedicated SCT information, access.
      
      @author: Grant Gorfine
      - modified and maintained by Nick Styles & Andreas Salzburger
      */
      
  class SCT_DetectorManager : public SiDetectorManager  {

    public:
    
      // Constructor
      SCT_DetectorManager( StoreGateSvc* detStore );
     
      // Destructor
      virtual ~SCT_DetectorManager();
     
      /** Access Raw Geometry */
      virtual unsigned int getNumTreeTops()           const override;
      virtual PVConstLink  getTreeTop(unsigned int i) const override;
      /** Add tree top */
      void addTreeTop(PVLink); 
    
    
      //
      // Access Readout Elements
      //
    
      /** access to individual elements via Identifier */
      virtual SiDetectorElement * getDetectorElement(const Identifier &id) const override;

      /** access to individual elements via IdentifierHash */
      virtual SiDetectorElement * getDetectorElement(const IdentifierHash &idHash) const override;
      
      /** access to individual elements via module numbering schema */
      SiDetectorElement * getDetectorElement(int barrel_endcap, int layer_wheel, int phi_module, int eta_module, int side) const;
    
      /** access to whole collectiom via iterators */
      virtual const SiDetectorElementCollection * getDetectorElementCollection() const override;
      virtual SiDetectorElementCollection::const_iterator getDetectorElementBegin() const override;
      virtual SiDetectorElementCollection::const_iterator getDetectorElementEnd() const override;
    
      /** Add elememts during construction */
      virtual void addDetectorElement(SiDetectorElement * element) override;
    
      /** Add alignable transforms. No access to these, they will be changed by manager: */
      virtual void addAlignableTransform (int level,
    				                      const Identifier &id, 
    				                      GeoAlignableTransform *xf,
    				                      const GeoVFullPhysVol * child);
      
      /**  As above but does a dynamic_cast to GeoVFullPhysVol */
      virtual void addAlignableTransform (int level,
    				                      const Identifier &id, 
    				                      GeoAlignableTransform *xf,
    				                      const GeoVPhysVol * child); 
    
      // DEPRECATED
      virtual void addAlignableTransform (int, const Identifier &, GeoAlignableTransform *) override {}; // For backward compatibility
    
      /** Initialize the neighbours. This can only be done when all elements are built. */
      virtual void initNeighbours() override;
        
      /** Check identifier is for this detector */
      virtual bool identifierBelongs(const Identifier & id) const override;
    
      /** Access to module design, casts to SCT_ModuleSideDesign */
      const SCT_ModuleSideDesign * getSCT_Design(int i) const;

      /** Process new global DB folders for L1 and L2 **/
      virtual
      bool processGlobalAlignment(const std::string &, int level, FrameType frame,
                                  const CondAttrListCollection* obj,
                                  GeoVAlignmentStore* alignStore) const override;

      // comply with InDetDetectorManager interface
      bool processSpecialAlignment(const std::string & key,
                                   TrackerDD::AlignFolderType alignfolder) const override;

      bool processSpecialAlignment(const std::string& key,
                                   const CondAttrListCollection* obj=nullptr,
                                   GeoVAlignmentStore* alignStore=nullptr) const override;


    private:
      /** implements the main alignment update for delta transforms in different frames,
          it translates into the LocalDelta or GlobalDelta function of SiDetectorManager
      */
      virtual bool setAlignableTransformDelta(int level, 
                                              const Identifier & id, 
                                              const Amg::Transform3D & delta,
                                              FrameType frame,
                                              GeoVAlignmentStore* alignStore) const override;
      
      /** Prevent copy and assignment */
      const SCT_DetectorManager & operator=(const SCT_DetectorManager &right);
      SCT_DetectorManager(const SCT_DetectorManager &right); 
    
      virtual const FaserSCT_ID  * getIdHelper() const override;
       
      // Private member data
      std::vector<PVLink>                                           m_volume;  
      SiDetectorElementCollection                                   m_elementCollection;
      typedef std::map<Identifier, ExtendedAlignableTransform *>    AlignableTransformMap;
      std::vector< AlignableTransformMap >                          m_higherAlignableTransforms;
      std::vector< ExtendedAlignableTransform *>                    m_alignableTransforms; 
      std::vector< ExtendedAlignableTransform *>                    m_moduleAlignableTransforms; 
      const FaserSCT_ID*                                                 m_idHelper;
      
      /** This variable switches the how the local alignment corrections are applied
          If true they will be calcualted on top  of all of other corrections but in the default reference frame
          If false they will be calcualted  on top  of all of other corrections but in the globally aligned reference frame    
      */
      bool                                                          m_isLogical;      
      
      
    };

} // namespace TrackerDD

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h" 
CLASS_DEF(TrackerDD::SCT_DetectorManager, 56285158, 1) 
#endif

#endif // TRACKERREADOUTGEOMETRY_SCT_DETECTORMANAGER_H
