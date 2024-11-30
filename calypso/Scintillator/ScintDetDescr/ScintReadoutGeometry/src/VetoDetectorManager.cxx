/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ScintReadoutGeometry/VetoDetectorManager.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "ScintIdentifier/VetoID.h"
#include "ScintReadoutGeometry/ScintDetectorElementCollection.h"
#include "ScintReadoutGeometry/ScintDetectorElement.h"
#include "ScintReadoutGeometry/ExtendedAlignableTransform.h"
#include "ScintReadoutGeometry/ScintDetectorDesign.h"
#include "StoreGate/StoreGateSvc.h"

#include <iostream>

namespace ScintDD {

  const int FIRST_HIGHER_LEVEL = 1;

  VetoDetectorManager::VetoDetectorManager( StoreGateSvc* detStore )
    : ScintDetectorManager(detStore, "Veto"),
      m_idHelper(0),
      m_isLogical(false) // Change to true to change the definition of local module corrections
  {
    //  
    // Initialized the Identifier helper.
    //
    StatusCode sc = detStore->retrieve(m_idHelper, "VetoID");  
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve Veto id helper");
    }
    // Initialize the collections.
    if (m_idHelper) {
      m_elementCollection.resize(m_idHelper->plate_hash_max());
      m_alignableTransforms.resize(m_idHelper->plate_hash_max());
      // m_moduleAlignableTransforms.resize(m_idHelper->plate_hash_max());
    } 
  }



  VetoDetectorManager::~VetoDetectorManager()
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

  unsigned int VetoDetectorManager::getNumTreeTops() const
  {
    return m_volume.size(); 
  }

  PVConstLink VetoDetectorManager::getTreeTop(unsigned int i) const
  {
    return m_volume[i];
  }

  void VetoDetectorManager::addTreeTop(PVLink vol){
    vol->ref();
    m_volume.push_back(vol);
  }


  ScintDetectorElement* VetoDetectorManager::getDetectorElement(const Identifier & id) const
  {  
    // NB the id helpers implementation for getting a hash is not optimal.
    // Essentially does a binary search.
    // Make sure it is a wafer Id
    Identifier waferId =  m_idHelper->plate_id(id);
    IdentifierHash idHash = m_idHelper->plate_hash(waferId);
    if (idHash.is_valid()) {
      return m_elementCollection[idHash];
    } else {
      return 0;
    }
  }

  ScintDetectorElement* VetoDetectorManager::getDetectorElement(const IdentifierHash & idHash) const
  {
    return m_elementCollection[idHash];
  }

  ScintDetectorElement* VetoDetectorManager::getDetectorElement(int station, int plate) const
  {
    return getDetectorElement(m_idHelper->plate_id(station, plate));
  }

  const ScintDetectorElementCollection* VetoDetectorManager::getDetectorElementCollection() const
  { 
    return &m_elementCollection;
  }

  ScintDetectorElementCollection::const_iterator VetoDetectorManager::getDetectorElementBegin() const
  {
    return m_elementCollection.begin();
  }

  ScintDetectorElementCollection::const_iterator VetoDetectorManager::getDetectorElementEnd() const
  {
    return m_elementCollection.end();
  }


  void VetoDetectorManager::addDetectorElement(ScintDetectorElement * element)
  {
    IdentifierHash idHash = element->identifyHash();
    if (idHash >=  m_elementCollection.size())
      throw std::runtime_error("VetoDetectorManager: Error adding detector element.");
    m_elementCollection[idHash] = element;
  }

  void VetoDetectorManager::initNeighbours()
  {
    ScintDetectorElementCollection::iterator iter;

    // Loop over all elements and set the neighbours
    for (iter = m_elementCollection.begin(); iter != m_elementCollection.end(); ++iter){

      ScintDetectorElement * element = *iter;
      if (element) {

        IdentifierHash idHash = element->identifyHash();
        IdentifierHash idHashOther;

        int result;
        // If no neighbour, result != 0 in which case we leave neighbour as null
        result = m_idHelper->get_next_in_z(idHash, idHashOther);
        if (result==0) element->setNextInZ(m_elementCollection[idHashOther]);

        result = m_idHelper->get_prev_in_z(idHash, idHashOther);
        if (result==0) element->setPrevInZ(m_elementCollection[idHashOther]);
      }
    }
  }

  const VetoID* VetoDetectorManager::getIdHelper() const
  {
    return m_idHelper;
  }


  bool VetoDetectorManager::setAlignableTransformDelta(int level, 
                                                       const Identifier & id, 
                                                       const Amg::Transform3D & delta,
                                                       FrameType frame,
                                                       GeoVAlignmentStore* alignStore) const
  {

    if (level == 0) { // 0 - At the element level

      // We retrieve it via a hashId.
      IdentifierHash idHash = m_idHelper->plate_hash(id);
      if (!idHash.is_valid()) return false;

      if (frame == ScintDD::global) { // global shift
        // Its a global transform
        return setAlignableTransformGlobalDelta(m_alignableTransforms[idHash], delta, alignStore);

      } else if (frame == ScintDD::local) { // local shift

        ScintDetectorElement * element =  m_elementCollection[idHash];
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
      if (frame != ScintDD::global) {
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

  void VetoDetectorManager::addAlignableTransform (int level, 
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

  void VetoDetectorManager::addAlignableTransform (int level, 
                                                   const Identifier & id, 
                                                   GeoAlignableTransform *transform,
                                                   const GeoVFullPhysVol * child)
  { 
    if (m_idHelper) {
      if (level == 0) { 
        // Element
        IdentifierHash idHash = m_idHelper->plate_hash(id);
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
  VetoDetectorManager::identifierBelongs(const Identifier & id) const
  {
    return getIdHelper()->is_veto(id);
  }


  const ScintDetectorDesign* VetoDetectorManager::getVetoDesign() const
  {
    return dynamic_cast<const ScintDetectorDesign *>(getDesign());
  }

  // New global alignment folders
  bool VetoDetectorManager::processGlobalAlignment(const std::string & key, int level, FrameType frame,
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
      ATH_MSG_FATAL("Using invalid alignment data for Veto detector (correct treatment not yet implemented");
      ident = getIdHelper()->plate_id(atrlist["station"].data<int>(),
                                      atrlist["plate"].data<int>()); // The last is the module side which is at this ident-level always the 0-side

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

bool VetoDetectorManager::processSpecialAlignment(
    const std::string &, ScintDD::AlignFolderType) const {
  return false;
}

bool VetoDetectorManager::processSpecialAlignment(const std::string& /*key*/,
                                                  const CondAttrListCollection* /*obj*/,
                                                  GeoVAlignmentStore* /*alignStore*/) const {
  return false;

}

} // namespace ScintDD
