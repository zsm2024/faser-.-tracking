/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASERUSERACTIONS_G4UA_FASERLOOPERKILLERTOOL_H
#define G4FASERUSERACTIONS_G4UA_FASERLOOPERKILLERTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "FaserLooperKiller.h"

namespace G4UA
{

  /// @class FaserLooperKillerTool
  /// @brief Tool which manages the LooperKiller action.
  ///
  /// Create the LooperKiller for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class FaserLooperKillerTool : public UserActionToolBase<FaserLooperKiller>
  {

    public:

      /// Standard constructor
      FaserLooperKillerTool(const std::string& type, const std::string& name,
		       const IInterface* parent);

      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;

    protected:

      /// Create action for this thread
      virtual std::unique_ptr<FaserLooperKiller>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      /// Configuration parameters
      G4UA::FaserLooperKiller::Config m_config;

  }; // class FaserLooperKillerTool

} // namespace G4UA

#endif
