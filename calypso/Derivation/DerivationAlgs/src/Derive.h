#ifndef DERIVATIONALGS_DERIVE_H
#define DERIVATIONALGS_DERIVE_H

// Base class
#include "AthenaBaseComps/AthFilterAlgorithm.h"

// FASER
#include "DerivationTools/IDerivationTool.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IAlgTool.h"

// Athena
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"

// ROOT
#include <TRandom3.h>

// STL

class Derive : public AthFilterAlgorithm {

public:
  // Constructor
  Derive(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Derive() = default;

  /** @name Usual algorithm methods */
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  //@}


 private:

  /** @name Disallow default instantiation, copy, assignment */
  //@{
  Derive() = delete;
  Derive(const Derive&) = delete;
  Derive &operator=(const Derive&) = delete;
  //@}

  /// 

  /** @name Steerable tools */
  //@
  ToolHandleArray<IDerivationTool> m_tools {this, "Tools", {}, "List of tools"};  
  //@}

  /** Number of events processed */
  int m_events {0};

  /** Number of events selected */
  int m_passed {0};
  
};


#endif // DERIVATIONALGS_DERIVE_H
