/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

///////////////////////////////////////////////////////////////////
// WaveformFitResult.h
//   Header file for class WaveformFitResult
///////////////////////////////////////////////////////////////////
// Class to handle the fit output for a raw waveform
///////////////////////////////////////////////////////////////////
// Version 1.0   2/21/2021 Eric Torrence
///////////////////////////////////////////////////////////////////
#ifndef WAVERECTOOLS_WAVEFORMFITRESULT_H
#define WAVERECTOOLS_WAVEFORMFITRESULT_H

#include <iostream>

/**
 * @class WaveformFitResult
 * The baseline is the estimate of the waveform reading for no signal.
 */
  
class WaveformFitResult {
      
 public:
      
  /** Default constructor */
  WaveformFitResult();

  // Set status directly
  WaveformFitResult(bool valid);

  // move assignment defaulted
  WaveformFitResult & operator= (WaveformFitResult &&) = default;
  //assignment defaulted
  WaveformFitResult & operator= (const WaveformFitResult &) = default;
  //copy c'tor defaulted
  WaveformFitResult(const WaveformFitResult &) = default;

  /** Destructor */
  virtual ~WaveformFitResult() = default;

  // Clear
  void clear();
            
  // Data members
  bool valid;   // Overall valididty (from fit result == 0)

  double peak;  // Best value for peak
  double mean;  // Best value for mean
  double sigma; // Best value for width
  double alpha; // Crystal ball parameter
  double nval;  // Crystal ball parameter
  double chi2ndf; 
  double integral;
  double time;  // Time of leading edge of pulse

  int fit_status; // Return value from fit

  double chi2n; // Reduced chi2/Ndof from fit

 private:

};

std::ostream
&operator<<(std::ostream &out, const WaveformFitResult &wfm);

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#endif // WAVERECTOOLS_WAVEFORMFITRESULT_H
