// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef FaserSCT_ReadCalibChipGainCondAlg_h
#define FaserSCT_ReadCalibChipGainCondAlg_h

// Include parent class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Include Gaudi classes
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

// Include Athena classes
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "FaserSCT_ConditionsData/FaserSCT_GainCalibData.h"

// Include boost stuff
#include "boost/array.hpp"

// Forward declarations
class FaserSCT_ID;

class FaserSCT_ReadCalibChipGainCondAlg : public AthReentrantAlgorithm 
{  
 public:
  FaserSCT_ReadCalibChipGainCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserSCT_ReadCalibChipGainCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  void insertNptGainFolderData(FaserSCT_ModuleGainCalibData& theseCalibData, const coral::AttributeList& folderData) const;

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/DAQ/Calibration/ChipGain", "Key of input (raw) gain conditions folder"};
  SG::WriteCondHandleKey<FaserSCT_GainCalibData> m_writeKey{this, "WriteKey", "SCT_GainCalibData", "Key of output (derived) gain conditions data"};
  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
  const FaserSCT_ID* m_id_sct{nullptr}; //!< Handle to FaserSCT ID helper
};

#endif // FaserSCT_ReadCalibChipGainCondAlg_h
