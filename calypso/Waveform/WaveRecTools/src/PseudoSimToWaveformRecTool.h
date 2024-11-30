/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/** @file PseduoSimToWaveformRecTool.h
 *  Header file for PseudoSimToWaveformRecTool.h
 *
 */
#ifndef WAVERECTOOLS_PSEUDOSIMTOWAVEFORMRECTOOL_H
#define WAVERECTOOLS_PSEUDOSIMTOWAVEFORMRECTOOL_H

//Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "WaveRecTools/IPseudoSimToWaveformRecTool.h"

// Tool classes
#include "WaveformConditionsTools/IWaveformCableMappingTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

class Identifier;

class PseudoSimToWaveformRecTool: public extends<AthAlgTool, IPseudoSimToWaveformRecTool> {
 public:

  /// Normal constructor for an AlgTool; 'properties' are also declared here
 PseudoSimToWaveformRecTool(const std::string& type, 
			    const std::string& name, const IInterface* parent);

  /// Retrieve the necessary services in initialize
  StatusCode initialize();

  virtual StatusCode make_hits(const std::map<Identifier, float>& sums,
			       xAOD::WaveformHitContainer* waveContainer) const;

 private:
  ToolHandle<IWaveformCableMappingTool> m_mappingTool
    {this, "WaveformCableMappingTool", "WaveformCableMappingTool"};

};

#endif // WAVERECTOOLS_PSEUDOSIMTOWAVEFORMRECTOOL_H
