/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VetoNuDetectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SCINTREADOUTGEOMETRY_VETONUDETECTORMANAGER_H
#define SCINTREADOUTGEOMETRY_VETONUDETECTORMANAGER_H

#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoVPhysVol.h"

#include "ScintReadoutGeometry/ScintDetectorManager.h"
#include "ScintReadoutGeometry/ScintDetectorElementCollection.h"
#include "ScintReadoutGeometry/ScintDD_Defs.h"

#include "ScintIdentifier/VetoNuID.h"
  
class StoreGateSvc;
class Identifier; 
class IdentifierHash;
class GeoAlignableTransform;
class GeoVFullPhysVol;
class GeoVPhysVol;
class GeoVAlignmentStore;
class CondAttrListCollection;

namespace ScintDD {

  class ScintDetectorElement;
  class ExtendedAlignableTransform;
  class VetoNuDetectorDesign;

  /** @class VetoNuDetectorManager
  
      Dedicated detector manager extending the functionality of the ScintDetectorManager
      with dedicated VetoNu information, access.
      
      @author: Grant Gorfine
      - modified and maintained by Nick Styles & Andreas Salzburger
      - modified for FASER by D. Casper
      */
      
  class VetoNuDetectorManager : public ScintDetectorManager  {

    public:
    
      // Constructor
      VetoNuDetectorManager( StoreGateSvc* detStore );
     
      // Destructor
      virtual ~VetoNuDetectorManager();
     
      /** Access Raw Geometry */
      virtual unsigned int getNumTreeTops()           const override;
      virtual PVConstLink  getTreeTop(unsigned int i) const override;
      /** Add tree top */
      void addTreeTop(PVLink); 
    
    
      //
      // Access Readout Elements
      //
    
      /** access to individual elements via Identifier */
      virtual ScintDetectorElement * getDetectorElement(const Identifier &id) const override;

      /** access to individual elements via IdentifierHash */
      virtual ScintDetectorElement * getDetectorElement(const IdentifierHash &idHash) const override;
      
      /** access to individual elements via module numbering schema */
      ScintDetectorElement * getDetectorElement(int station, int plate) const;
    
      /** access to whole collectiom via iterators */
      virtual const ScintDetectorElementCollection * getDetectorElementCollection() const override;
      virtual ScintDetectorElementCollection::const_iterator getDetectorElementBegin() const override;
      virtual ScintDetectorElementCollection::const_iterator getDetectorElementEnd() const override;
    
      /** Add elememts during construction */
      virtual void addDetectorElement(ScintDetectorElement * element) override;
    
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
    
      /** Initialize the neighbours. This can only be done when all elements are built. */
      virtual void initNeighbours() override;

      /** Check identifier is for this detector */
      virtual bool identifierBelongs(const Identifier & id) const override;
    
      /** Access to module design, casts to VetoNuDetectorDesign */
      const ScintDetectorDesign * getVetoNuDesign() const;

      /** Process new global DB folders for L1 and L2 **/
      virtual
      bool processGlobalAlignment(const std::string &, int level, FrameType frame,
                                  const CondAttrListCollection* obj,
                                  GeoVAlignmentStore* alignStore) const override;

      // comply with ScintDetectorManager interface
      bool processSpecialAlignment(const std::string & key,
                                   ScintDD::AlignFolderType alignfolder) const override;

      bool processSpecialAlignment(const std::string& key,
                                   const CondAttrListCollection* obj=nullptr,
                                   GeoVAlignmentStore* alignStore=nullptr) const override;


    private:
      /** implements the main alignment update for delta transforms in different frames,
          it translates into the LocalDelta or GlobalDelta function of ScintDetectorManager
      */
      virtual bool setAlignableTransformDelta(int level, 
                                              const Identifier & id, 
                                              const Amg::Transform3D & delta,
                                              FrameType frame,
                                              GeoVAlignmentStore* alignStore) const override;
      
      /** Prevent copy and assignment */
      const VetoNuDetectorManager & operator=(const VetoNuDetectorManager &right);
      VetoNuDetectorManager(const VetoNuDetectorManager &right); 
    
      virtual const VetoNuID* getIdHelper() const override;
       
      // Private member data
      std::vector<PVLink>                                           m_volume;  
      ScintDetectorElementCollection                                m_elementCollection;
      typedef std::map<Identifier, ExtendedAlignableTransform *>    AlignableTransformMap;
      std::vector< AlignableTransformMap >                          m_higherAlignableTransforms;
      std::vector< ExtendedAlignableTransform *>                    m_alignableTransforms; 
      const VetoNuID*                                               m_idHelper;
      
      /** This variable switches the how the local alignment corrections are applied
          If true they will be calcualted on top  of all of other corrections but in the default reference frame
          If false they will be calcualted  on top  of all of other corrections but in the globally aligned reference frame    
      */
      bool                                                          m_isLogical;      
      
      
    };

} // namespace ScintDD

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h" 
CLASS_DEF(ScintDD::VetoNuDetectorManager, 238247911, 1) 
#endif

#endif // SCINTREADOUTGEOMETRY_VETONIDETECTORMANAGER_H
