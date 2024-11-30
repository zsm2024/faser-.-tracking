/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_ReadCalibChipNoiseCondAlg.h"

#include "FillFromStringUtility.h"

#include "Identifier/IdentifierHash.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "FaserSCT_ConditionsData/FaserSCT_ConditionsParameters.h"
#include "FaserSCT_ConditionsTools/SCT_ReadCalibChipDefs.h"

#include "GaudiKernel/EventIDRange.h"

#include <limits>
#include <memory>

using namespace FaserSCT_ConditionsData;
using namespace SCT_ReadCalibChipDefs;

FaserSCT_ReadCalibChipNoiseCondAlg::FaserSCT_ReadCalibChipNoiseCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode FaserSCT_ReadCalibChipNoiseCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  // Get SCT helper
  ATH_CHECK(detStore()->retrieve(m_id_sct, "FaserSCT_ID"));

  // Read Cond Handle
  ATH_CHECK(m_readKey.initialize());
  // Write Cond Handle
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_ReadCalibChipNoiseCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<FaserSCT_NoiseCalibData> writeHandle{m_writeKey, ctx};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey, ctx};
  const CondAttrListCollection* readCdo{*readHandle};
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_INFO("Range of input is " << rangeW);
  
  // Construct the output Cond Object and fill it in
  std::unique_ptr<FaserSCT_NoiseCalibData> writeCdo{std::make_unique<FaserSCT_NoiseCalibData>()};

  // Initialization
  const float errVal{std::numeric_limits<float>::quiet_NaN()};
  for (unsigned int m{0}; m!=NUMBER_OF_MODULES; ++m) {
    for (unsigned int p{0}; p!=N_NOISEOCC; ++p) {
      for (unsigned int c{0}; c!=CHIPS_PER_MODULE; ++c) {
        (*writeCdo)[m][p][c]=errVal;
      }
    }
  }

  // loop over collection
  CondAttrListCollection::const_iterator itLoop{readCdo->begin()};
  CondAttrListCollection::const_iterator itLoop_end{readCdo->end()};
  for (; itLoop!=itLoop_end; ++itLoop) {
    CondAttrListCollection::ChanNum chanNum{itLoop->first};
    const coral::AttributeList& anAttrList{itLoop->second};
    // Convert chanNum=offlineID into identifier
    Identifier32 moduleId{chanNum};
    //find the corresponding hash
    const IdentifierHash hashId{m_id_sct->wafer_hash(moduleId)};
    //find the index to the module (hash is for each side), to use as index into array
    const unsigned int moduleIdx{hashId/SIDES_PER_MODULE};
    FaserSCT_ModuleNoiseCalibData& theseCalibData{(*writeCdo)[moduleIdx]};
    insertNoiseOccFolderData(theseCalibData, anAttrList);
  }

  // Record the output cond object
  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record FaserSCT_NoiseCalibData " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_ReadCalibChipNoiseCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

void 
FaserSCT_ReadCalibChipNoiseCondAlg::insertNoiseOccFolderData(FaserSCT_ModuleNoiseCalibData& theseCalibData, const coral::AttributeList& folderData) const {
  for (int i{0}; i!=N_NOISEOCC; ++i) {
    FaserSCT_ModuleCalibParameter& datavec{theseCalibData[i]};
    std::string dbData{((folderData)[noiseOccDbParameterNames[i]]).data<std::string>()};
    fillArrayFromString(dbData, datavec);
  }
}
