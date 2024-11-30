/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

///////////////////////////////////////////////////////////////////
// WaveformBaselineData.h
//   Header file for class WaveformBaselineData
///////////////////////////////////////////////////////////////////
// Class to handle the baseline information for a raw waveform
///////////////////////////////////////////////////////////////////
// Version 1.0   2/21/2021 Eric Torrence
///////////////////////////////////////////////////////////////////
#ifndef WAVERECTOOLS_WAVEFORMBASELINEDATA_H
#define WAVERECTOOLS_WAVEFORMBASELINEDATA_H

#include <iostream>

/**
 * @class WaveformBaselineData
 * The baseline is the estimate of the waveform reading for no signal.
 */
  
class WaveformBaselineData {
      
 public:
      
  /** Default constructor */
  WaveformBaselineData();

  // Set status directly
  WaveformBaselineData(bool valid);

  // move assignment defaulted
  WaveformBaselineData & operator= (WaveformBaselineData &&) = default;
  //assignment defaulted
  WaveformBaselineData & operator= (const WaveformBaselineData &) = default;
  //copy c'tor defaulted
  WaveformBaselineData(const WaveformBaselineData &) = default;

  /** Destructor */
  virtual ~WaveformBaselineData() = default;

  /** Clones */
  virtual WaveformBaselineData* clone() const ;       

  // Clear
  void clear();
            
  // Data members
  int channel;  // Channel number
  bool valid;   // Overall valididty flag

  double mean;  // Best value for mean baseline
  double rms;   // Best value for baseline RMS
  int fit_status; // Return value from fit
  double peak;  // Amplitude of baseline fit
  double chi2n; // Reduced chi2/Ndof from gauss fit

 private:

};

std::ostream
&operator<<(std::ostream &out, const WaveformBaselineData &wfm);

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline WaveformBaselineData* 
WaveformBaselineData::clone() const { 
  return new WaveformBaselineData(*this);  
}

#endif // WAVERECTOOLS_WAVEFORMBASELINEDATA_H
