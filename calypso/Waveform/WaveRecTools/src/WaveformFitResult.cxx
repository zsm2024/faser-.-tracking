/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

#include "WaveformFitResult.h"

WaveformFitResult::WaveformFitResult() {
  clear();
}

WaveformFitResult::WaveformFitResult(bool isvalid) {
  clear();
  valid = isvalid;
}

void
WaveformFitResult::clear() {
  valid = false;
  peak = 0.;
  mean = 0.;
  sigma = 0.;
  fit_status = -1;
  chi2n = 0.;
}

std::ostream
&operator<<( std::ostream& out, const WaveformFitResult &fit ) {
  out << "WaveformFitResult - Mean: " << fit.mean << ", Sigma: " << fit.sigma << ", Peak: " << fit.peak << std::endl;
  return out;
}





