/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** S.Oda, Oct 2018
 * replace unsigned int by unsigned short for
   for the premixing samples for Overlay MC. */

#ifndef FASERSCT3_RAWDATA_P4_H
#define FASERSCT3_RAWDATA_P4_H

#include "Identifier/Identifier.h"

class FaserSCT3_RawData_p4 {
 public:
  FaserSCT3_RawData_p4() {};
// List of Cnv classes that convert this into Rdo objects
  friend class FaserSCT3_RawDataCnv_p4;
  //private:
  unsigned short m_rowStrip; // row and strip numbers from Identifier. Bits 0-12 for strip, Bits 13-15 for row
  unsigned short m_word; // raw data word. Bits 0-10 for group size, Bits 11-13 for time bin

  void setRowStrip(const int row, const int strip) {
    m_rowStrip  =  (static_cast<unsigned int>(strip) & 0x1FFF);
    m_rowStrip |= ((static_cast<unsigned int>(row) & 0x7) << 13);
  }

  int getRow() const {
    return ((m_rowStrip >> 13) & 0x7);
  }

  int getStrip() const {
    return (m_rowStrip & 0x1FFF);
  }

};

#endif // FASERSCT3_RAWDATA_P4_H
