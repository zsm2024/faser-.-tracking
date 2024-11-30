/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserGeo2G4Svc.h"
#include "FaserGeo2G4AssemblyVolume.h"
#include "FaserExtParameterisedVolumeBuilder.h"

BuilderMap InitializeBuilders(FaserGeo2G4AssemblyFactory*);

FaserGeo2G4Svc::FaserGeo2G4Svc(const std::string& name, ISvcLocator* svcLocator)
  : base_class(name,svcLocator)
  , m_defaultBuilder()
  , m_getTopTransform(true)
  , m_G4AssemblyFactory(nullptr)
{
  ATH_MSG_VERBOSE ("Creating the FaserGeo2G4Svc.");
  declareProperty("GetTopTransform", m_getTopTransform);
}
FaserGeo2G4Svc::~FaserGeo2G4Svc()
{;}

StatusCode FaserGeo2G4Svc::initialize()
{
  static int initialized=0;
  if (initialized)
    {
      ATH_MSG_VERBOSE (" FaserGeo2G4Svc already initialized.");
      return StatusCode::SUCCESS;
    }
  ATH_MSG_VERBOSE ("Initializing the FaserGeo2G4Svc.");
  ATH_MSG_VERBOSE ("Creating all builders available.");
  m_G4AssemblyFactory = std::make_unique<FaserGeo2G4AssemblyFactory>();
  m_builders = InitializeBuilders(m_G4AssemblyFactory.get()); // separate function not part of this class
  
  const std::string nameBuilder = "Extended_Parameterised_Volume_Builder"; //TODO Configurable property??
  this->SetDefaultBuilder(nameBuilder);
  ATH_MSG_VERBOSE (nameBuilder << " --> set as default builder" );
  ATH_MSG_VERBOSE (nameBuilder << " --> ParamOn flag = " << GetDefaultBuilder()->GetParam());
  initialized=1;
  if(msgLvl(MSG::VERBOSE))
    {
      this->ListVolumeBuilders();
    }
  return StatusCode::SUCCESS;
}

StatusCode FaserGeo2G4Svc::finalize()
{
  ATH_MSG_VERBOSE ("Finalizing the FaserGeo2G4Svc.");
  
  return StatusCode::SUCCESS;
}

void FaserGeo2G4Svc::handle(const Incident& )
{
}

void FaserGeo2G4Svc::ListVolumeBuilders() const
{
  ATH_MSG_INFO("---- List of all Volume Builders registered with FaserGeo2G4Svc ----");
  ATH_MSG_INFO("--------------------------------------------------------------------");
  for (const auto& builder : m_builders)
    {
      ATH_MSG_INFO(" Volume Builder: "<<builder.second->GetKey());
    }
  ATH_MSG_INFO("--------------------------------------------------------------------");
  ATH_MSG_INFO(" default builder is "<< GetDefaultBuilder()->GetKey());
}

FaserVolumeBuilder* FaserGeo2G4Svc::GetVolumeBuilder(std::string s) const
{
  const auto builderItr(m_builders.find(s));
  if (builderItr!=m_builders.end())
    {
      return builderItr->second.get();
    }
  else
    {
      ATH_MSG_ERROR ("Trying to retrieve a not existing builder "<<s);
      ATH_MSG_ERROR ("\treturning Default Builder");
    }
  return GetDefaultBuilder();
}
