// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef FaserSCT_ReadCalibChipNoiseCondAlg_h
#define FaserSCT_ReadCalibChipNoiseCondAlg_h

// Include parent class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Include Athena classes
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "FaserSCT_ConditionsData/FaserSCT_NoiseCalibData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

// Include Gaudi classes
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

// Include boost stuff
#include "boost/array.hpp"

// Forward declarations
class FaserSCT_ID;

class FaserSCT_ReadCalibChipNoiseCondAlg : public AthReentrantAlgorithm 
{  
 public:
  FaserSCT_ReadCalibChipNoiseCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserSCT_ReadCalibChipNoiseCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  void insertNoiseOccFolderData(FaserSCT_ModuleNoiseCalibData& theseCalibData, const coral::AttributeList& folderData) const;

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/DAQ/Calibration/ChipNoise", "Key of input (raw) noise conditions folder"};
  SG::WriteCondHandleKey<FaserSCT_NoiseCalibData> m_writeKey{this, "WriteKey", "SCT_NoiseCalibData", "Key of output (derived) noise conditions data"};
  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
  const FaserSCT_ID* m_id_sct{nullptr}; //!< Handle to SCT ID helper
};

#endif // FaserSCT_ReadCalibChipNoiseCondAlg_h
