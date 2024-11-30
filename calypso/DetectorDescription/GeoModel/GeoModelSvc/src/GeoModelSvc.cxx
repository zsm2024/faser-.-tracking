/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h" 
#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelFaserUtilities/GeoModelExperiment.h"
#include "GeoModelDBManager/GMDBManager.h"
#include "GeoModelWrite/WriteGeoModel.h"

#include "GeoModelSvc.h"
#include "RDBMaterialManager.h"
#include "GeoDbTagSvc.h"
#include "GeoModelInterfaces/IGeoAlignTool.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelKernel/GeoPerfUtils.h"
#include <fstream>
#include <filesystem>

#include "AthenaKernel/ClassID_traits.h"
#include "SGTools/DataProxy.h"


GeoModelSvc::GeoModelSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc),
    m_detectorTools(this), 
    m_pSvcLocator(svc),
    m_toolSvc("ToolSvc",name),
    m_detStore("DetectorStore",name),
    m_tagInfoMgr("TagInfoMgr",name),
    m_geoDbTagSvc("GeoDbTagSvc",name),
    m_FaserVersion("AUTO"),
    m_printMaterials(false),
    m_callBackON(true),
    m_ignoreTagDifference(false),
    m_useTagInfo(true),
    m_statisticsToFile(false),
    m_geoExportFile(""),
    m_supportedGeometry(0),
    m_ignoreTagSupport(false)
{
  declareProperty( "DetectorTools",               m_detectorTools);
  declareProperty( "PrintMaterials",              m_printMaterials);
  declareProperty( "FaserVersion",                m_FaserVersion);
  declareProperty( "NeutrinoVersionOverride",     m_NeutrinoVersionOverride);
  declareProperty( "EmulsionVersionOverride",     m_EmulsionVersionOverride);
  declareProperty( "ScintVersionOverride",        m_ScintVersionOverride);
  declareProperty( "VetoVersionOverride",         m_VetoVersionOverride);
  declareProperty( "VetoNuVersionOverride",       m_VetoNuVersionOverride);
  declareProperty( "TriggerVersionOverride",      m_TriggerVersionOverride);
  declareProperty( "PreshowerVersionOverride",    m_PreshowerVersionOverride);
  declareProperty( "TrackerVersionOverride",      m_TrackerVersionOverride);
  declareProperty( "SCTVersionOverride",          m_SCTVersionOverride);
  declareProperty( "DipoleVersionOverride",       m_DipoleVersionOverride);
  declareProperty( "CaloVersionOverride",         m_CaloVersionOverride);
  declareProperty( "EcalVersionOverride",         m_EcalVersionOverride);
  // declareProperty( "MagFieldVersionOverride",     m_MagFieldVersionOverride);
  declareProperty( "TrenchVersionOverride",       m_TrenchVersionOverride);
  declareProperty( "AlignCallbacks",              m_callBackON);
  declareProperty( "IgnoreTagDifference",         m_ignoreTagDifference);
  declareProperty( "UseTagInfo",                  m_useTagInfo);
  declareProperty( "StatisticsToFile",            m_statisticsToFile);
  declareProperty( "SupportedGeometry",           m_supportedGeometry);
  declareProperty( "IgnoreTagSupport",            m_ignoreTagSupport);
  declareProperty( "GeoExportFile",               m_geoExportFile);
}

GeoModelSvc::~GeoModelSvc()
{
}

StatusCode GeoModelSvc::initialize()
{
  if(m_supportedGeometry==0) {
    ATH_MSG_FATAL("The Supported Geometry flag was not set in Job Options! Exiting ...");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_detStore.retrieve() );
  ATH_CHECK( m_toolSvc.retrieve() );

  // --- Sebastien
  // clients (detector tools) are assuming the DetDescrCnvSvc has been
  // correctly initialized.
  // We ensure this is indeed correct by manually initialize it so there is
  // no more service-ordering problem triggered by jobO mix-up
  ATH_MSG_VERBOSE("Initializing DetDescrCnvSvc");
  ServiceHandle<IConversionSvc> conversionSvc("DetDescrCnvSvc", this->name());
  ATH_CHECK( conversionSvc.retrieve() );
  // --- Sebastien

   // Working around Gaudi Issue https://gitlab.cern.ch/gaudi/Gaudi/issues/82
  Service* convSvc=dynamic_cast<Service*>(conversionSvc.get());
  if (convSvc->FSMState() < Gaudi::StateMachine::INITIALIZED) {
    ATH_MSG_INFO("Explicitly initializing DetDescrCnvSvc");
    ATH_CHECK( convSvc->sysInitialize() );
  } 
  
  ATH_CHECK( m_detectorTools.retrieve() );

  ToolHandleArray< IGeoModelTool >::iterator itPriv = m_detectorTools.begin(),
    itPrivEnd = m_detectorTools.end();

  if(m_useTagInfo) {
    ATH_CHECK( m_tagInfoMgr.retrieve() );
  }

  // build regular geometry
  ATH_CHECK( geoInit() );

  if(!m_callBackON) {
    // _________________ Align functions NOT registered as callbacks _____________

    // Apply possible alignments to detectors.
    // Dummy parameters for the callback
    int par1 = 0;
    std::list<std::string> par2;
    for(; itPriv!=itPrivEnd; ++itPriv) {
      if((*itPriv)->align(par1,par2) != StatusCode::SUCCESS) {
        ATH_MSG_DEBUG("align() failed for the tool " << (*itPriv)->name());
      }
    }
    
    // Fill in the contents of TagInfo
    if(m_useTagInfo) {
      ATH_CHECK(fillTagInfo());
    }
  }
  else {
    // Register align() functions for all Tools 
    for (; itPriv!=itPrivEnd; ++itPriv) {
      IGeoModelTool* theTool = &(**itPriv);
	
      if(StatusCode::SUCCESS != theTool->registerCallback()) {
        ATH_MSG_DEBUG("IGeoModelTool::align() was not registerred on CondDB object for the tool " << theTool->name());
      }
    }

    // Register a callback on TagInfo in order to compare geometry configurations defined in job options
    // to the one read from the input file
    if(m_useTagInfo) {
       m_tagInfoMgr->addListener( this );
       // Fill in the contents of TagInfo
       ATH_CHECK(fillTagInfo());
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode GeoModelSvc::finalize()
{
  m_tagInfoMgr->removeListener(this);
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode GeoModelSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if(IGeoModelSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = (IGeoModelSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  } 
  else {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
}

StatusCode GeoModelSvc::geoInit()
{
  ATH_MSG_DEBUG("** Building geometry configuration: ");
  ATH_MSG_DEBUG("* FASER     tag: " << m_FaserVersion);
  ATH_MSG_DEBUG("* Neutrino  tag: " << m_NeutrinoVersionOverride);
  ATH_MSG_DEBUG("* Emulsion  tag: " << m_EmulsionVersionOverride);
  ATH_MSG_DEBUG("* Scint     tag: " << m_ScintVersionOverride);
  ATH_MSG_DEBUG("* Veto      tag: " << m_VetoVersionOverride);
  ATH_MSG_DEBUG("* VetoNu    tag: " << m_VetoNuVersionOverride);
  ATH_MSG_DEBUG("* Trigger   tag: " << m_TriggerVersionOverride);
  ATH_MSG_DEBUG("* Preshower tag: " << m_PreshowerVersionOverride);
  ATH_MSG_DEBUG("* Tracker   tag: " << m_TrackerVersionOverride);
  ATH_MSG_DEBUG("* SCT       tag: " << m_SCTVersionOverride);
  ATH_MSG_DEBUG("* Dipole    tag: " << m_DipoleVersionOverride);
  ATH_MSG_DEBUG("* Calo      tag: " << m_CaloVersionOverride);
  ATH_MSG_DEBUG("* Ecal      tag: " << m_EcalVersionOverride);
  // ATH_MSG_DEBUG("* MagField  tag: " << m_MagFieldVersionOverride);
  ATH_MSG_DEBUG("* Trench    tag: " << m_TrenchVersionOverride);
  
  // GetRDBAccessSvc and open connection to DB
  ServiceHandle<IRDBAccessSvc> rdbAccess("RDBAccessSvc",name());
  ATH_CHECK( rdbAccess.retrieve() );

  if(!rdbAccess->connect("FASERDD")) {
    ATH_MSG_ERROR("Unable to connect to the Geometry DB");
    return StatusCode::FAILURE;
  }

  // Check the existence of FASER tag in the database
  if(rdbAccess->getChildTag("FASER",m_FaserVersion,"FASER", "FASERDD")=="") {
    ATH_MSG_FATAL(" *** *** Wrong FASER layout: " << m_FaserVersion << " *** ***");
    ATH_MSG_FATAL(" Either FASER geometry tag has been misspelled, or the DB Release does not contain the geometry specified.");
    ATH_MSG_FATAL(" In latter case please update DB Release version");
    return StatusCode::FAILURE;
  }

  if(!m_ignoreTagSupport) {
    RDBTagDetails faserTagDetails;
    rdbAccess->getTagDetails(faserTagDetails, m_FaserVersion, "FASERDD");
    const coral::AttributeSpecification& supportedSpec = faserTagDetails["SUPPORTED"].specification();
    if(supportedSpec.type()==typeid(bool)) {
      if(!faserTagDetails["SUPPORTED"].data<bool>()) {
	      ATH_MSG_FATAL(" *** *** FASER layout " << m_FaserVersion << " is OBSOLETE and can NOT be supported any more! *** ***");
	      return StatusCode::FAILURE;
      }
    }
    else if(supportedSpec.type()==typeid(int)) {
      if(faserTagDetails["SUPPORTED"].data<int>()<m_supportedGeometry) {
	      ATH_MSG_FATAL(" *** *** FASER layout " << m_FaserVersion 
		      << " is OBSOLETE in rel " << m_supportedGeometry 
		      << " and can NOT be supported any more! *** ***");
	      return StatusCode::FAILURE;
      }
    }
  }

  // Create a material manager
  StoredMaterialManager *theMaterialManager{nullptr};
  try{
    theMaterialManager = new RDBMaterialManager(m_pSvcLocator);
  }
  catch(std::runtime_error& e) {
    ATH_MSG_FATAL(e.what());
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_detStore->record(theMaterialManager,"MATERIALS") );
  
  // Setup the GeoDbTagSvc
  ATH_CHECK( m_geoDbTagSvc.retrieve() );
  
  GeoDbTagSvc* dbTagSvc = dynamic_cast<GeoDbTagSvc*>(m_geoDbTagSvc.operator->());
  if(dbTagSvc==nullptr) {
    ATH_MSG_FATAL("Unable to dyn-cast the IGeoDbTagSvc pointer to GeoDbTagSvc");
    return StatusCode::FAILURE;
  }

  dbTagSvc->setFaserVersion(m_FaserVersion);
  dbTagSvc->setNeutrinoVersionOverride(m_NeutrinoVersionOverride);
  dbTagSvc->setEmulsionVersionOverride(m_EmulsionVersionOverride);
  dbTagSvc->setScintVersionOverride(m_ScintVersionOverride);
  dbTagSvc->setVetoVersionOverride(m_VetoVersionOverride);
  dbTagSvc->setVetoNuVersionOverride(m_VetoNuVersionOverride);
  dbTagSvc->setTriggerVersionOverride(m_TriggerVersionOverride);
  dbTagSvc->setPreshowerVersionOverride(m_PreshowerVersionOverride);
  dbTagSvc->setTrackerVersionOverride(m_TrackerVersionOverride);
  dbTagSvc->setSCTVersionOverride(m_SCTVersionOverride);
  dbTagSvc->setDipoleVersionOverride(m_DipoleVersionOverride);
  dbTagSvc->setCaloVersionOverride(m_CaloVersionOverride);
  dbTagSvc->setEcalVersionOverride(m_EcalVersionOverride);
  // dbTagSvc->setMagFieldVersionOverride(m_MagFieldVersionOverride);
  dbTagSvc->setTrenchVersionOverride(m_TrenchVersionOverride);

  if(dbTagSvc->setupTags().isFailure()) {
    ATH_MSG_FATAL("Failed to setup subsystem tags");
    return StatusCode::FAILURE;
  }

  // Build the world node from which everything else will be suspended
  const GeoMaterial* air = theMaterialManager->getMaterial("std::Air");  
  const GeoBox* worldBox = new GeoBox(6000*Gaudi::Units::cm,2500*Gaudi::Units::cm, 12000*Gaudi::Units::cm);
  const GeoLogVol* worldLog = new GeoLogVol("WorldLog", worldBox, air);
  GeoPhysVol *worldPhys=new GeoPhysVol(worldLog);
  
  // Create FaserExperiment and register it within the transient detector store
  GeoModelExperiment* theExperiment = new GeoModelExperiment(worldPhys);
  ATH_CHECK( m_detStore->record(theExperiment,"FASER") );

  int mem,cpu;
  std::unique_ptr<std::ofstream> geoModelStats;
  if(m_statisticsToFile) {
    geoModelStats = std::make_unique<std::ofstream>("GeoModelStatistics");
    *geoModelStats << "Detector Configuration flag = " << m_FaserVersion << std::endl; 
  }
    
  // Loop over all tools
  ToolHandleArray< IGeoModelTool >::iterator itPriv = m_detectorTools.begin(),
    itPrivEnd = m_detectorTools.end();
  
  for(; itPriv!=itPrivEnd; ++itPriv) {
    IGeoModelTool* theTool = &(**itPriv);
      
    mem = GeoPerfUtils::getMem();
    cpu = GeoPerfUtils::getCpu();
      
    if(theTool->create().isFailure()) {
      ATH_MSG_ERROR("Unable to create detector " << theTool->name());
      return StatusCode::FAILURE;
    }
      
    if(m_statisticsToFile) {
      *geoModelStats << theTool->name() << "\t SZ= " 
		     << GeoPerfUtils::getMem() - mem << "Kb \t Time = " << (GeoPerfUtils::getCpu() - cpu) * 0.01 << "S" << std::endl;
    }
    else {
      ATH_MSG_INFO(theTool->name() << "\t SZ= " 
		   << GeoPerfUtils::getMem() - mem << "Kb \t Time = " << (GeoPerfUtils::getCpu() - cpu) * 0.01 << "S");
    }
  }
    
  if(m_statisticsToFile) {
    geoModelStats->close();
  }
	
  // Close DB connection
  rdbAccess->shutdown("FASERDD");
  if(m_printMaterials)
    theMaterialManager->printAll();

  // Try to dump the geometry
  if (m_geoExportFile != "")
  {
    if (std::filesystem::exists(m_geoExportFile) && !std::filesystem::is_directory(m_geoExportFile))
    {
      ATH_MSG_INFO("Removing existing version of " << m_geoExportFile );
      std::filesystem::remove(m_geoExportFile);
    }
    if (std::filesystem::is_directory(m_geoExportFile))
    {
      ATH_MSG_WARNING("Requested export file is an existing directory.");
    } 
    else if (std::filesystem::exists(m_geoExportFile))
    {
      ATH_MSG_WARNING("File " << m_geoExportFile << " exists and could not removed for recreation.");
    }
    else
    {
      PVConstLink world(worldPhys);
      GMDBManager db(m_geoExportFile.c_str());
      if (db.checkIsDBOpen())
      {
        GeoModelIO::WriteGeoModel dump{db};
        world->exec(&dump);
        dump.saveToDB();
      }
    }
  }
  return StatusCode::SUCCESS;
}

void GeoModelSvc::tagInfoUpdated()
{
  compareTags().ignore();
}

StatusCode GeoModelSvc::compareTags()
{
  bool tagsMatch = true;  
  // Get tags from TagInfoMgr
  const ITagInfoMgr::NameTagPairVec pairs = m_tagInfoMgr->getInputTags();
  for( const auto& pair : pairs ) {
    std::string tagPairName = pair.first;
    if(tagPairName=="GeoFaser") {
      // ** Two possible cases need to be taken into account
      // ** 
      // ** 1. The retrieved FASER tag is following naming schema FASER-...-XX-YY-ZZ
      // ** where '...' can be anything, it may also containg one or more '-'.
      // ** If this is the case, then we need to check whether the job option tag
      // ** is also following the same schema and they have the same 'FASER-...-XX' part
      // **
      // ** 2. The retrieved FASER tag is not following the schema mentioned above
      // ** If this is the case, we just need to check the exact match
      std::vector<std::string> tokensTagInfo, tokensJobOpt;

      // Parse Tag Info tag
      std::string::size_type startpos = 0;
      std::string currStr = pair.second;
      for(std::string::size_type endpos=currStr.find("-"); endpos!=std::string::npos; endpos=currStr.find("-",startpos)) {
        tokensTagInfo.push_back(currStr.substr(startpos,endpos-startpos));
        startpos = endpos+1;
      }
      tokensTagInfo.push_back(currStr.substr(startpos));
      
      size_t tokensTagInfoSize = tokensTagInfo.size();
      bool tagInfoFollowsTheScheme = (tokensTagInfoSize>=5 
				      && tokensTagInfo[tokensTagInfoSize-1].size()==2
				      && tokensTagInfo[tokensTagInfoSize-2].size()==2
				      && tokensTagInfo[tokensTagInfoSize-3].size()==2);

      if(tagInfoFollowsTheScheme) {
        // Parse Job Options tag
        startpos = 0;
        currStr = m_FaserVersion;
        for(std::string::size_type endpos=currStr.find("-"); endpos!=std::string::npos; endpos=currStr.find("-",startpos)) {
          tokensJobOpt.push_back(currStr.substr(startpos,endpos-startpos));
          startpos = endpos+1;
        }
        tokensJobOpt.push_back(currStr.substr(startpos));
        
        size_t tokensJobOptSize = tokensJobOpt.size();
        bool jobOptFollowsTheScheme = (tokensJobOptSize>=5
                    && tokensJobOpt[tokensJobOptSize-1].size()==2
                    && tokensJobOpt[tokensJobOptSize-2].size()==2
                    && tokensJobOpt[tokensJobOptSize-3].size()==2);
        if(jobOptFollowsTheScheme) {
          tagsMatch = (pair.second.substr(0,currStr.size()-6)==m_FaserVersion.substr(0,m_FaserVersion.size()-6));
        }
        else {
          tagsMatch = false;
        }
      }
      else {// Check for the exact match 
        tagsMatch = m_FaserVersion == pair.second;
      }
    }
    else if(tagPairName=="GeoNeutrino")
      tagsMatch = m_NeutrinoVersionOverride == pair.second;
    else if(tagPairName=="GeoEmulsion")
      tagsMatch = m_EmulsionVersionOverride == pair.second;
    else if(tagPairName=="GeoScint")
      tagsMatch = m_ScintVersionOverride == pair.second;
    else if(tagPairName=="GeoVeto")
      tagsMatch = m_VetoVersionOverride == pair.second;
    else if(tagPairName=="GeoVetoNu")
      tagsMatch = m_VetoNuVersionOverride == pair.second;
    else if(tagPairName=="GeoTrigger")
      tagsMatch = m_TriggerVersionOverride == pair.second;
    else if(tagPairName=="GeoPreshower")
      tagsMatch = m_PreshowerVersionOverride == pair.second;
    else if(tagPairName=="GeoTracker")
      tagsMatch = m_TrackerVersionOverride == pair.second;
    else if(tagPairName=="GeoSCT")
      tagsMatch = m_SCTVersionOverride == pair.second;
    else if(tagPairName=="GeoDipole")
      tagsMatch = m_DipoleVersionOverride == pair.second;
    else if(tagPairName=="GeoCalo")
      tagsMatch = m_CaloVersionOverride == pair.second;
    else if(tagPairName=="GeoEcal")
      tagsMatch = m_EcalVersionOverride == pair.second;
    else if(tagPairName=="GeoTrench")
      tagsMatch = m_TrenchVersionOverride == pair.second;
    
    if(!tagsMatch) break;
  }

  if(!tagsMatch) {
    msg((m_ignoreTagDifference? MSG::WARNING : MSG::ERROR)) 
      << "*** *** Geometry configured through jobOptions does not match TagInfo tags! *** ***" << endmsg;
    ATH_MSG_INFO("** Job Option configuration: ");
    ATH_MSG_INFO("* FASER       tag: " << m_FaserVersion);
    ATH_MSG_INFO("* Neutrino    tag: " << m_NeutrinoVersionOverride);
    ATH_MSG_INFO("*   Emulsion  tag: " << m_EmulsionVersionOverride);
    ATH_MSG_INFO("* Scint       tag: " << m_ScintVersionOverride);
    ATH_MSG_INFO("*   Veto      tag: " << m_VetoVersionOverride);
    ATH_MSG_INFO("*   VetoNu    tag: " << m_VetoNuVersionOverride);
    ATH_MSG_INFO("*   Trigger   tag: " << m_TriggerVersionOverride);
    ATH_MSG_INFO("*   Preshower tag: " << m_PreshowerVersionOverride);
    ATH_MSG_INFO("* Tracker     tag: " << m_TrackerVersionOverride);
    ATH_MSG_INFO("*   SCT       tag: " << m_SCTVersionOverride);
    ATH_MSG_INFO("*   Dipole    tag: " << m_DipoleVersionOverride);
    ATH_MSG_INFO("* Calo        tag: " << m_CaloVersionOverride);
    ATH_MSG_INFO("*   Ecal      tag: " << m_EcalVersionOverride);
    // ATH_MSG_INFO("* MagField  tag: " << m_MagFieldVersionOverride);
    ATH_MSG_INFO("* Trench      tag: " << m_TrenchVersionOverride);
    ATH_MSG_INFO("** TAG INFO configuration: ");
    for (const auto& pair : pairs) {
      std::string tagPairName = pair.first;
      if(tagPairName=="GeoFaser")
        ATH_MSG_INFO("*FASER   tag: " << pair.second);
      else if(tagPairName=="GeoNeutrino")
        ATH_MSG_INFO("*Neutrino tag: " << pair.second);
      else if(tagPairName=="GeoEmulsion")
        ATH_MSG_INFO("*Emulsion   tag: " << pair.second);
      else if(tagPairName=="GeoScint")
        ATH_MSG_INFO("*Scint    tag: " << pair.second);
      else if(tagPairName=="GeoVeto")
        ATH_MSG_INFO("*Veto       tag: " << pair.second);
      else if(tagPairName=="GeoVetoNu")
        ATH_MSG_INFO("*VetoNu     tag: " << pair.second);
      else if(tagPairName=="GeoTrigger")
        ATH_MSG_INFO("*Trigger    tag: " << pair.second);
      else if(tagPairName=="GeoPreshower")
        ATH_MSG_INFO("*Preshower  tag: " << pair.second);
      else if(tagPairName=="GeoTracker")
        ATH_MSG_INFO("*Tracker  tag: " << pair.second);
      else if(tagPairName=="GeoSCT")
        ATH_MSG_INFO("*SCT        tag: " << pair.second);
      else if(tagPairName=="GeoDipole")
        ATH_MSG_INFO("*Dipole     tag: " << pair.second);
      else if(tagPairName=="GeoCalo")
        ATH_MSG_INFO("*Calo     tag: " << pair.second);
      else if(tagPairName=="GeoEcal")
        ATH_MSG_INFO("*Ecal     tag: " << pair.second);
      // else if(tagPairName=="GeoMagField")
      //   ATH_MSG_INFO("*MagField  tag: " << pair.second);
      else if(tagPairName=="GeoTrench")
        ATH_MSG_INFO("*Trench  tag: " << pair.second);
    }
    
    if(!m_ignoreTagDifference) {
      ATH_MSG_INFO("*** *** Please fix geometry tag settings *** ***");
      return StatusCode::FAILURE;
    }
  }
  else
    ATH_MSG_DEBUG("Geometry configurations in jobOptions and TagInfo are consistent");

  return StatusCode::SUCCESS;
}

/**********************************************************************************
 ** Private Member Functions
 **********************************************************************************/
StatusCode GeoModelSvc::fillTagInfo() const
{
  if(m_FaserVersion == "") {
    ATH_MSG_ERROR("FASER version is empty");
    return StatusCode::FAILURE; 
  }
  
  if(m_tagInfoMgr->addTag("GeoFaser",m_FaserVersion).isFailure()) {
    ATH_MSG_ERROR("GeoModelSvc Faser tag: " << m_FaserVersion	<< " not added to TagInfo ");
    return StatusCode::FAILURE; 
  }

  if(m_NeutrinoVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoNeutrino",m_NeutrinoVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Neutrino tag: " << m_NeutrinoVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  } 

  if(m_EmulsionVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoEmulsion",m_EmulsionVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Emulsion tag: " << m_EmulsionVersionOverride << " not added to TagInfo " );
      return StatusCode::FAILURE;
    }
  }

  if(m_ScintVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoScint",m_ScintVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Scint tag: " << m_ScintVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  } 

  if(m_VetoVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoVeto",m_VetoVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Veto tag: " << m_VetoVersionOverride << " not added to TagInfo " );
      return StatusCode::FAILURE;
    }
  }

  if(m_VetoNuVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoVetoNu",m_VetoNuVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc VetoNu tag: " << m_VetoNuVersionOverride << " not added to TagInfo " );
      return StatusCode::FAILURE;
    }
  }

  if(m_TriggerVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoTrigger",m_TriggerVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Trigger tag: " << m_TriggerVersionOverride << " not added to TagInfo " );
      return StatusCode::FAILURE;
    }
  }

  if(m_PreshowerVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoPreshower",m_PreshowerVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Preshower tag: " << m_PreshowerVersionOverride << " not added to TagInfo " );
      return StatusCode::FAILURE;
    }
  }

  if(m_TrackerVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoTracker",m_TrackerVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Tracker tag: " << m_TrackerVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_SCTVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoSCT",m_SCTVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc SCT tag: " << m_SCTVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_DipoleVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoDipole",m_DipoleVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Dipole tag: " << m_DipoleVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_CaloVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoCalo",m_CaloVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Calo tag: " << m_CaloVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_EcalVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoEcal",m_EcalVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Ecal tag: " << m_EcalVersionOverride << " not added to TagInfo " );
      return StatusCode::FAILURE;
    }
  }

  // if(m_MagFieldVersionOverride != "") {
  //   if(m_tagInfoMgr->addTag("GeoMagField",m_MagFieldVersionOverride).isFailure()) {
  //     ATH_MSG_ERROR("GeoModelSvc MagField tag: " << m_MagFieldVersionOverride << " not added to TagInfo ");
  //     return StatusCode::FAILURE; 
  //   }
  // }

  if(m_TrenchVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoTrench",m_TrenchVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Trench tag: " << m_TrenchVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }
  return StatusCode::SUCCESS;
}

const IGeoModelTool* GeoModelSvc::getTool(std::string toolName) const
{
  ToolHandleArray< IGeoModelTool >::const_iterator itPriv = m_detectorTools.begin();

  for(; itPriv!=m_detectorTools.end(); itPriv++) {
    const IGeoModelTool* theTool = &(**itPriv);
    if(theTool->name().find(toolName)!=std::string::npos)
      return theTool;
  }

  return 0;
}

StatusCode GeoModelSvc::clear()
{
  ATH_MSG_DEBUG("In clear()"); 

  // Call clear() for all tools
  ToolHandleArray< IGeoModelTool >::iterator itPriv = m_detectorTools.begin();
  for(; itPriv!=m_detectorTools.end(); itPriv++) {
    IGeoModelTool* theTool = &(**itPriv);
    if(theTool->clear().isFailure()) {
      ATH_MSG_ERROR("clear() failed for the tool: " << theTool->name());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG(theTool->name() << " tool released");
    }
  }

  // Delete GeoModelExperiment - cascade delete of the entire GeoModel tree
  std::vector<std::string>::const_iterator it;
  std::vector<std::string> sgkeysExp;
  m_detStore->keys<GeoModelExperiment>(sgkeysExp);
  for(it=sgkeysExp.begin(); it!=sgkeysExp.end(); it++) {
    SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<GeoModelExperiment>::ID(),*it);
    if(proxy) {
      proxy->reset();
      ATH_MSG_DEBUG(*it << " GeoModel experiment released");
    }
  }

  // Release stored material manager
  std::vector<std::string> sgkeysMat;
  m_detStore->keys<StoredMaterialManager>(sgkeysMat);
  for(it=sgkeysMat.begin(); it!=sgkeysMat.end(); it++) {
    SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<StoredMaterialManager>::ID(),*it);
    if(proxy) {
      proxy->reset();
      ATH_MSG_DEBUG(*it << " material manager released");
    }
  }

  return StatusCode::SUCCESS;
}
