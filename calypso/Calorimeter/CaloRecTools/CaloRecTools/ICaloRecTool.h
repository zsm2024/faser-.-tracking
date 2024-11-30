/*
  Copyright (C) 2022 CERN for the benefit of the FASER collaboration
*/

/**
 * @file ICaloRecTool.h
 * Header file for the ICaloRecTool class
 * @author Deion Fellers, 2022
 */


#ifndef CALORECTOOL_ICALORECTOOL_H
#define CALORECTOOL_ICALORECTOOL_H

// Base class
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"

// Data Classes
// #include "xAODFaserWaveform/WaveformHit.h"
#include "xAODFaserCalorimeter/CalorimeterHit.h"

#include "TF1.h"

///Interface for Calo reco algorithms
class ICaloRecTool : virtual public IAlgTool 
{
  public:

  // InterfaceID
  DeclareInterfaceID(ICaloRecTool, 1, 0);

  virtual ~ICaloRecTool() = default; //!< Destructor

  // methods to return calibration database data
  virtual float getHV(const EventContext& ctx, int channel) const = 0;
  virtual float getHV(int channel) const = 0;

  virtual float getHV_ref(const EventContext& ctx, int channel) const = 0;
  virtual float getHV_ref(int channel) const = 0;

  virtual float getMIPcharge_ref(const EventContext& ctx, int channel) const = 0;
  virtual float getMIPcharge_ref(int channel) const = 0;

  virtual TF1 get_PMT_HV_curve(int channel) const = 0;

  // Reconstruct one waveform hit and put the resulting Calorimeter hit in the container
  virtual void reconstruct(const EventContext& ctx,
			   xAOD::CalorimeterHit* hit,
			   bool correct_gain) const = 0;
};

#endif // CALORECTOOL_ICALORECTOOL_H
