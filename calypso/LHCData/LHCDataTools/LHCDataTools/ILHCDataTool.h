/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FAsER collaborations
*/

/** @file ILHCDataTool.h Interface file for LHCDataTool.
 */

// Multiple inclusion protection
#ifndef ILHCDATATOOL
#define ILHCDATATOOL

//STL includes
#include <map>

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

class ILHCDataTool: virtual public IAlgTool {

 public:
  
  //----------Public Member Functions----------//
  // Structors
  virtual ~ILHCDataTool() = default; //!< Destructor

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ILHCDataTool, 1, 0);

  // Methods to return fill data
  virtual int getFillNumber(const EventContext& ctx) const = 0;
  virtual int getFillNumber(void) const = 0;

  virtual std::string getMachineMode(const EventContext& ctx) const = 0;
  virtual std::string getMachineMode(void) const = 0;

  virtual std::string getInjectionScheme(const EventContext& ctx) const = 0;
  virtual std::string getInjectionScheme(void) const = 0;

  virtual int getBeamType1(const EventContext& ctx) const = 0;
  virtual int getBeamType1(void) const = 0;

  virtual int getBeamType2(const EventContext& ctx) const = 0;
  virtual int getBeamType2(void) const = 0;

  // Methods to return beam data
  virtual std::string getBeamMode(const EventContext& ctx) const = 0;
  virtual std::string getBeamMode(void) const = 0;

  virtual float getBetaStar(const EventContext& ctx) const = 0;
  virtual float getBetaStar(void) const = 0;

  virtual float getCrossingAngle(const EventContext& ctx) const = 0;
  virtual float getCrossingAngle(void) const = 0;

  virtual bool getStableBeams(const EventContext& ctx) const = 0;
  virtual bool getStableBeams(void) const = 0;

  // Methods to return BCID data
  virtual unsigned int getBeam1Bunches(const EventContext& ctx) const = 0;
  virtual unsigned int getBeam1Bunches(void) const = 0;

  virtual unsigned int getBeam2Bunches(const EventContext& ctx) const = 0;
  virtual unsigned int getBeam2Bunches(void) const = 0;

  virtual unsigned int getCollidingBunches(const EventContext& ctx) const = 0;
  virtual unsigned int getCollidingBunches(void) const = 0;

  virtual std::vector<unsigned char> getBCIDMasks(const EventContext& ctx) const = 0;
  virtual std::vector<unsigned char> getBCIDMasks(void) const = 0;

};

//---------------------------------------------------------------------- 
#endif // LHCDATATOOL
