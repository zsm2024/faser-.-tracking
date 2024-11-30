#include "GenEventReadAlg.h"
#ifdef HEPMC3
#include "HepMC3/PrintStreams.h"
#endif

GenEventReadAlg::GenEventReadAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthHistogramAlgorithm(name, pSvcLocator) { m_cosThetaHist = nullptr; m_rVertexHist = nullptr; }

GenEventReadAlg::~GenEventReadAlg() { }

StatusCode GenEventReadAlg::initialize()
{
    // initialize a histogram 
    // letter at end of TH1 indicated variable type (D double, F float etc)
    m_cosThetaHist = new TH1D("CosTheta", "Cosine of ZenithAngle; cos #q; Events/bin", 20, 0., 1.); //first string is root object name, second is histogram title
    m_rVertexHist = new TH1D("RVertex", "Vertex Radius; r (mm); Events/bin", 40, 0, m_radius * 1.2);
    m_phiHist = new TH1D("Phi", "Azimuth; phi; Events/bin", 40, -4*atan(1), 4*atan(1));
    m_rPerpHist = new TH1D("rPerp", "Transverse radius squared; r^2 (mm); Events/bin", 40, 0, pow(m_radius, 2) * 1.2);

    ATH_CHECK(histSvc()->regHist("/HIST/coshist", m_cosThetaHist));
    ATH_CHECK(histSvc()->regHist("/HIST/rhist", m_rVertexHist));
    ATH_CHECK(histSvc()->regHist("/HIST/phihist", m_phiHist));
    ATH_CHECK(histSvc()->regHist("/HIST/rperphist", m_rPerpHist));

    // initialize data handle keys
    ATH_CHECK( m_mcEventKey.initialize() );
    ATH_MSG_INFO( "Using GenEvent collection with key " << m_mcEventKey.key());
    return StatusCode::SUCCESS;
}

StatusCode GenEventReadAlg::execute()
{
    // Handles created from handle keys behave like pointers to the corresponding container
    SG::ReadHandle<McEventCollection> h_mcEvents(m_mcEventKey);
    ATH_MSG_INFO("Read McEventContainer with " << h_mcEvents->size() << " events");
    if (h_mcEvents->size() == 0) return StatusCode::FAILURE;
    const HepMC::GenEvent* theEvent = h_mcEvents->at(0);
    // if (theEvent->particles_size() != 1 && theEvent->vertices_size() != 1) return StatusCode::FAILURE;
#ifdef HEPMC3
    msg(MSG::INFO) << *theEvent << endmsg;
    const HepMC::ConstGenParticlePtr& pHat {theEvent->particles().front()};
    for (auto p : theEvent->particles())
    {
        m_cosThetaHist->Fill(-p->momentum().y()/p->momentum().rho());
        m_phiHist->Fill(atan2(p->momentum().x(), p->momentum().z()));
    }

    for (auto v : theEvent->vertices())
    {
        HepMC::FourVector vOffset { v->position().x() , v->position().y(), v->position().z() - m_zOffset, v->position().t() };
        m_rVertexHist->Fill(vOffset.rho());
        m_rPerpHist->Fill(pow(vOffset.rho(), 2) - pow((vOffset.x() * pHat->momentum().x() +
                                                       vOffset.y() * pHat->momentum().y() + 
                                                       vOffset.z() * pHat->momentum().z())/pHat->momentum().rho(), 2));
    }
#else
    const HepMC::GenParticle* pHat { nullptr };
    auto pParticles    = theEvent->particles_begin();
    auto pParticlesEnd = theEvent->particles_end();
    for (; pParticles != pParticlesEnd ; ++pParticles)
    {
        const HepMC::GenParticle* p = *pParticles;
        if (pHat == nullptr) pHat = p;
        m_cosThetaHist->Fill(-p->momentum().y()/p->momentum().rho());
        m_phiHist->Fill(atan2(p->momentum().x(), p->momentum().z()));
    }
    auto pVertices    = theEvent->vertices_begin();
    auto pVerticesEnd = theEvent->vertices_end();
    for (; pVertices != pVerticesEnd; ++pVertices)
    {
        const HepMC::GenVertex* v = *pVertices;
        HepMC::FourVector vOffset { v->position().x() , v->position().y(), v->position().z() - m_zOffset, v->position().t() };
        m_rVertexHist->Fill(vOffset.rho());
        m_rPerpHist->Fill(pow(vOffset.rho(), 2) - pow((vOffset.x() * pHat->momentum().x() +
                                                       vOffset.y() * pHat->momentum().y() + 
                                                       vOffset.z() * pHat->momentum().z())/pHat->momentum().rho(), 2));
    }
#endif

    return StatusCode::SUCCESS;
}

StatusCode GenEventReadAlg::finalize()
{
    return StatusCode::SUCCESS;
}