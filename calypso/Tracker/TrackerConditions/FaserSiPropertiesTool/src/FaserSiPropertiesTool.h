/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file FaserSiPropertiesTool.h
 * @author Susumu.Oda@cern.ch
**/
#ifndef FaserSiPropertiesTool_h
#define FaserSiPropertiesTool_h

#include "FaserSiPropertiesTool/IFaserSiPropertiesTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserSiPropertiesTool/FaserSiliconPropertiesVector.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/EventContext.h"

/**
 * @class FaserSiPropertiesTool
 * Concrete class for service providing silicon properties (mobility, etc).
 * for each detector element. These depend on conditions such as temperature.
**/
class FaserSiPropertiesTool: public extends<AthAlgTool, IFaserSiPropertiesTool>
{
public:
  FaserSiPropertiesTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~FaserSiPropertiesTool() = default;
 
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /// Get properties for the detector element.
  virtual const Tracker::FaserSiliconProperties& getSiProperties(const IdentifierHash& elementHash, const EventContext& ctx) const override;

private:
  // Properties
  std::string m_detectorName;
  SG::ReadCondHandleKey<Tracker::FaserSiliconPropertiesVector> m_propertiesVector;

  static const Tracker::FaserSiliconProperties s_defaultProperties;
};

#endif // FaserSiPropertiesTool_h
