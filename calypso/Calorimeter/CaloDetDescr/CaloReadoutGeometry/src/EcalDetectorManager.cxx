/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloReadoutGeometry/EcalDetectorManager.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "FaserCaloIdentifier/EcalID.h"
#include "CaloReadoutGeometry/CaloDetectorElementCollection.h"
#include "CaloReadoutGeometry/CaloDetectorElement.h"
#include "CaloReadoutGeometry/ExtendedAlignableTransform.h"
#include "CaloReadoutGeometry/CaloDetectorDesign.h"
#include "StoreGate/StoreGateSvc.h"

#include <iostream>

namespace CaloDD {

  const int FIRST_HIGHER_LEVEL = 1;

  EcalDetectorManager::EcalDetectorManager( StoreGateSvc* detStore )
    : CaloDetectorManager(detStore, "Ecal"),
      m_idHelper(0),
      m_isLogical(false) // Change to true to change the definition of local module corrections
  {
    //  
    // Initialized the Identifier helper.
    //
    StatusCode sc = detStore->retrieve(m_idHelper, "EcalID");  
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve Ecal id helper");
    }
    // Initialize the collections.
    if (m_idHelper) {
      m_elementCollection.resize(m_idHelper->module_hash_max());
      m_alignableTransforms.resize(m_idHelper->module_hash_max());
      // m_moduleAlignableTransforms.resize(m_idHelper->module_hash_max());
    } 
  }



  EcalDetectorManager::~EcalDetectorManager()
  {
    // Clean up
    for (size_t i=0; i < m_volume.size(); i++) {
      m_volume[i]->unref();
    }

    for (size_t j=0; j < m_higherAlignableTransforms.size(); j++){
      AlignableTransformMap::iterator iterMap;  
      for (iterMap = m_higherAlignableTransforms[j].begin(); 
           iterMap != m_higherAlignableTransforms[j].end();
           ++iterMap) {
        delete iterMap->second;
      }
    }

    for (size_t k=0; k < m_alignableTransforms.size(); k++){
      delete m_alignableTransforms[k];
    }

    // for (size_t l=0; l < m_moduleAlignableTransforms.size(); l++){
    //   delete m_moduleAlignableTransforms[l];
    // }
  }

  unsigned int EcalDetectorManager::getNumTreeTops() const
  {
    return m_volume.size(); 
  }

  PVConstLink EcalDetectorManager::getTreeTop(unsigned int i) const
  {
    return m_volume[i];
  }

  void EcalDetectorManager::addTreeTop(PVLink vol){
    vol->ref();
    m_volume.push_back(vol);
  }


  CaloDetectorElement* EcalDetectorManager::getDetectorElement(const Identifier & id) const
  {  
    // NB the id helpers implementation for getting a hash is not optimal.
    // Essentially does a binary search.
    // Make sure it is a wafer Id
    Identifier waferId =  m_idHelper->module_id(id);
    IdentifierHash idHash = m_idHelper->module_hash(waferId);
    if (idHash.is_valid()) {
      return m_elementCollection[idHash];
    } else {
      return 0;
    }
  }

  CaloDetectorElement* EcalDetectorManager::getDetectorElement(const IdentifierHash & idHash) const
  {
    return m_elementCollection[idHash];
  }

  CaloDetectorElement* EcalDetectorManager::getDetectorElement(int row, int module) const
  {
    return getDetectorElement(m_idHelper->module_id(row, module));
  }

  const CaloDetectorElementCollection* EcalDetectorManager::getDetectorElementCollection() const
  { 
    return &m_elementCollection;
  }

  CaloDetectorElementCollection::const_iterator EcalDetectorManager::getDetectorElementBegin() const
  {
    return m_elementCollection.begin();
  }

  CaloDetectorElementCollection::const_iterator EcalDetectorManager::getDetectorElementEnd() const
  {
    return m_elementCollection.end();
  }


  void EcalDetectorManager::addDetectorElement(CaloDetectorElement * element)
  {
    IdentifierHash idHash = element->identifyHash();
    if (idHash >=  m_elementCollection.size())
      throw std::runtime_error("EcalDetectorManager: Error adding detector element.");
    m_elementCollection[idHash] = element;
  }

  void EcalDetectorManager::initNeighbours()
  {
    CaloDetectorElementCollection::iterator iter;

    // Loop over all elements and set the neighbours
    for (iter = m_elementCollection.begin(); iter != m_elementCollection.end(); ++iter){

      CaloDetectorElement * element = *iter;
      if (element) {

        IdentifierHash idHash = element->identifyHash();
        IdentifierHash idHashOther;

        int result;
        // If no neighbour, result != 0 in which case we leave neighbour as null
        result = m_idHelper->get_next_in_eta(idHash, idHashOther);
        if (result==0) element->setNextInEta(m_elementCollection[idHashOther]);

        result = m_idHelper->get_prev_in_eta(idHash, idHashOther);
        if (result==0) element->setPrevInEta(m_elementCollection[idHashOther]);

        result = m_idHelper->get_next_in_phi(idHash, idHashOther);
        if (result==0) element->setNextInPhi(m_elementCollection[idHashOther]);

        result = m_idHelper->get_prev_in_phi(idHash, idHashOther);
        if (result==0) element->setPrevInPhi(m_elementCollection[idHashOther]);
      }
    }
  }

  const EcalID* EcalDetectorManager::getIdHelper() const
  {
    return m_idHelper;
  }


  bool EcalDetectorManager::setAlignableTransformDelta(int level, 
                                                       const Identifier & id, 
                                                       const Amg::Transform3D & delta,
                                                       FrameType frame,
                                                       GeoVAlignmentStore* alignStore) const
  {

    if (level == 0) { // 0 - At the element level

      // We retrieve it via a hashId.
      IdentifierHash idHash = m_idHelper->module_hash(id);
      if (!idHash.is_valid()) return false;

      if (frame == CaloDD::global) { // global shift
        // Its a global transform
        return setAlignableTransformGlobalDelta(m_alignableTransforms[idHash], delta, alignStore);

      } else if (frame == CaloDD::local) { // local shift

        CaloDetectorElement * element =  m_elementCollection[idHash];
        if (!element) return false;


        // Its a local transform
        //See header file for definition of m_isLogical          
        if( m_isLogical ){
          //Ensure cache is up to date and use the alignment corrected local to global transform
          element->setCache();
          return setAlignableTransformLocalDelta(m_alignableTransforms[idHash], element->transform(), delta, alignStore);
        } else 
          //Use default local to global transform
          return setAlignableTransformLocalDelta(m_alignableTransforms[idHash], element->defTransform(), delta, alignStore);

      } else {   
        // other not supported
        ATH_MSG_WARNING("Frames other than global or local are not supported.");
        return false;
      }
    } else { // higher level
      if (frame != CaloDD::global) {
        ATH_MSG_WARNING("Non global shift at higher levels is not supported.");
        return false;
      }

      int index = level - FIRST_HIGHER_LEVEL; // level 0 is treated separately.
      if (index  >=  static_cast<int>(m_higherAlignableTransforms.size())) return false;

      // We retrieve it from a map. 
      AlignableTransformMap::const_iterator iter;    
      iter = m_higherAlignableTransforms[index].find(id);
      if (iter == m_higherAlignableTransforms[index].end()) return false;      

      // Its a global transform
      return setAlignableTransformGlobalDelta(iter->second, delta, alignStore);
    }
  }

  void EcalDetectorManager::addAlignableTransform (int level, 
                                                   const Identifier & id, 
                                                   GeoAlignableTransform *transform,
                                                   const GeoVPhysVol * child)
  {
    if (m_idHelper) {

      const GeoVFullPhysVol * childFPV = dynamic_cast<const GeoVFullPhysVol *>(child);
      if (!childFPV) { 
        ATH_MSG_ERROR("Child of alignable transform is not a full physical volume");
      } else {
        addAlignableTransform (level, id, transform, childFPV);
      }
    }
  }

  void EcalDetectorManager::addAlignableTransform (int level, 
                                                   const Identifier & id, 
                                                   GeoAlignableTransform *transform,
                                                   const GeoVFullPhysVol * child)
  { 
    if (m_idHelper) {
      if (level == 0) { 
        // Element
        IdentifierHash idHash = m_idHelper->module_hash(id);
        if (idHash.is_valid()) {
          m_alignableTransforms[idHash]= new ExtendedAlignableTransform(transform, child);
        } 
      } else {
        // Higher levels are saved in a map. NB level=0 is treated above.   
        int index = level - FIRST_HIGHER_LEVEL; // level 0 is treated separately.
        if (index >= static_cast<int>(m_higherAlignableTransforms.size())) m_higherAlignableTransforms.resize(index+1); 
        m_higherAlignableTransforms[index][id] = new ExtendedAlignableTransform(transform, child);
      }  
    }
  }

  bool
  EcalDetectorManager::identifierBelongs(const Identifier & id) const
  {
    return getIdHelper()->is_ecal(id);
  }


  const CaloDetectorDesign* EcalDetectorManager::getEcalDesign() const
  {
    return dynamic_cast<const CaloDetectorDesign *>(getDesign());
  }

  // New global alignment folders
  bool EcalDetectorManager::processGlobalAlignment(const std::string & key, int level, FrameType frame,
                                                   const CondAttrListCollection* obj, GeoVAlignmentStore* alignStore) const
  {
    ATH_MSG_INFO("Processing new global alignment containers with key " << key << " in the " << frame << " frame at level ");

    const CondAttrListCollection* atrlistcol=obj;
    if(atrlistcol==nullptr and m_detStore->retrieve(atrlistcol,key)!=StatusCode::SUCCESS) {
      ATH_MSG_INFO("Cannot find new global align Container for key "
                   << key << " - no new global alignment ");
      return false;
    }

    bool alignmentChange = false;
    Identifier ident=Identifier();

    // loop over objects in collection
    for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {
      const coral::AttributeList& atrlist=citr->second;
      // SCT manager, therefore ignore all that is not a SCT Identifier
      //   if (atrlist["det"].data<int>()!=2) continue;
      ATH_MSG_FATAL("Using invalid alignment data for Ecal detector (correct treatment not yet implemented");
      ident = getIdHelper()->module_id(atrlist["row"].data<int>(),
                                      atrlist["module"].data<int>()); // The last is the module side which is at this ident-level always the 0-side

      // construct new transform
      // Order of rotations is defined as around z, then y, then x.
      Amg::Translation3D  newtranslation(atrlist["Tx"].data<float>(),atrlist["Ty"].data<float>(),atrlist["Tz"].data<float>());
      Amg::Transform3D newtrans = newtranslation * Amg::RotationMatrix3D::Identity();
      newtrans *= Amg::AngleAxis3D(atrlist["Rz"].data<float>()*CLHEP::mrad, Amg::Vector3D(0.,0.,1.));
      newtrans *= Amg::AngleAxis3D(atrlist["Ry"].data<float>()*CLHEP::mrad, Amg::Vector3D(0.,1.,0.));
      newtrans *= Amg::AngleAxis3D(atrlist["Rx"].data<float>()*CLHEP::mrad, Amg::Vector3D(1.,0.,0.));

      ATH_MSG_DEBUG("New global DB -- channel: " << citr->first
                    << " ,det: "    << atrlist["det"].data<int>()
                    << " ,bec: "    << atrlist["bec"].data<int>()
                    << " ,layer: "  << atrlist["layer"].data<int>()
                    << " ,ring: "   << atrlist["ring"].data<int>()
                    << " ,sector: " << atrlist["sector"].data<int>()
                    << " ,Tx: "     << atrlist["Tx"].data<float>()
                    << " ,Ty: "     << atrlist["Ty"].data<float>()
                    << " ,Tz: "     << atrlist["Tz"].data<float>()
                    << " ,Rx: "     << atrlist["Rx"].data<float>()
                    << " ,Ry: "     << atrlist["Ry"].data<float>()
                    << " ,Rz: "     << atrlist["Rz"].data<float>());

      // Set the new transform; Will replace existing one with updated transform
      bool status = setAlignableTransformDelta(level,
                                               ident,
                                               newtrans,
                                               frame,
                                               alignStore);

      if (!status) {
        ATH_MSG_DEBUG("Cannot set AlignableTransform for identifier."
                      << getIdHelper()->show_to_string(ident)
                      << " at level " << level << " for new global DB ");
      }

      alignmentChange = (alignmentChange || status);
    }
    return alignmentChange;
  }

bool EcalDetectorManager::processSpecialAlignment(
    const std::string &, CaloDD::AlignFolderType) const {
  return false;
}

bool EcalDetectorManager::processSpecialAlignment(const std::string& /*key*/,
                                                  const CondAttrListCollection* /*obj*/,
                                                  GeoVAlignmentStore* /*alignStore*/) const {
  return false;

}

} // namespace CaloDD
