// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and CERN collaborations
*/

/** @file LHCDataTool.h Header file for LHCDataTool.
    @author Eric Torrence, 20/04/22
*/

// Multiple inclusion protection
#ifndef LHCDATA_TOOL
#define LHCDATA_TOOL

// Include interface class
#include "AthenaBaseComps/AthAlgTool.h"
#include "LHCDataTools/ILHCDataTool.h"

// Include Athena stuff
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

// Include Gaudi classes
#include "GaudiKernel/EventContext.h"

/** This class contains a Tool that reads Waveform range data and makes it available to 
    other algorithms. The current implementation reads the data from a COOL database. 
*/

class LHCDataTool: public extends<AthAlgTool, ILHCDataTool> {

 public:
  //----------Public Member Functions----------//
  // Structors
  LHCDataTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor
  virtual ~LHCDataTool() = default; //!< Destructor

  // Standard Gaudi functions
  virtual StatusCode initialize() override; //!< Gaudi initialiser
  virtual StatusCode finalize() override; //!< Gaudi finaliser

  // Methods to return fill data
  // Methods to return beam data
  virtual int getFillNumber(const EventContext& ctx) const override;
  virtual int getFillNumber(void) const override;

  virtual std::string getMachineMode(const EventContext& ctx) const override;
  virtual std::string getMachineMode(void) const override;

  virtual std::string getBeamMode(const EventContext& ctx) const override;
  virtual std::string getBeamMode(void) const override;

  virtual int getBeamType1(const EventContext& ctx) const override;
  virtual int getBeamType1(void) const override;

  virtual int getBeamType2(const EventContext& ctx) const override;
  virtual int getBeamType2(void) const override;

  virtual float getBetaStar(const EventContext& ctx) const override;
  virtual float getBetaStar(void) const override;

  virtual float getCrossingAngle(const EventContext& ctx) const override;
  virtual float getCrossingAngle(void) const override;

  virtual bool getStableBeams(const EventContext& ctx) const override;
  virtual bool getStableBeams(void) const override;

  virtual std::string getInjectionScheme(const EventContext& ctx) const override;
  virtual std::string getInjectionScheme(void) const override;

  // Methods to return BCID data
  virtual unsigned int getBeam1Bunches(const EventContext& ctx) const override;
  virtual unsigned int getBeam1Bunches(void) const override;

  virtual unsigned int getBeam2Bunches(const EventContext& ctx) const override;
  virtual unsigned int getBeam2Bunches(void) const override;

  virtual unsigned int getCollidingBunches(const EventContext& ctx) const override;
  virtual unsigned int getCollidingBunches(void) const override;

  // This returns a char for each BCID encoding beam1/beam2
  // A colliding BCID will have value 3
  // BCIDs always count starting at 1
  virtual std::vector<unsigned char> getBCIDMasks(const EventContext& ctx) const override;
  virtual std::vector<unsigned char> getBCIDMasks(void) const override;

 private:
  // Read Cond Handles
  //SG::ReadCondHandleKey<AthenaAttributeList> m_fillDataKey{this, "FillDataKey", "/LHC/FillData", "Key of fill data folder"};
  SG::ReadCondHandleKey<AthenaAttributeList> m_beamDataKey{this, "BeamDataKey", "/LHC/BeamData", "Key of fill data folder"};
  SG::ReadCondHandleKey<AthenaAttributeList> m_bcidDataKey{this, "BcidDataKey", "/LHC/BCIDData", "Key of fill data folder"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

};

//---------------------------------------------------------------------- 
#endif // LHCDATA_TOOL
