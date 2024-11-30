/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FASER collaborations
*/

/** @file LHCDataTool.cxx Implementation file for LHCDataTool.
    @author Eric Torrence (05/02/22)
*/

#include "LHCDataTool.h"
#include "CoralBase/Blob.h"

//----------------------------------------------------------------------
LHCDataTool::LHCDataTool (const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

//----------------------------------------------------------------------
StatusCode 
LHCDataTool::initialize() {

  ATH_MSG_DEBUG("LHCDataTool::initialize()");

  // Read Handles
  //ATH_CHECK(m_fillDataKey.initialize());
  ATH_CHECK(m_beamDataKey.initialize());
  ATH_CHECK(m_bcidDataKey.initialize());

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
StatusCode
LHCDataTool::finalize() {
  // Print where you are
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
int
LHCDataTool::getFillNumber(const EventContext& ctx) const {
  // Read Cond Handle
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_beamDataKey, ctx};
  return (**readHandle)["FillNumber"].data<int>();
} 

int
LHCDataTool::getFillNumber(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getFillNumber(ctx);
}

//----------------------------------------------------------------------
std::string
LHCDataTool::getMachineMode(const EventContext& ctx) const {
  // Read Cond Handle
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_beamDataKey, ctx};
  return (**readHandle)["MachineMode"].data<std::string>();
} 

std::string
LHCDataTool::getMachineMode(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getMachineMode(ctx);
}

//----------------------------------------------------------------------
std::string
LHCDataTool::getBeamMode(const EventContext& ctx) const {
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_beamDataKey, ctx};
  return(**readHandle)["BeamMode"].data<std::string>();
}

std::string
LHCDataTool::getBeamMode(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getBeamMode(ctx);
}

//----------------------------------------------------------------------
int
LHCDataTool::getBeamType1(const EventContext& ctx) const {
  // Read Cond Handle
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_beamDataKey, ctx};
  return (**readHandle)["BeamType1"].data<int>();
} 

int
LHCDataTool::getBeamType1(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getBeamType1(ctx);
}

//----------------------------------------------------------------------
int
LHCDataTool::getBeamType2(const EventContext& ctx) const {
  // Read Cond Handle
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_beamDataKey, ctx};
  return (**readHandle)["BeamType2"].data<int>();
} 

int
LHCDataTool::getBeamType2(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getBeamType2(ctx);
}

//----------------------------------------------------------------------
float
LHCDataTool::getBetaStar(const EventContext& ctx) const {
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_beamDataKey, ctx};
  return(**readHandle)["BetaStar"].data<float>();
}

float
LHCDataTool::getBetaStar(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getBetaStar(ctx);
}

//----------------------------------------------------------------------
float
LHCDataTool::getCrossingAngle(const EventContext& ctx) const {
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_beamDataKey, ctx};
  return(**readHandle)["CrossingAngle"].data<float>();
}

float
LHCDataTool::getCrossingAngle(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getCrossingAngle(ctx);
}

//----------------------------------------------------------------------
bool
LHCDataTool::getStableBeams(const EventContext& ctx) const {
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_beamDataKey, ctx};
  return(**readHandle)["StableBeams"].data<bool>();
}

bool
LHCDataTool::getStableBeams(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getStableBeams(ctx);
}

//----------------------------------------------------------------------
std::string
LHCDataTool::getInjectionScheme(const EventContext& ctx) const {
  // Read Cond Handle
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_beamDataKey, ctx};
  return (**readHandle)["InjectionScheme"].data<std::string>();
} 

std::string
LHCDataTool::getInjectionScheme(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getInjectionScheme(ctx);
}

//----------------------------------------------------------------------
unsigned int
LHCDataTool::getBeam1Bunches(const EventContext& ctx) const {
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_bcidDataKey, ctx};
  return(**readHandle)["Beam1Bunches"].data<unsigned int>();
}

unsigned int
LHCDataTool::getBeam1Bunches(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getBeam1Bunches(ctx);
}

unsigned int
LHCDataTool::getBeam2Bunches(const EventContext& ctx) const {
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_bcidDataKey, ctx};
  return(**readHandle)["Beam2Bunches"].data<unsigned int>();
}

unsigned int
LHCDataTool::getBeam2Bunches(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getBeam2Bunches(ctx);
}

unsigned int
LHCDataTool::getCollidingBunches(const EventContext& ctx) const {
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_bcidDataKey, ctx};
  return(**readHandle)["CollidingBunches"].data<unsigned int>();
}

unsigned int
LHCDataTool::getCollidingBunches(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getCollidingBunches(ctx);
}


//----------------------------------------------------------------------
// This function unpacks the blob every time this is accesed
// Should probably cache this using a callback
std::vector<unsigned char>
LHCDataTool::getBCIDMasks(const EventContext& ctx) const {

  SG::ReadCondHandle<AthenaAttributeList> bcidHandle{m_bcidDataKey, ctx};
  const coral::Blob& blob = (**bcidHandle)["BCIDmasks"].data<coral::Blob>();
  const unsigned char* p = static_cast<const unsigned char*>(blob.startingAddress());

  // Should always be 3564 BCIDs
  if (blob.size() != 3564) {
    ATH_MSG_WARNING("Found BCID blob with size " << blob.size() << "!");
  }

  std::vector<unsigned char> bcid_vector(3564);

  bool first = true;
  for (int i=0; i<blob.size(); i++) {
    // First BCID is 1, but this is stored at location i=1
    // So you can index this vector as bcid_vector[bcid_number]
    bcid_vector[i] = *p++;  
    if (first && (bcid_vector[i] == 3)) {
      first = false;
      ATH_MSG_DEBUG("Found first colliding BCID at " << i);
    }
  }

  return bcid_vector;
}

std::vector<unsigned char>
LHCDataTool::getBCIDMasks(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getBCIDMasks(ctx);
}



