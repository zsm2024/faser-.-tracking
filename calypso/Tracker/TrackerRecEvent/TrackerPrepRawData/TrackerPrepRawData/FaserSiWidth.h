/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

///////////////////////////////////////////////////////////////////
// FaserSiWidth.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRACKERRAWDATA_FASERSIWIDTH_H
#define TRACKERRAWDATA_FASERSIWIDTH_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"

class MsgStream;

/** standard namespace for the Tracker */
namespace Tracker
{
/** @class FaserSiWidth

*/

class FaserSiWidth {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
  public:

    // Implicit constructor:
    FaserSiWidth();

    // Copy constructor:
    FaserSiWidth(const FaserSiWidth& position);

    // Constructor with parameters: <col, row> in units of RDOs (so should be int), 
    //                              <phiR width in mm, Z width in mm>
    FaserSiWidth(const Amg::Vector2D& colrow, const  Amg::Vector2D& phiRZ);

    // online constructor: only the col, row in units of RDOs
    FaserSiWidth(const Amg::Vector2D& colrow);

    // Destructor:
    virtual ~FaserSiWidth();

    // Assignment operator:
    FaserSiWidth &operator=(const FaserSiWidth& width);

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    // return col row:
    const Amg::Vector2D& colRow() const;

    // return phiRZ in mm:
    const Amg::Vector2D& widthPhiRZ() const;

    // return PhiR
    double phiR() const;

    // return z
    double z() const;


    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////

    void setColumn(const double col);

    void setRow(const double row);

    void setColRow(const Amg::Vector2D& colRow);

    void setPhirWidth(const double phirWidth);

    void setZWidth(const double zWidth);

    void setPhirzWidth(const Amg::Vector2D& phirzWidth);


    // addition

    /** dump information about the PRD object. */
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /** dump information about the PRD object. */
    virtual std::ostream& dump( std::ostream& stream) const;


    //scaling

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
  private:
    // Need to force proper alignment; otherwise cling gets it wrong.
    alignas(16) Amg::Vector2D m_colrow;//<col, row>
    alignas(16) Amg::Vector2D m_phirzWidth;
};

MsgStream&    operator << (MsgStream& stream,    const FaserSiWidth& prd);
std::ostream& operator << (std::ostream& stream, const FaserSiWidth& prd);


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline const Amg::Vector2D& FaserSiWidth::colRow() const
{
  return (m_colrow);
}


inline const Amg::Vector2D& FaserSiWidth::widthPhiRZ() const
{
  return m_phirzWidth;        
}

inline double FaserSiWidth::phiR() const
{
  return m_phirzWidth[0]; 
}

inline double FaserSiWidth::z() const
{
  return m_phirzWidth[1];    
}

inline void FaserSiWidth::setColumn(const double col)
{
  m_colrow[0] = col;
}

inline void FaserSiWidth::setRow(const double row)
{
  m_colrow[1] = row;
}

inline void FaserSiWidth::setColRow(const Amg::Vector2D& colRow)
{
  m_colrow=colRow;
}

inline void FaserSiWidth::setPhirWidth(const double phir)
{
  m_phirzWidth[0] = phir;
}
inline void FaserSiWidth::setZWidth(const double zwidth)
{
  m_phirzWidth[1] = zwidth;
}
inline void FaserSiWidth::setPhirzWidth(const Amg::Vector2D& phirzwidth)
{
  m_phirzWidth = phirzwidth;
}


///////////////////////////////////////////////////////////////////
// Binary operators:
///////////////////////////////////////////////////////////////////
}
#endif // TRACKERRAWDATA_FASERSIWIDTH_H
