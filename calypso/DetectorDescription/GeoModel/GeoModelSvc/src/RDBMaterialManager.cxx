/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RDBMaterialManager.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/Units.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

bool RDBMaterialManager::s_specialMaterials = false;

//---------------------------Help find elements in the list-----------------------//
class NameEquals {                                                                //
public:                                                                           //
  NameEquals(const std::string & name):m_name(name){}                              //
  bool operator() (const GeoElement *e) const {return m_name==e->getName();}       //
private:                                                                          //
  std::string m_name;                                                              //
};                                                                                //
//--------------------------------------------------------------------------------//

//---------------------------Help find elements in the list-----------------------//
class NumberEquals {                                                              //
public:                                                                           //
  NumberEquals(unsigned int number):m_number(number){}                             //
  bool operator() (const GeoElement *e) const {return m_number==e->getZ();}        //
private:                                                                          //
  unsigned int m_number;                                                           //
};                                                                                //
//--------------------------------------------------------------------------------//

int CheckElement(std::string &name)
{
  if(name.find("::",0) == std::string::npos) {
    return 1;
  }
  else {
    return 0;	
  }
}

int printElement ( GeoElement* &p_element)
{
  std::string name = p_element->getName();
  std::string symbol = p_element->getSymbol();
  double a = p_element->getA();
  double z = p_element->getZ();
	
  std::cout << " ***** CheckElement(): Print the Element:  " << name << std::endl; 
  std::cout << " ***** The Element: name,		symbol, 	A, 	Z " << std::endl; 
  std::cout << " *****             "<<name <<"		"<<symbol <<"		"<< a * (Gaudi::Units::mole / GeoModelKernelUnits::gram) <<"	"<< z <<"	"  << std::endl;
	
  return 1;
}

int printElement ( const GeoElement* &p_element)
{
  std::string name = p_element->getName();
  std::string symbol = p_element->getSymbol();
  double a = p_element->getA();
  double z = p_element->getZ();
	
  std::cout << " ***** PrintElement(): Print the Element:  " << name << std::endl; 
  std::cout << " ***** The Element: name,		symbol, 	A, 	Z " << std::endl; 
  std::cout << " *****             "<<name <<"		"<<symbol <<"		"<< a * (Gaudi::Units::mole / GeoModelKernelUnits::gram) <<"	"<< z <<"	"  << std::endl;
	
  return 1;
}

int printMaterial ( GeoMaterial* &p_material)
{
  std::string name = p_material->getName();
  double density = p_material->getDensity() * (Gaudi::Units::cm3 / GeoModelKernelUnits::gram);

  std::cout << " ***** PrintMaterial(): Print the Material:  " << name << std::endl; 
  std::cout << " ***** The Material: name,	density	" << std::endl; 
  std::cout << " *****              "<< name <<"		"<<density <<"		" << std::endl; 	
	
  return 1;
}

int printFullMaterial ( GeoMaterial* &p_material)
{
  std::string name = p_material->getName();
  double density = p_material->getDensity() * (Gaudi::Units::cm3 / GeoModelKernelUnits::gram);
	
  std::cout << " ***** PrintFullMaterial(): Print the Material:  " << name << std::endl; 
  std::cout << " ***** The Material: name, 	density" << std::endl; 
  std::cout << " *****              "<< name <<" 	 "<<density <<"  " << std::endl; 
	
  p_material->lock();
  int element_number = p_material->getNumElements();	
		
 			
  if ( element_number  == 0){
    std::cout << " ***** No Elements now in this printMaterial( ) " << std::endl;	
    return 1;
  }
  else {
    element_number = p_material->getNumElements();	
	
    for(int i =0; i< element_number;i ++)
      {
	const GeoElement* tmp_element = p_material->getElement(i);
	double element_fraction = p_material->getFraction(i);
		
	std::cout<<" ***** ***** Number:  " << i << " Fraction:  " << element_fraction<< std::endl;
	printElement( tmp_element); 
      }	
  }
  return 1;
}
	
	

RDBMaterialManager::RDBMaterialManager(ISvcLocator* pSvcLocator)
{
  if(!readMaterialsFromDB(pSvcLocator).isSuccess()) {
    throw std::runtime_error("RDBMaterialManager failed to read Geometry DB");
  }
}

StatusCode RDBMaterialManager::readMaterialsFromDB(ISvcLocator* pSvcLocator)
{
  IGeoModelSvc*  iGeoModel;		
  IRDBAccessSvc* iAccessSvc;
  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 		

  ATH_CHECK(pSvcLocator->service("GeoModelSvc",iGeoModel));
  ATH_CHECK(pSvcLocator->service("RDBAccessSvc",iAccessSvc));

  // --- Standard materials, elements
  DecodeFaserVersionKey keyFaser(iGeoModel, "FASER");
  m_elements = iAccessSvc->getRecordsetPtr("Elements",keyFaser.tag(),keyFaser.node(), "FASERDD");
  if(m_elements->size()==0) {
    if(log.level()<=MSG::WARNING)
      log << MSG::WARNING << " Getting Elements with default tag" <<endmsg;
    m_elements = iAccessSvc->getRecordsetPtr("Elements","Materials-00","Materials", "FASERDD");
  }
  m_stdmatcomponents = iAccessSvc->getRecordsetPtr("StdMatComponents",keyFaser.tag(),keyFaser.node(), "FASERDD");
  if(m_stdmatcomponents->size()==0)	{
    if(log.level()<=MSG::WARNING)
      log << MSG::WARNING << " Getting StdMatComponents with default tag" <<endmsg;
    m_stdmatcomponents = iAccessSvc->getRecordsetPtr("StdMatComponents","Materials-00","Materials", "FASERDD");
  }
  m_stdmaterials = iAccessSvc->getRecordsetPtr("StdMaterials",keyFaser.tag(),keyFaser.node(), "FASERDD");
  if(m_stdmaterials->size()==0) {
    if(log.level()<=MSG::WARNING)
      log << MSG::WARNING << " Getting StdMaterials with default tag" <<endmsg;
    m_stdmaterials = iAccessSvc->getRecordsetPtr("StdMaterials","Materials-00","Materials", "FASERDD");
  }
  
  // --- Neutrino materials
  DecodeFaserVersionKey keyNeutrino(iGeoModel, "Neutrino");
  m_neutrinomatcomponents = iAccessSvc->getRecordsetPtr("NeutrinoMatComponents",keyNeutrino.tag(),keyNeutrino.node(),"FASERDD");
  if(m_neutrinomatcomponents->size()==0) {
    if(log.level()<=MSG::WARNING)
      log << MSG::WARNING << " Getting NeutrinoMatComponents with default tag" <<endmsg;
    m_neutrinomatcomponents = iAccessSvc->getRecordsetPtr("NeutrinoMatComponents","NeutrinoMatComponents-00", "", "FASERDD");
  }
  m_neutrinomaterials = iAccessSvc->getRecordsetPtr("NeutrinoMaterials",keyNeutrino.tag(),keyNeutrino.node(), "FASERDD");
  if(m_neutrinomaterials->size()==0) {
    if(log.level()<=MSG::WARNING)
      log << MSG::WARNING << " Getting NeutrinoMaterials with default tag" <<endmsg;
    m_neutrinomaterials = iAccessSvc->getRecordsetPtr("NeutrinoMaterials","NeutrinoMaterials-00", "", "FASERDD");
  }
  
  // --- Scintillator materials
  DecodeFaserVersionKey keyScintillator(iGeoModel, "Scintillator");
  m_scintmatcomponents = iAccessSvc->getRecordsetPtr("ScintMatComponents",keyScintillator.tag(),keyScintillator.node(),"FASERDD");
  if(m_scintmatcomponents->size()==0) {
    if(log.level()<=MSG::WARNING)
      log << MSG::WARNING << " Getting ScintMatComponents with default tag" <<endmsg;
    m_scintmatcomponents = iAccessSvc->getRecordsetPtr("ScintMatComponents","ScintMatComponents-00", "", "FASERDD");
  }
  m_scintmaterials = iAccessSvc->getRecordsetPtr("ScintMaterials",keyScintillator.tag(),keyScintillator.node(), "FASERDD");
  if(m_scintmaterials->size()==0) {
    if(log.level()<=MSG::WARNING)
      log << MSG::WARNING << " Getting ScintMaterials with default tag" <<endmsg;
    m_scintmaterials = iAccessSvc->getRecordsetPtr("ScintMaterials","ScintMaterials-00", "", "FASERDD");
  }
  
  // --- SCT materials
  DecodeFaserVersionKey keySCT(iGeoModel, "SCT");
  m_sctmatcomponents = iAccessSvc->getRecordsetPtr("SCTMatComponents",keySCT.tag(),keySCT.node(),"FASERDD");
  if(m_sctmatcomponents->size()==0) {
    if(log.level()<=MSG::WARNING)
      log << MSG::WARNING << " Getting SCTMatComponents with default tag" <<endmsg;
    m_sctmatcomponents = iAccessSvc->getRecordsetPtr("SCTMatComponents","SCTMatComponents-00", "", "FASERDD");
  }
  m_sctmaterials = iAccessSvc->getRecordsetPtr("SCTMaterials",keySCT.tag(),keySCT.node(), "FASERDD");
  if(m_sctmaterials->size()==0) {
    if(log.level()<=MSG::WARNING)
      log << MSG::WARNING << " Getting SCTMaterials with default tag" <<endmsg;
    m_sctmaterials = iAccessSvc->getRecordsetPtr("SCTMaterials","SCTMaterials-00", "", "FASERDD");
  }
  // TrackerMatComponents and TrackerMaterials tables in DB are empty
  // --- Tracker materials
  // DecodeFaserVersionKey keyTracker(iGeoModel, "Tracker");
  // m_trackermatcomponents = iAccessSvc->getRecordsetPtr("TrackerMatComponents",keyTracker.tag(),keyTracker.node(),"FASERDD");
  // if(m_trackermatcomponents->size()==0) {
  //   if(log.level()<=MSG::WARNING)
  //     log << MSG::WARNING << " Getting TrackerMatComponents with default tag" <<endmsg;
  //   m_trackermatcomponents = iAccessSvc->getRecordsetPtr("TrackerMatComponents","TrackerMatComponents-00", "", "FASERDD");
  // }
  // m_trackermaterials = iAccessSvc->getRecordsetPtr("TrackerMaterials",keyTracker.tag(),keyTracker.node(), "FASERDD");
  // if(m_trackermaterials->size()==0) {
  //   if(log.level()<=MSG::WARNING)
  //     log << MSG::WARNING << " Getting TrackerMaterials with default tag" <<endmsg;
  //   m_trackermaterials = iAccessSvc->getRecordsetPtr("TrackerMaterials","TrackerMaterials-00", "", "FASERDD");
  // }

  // CaloMatComponents and CaloMaterials tables are also empty
  // // --- Calorimeter materials
  // DecodeFaserVersionKey keyCalorimeter(iGeoModel, "Calorimeter");
  // m_calomatcomponents = iAccessSvc->getRecordsetPtr("CaloMatComponents",keyCalorimeter.tag(),keyCalorimeter.node(),"FASERDD");
  // if(m_calomatcomponents->size()==0) {
  //   if(log.level()<=MSG::WARNING)
  //     log << MSG::WARNING << " Getting CaloMatComponents with default tag" <<endmsg;
  //   m_calomatcomponents = iAccessSvc->getRecordsetPtr("CaloMatComponents","CaloMatComponents-00", "", "FASERDD");
  // }
  // m_calomaterials = iAccessSvc->getRecordsetPtr("CaloMaterials",keyCalorimeter.tag(),keyCalorimeter.node(), "FASERDD");
  // if(m_calomaterials->size()==0) {
  //   if(log.level()<=MSG::WARNING)
  //     log << MSG::WARNING << " Getting CaloMaterials with default tag" <<endmsg;
  //   m_calomaterials = iAccessSvc->getRecordsetPtr("CaloMaterials","CaloMaterials-00", "", "FASERDD");
  // }
     
  return StatusCode::SUCCESS;
}

// Destructor:
RDBMaterialManager::~RDBMaterialManager() {
	
  // Unreference the materials:
  std::map< std::string, GeoMaterial * >::iterator m, begin = m_materialMap.begin(),end = m_materialMap.end();
  for (m=begin;m!=end;m++) (*m).second->unref();	
	 	
  // Unreference the elements:
  for (size_t i=0;i<m_elementVector.size();i++)   	m_elementVector[i]->unref();
	
}

GeoMaterial* RDBMaterialManager::searchMaterialMap(const std::string & name)  
{
  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
	
  std::map< std::string, GeoMaterial * >::const_iterator m   = m_materialMap.find(std::string(name));
  std::map< std::string, GeoMaterial * >::const_iterator end = m_materialMap.end();
  	
  if (m!=end) {
    if(log.level()==MSG::VERBOSE)
      log << MSG::VERBOSE << " ***** in searchMaterialMap(" << name << "): cache search success "  << endmsg;	
    return (*m).second;
  }
  else {
    if(log.level()==MSG::VERBOSE)
      log << MSG::VERBOSE << " ***** in searchMaterialMap(" << name << "): cache search fail "  << endmsg;	
    return NULL;
  }
}

GeoMaterial* RDBMaterialManager::searchMaterialMap(const std::string & name) const
{
  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
	
  std::map< std::string, GeoMaterial * >::const_iterator m   = m_materialMap.find(std::string(name));
  std::map< std::string, GeoMaterial * >::const_iterator end = m_materialMap.end();
  if (m!=end) {
    if(log.level()==MSG::VERBOSE)
      log << MSG::VERBOSE << " ***** in searchMaterialMap(" << name << "): cache search success "  << endmsg;	
    return (*m).second;
  }
  else {
    if(log.level()==MSG::VERBOSE)
      log << MSG::VERBOSE << " ***** in searchMaterialMap(" << name << "): cache search fail "  << endmsg;	
    return NULL;
  }
}


GeoElement *RDBMaterialManager::searchElementVector(const std::string & name)  
{ 
  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
	
  NameEquals matchByName(name);
  std::vector<GeoElement *>::const_iterator e=std::find_if(m_elementVector.begin(), m_elementVector.end(),matchByName);
  	
  if (e!=m_elementVector.end()) {	
    if(log.level()==MSG::VERBOSE)
      log << MSG::VERBOSE << " ***** in searchElementVector(" << name << ") cache search success "  << endmsg;	
    return *e;
  }
  else {
    if(log.level()==MSG::VERBOSE)
      log << MSG::VERBOSE << " ***** in searchElementVector(" << name << ") cache search fail "  << endmsg;	
    return NULL;
  }
}


GeoElement *RDBMaterialManager::searchElementVector(const std::string & name)  const
{ 
  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
	
  NameEquals matchByName(name);
  std::vector<GeoElement *>::const_iterator e=std::find_if(m_elementVector.begin(), m_elementVector.end(),matchByName);
  	
  if (e!=m_elementVector.end()) {	
    if(log.level()==MSG::VERBOSE)    		
      log << MSG::VERBOSE << " ***** in searchElementVector(" << name << ") cache search success "  << endmsg;	
    return *e;
  }
  else {
    if(log.level()==MSG::VERBOSE)
      log << MSG::VERBOSE << " ***** in searchElementVector(" << name << ") cache search fail "  << endmsg;	
    return NULL;
  }
}


GeoElement *RDBMaterialManager::searchElementVector(const unsigned int atomicNumber)  
{ 
  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
	
  NumberEquals matchByNumber(atomicNumber);
  std::vector<GeoElement *>::const_iterator e=std::find_if(m_elementVector.begin(), m_elementVector.end(), matchByNumber);
  	
  if (e!=m_elementVector.end()) {
    if(log.level()==MSG::VERBOSE)  		
      log << MSG::VERBOSE << " ***** in searchElementVector(atomicNumber = " << atomicNumber << ") cache search succes "  << endmsg;
    return *e;
  }
  else {
    if(log.level()==MSG::VERBOSE)
      log << MSG::VERBOSE << " ***** in searchElementVector(atomicNumber = " << atomicNumber << ") cache search success "  << endmsg;
    return NULL;
  }
}

GeoElement *RDBMaterialManager::searchElementVector(const unsigned int atomicNumber) const
{ 
  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
	
  NumberEquals matchByNumber(atomicNumber);
  std::vector<GeoElement *>::const_iterator e=std::find_if(m_elementVector.begin(), m_elementVector.end(), matchByNumber);
  	
  if (e!=m_elementVector.end()) {
    if(log.level()==MSG::VERBOSE)  		
      log << MSG::VERBOSE << " ***** in searchElementVector(atomicNumber = " << atomicNumber << ") cache search success "  << endmsg;
    return *e;
  }
  else {
    if(log.level()==MSG::VERBOSE)
      log << MSG::VERBOSE << " ***** in searchElementVector(atomicNumber = " << atomicNumber << ") cache search success "  << endmsg;
    return NULL;
  }
}

GeoMaterial* RDBMaterialManager::getMaterial(const std::string & name) {
	
  unsigned int  ind, com_ind;
	
  std::string material_name;
  std::string tmp_name;
  long 	    material_id = 0;
  double      material_density = 0;
	
	
  std::string component_name;
  double      component_fraction;
  int 	    component_id;
		
  std::string detector;
  std::string tmp_det;
  std::string data_id;
	
	
  std::string matcomponents_table;

  if(!s_specialMaterials)
  {
    buildSpecialMaterials();
    s_specialMaterials = true;
  }
	
  GeoMaterial* pmaterial;
	
  GeoMaterial* p_com_material;
  GeoElement*  p_com_element;
	
  IRDBRecordset_ptr tmp_materials;
  IRDBRecordset_ptr tmp_matcomponents;
	
  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
  if(log.level()<=MSG::DEBUG) 
    log << MSG::DEBUG  << " ***** getMaterial( ): "  << name << endmsg;	
 

  pmaterial = NULL;
  pmaterial = searchMaterialMap( name);
  if (pmaterial!= NULL) 
      return pmaterial;
		
  if(name.find("std",0) == 0)
    {
      detector = "std";
      tmp_materials = m_stdmaterials;
      tmp_matcomponents = m_stdmatcomponents;
      data_id = "STDMATERIALS_DATA_ID";
    }
  else if(name.find("neutrino",0) == 0)
    {
      detector = "neutrino";
      tmp_materials = m_neutrinomaterials;
      tmp_matcomponents = m_neutrinomatcomponents;
      data_id = "NEUTRINOMATERIALS_DATA_ID";
    }
  else if(name.find("scint",0) == 0)
    {
      detector = "scint";
      tmp_materials = m_scintmaterials;
      tmp_matcomponents = m_scintmatcomponents;
      data_id = "SCINTMATERIALS_DATA_ID";
    }
  // else if(name.find("tracker",0) == 0)
  //   {
  //     detector = "tracker";
  //     tmp_materials = m_trackermaterials;
  //     tmp_matcomponents = m_trackermatcomponents;
  //     data_id = "TRACKERMATERIALS_DATA_ID";
  //   }
    else if(name.find("sct",0) == 0)
    {
      detector = "sct";
      tmp_materials = m_sctmaterials;
      tmp_matcomponents = m_sctmatcomponents;
      data_id = "SCTMATERIALS_DATA_ID";
    }

  // else if(name.find("calo",0) == 0)
  //   {
  //     detector = "calo";
  //     tmp_materials = m_calomaterials;
  //     tmp_matcomponents = m_calomatcomponents;
  //     data_id = "CALOMATERIALS_DATA_ID";
  //   }
  else {return 0 ;}
	
  for( ind = 0; ind < tmp_materials->size(); ind++)
    {
      const IRDBRecord* rec = (*tmp_materials)[ind];
      tmp_name = detector+"::"+rec->getString("NAME");
		
      if( name == tmp_name){

	material_name  = rec->getString("NAME");
	material_id = rec->getLong(data_id);
	material_density = rec->getDouble("DENSITY");
        		
	if(log.level()<=MSG::DEBUG)
	  log << MSG::DEBUG  << " ***** Material: name id density: "  << material_name <<" " << material_id <<" "<< material_density << endmsg;	
	break;
      }
    }
		
  if (ind == tmp_materials->size()) 
      return NULL;

  pmaterial = new GeoMaterial( material_name,material_density * (GeoModelKernelUnits::gram / Gaudi::Units::cm3));


  bool firstComponent = true;
  bool hasSubMaterial = false;
  bool calculateFraction = false;
  double totalFraction = 0.;
  std::vector < GeoElement* > elementComponents;
  std::vector <double>        elementFractions;

  for(  com_ind = 0; com_ind <tmp_matcomponents->size(); com_ind++)
    {
      const IRDBRecord* com_rec = (*tmp_matcomponents)[com_ind];
		
      component_id = com_rec->getLong("MATERIAL_ID");
      if( component_id == material_id)
	{
	  component_name = com_rec->getString("COMPNAME");
	  component_fraction = com_rec->getDouble("FRACTION");
			
	  if(firstComponent)
	  {
	    firstComponent = false;
	    if(component_fraction>=1.)
	      calculateFraction = true;
	  }

	  if( CheckElement( component_name) == 1)
	    {
	      p_com_element = getElement(component_name);

	      if(calculateFraction)
	      {
          totalFraction += component_fraction*p_com_element->getA();
          elementComponents.push_back(p_com_element);
          elementFractions.push_back(component_fraction);
	      }
	      else
          pmaterial->add( p_com_element, component_fraction);
										
	    }
	  else{
	    hasSubMaterial = true;
	    p_com_material = getMaterial(component_name);

	    pmaterial->add(p_com_material, component_fraction);
			
	  }		
	}
    }    

  if(calculateFraction && hasSubMaterial && elementComponents.size()>0)
    std::cerr << material_name << " description should be changed. Please indicate the exact fraction for elements\n";

  if(calculateFraction && !elementComponents.empty()) {
    const double inv_totalFraction = 1. / totalFraction;
    for(unsigned i=0; i<elementComponents.size(); i++)
      pmaterial->add(elementComponents[i],elementFractions[i]*elementComponents[i]->getA() * inv_totalFraction);
  }
	
  // a table to keep the memory allocation, and easy for delete 
  addMaterial(detector,pmaterial);
	
  /*	{
	printFullMaterial ( pmaterial);
	printAll( std:: cout);
	}
  */	
  return pmaterial;
}

const GeoMaterial*  RDBMaterialManager:: getMaterial(const std::string &name) const{

  unsigned int  ind, com_ind;
	
  std::string material_name;
  std::string tmp_name;
  long 	    material_id = 0;
  double      material_density = 0;
	
	
  std::string component_name;
  double      component_fraction;
  int 	    component_id;
		
  std::string detector;
  std::string tmp_det;
  std::string data_id;
	
	
  std::string matcomponents_table;

  if(!s_specialMaterials)
  {
    buildSpecialMaterials();
    s_specialMaterials = true;
  }

	
  GeoMaterial* pmaterial;
	
  GeoMaterial* p_com_material;
  GeoElement*  p_com_element;
	
  IRDBRecordset_ptr tmp_materials;
  IRDBRecordset_ptr tmp_matcomponents;
	
  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
  if(log.level()<=MSG::DEBUG) 
    log << MSG::DEBUG  << " ***** getMaterial( ): "  << name << endmsg;	

  pmaterial = NULL;
  pmaterial = searchMaterialMap( name);
  if (pmaterial!= NULL) 
      return pmaterial;
		
  if(name.find("std",0) == 0)
    {
      detector = "std";
      tmp_materials = m_stdmaterials;
      tmp_matcomponents = m_stdmatcomponents;
      data_id = "STDMATERIALS_DATA_ID";
    }
  else if(name.find("neutrino",0) == 0)
    {
      detector = "neutrino";
      tmp_materials = m_neutrinomaterials;
      tmp_matcomponents = m_neutrinomatcomponents;
      data_id = "NEUTRINOMATERIALS_DATA_ID";
    }
  else if(name.find("scint",0) == 0)
    {
      detector = "scint";
      tmp_materials = m_scintmaterials;
      tmp_matcomponents = m_scintmatcomponents;
      data_id = "SCINTMATERIALS_DATA_ID";
    }
  // else if(name.find("tracker",0) == 0)
  //   {
  //     detector = "tracker";
  //     tmp_materials = m_trackermaterials;
  //     tmp_matcomponents = m_trackermatcomponents;
  //     data_id = "TRACKERMATERIALS_DATA_ID";
  //   }
  else if(name.find("sct",0) == 0)
    {
      detector = "sct";
      tmp_materials = m_sctmaterials;
      tmp_matcomponents = m_sctmatcomponents;
      data_id = "SCTMATERIALS_DATA_ID";
    }
  // else if(name.find("calo",0) == 0)
  //   {
  //     detector = "calo";
  //     tmp_materials = m_calomaterials;
  //     tmp_matcomponents = m_calomatcomponents;
  //     data_id = "CALOMATERIALS_DATA_ID";
  //   }
  else {return 0 ;}
	
  for( ind = 0; ind < tmp_materials->size(); ind++)
    {
      const IRDBRecord* rec = (*tmp_materials)[ind];
      tmp_name = detector+"::"+rec->getString("NAME");
		
      if( name == tmp_name){
        material_name  = rec->getString("NAME");
        material_id = rec->getLong(data_id);
        material_density = rec->getDouble("DENSITY");
        		
        if(log.level()<=MSG::DEBUG)
          log << MSG::DEBUG  << " ***** Material: name id density: "  << material_name <<" " << material_id <<" "<< material_density << endmsg;	
        break;
      }
    }
		
  if (ind == tmp_materials->size()) 
      return NULL;

  pmaterial = new GeoMaterial( material_name,material_density * (GeoModelKernelUnits::gram / Gaudi::Units::cm3));

  bool firstComponent = true;
  bool hasSubMaterial = false;
  bool calculateFraction = false;
  double totalFraction = 0.;
  std::vector < GeoElement* > elementComponents;
  std::vector <double>        elementFractions;

  for(  com_ind = 0; com_ind <tmp_matcomponents->size(); com_ind++)
    {
      const IRDBRecord* com_rec = (*tmp_matcomponents)[com_ind];
		
      component_id = com_rec->getLong("MATERIAL_ID");
      if( component_id == material_id)
	{
	  component_name = com_rec->getString("COMPNAME");
	  component_fraction = com_rec->getDouble("FRACTION");
			
	  if(firstComponent)
	  {
	    firstComponent = false;
	    if(component_fraction>=1.)
	      calculateFraction = true;
	  }

	  if( CheckElement( component_name) == 1)
	    {
	      p_com_element = (GeoElement *)(getElement(component_name));

	      if(calculateFraction)
	      {
          totalFraction += component_fraction*p_com_element->getA();
          elementComponents.push_back(p_com_element);
          elementFractions.push_back(component_fraction);
	      }
	      else
          pmaterial->add( p_com_element, component_fraction);
										
	    }
	  else{
	    hasSubMaterial = true;
	    p_com_material = (GeoMaterial  *)getMaterial(component_name);

	    pmaterial->add(p_com_material, component_fraction);
			
	  }		
	}
    }    

  if(calculateFraction && hasSubMaterial && elementComponents.size()>0)
    std::cerr << material_name << " description should be changed. Please indicate the exact fraction for elements\n";

  if(calculateFraction && !elementComponents.empty()) {
    double inv_totalFraction = 1. / totalFraction;
    for(unsigned i=0; i<elementComponents.size(); i++)
      pmaterial->add(elementComponents[i],elementFractions[i]*elementComponents[i]->getA() * inv_totalFraction);
  }

  // a table to keep the memory allocation, and easy for delete 
  addMaterial(detector,pmaterial);
	
  return pmaterial;
}


const GeoElement *RDBMaterialManager::getElement(const std::string & name) const{
	
  unsigned int ind;

  std::string element_name;
  std::string element_symbol;
  std::string tmp_name;
	
  double      element_a;
  double      element_z;
	
  GeoElement *pelement;

  pelement = NULL;
  pelement = searchElementVector( name);
  if (pelement != NULL) 
      return pelement;

  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
  if(log.level()==MSG::VERBOSE)
    log << MSG::VERBOSE << " ***** getElement(): " << name  <<endmsg;

  for(ind = 0; ind < m_elements->size(); ind++)
    {
      const IRDBRecord* rec = (*m_elements)[ind];
		
      tmp_name = rec->getString("NAME");
	
      if( name == tmp_name)
      { 
        element_name   = rec->getString("NAME");
        element_symbol = rec->getString("SYMBOL");
        element_a = rec->getDouble("A");
        element_z = rec->getDouble("Z");
                      
        pelement = new GeoElement( element_name , element_symbol  ,element_z , element_a *(GeoModelKernelUnits::gram/Gaudi::Units::mole));

        // a table to keep the memory allocation, and easy for delete 
        pelement->ref();
        m_elementVector.push_back( pelement);
          
        break;
      }
    }
  if (ind == m_elements->size()) 		return NULL;
	
  return pelement;

}


const GeoElement *RDBMaterialManager::getElement(unsigned int atomicNumber) const {

  unsigned int ind;

  std::string element_name;
  std::string element_symbol;
	
  double      element_a;
  double      element_z;
	
  GeoElement* pelement(0);

  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
  if(log.level()==MSG::VERBOSE)
    log << MSG::VERBOSE << " ***** const getElement(atomicNumber) const : " << atomicNumber <<endmsg;	

  for(ind = 0; ind < m_elements->size(); ind++)
    {
      const IRDBRecord* rec = (*m_elements)[ind];
		
      if(atomicNumber == rec->getDouble("A"))
      { 
        element_name   = rec->getString("NAME");
        element_symbol = rec->getString("SYMBOL");
        element_a = rec->getDouble("A");
        element_z = rec->getDouble("Z");
                      
        pelement = new GeoElement( element_name , element_symbol  ,element_z , element_a *(GeoModelKernelUnits::gram/Gaudi::Units::mole));

        // a table to keep the memory allocation, and easy for delete 
        pelement->ref();
        m_elementVector.push_back( pelement);
          
        break;
      }
    }
  if (ind == m_elements->size()) 	return NULL;
	
  return pelement;
}


GeoElement *RDBMaterialManager::getElement(const std::string & name)  {

  unsigned int ind;

  std::string element_name;
  std::string element_symbol;
  std::string tmp_name;
	
  double      element_a;
  double      element_z;
	
  GeoElement *pelement;

  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
  if(log.level()==MSG::VERBOSE)
    log << MSG::VERBOSE << " ***** getElement(): " << element_name  <<endmsg;
	
  pelement = NULL;
  pelement = searchElementVector( name);
  if (pelement != NULL) 
      return pelement;

  for(ind = 0; ind < m_elements->size(); ind++)
    {
      const IRDBRecord* rec = (*m_elements)[ind];
		
      tmp_name = rec->getString("NAME");
	
      if( name == tmp_name)
      { 
        element_name   = rec->getString("NAME");
        element_symbol = rec->getString("SYMBOL");
        element_a = rec->getDouble("A");
        element_z = rec->getDouble("Z");
                      
        pelement = new GeoElement( element_name , element_symbol  ,element_z , element_a *(GeoModelKernelUnits::gram/Gaudi::Units::mole));

        // a table to keep the memory allocation, and easy for delete 
        pelement->ref();
        m_elementVector.push_back( pelement);
        break;
          
      }
    }
  if (ind == m_elements->size()) 		return NULL;
	
  return pelement;
}


GeoElement *RDBMaterialManager::getElement(unsigned int atomicNumber) {
  
  unsigned int ind;

  std::string element_name;
  std::string element_symbol;
	
  double      element_a;
  double      element_z;
	
  GeoElement* pelement(0);

  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
  if(log.level()==MSG::VERBOSE)
    log << MSG::VERBOSE << " ***** getElement(atomicNumber): " << element_name  <<endmsg;	
	
  for(ind = 0; ind < m_elements->size(); ind++)
    {
      const IRDBRecord* rec = (*m_elements)[ind];
		
      if(atomicNumber == rec->getDouble("A"))
      { 
        element_name   = rec->getString("NAME");
        element_symbol = rec->getString("SYMBOL");
        element_a = rec->getDouble("A");
        element_z = rec->getDouble("Z");
                      
        pelement = new GeoElement( element_name , element_symbol  ,element_z , element_a *(GeoModelKernelUnits::gram/Gaudi::Units::mole));

        // a table to keep the memory allocation, and easy for delete 
        pelement->ref();
        m_elementVector.push_back( pelement);
          
        break;
      }
    }
  if (ind == m_elements->size()) 	return NULL;
	
  return pelement;
}

void RDBMaterialManager::addMaterial(const std::string & space, GeoMaterial *material) {

  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
  std::string key = space + std::string("::")+ std::string(material->getName());
  if(log.level()==MSG::VERBOSE)
    log << MSG::VERBOSE << " ***** RDBMaterialManager::addMaterial(" << key << ") "<<endmsg;
	
  // Check whether we already have materials with the same space::name defined
  if(m_materialMap.find(key)!=m_materialMap.end())
    log << MSG::WARNING << " Attempt to redefine material " << key << "!. The existing instance is kept. Please choose another name for new material" << endmsg;
  else {
    material->lock();             
    material->ref();
    m_materialMap[key]=material;
  }
}

void RDBMaterialManager::addMaterial(const std::string & space, GeoMaterial *material) const {
	
  MsgStream log(Athena::getMessageSvc(), "GeoModelSvc::RDBMaterialManager"); 
  std::string key = space + std::string("::")+ std::string(material->getName());
  if(log.level()==MSG::VERBOSE)
    log << MSG::VERBOSE << " ***** RDBMaterialManager::addMaterial(" << key << ") "<<endmsg;
	
  // Check whether we already have materials with the same space::name defined
  if(m_materialMap.find(key)!=m_materialMap.end())
    log << MSG::WARNING << " Attempt to redefine material " << key << "!. The existing instance is kept. Please choose another name for new material" << endmsg;
  else {
    material->lock();             
    material->ref();
    m_materialMap[key]=material;
  }
}

StoredMaterialManager::MaterialMapIterator RDBMaterialManager::begin() const
{
  return m_materialMap.begin();
}

StoredMaterialManager::MaterialMapIterator RDBMaterialManager::end() const
{
  return m_materialMap.end();
}

size_t RDBMaterialManager::size()
{
  return m_materialMap.size();
}

std::ostream &  RDBMaterialManager::printAll(std::ostream & o) const 
{
  o << "============Material Manager Element List========================" << std::endl;
  std::vector<GeoElement *>::const_iterator e;
  for (e=m_elementVector.begin();e!= m_elementVector.end();e++) 
    {
      o << (*e)->getSymbol() << '\t' << (*e)->getZ() <<  '\t' << (*e)->getA() * (Gaudi::Units::mole / GeoModelKernelUnits::gram) << '\t' << (*e)->getName() << std::endl;
    }
  std::map<std::string, GeoMaterial *>::const_iterator m;
  	
  for (m=m_materialMap.begin();m!=m_materialMap.end();m++) 
    {
      o << "Material: " << (*m).first <<  " Density " << (*m).second->getDensity() * (Gaudi::Units::cm3 / GeoModelKernelUnits::gram)  << std::endl;
      for (size_t i = 0; i< (*m).second->getNumElements();i++) 
	{
	  o <<" ***** ***** "<< int ((*m).second->getFraction(i)*100) << "% \t"  << (*m).second->getElement(i)->getName() << std::endl;
	}
    }
  	  	
  return o;
}

void RDBMaterialManager::buildSpecialMaterials()
{
  // Create special materials
  GeoElement* ethElement = new GeoElement("Ether","ET",500.0,0.0);
  ethElement->ref();
  m_elementVector.push_back(ethElement);
  GeoMaterial* ether = new GeoMaterial("Ether",0.0);	
  ether->add(ethElement,1.);
  addMaterial("special",ether);
  // "Alternative" assembly material
  GeoMaterial* hu = new GeoMaterial("HyperUranium",0.0);	
  hu->add(ethElement,1.);
  addMaterial("special",hu);
}

void RDBMaterialManager::buildSpecialMaterials() const
{
  // Create special materials
  GeoElement* ethElement = new GeoElement("Ether","ET",500.0,0.0);
  ethElement->ref();
  m_elementVector.push_back(ethElement);  
  GeoMaterial* ether = new GeoMaterial("Ether",0.0);	
  ether->add(ethElement,1.);
  addMaterial("special",ether);
  // "Alternative" assembly material
  GeoMaterial* hu = new GeoMaterial("HyperUranium",0.0);	
  hu->add(ethElement,1.);
  addMaterial("special",hu);
}
