/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EcalDetectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CALOREADOUTGEOMETRY_ECALDETECTORMANAGER_H
#define CALOREADOUTGEOMETRY_ECALDETECTORMANAGER_H

#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoVPhysVol.h"

#include "CaloReadoutGeometry/CaloDetectorManager.h"
#include "CaloReadoutGeometry/CaloDetectorElementCollection.h"
#include "CaloReadoutGeometry/CaloDD_Defs.h"

#include "FaserCaloIdentifier/EcalID.h"
  
class StoreGateSvc;
class Identifier; 
class IdentifierHash;
class GeoAlignableTransform;
class GeoVFullPhysVol;
class GeoVPhysVol;
class GeoVAlignmentStore;
class CondAttrListCollection;

namespace CaloDD {

  class CaloDetectorElement;
  class ExtendedAlignableTransform;
  class EcalDetectorDesign;

  /** @class EcalDetectorManager
  
      Dedicated detector manager extending the functionality of the CaloDetectorManager
      with dedicated Ecal information, access.
      
      @author: Grant Gorfine
      - modified and maintained by Nick Styles & Andreas Salzburger
      - modified for FASER by D. Casper
      */
      
  class EcalDetectorManager : public CaloDetectorManager  {

    public:
    
      // Constructor
      EcalDetectorManager( StoreGateSvc* detStore );
     
      // Destructor
      virtual ~EcalDetectorManager();
     
      /** Access Raw Geometry */
      virtual unsigned int getNumTreeTops()           const override;
      virtual PVConstLink  getTreeTop(unsigned int i) const override;
      /** Add tree top */
      void addTreeTop(PVLink); 
    
    
      //
      // Access Readout Elements
      //
    
      /** access to individual elements via Identifier */
      virtual CaloDetectorElement * getDetectorElement(const Identifier &id) const override;

      /** access to individual elements via IdentifierHash */
      virtual CaloDetectorElement * getDetectorElement(const IdentifierHash &idHash) const override;
      
      /** access to individual elements via module numbering schema */
      CaloDetectorElement * getDetectorElement(int row, int module) const;
    
      /** access to whole collectiom via iterators */
      virtual const CaloDetectorElementCollection * getDetectorElementCollection() const override;
      virtual CaloDetectorElementCollection::const_iterator getDetectorElementBegin() const override;
      virtual CaloDetectorElementCollection::const_iterator getDetectorElementEnd() const override;
    
      /** Add elememts during construction */
      virtual void addDetectorElement(CaloDetectorElement * element) override;
    
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
    
      /** Access to module design, casts to EcalDetectorDesign */
      const CaloDetectorDesign * getEcalDesign() const;

      /** Process new global DB folders for L1 and L2 **/
      virtual
      bool processGlobalAlignment(const std::string &, int level, FrameType frame,
                                  const CondAttrListCollection* obj,
                                  GeoVAlignmentStore* alignStore) const override;

      // comply with CaloDetectorManager interface
      bool processSpecialAlignment(const std::string & key,
                                   CaloDD::AlignFolderType alignfolder) const override;

      bool processSpecialAlignment(const std::string& key,
                                   const CondAttrListCollection* obj=nullptr,
                                   GeoVAlignmentStore* alignStore=nullptr) const override;


    private:
      /** implements the main alignment update for delta transforms in different frames,
          it translates into the LocalDelta or GlobalDelta function of CaloDetectorManager
      */
      virtual bool setAlignableTransformDelta(int level, 
                                              const Identifier & id, 
                                              const Amg::Transform3D & delta,
                                              FrameType frame,
                                              GeoVAlignmentStore* alignStore) const override;
      
      /** Prevent copy and assignment */
      const EcalDetectorManager & operator=(const EcalDetectorManager &right);
      EcalDetectorManager(const EcalDetectorManager &right); 
    
      virtual const EcalID* getIdHelper() const override;
       
      // Private member data
      std::vector<PVLink>                                           m_volume;  
      CaloDetectorElementCollection                                 m_elementCollection;
      typedef std::map<Identifier, ExtendedAlignableTransform *>    AlignableTransformMap;
      std::vector< AlignableTransformMap >                          m_higherAlignableTransforms;
      std::vector< ExtendedAlignableTransform *>                    m_alignableTransforms; 
      const EcalID*                                                 m_idHelper;
      
      /** This variable switches the how the local alignment corrections are applied
          If true they will be calcualted on top  of all of other corrections but in the default reference frame
          If false they will be calcualted  on top  of all of other corrections but in the globally aligned reference frame    
      */
      bool                                                          m_isLogical;      
      
      
    };

} // namespace CaloDD

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h" 
CLASS_DEF(CaloDD::EcalDetectorManager, 155753500, 1) 
#endif

#endif // CALOREADOUTGEOMETRY_ECALDETECTORMANAGER_H
