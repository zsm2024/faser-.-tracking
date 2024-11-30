// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SCTinformation
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for sct strips information
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 19/04/2018 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef FaserSCTinformation_h
#define FaserSCTinformation_h


class FaserSCT_Cluster;

namespace Tracker
{
  class FaserSCTinformation {
   
    /////////////////////////////////////////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////////////////////////////////////////
   
  public:
   
    FaserSCTinformation(const Tracker::FaserSCT_Cluster* CL, const Amg::Vector3D& a, const Amg::Vector3D& b, const Amg::Vector3D& vec);

    FaserSCTinformation(const FaserSCTinformation&) = default;
    virtual ~FaserSCTinformation() = default;
    FaserSCTinformation& operator = (const FaserSCTinformation&) = default;

    const Tracker::FaserSCT_Cluster* cluster() const;
    const Amg::Vector3D& a() const;
    const Amg::Vector3D& q() const;
    const Amg::Vector3D& s() const;
    const Amg::Vector3D& qs() const;
    double qm() const;

    double step(FaserSCTinformation&) const;
    Amg::Vector3D position(const double s) const;

  private:

    const Tracker::FaserSCT_Cluster* m_CL;
    Amg::Vector3D m_A;
    Amg::Vector3D m_Q;
    Amg::Vector3D m_S;
    Amg::Vector3D m_QS;
    double m_QM;
  };


  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  // Constructor with initialisation
  /////////////////////////////////////////////////////////////////////////////////
 
  inline FaserSCTinformation::FaserSCTinformation
    (const Tracker::FaserSCT_Cluster* CL, const Amg::Vector3D& a, const Amg::Vector3D& b, const Amg::Vector3D& vec) 
  {
    m_CL = CL            ;
    m_A  = 0.5*(a+b)      ; // Center of strip
    m_Q  = a-b           ; // Direction of strip      a-b
    m_S  = 2.*(m_A-vec)  ; // Direction of trajectory a+b-2*vertexVec
    m_QS = m_Q.cross(m_S); // Normal to strip diretion and trjectory direction plane
    m_QM = 1./m_Q.mag()  ; // Invert length of the strip
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Getter methods
  /////////////////////////////////////////////////////////////////////////////////
  inline const Tracker::FaserSCT_Cluster* FaserSCTinformation::cluster() const
  {
    return m_CL;
  }

  inline const Amg::Vector3D& FaserSCTinformation::a() const
  {
    return m_A;
  }

  inline const Amg::Vector3D& FaserSCTinformation::q() const
  {
    return m_Q;
  }

  inline const Amg::Vector3D& FaserSCTinformation::s() const
  {
    return m_S;
  }

  inline const Amg::Vector3D& FaserSCTinformation::qs() const
  {
    return m_QS;
  }

  inline double FaserSCTinformation::qm() const
  {
    return m_QM;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Step along strip
  /////////////////////////////////////////////////////////////////////////////////

  inline double FaserSCTinformation::step(FaserSCTinformation& In) const
  {
    double a = m_Q.dot(In.m_QS);
    if (a!=0.) return (-m_S.dot(In.m_QS)/a);
    else return 10000.;
  }

  inline Amg::Vector3D FaserSCTinformation::position(const double s) const
  {
    return (m_A+(0.5*s)*m_Q);
  }
} 

#endif  // FaserSCTinformation_h
