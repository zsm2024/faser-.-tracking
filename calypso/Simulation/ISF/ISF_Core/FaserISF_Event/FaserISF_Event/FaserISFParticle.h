/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserISFParticle.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASERISF_EVENT_FASERISFPARTICLE_H
#define FASERISF_EVENT_FASERISFPARTICLE_H

// Gaudi Kernel
#include "GaudiKernel/MsgStream.h"
// Barcode includes
#include "BarcodeEvent/Barcode.h"
// ISF include
#include "ISF_Event/TruthBinding.h"
#include "ISF_Event/SimSvcID.h"
#include "ISF_Event/ParticleOrder.h"
#include "ISF_Event/ParticleUserInformation.h"
// DetectorDescription
#include "FaserDetDescr/FaserRegion.h"
// Amg classes
#include "GeoPrimitives/GeoPrimitives.h"
// CHLEP classes
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
// Common classes
#include "GeneratorObjects/HepMcParticleLink.h"

namespace ISF {

  /** the datatype to be used to store each individual particle hop */
  typedef std::pair< FaserDetDescr::FaserRegion, ISF::SimSvcID >   FaserDetRegionSvcIDPair;
  /** the container type to be used for the particle hops */
  typedef std::vector< FaserDetRegionSvcIDPair >                   FaserParticleHistory;

  /**
     @class FaserISFParticle

     The generic ISF particle definition,

     @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
  */

  class FaserISFParticle {

  public:
    /** disallow default constructor */
    FaserISFParticle() = delete;

    /** use this constructor whenever possible */
    FaserISFParticle(const Amg::Vector3D& pos,
                const Amg::Vector3D& mom,
                double mass,
                double charge,
                int pdgCode,
                int status,
                double time,
                const FaserISFParticle &parent,
                Barcode::ParticleBarcode barcode = Barcode::fUndefinedBarcode,
                TruthBinding* truth = nullptr,
                const HepMcParticleLink * partLink = nullptr );

    /** CLHEP-compatible constructor */
    FaserISFParticle(const HepGeom::Point3D<double>& pos,
                const HepGeom::Vector3D<double>& mom,
                double mass,
                double charge,
                int pdgCode,
                int status,
                double time,
                const FaserISFParticle &parent,
                Barcode::ParticleBarcode barcode = Barcode::fUndefinedBarcode,
                TruthBinding* truth = nullptr,
                const HepMcParticleLink * partLink = nullptr );

    /** this constructor should only be used for event read-in */
    FaserISFParticle(const Amg::Vector3D& pos,
                const Amg::Vector3D& mom,
                double mass,
                double charge,
                int pdgCode,
                int status,
                double time,
                const FaserDetRegionSvcIDPair &origin,
                int bcid,
                Barcode::ParticleBarcode barcode = Barcode::fUndefinedBarcode,
                TruthBinding* truth = nullptr,
                const HepMcParticleLink * partLink = nullptr );

    /** Copy constructor */
    FaserISFParticle(const FaserISFParticle& isfp);

    /** Move copy constructor */
    FaserISFParticle(FaserISFParticle&& isfp);

    /** Destructor */
    ~FaserISFParticle();

    /** Assignment operator */
    FaserISFParticle& operator=(const FaserISFParticle& rhs);

    /** Move assignment operator */
    FaserISFParticle& operator=(FaserISFParticle&& rhs);

    /** Comparisons */
    bool operator==(const FaserISFParticle& rhs) const;
    bool isEqual(const FaserISFParticle& rhs) const;
    bool isIdent(const FaserISFParticle& rhs) const;

    /** The current momentum vector of the ISFParticle */
    const Amg::Vector3D& momentum() const;

    /** The current position of the ISFParticle */
    const Amg::Vector3D& position() const;

    /** Update the position of the ISFParticle */
    void updatePosition(const Amg::Vector3D& position);

    /** Update the momentum of the ISFParticle */
    void updateMomentum(const Amg::Vector3D& momentum);

    /** mass of the particle */
    double mass() const;

    /** charge of the particle */
    double charge() const;

    /** Timestamp of the ISFParticle */
    double timeStamp() const;

    /** PDG value */
    int pdgCode() const;

    /** Kinetic energy */
    double ekin() const;

    /** the particle's full history */
    const FaserParticleHistory& history() const;
    /** the sub-detector ID the particle's originates from */
    FaserDetDescr::FaserRegion  originGeoID() const;
    /** the simulation service the particle originates from */
    SimSvcID                    originSimID() const;
    /** previous geoID that the particle was simulated in */
    FaserDetDescr::FaserRegion  prevGeoID() const;
    /** the simulation service that previously treated this particle */
    SimSvcID                    prevSimID() const;
    /** next geoID the particle will be simulated in */
    FaserDetDescr::FaserRegion  nextGeoID() const;
    /** the next simulation service the particle will be sent to */
    SimSvcID                    nextSimID() const;
    /** register the next FaserDetDescr::FaserRegion */
    void                        setNextGeoID(FaserDetDescr::FaserRegion geoID);
    /** register the next SimSvcID */
    void                        setNextSimID(SimSvcID simID);

    /** the barcode */
    Barcode::ParticleBarcode barcode() const;

    /** set a new barcode */
    void setBarcode(Barcode::ParticleBarcode bc);
    /** set a new barcode and update the HepMcParticleLink  */
    void setBarcodeAndUpdateHepMcParticleLink(Barcode::ParticleBarcode bc);

    /** bunch-crossing identifier */
    int getBCID() const;

    /** set bunch-crossing identifier */
    void setBCID(int bcid);

    /** pointer to the simulation truth - optional, can be 0 */
    TruthBinding* getTruthBinding() const;
    void          setTruthBinding(TruthBinding *truth);

    /** HepMcParticleLink accessors */
    inline const HepMcParticleLink* getParticleLink() const {return  m_partLink;};
    inline void setParticleLink(const HepMcParticleLink* partLink) {m_partLink=partLink;};

    /** return the particle order (eg used to assure ID->Calo->MS simulation order) */
    ParticleOrder  getOrder() const;
    void           setOrder(ParticleOrder order);

    /** get/set ParticleUserInformation */
    ParticleUserInformation *getUserInformation() const;
    void                     setUserInformation(ParticleUserInformation *userInfo);

    /** Dump methods to be used by the overloaded stream operator (inheritance!) */
    MsgStream&    dump(MsgStream& out) const;
    std::ostream& dump(std::ostream& out) const;

    void setStatus(int a) { m_status=a; }
    int status() const { return m_status;}

  private :
    Amg::Vector3D                m_position;
    Amg::Vector3D                m_momentum;
    double                       m_mass;
    double                       m_charge;
    int                          m_pdgCode;
    int                          m_status;
    double                       m_tstamp;
    FaserParticleHistory         m_history;
    Barcode::ParticleBarcode     m_barcode;
    int                          m_bcid;                  //!< bunch-crossing identifier
    TruthBinding*                m_truth;
    ParticleOrder                m_order;                 //!< particle simulation order
    mutable ParticleUserInformation *m_userInfo;          //!< user information stored with the ISFParticle
    const HepMcParticleLink*     m_partLink;
  };

  // Overload of << operator for MsgStream for debug output
  //
  inline MsgStream&    operator << ( MsgStream& sl,    const ISF::FaserISFParticle& isfp) { isfp.dump(sl); return sl; }
  inline std::ostream& operator << ( std::ostream& sl, const ISF::FaserISFParticle& isfp) { isfp.dump(sl); return sl; }

} // end of namespace

/* implementation for inlined methods */
#include <FaserISF_Event/FaserISFParticle.icc>

#endif // FASERISF_EVENT_FASERISFPARTICLE_H
