/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELSVC_GEOMODELSVC_H
#define GEOMODELSVC_GEOMODELSVC_H

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelInterfaces/IGeoModelTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfoMgt/ITagInfoMgr.h"
#include <fstream>

class ISvcLocator;

template <class TYPE> class SvcFactory;

class GeoModelSvc : public AthService, virtual public IGeoModelSvc,
                    virtual public ITagInfoMgr::Listener
{
public:

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    // Query the interfaces.
    //   Input: riid, Requested interface ID
    //          ppvInterface, Pointer to requested interface
    //   Return: StatusCode indicating SUCCESS or FAILURE.
    // N.B. Don't forget to release the interface after use!!!
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    /// Callback from TagInfoMgr on TagInfo change
    virtual void       tagInfoUpdated() override final;

    StatusCode compareTags();

    virtual const IGeoModelTool* getTool(std::string toolName) const;

    virtual StatusCode clear();

    friend class SvcFactory<GeoModelSvc>;

    // Standard Constructor
    GeoModelSvc(const std::string& name, ISvcLocator* svc);

    // Standard Destructor
    virtual ~GeoModelSvc();


protected:

    //     StatusCode append( IGeoModelTool* pddTool, 
    // 		       std::vector<IGeoModelTool*>* theTools ); 
    //     StatusCode decodeNames( StringArrayProperty& theNames, 
    //        						std::vector<IGeoModelTool*>* theTools ); 
	
private:

    ToolHandleArray< IGeoModelTool > m_detectorTools; // Detector Tools

    ISvcLocator*        m_pSvcLocator;

    ServiceHandle<IToolSvc>     m_toolSvc;     
    ServiceHandle<StoreGateSvc> m_detStore;   
    ServiceHandle<ITagInfoMgr>  m_tagInfoMgr;   
    ServiceHandle<IGeoDbTagSvc> m_geoDbTagSvc;

    std::string           m_FaserVersion;

    std::string           m_NeutrinoVersionOverride;
    std::string           m_EmulsionVersionOverride;
    std::string           m_ScintVersionOverride;
    std::string           m_VetoVersionOverride;
    std::string           m_VetoNuVersionOverride;
    std::string           m_TriggerVersionOverride;
    std::string           m_PreshowerVersionOverride;
    std::string           m_TrackerVersionOverride;
    std::string           m_SCTVersionOverride;
    std::string           m_DipoleVersionOverride;
    std::string           m_CaloVersionOverride;
    std::string           m_EcalVersionOverride;
    // std::string           m_MagFieldVersionOverride;
    std::string           m_TrenchVersionOverride;

    bool          m_printMaterials;               // Print the contents of the Material Manager at the end of geoInit
    bool          m_callBackON;                   // Register callback for Detector Tools
    bool          m_ignoreTagDifference;          // Keep going if TagInfo and property tags are different
                                                  // when geometry configured manually 
    bool          m_useTagInfo;                   // Flag for TagInfo usage
    bool          m_statisticsToFile;             // Flag for generating GeoModelStatistics file in the run directory
    std::string   m_geoExportFile;                // Name of file to export GeoModel trees (constructed geometry) to

    int           m_supportedGeometry;            // Supported geometry flag is set in jobOpt and is equal to major release version
    bool          m_ignoreTagSupport;             // If true then don't check SUPPORT flag for ATLAS tag

    const std::string & faserVersion()             const {return m_FaserVersion; }
    const std::string & neutrinoVersionOverride()  const {return m_NeutrinoVersionOverride; }
    const std::string & emulsionVersionOverride()  const {return m_EmulsionVersionOverride; }
    const std::string & scintVersionOverride()     const {return m_ScintVersionOverride; }
    const std::string & vetoVersionOverride()      const {return m_VetoVersionOverride; }
    const std::string & vetoNuVersionOverride()    const {return m_VetoNuVersionOverride; }
    const std::string & triggerVersionOverride()   const {return m_TriggerVersionOverride  ;}
    const std::string & preshowerVersionOverride() const {return m_PreshowerVersionOverride  ;}
    const std::string & trackerVersionOverride()   const {return m_TrackerVersionOverride  ;}
    const std::string & sctVersionOverride()       const {return m_SCTVersionOverride  ;}
    const std::string & dipoleVersionOverride()    const {return m_DipoleVersionOverride; }
    const std::string & caloVersionOverride()      const {return m_CaloVersionOverride  ;}
    const std::string & ecalVersionOverride()      const {return m_EcalVersionOverride  ;}
    // const std::string & magFieldVersionOverride()     const {return m_MagFieldVersionOverride  ;}
    const std::string & trenchVersionOverride()    const {return m_TrenchVersionOverride  ;}

    const std::string & neutrinoVersion()      const {return m_geoDbTagSvc->neutrinoVersion(); }
    const std::string & emulsionVersion()      const {return m_geoDbTagSvc->emulsionVersion(); }
    const std::string & scintVersion()         const {return m_geoDbTagSvc->scintVersion(); }
    const std::string & vetoVersion()          const {return m_geoDbTagSvc->vetoVersion(); }
    const std::string & vetoNuVersion()        const {return m_geoDbTagSvc->vetoNuVersion(); }
    const std::string & triggerVersion()       const {return m_geoDbTagSvc->triggerVersion(); }
    const std::string & preshowerVersion()     const {return m_geoDbTagSvc->preshowerVersion(); }
    const std::string & trackerVersion()       const {return m_geoDbTagSvc->trackerVersion(); }
    const std::string & sctVersion()           const {return m_geoDbTagSvc->sctVersion(); }
    const std::string & dipoleVersion()        const {return m_geoDbTagSvc->dipoleVersion(); }
    const std::string & caloVersion()          const {return m_geoDbTagSvc->caloVersion(); }
    const std::string & ecalVersion()          const {return m_geoDbTagSvc->ecalVersion(); }
    // const std::string & magFieldVersion()      const {return m_geoDbTagSvc->magFieldVersion(); }
    const std::string & trenchVersion()        const {return m_geoDbTagSvc->trenchVersion(); }

    GeoModel::GeoConfig geoConfig() const {return m_geoDbTagSvc->geoConfig();}

    StatusCode geoInit ();
    StatusCode fillTagInfo() const;
};

#endif // GEOMODELSVC_GEOMODELSVC_H
