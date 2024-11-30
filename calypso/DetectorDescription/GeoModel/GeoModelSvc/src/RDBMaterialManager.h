/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _RDBMATERIALMANAGER_H_
#define _RDBMATERIALMANAGER_H_
//---------------------------------------------------------//
//                                                         //
// class RDBMaterialManager  This is a material manager   //
// which gets its material from RDB.                       //
//                                                         //
// Joe Boudreau March 2003                                 //
//                                                         //
// Heavily modified for FASER                              //
//                                                         //
//---------------------------------------------------------//
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include <string>
#include <map>
#include <vector>
#include <iosfwd>

class GeoMaterial;
class ISvcLocator;


class RDBMaterialManager: public StoredMaterialManager {

 public:

  // Constructor:
  RDBMaterialManager(ISvcLocator* pSvcLocator);
  
  // Destructor:
  virtual ~RDBMaterialManager();

  // Query the material:
  virtual       GeoMaterial *getMaterial(const std::string &name) ;
  virtual const GeoMaterial *getMaterial(const std::string &name) const;
 

  // Query the elements:
  virtual const GeoElement *getElement(const std::string & name) const;
  virtual       GeoElement *getElement(const std::string & name);

  // Query the elements (by atomic number):
  virtual const GeoElement *getElement(unsigned int atomicNumber) const;
  virtual       GeoElement *getElement(unsigned int atomicNumber);

  // Add new material
  virtual void addMaterial(const std::string& space, GeoMaterial* material);
  virtual void addMaterial(const std::string& space, GeoMaterial* material) const;

  virtual StoredMaterialManager::MaterialMapIterator begin() const;
  virtual StoredMaterialManager::MaterialMapIterator end() const;

  // Number of materials in the manager
  virtual size_t size();

  virtual std::ostream & printAll(std::ostream & o=std::cout) const;

 private:
  static bool s_specialMaterials;

  StatusCode readMaterialsFromDB(ISvcLocator* pSvcLocator);

  void buildSpecialMaterials();
  void buildSpecialMaterials() const;

  GeoElement *searchElementVector (const std::string & name);
  GeoElement *searchElementVector (const std::string & name) const;
  
  GeoElement *searchElementVector (const unsigned int atomicNumber);
  GeoElement *searchElementVector (const unsigned int atomicNumber) const;
  
  GeoMaterial *searchMaterialMap (const std::string & name);
  GeoMaterial *searchMaterialMap (const std::string & name) const;
  
  // For DataBase connection and Query
  IRDBRecordset_ptr m_elements;
  
  IRDBRecordset_ptr m_stdmaterials;
  IRDBRecordset_ptr m_stdmatcomponents;
  IRDBRecordset_ptr m_neutrinomaterials;
  IRDBRecordset_ptr m_neutrinomatcomponents;
  IRDBRecordset_ptr m_scintmaterials;
  IRDBRecordset_ptr m_scintmatcomponents;
  // IRDBRecordset_ptr m_trackermaterials;
  // IRDBRecordset_ptr m_trackermatcomponents;
  IRDBRecordset_ptr m_sctmaterials;
  IRDBRecordset_ptr m_sctmatcomponents;
  // IRDBRecordset_ptr m_calomaterials;
  // IRDBRecordset_ptr m_calomatcomponents;

  mutable std::vector < GeoElement *>        m_elementVector;
  mutable StoredMaterialManager::MaterialMap m_materialMap;
};


#endif
