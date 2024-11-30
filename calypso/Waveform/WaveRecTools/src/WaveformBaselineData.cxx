/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

#include "WaveformBaselineData.h"

WaveformBaselineData::WaveformBaselineData() {
  clear();
}

WaveformBaselineData::WaveformBaselineData(bool isvalid) {
  clear();
  valid = isvalid;
}

void
WaveformBaselineData::clear() {
  channel = -1;
  valid = false;
  mean = 0.;
  rms = 0.;
  fit_status = -1;
  peak = 0.;
  chi2n = 0.;
}

std::ostream
&operator<<( std::ostream& out, const WaveformBaselineData &base ) {
  out << "WaveformBaselineData - Mean: " << base.mean << ", RMS: " << base.rms << std::endl;
  return out;
}





