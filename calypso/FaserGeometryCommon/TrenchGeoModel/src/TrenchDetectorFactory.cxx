/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrenchDetectorFactory.h"

#include "GeoModelXMLParser/XercesParser.h"
#include "PathResolver/PathResolver.h"


#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"
#include "StoreGate/StoreGateSvc.h"


#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GaudiKernel/MsgStream.h"

#include <string>
#include <map>

TrenchDetectorFactory::TrenchDetectorFactory(StoreGateSvc *detStore,
				       IRDBAccessSvc *pAccess)
  :m_detectorManager(NULL),
   m_detectorStore(detStore),
   m_access(pAccess)
{
}

TrenchDetectorFactory::~TrenchDetectorFactory()
{
}

void TrenchDetectorFactory::create(GeoPhysVol *world)
{ 
    m_detectorManager=new TrenchDetectorManager();

    std::string gdmlFile;
    IRDBRecordset_ptr switchSet{m_access->getRecordsetPtr("TrenchSwitches", m_versionTag, m_versionNode, "FASERDD")};
    if (!switchSet || switchSet->size() == 0)
    {
        MsgStream gLog(Athena::getMessageSvc(), "TrenchDetectorFactory");
        gLog << MSG::WARNING << "Unable to retrieve switches; Trench cannot be created" << endmsg;
        return;
    }
    const IRDBRecord* switches{(*switchSet)[0]};
    if (not switches->isFieldNull("GDMLFILE") && gdmlFile.empty())
    {
        gdmlFile = switches->getString("GDMLFILE");
    } 
    if (gdmlFile.empty())
    {
        MsgStream gLog(Athena::getMessageSvc(), "TrenchDetectorFactory");
        gLog << MSG::WARNING << "GDML file name not found; Trench cannot be created"  << endmsg;
        return;
    }

    std::string resolvedFile = PathResolver::find_file(gdmlFile, "XMLPATH", PathResolver::RecursiveSearch);

    auto store = GeoModelTools::XMLHandlerStore::GetHandlerStore();
    // for (auto p : *store)
    // {
    //   delete p.second;
    // }
    store->clear();

    GDMLController controller {"TrenchGDMLController"};

    // std::cout << "creating parser" << std::endl;
    GeoModelTools::XercesParser xercesParser;

    // std::cout << "parsing " << resolvedFile << std::endl;
    xercesParser.ParseFileAndNavigate(resolvedFile);
    // std::cout << "done parsing " << resolvedFile << std::endl;

//    const GeoLogVol* trenchLog  = controller.retrieveLogicalVolume("Trench").first;
    const GeoLogVol* trenchLog  = controller.retrieveLogicalVolume("TrenchAndWall").first;
    GeoPhysVol*      trenchPhys = new GeoPhysVol(trenchLog);
    GeoNameTag *tag = new GeoNameTag("Trench");
    world->add(tag);
    world->add(trenchPhys);
    m_detectorManager->addTreeTop(trenchPhys);
}

const TrenchDetectorManager * TrenchDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}

void TrenchDetectorFactory::setTagNode(const std::string& tag,
                                       const std::string& node)
{
  m_versionTag = tag;
  m_versionNode = node;
}

