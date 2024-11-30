/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file IPseudoSimToWaveformRecTool.h
 * Header file for the IPseudoSimToWaveformRecTool class
 * @author Carl Gwilliam, 2021
 */


#ifndef WAVERECTOOLS_IPSEUDOSIMTOWAVEFORMRECTOOL_H
#define WAVERECTOOLS_IPSEUDOSIMTOWAVEFORMRECTOOL_H

// Base class
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "xAODFaserWaveform/WaveformHitContainer.h"
#include "xAODFaserWaveform/WaveformHit.h"

class Identifier;

///Interface for Pseudo waveform rec tools
class IPseudoSimToWaveformRecTool : virtual public IAlgTool 
{
public:

  // InterfaceID
  DeclareInterfaceID(IPseudoSimToWaveformRecTool, 1, 0);

  /*
  IPseudoSimToWaveformRecTool():
    m_msgSvc         ( "MessageSvc",   "ITrkEventCnvTool" )
  {}
  */

  virtual ~IPseudoSimToWaveformRecTool() = default;

  // Convert sim hit energies to pseudo-waveform 
  template<class CONT>
  StatusCode reconstruct(const CONT* hitCollection, 
			 xAOD::WaveformHitContainer* waveContainer) const;

  // Make the actual hits (separate this so it doesn't need to be in templated function
  virtual StatusCode make_hits(const std::map<Identifier, float>& sums, 
	      xAOD::WaveformHitContainer* waveContainer) const = 0;

};

#include "WaveRecTools/IPseudoSimToWaveformRecTool.icc"


#endif // SCINTRECTOOLS_IPSEUDOSIMTOWAVEFORMRECTOOL_H
