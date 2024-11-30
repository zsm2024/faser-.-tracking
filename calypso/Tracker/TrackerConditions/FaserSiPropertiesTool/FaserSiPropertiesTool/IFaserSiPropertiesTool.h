/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IFaserSiPropertiesTool.h
 * @author Susumu.Oda@cern.ch
 **/
#ifndef IFaserSiPropertiesTool_h
#define IFaserSiPropertiesTool_h

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
#include "FaserSiPropertiesTool/FaserSiliconProperties.h"
#include "Identifier/IdentifierHash.h"
class EventContext;

/**
 * @class IFaserSiPropertiesTool
 * Interface class for tool providing silicon properties (mobility, etc).
 * for each detector element. These depend on conditions such as temperature.
 **/

class IFaserSiPropertiesTool: virtual public IAlgTool
{
 public:
  virtual ~IFaserSiPropertiesTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IFaserSiPropertiesTool, 1, 0);

  /// Get Silicon properties for a given detector element.
  virtual const Tracker::FaserSiliconProperties& getSiProperties(const IdentifierHash& elementHash, const EventContext& ctx) const = 0;
};

#endif // IFaserSiPropertiesTool_h
