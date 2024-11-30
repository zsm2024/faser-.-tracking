#include "WaveRawEvent/RawWaveformContainer.h"

void
RawWaveformContainer::print() const {
  std::cout << "Waveform container with size=" << this->size() << std::endl;
  for(auto wfm: *this) std::cout << *wfm;
}

std::ostream
&operator<<(std::ostream &out, const RawWaveformContainer& cont) {
  out << "Waveform container with size=" << cont.size() << std::endl;
  for(auto wfm: cont) out << *wfm;
  return out;
}
