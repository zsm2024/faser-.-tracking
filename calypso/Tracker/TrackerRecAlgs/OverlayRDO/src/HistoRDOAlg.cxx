#include "HistoRDOAlg.h"
#include "TrkTrack/Track.h"
#include <TTree.h>


HistoRDOAlg::HistoRDOAlg(const std::string &name, 
                         ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator), 
      AthHistogramming(name),
      m_histSvc("THistSvc/THistSvc", name) {}

StatusCode HistoRDOAlg::initialize()
{
    m_tree = new TTree("overlay", "Overlay tree");

    m_tree->Branch("longTracks1", &m_longTracks1, "longTracks1/I");
    m_tree->Branch("x1", &m_x1);
    m_tree->Branch("y1", &m_y1);
    m_tree->Branch("p1", &m_p1);
    m_tree->Branch("theta1", &m_theta1);
    m_tree->Branch("phi1", &m_phi1);
    m_tree->Branch("charge1", &m_charge1);
    m_tree->Branch("chiSquared1", &m_chiSquared1);
    m_tree->Branch("nDoF1", &m_nDoF1);
    m_tree->Branch("pPolar1", &m_pTotPolar1, "pPolar1/D");

    m_tree->Branch("longTracks2", &m_longTracks2, "longTracks2/I");
    m_tree->Branch("x2", &m_x2);
    m_tree->Branch("y2", &m_y2);
    m_tree->Branch("p2", &m_p2);
    m_tree->Branch("theta2", &m_theta2);
    m_tree->Branch("phi2", &m_phi2);
    m_tree->Branch("charge2", &m_charge2);
    m_tree->Branch("chiSquared2", &m_chiSquared2);
    m_tree->Branch("nDoF2", &m_nDoF2);
    m_tree->Branch("pPolar2", &m_pTotPolar2, "pPolar2/D");

    m_tree->Branch("yAgreement", &m_yAgreement, "yAgreement/D");
    m_tree->Branch("matchFraction", &m_matchFraction, "matchFraction/D");

    ATH_CHECK(histSvc()->regTree("/HIST2/tree", m_tree));

    ATH_CHECK(m_trackCollection1.initialize());
    ATH_CHECK(m_trackCollection2.initialize());

    return StatusCode::SUCCESS;
}

StatusCode HistoRDOAlg::execute(const EventContext& ctx) const
{

    m_longTracks1 = 0;
    m_x1.clear();
    m_y1.clear();
    m_p1.clear();
    m_theta1.clear();
    m_phi1.clear();
    m_charge1.clear();
    m_chiSquared1.clear();
    m_nDoF1.clear();
    m_pTotPolar1 = 0;

    m_longTracks2 = 0;
    m_x2.clear();
    m_y2.clear();
    m_p2.clear();
    m_theta2.clear();
    m_phi2.clear();
    m_charge2.clear();
    m_chiSquared2.clear();
    m_nDoF2.clear();
    m_pTotPolar2 = 0;

    m_yAgreement = 0;
    m_matchFraction = 0;

    SG::ReadHandle<TrackCollection> trackCollection1 {m_trackCollection1, ctx};
    // ATH_CHECK(trackCollection1.isValid());
    SG::ReadHandle<TrackCollection> trackCollection2 {m_trackCollection2, ctx};
    // ATH_CHECK(trackCollection2.isValid());

    std::vector<const Trk::TrackParameters*> upstreamSingle {};
    if (trackCollection1.isValid())
    {
        Amg::Vector3D pTot {0, 0, 0};
        for (const Trk::Track* track : *trackCollection1)
        {
            if (track == nullptr) continue;
            m_longTracks1++;
            const Trk::TrackParameters* upstreamParameters {nullptr};
            for (auto params : *(track->trackParameters()))
            {
                if (params->position().z() < 0) continue;  // Ignore IFT hits
                if (upstreamParameters == nullptr || params->position().z() < upstreamParameters->position().z()) upstreamParameters = params;
                ATH_MSG_VERBOSE("TrackCollection 1, Track " << m_longTracks1 << ", z = " << params->position().z() << ", y = " << params->position().y() << ", q/p(TeV) = " << params->charge()/(params->momentum().mag()/1e6));
            }
            upstreamSingle.push_back(upstreamParameters);
            m_x1.push_back(upstreamParameters->position().x());
            m_y1.push_back(upstreamParameters->position().y());
            m_p1.push_back(upstreamParameters->momentum().mag());
            m_charge1.push_back(upstreamParameters->charge());
            m_theta1.push_back(asin(upstreamParameters->momentum().perp()/upstreamParameters->momentum().mag()));
            m_phi1.push_back(atan2(upstreamParameters->momentum().y(), upstreamParameters->momentum().x()));
            m_chiSquared1.push_back(track->fitQuality()->chiSquared());
            m_nDoF1.push_back(track->fitQuality()->doubleNumberDoF());
            pTot += upstreamParameters->momentum();
        }
        m_pTotPolar1 = asin(pTot.perp()/pTot.mag());
    }

    std::vector<const Trk::TrackParameters*> upstreamOverlay {};
    if (trackCollection2.isValid())
    {
        Amg::Vector3D pTot {0, 0, 0};
        for (const Trk::Track* track : *trackCollection2)
        {
            if (track == nullptr) continue;
            m_longTracks2++;
            const Trk::TrackParameters* upstreamParameters {nullptr};
            for (auto params : *(track->trackParameters()))
            {
                if (params->position().z() < 0) continue;  // Ignore IFT hits
                if (upstreamParameters == nullptr || params->position().z() < upstreamParameters->position().z()) upstreamParameters = params;
                ATH_MSG_VERBOSE("TrackCollection 2, Track " << m_longTracks2 << ", z = " << params->position().z()<< ", y = " << params->position().y()<< ", q/p(TeV) = " << params->charge()/(params->momentum().mag()/1e6));
            }
            upstreamOverlay.push_back(upstreamParameters);
            m_x2.push_back(upstreamParameters->position().x());
            m_y2.push_back(upstreamParameters->position().y());
            m_p2.push_back(upstreamParameters->momentum().mag());
            m_charge2.push_back(upstreamParameters->charge());
            m_theta2.push_back(asin(upstreamParameters->momentum().perp()/upstreamParameters->momentum().mag()));
            m_phi2.push_back(atan2(upstreamParameters->momentum().y(), upstreamParameters->momentum().x()));
            m_chiSquared2.push_back(track->fitQuality()->chiSquared());
            m_nDoF2.push_back(track->fitQuality()->doubleNumberDoF());
            pTot += upstreamParameters->momentum();
        }
        m_pTotPolar2 = asin(pTot.perp()/pTot.mag());
    }

    if (trackCollection1.isValid() && trackCollection2.isValid() && m_longTracks2 ==2)
    {
        double qOverPSingle1 = upstreamSingle[0]->charge()/upstreamSingle[0]->momentum().mag();
        double qOverPCovSingle1 = (*upstreamSingle[0]->covariance())(4,4);
        double qOverPSingle2 = upstreamSingle[1]->charge()/upstreamSingle[1]->momentum().mag();
        double qOverPCovSingle2 = (*upstreamSingle[1]->covariance())(4,4);
        double qOverPOverlay1 = upstreamOverlay[0]->charge()/upstreamOverlay[0]->momentum().mag();
        double qOverPCovOverlay1 = (*upstreamOverlay[0]->covariance())(4,4);
        double qOverPOverlay2 = upstreamOverlay[1]->charge()/upstreamOverlay[1]->momentum().mag();
        double qOverPCovOverlay2 = (*upstreamOverlay[1]->covariance())(4,4);

        double agreementDirect = pow(qOverPSingle1 - qOverPOverlay1, 2) / (qOverPCovSingle1 + qOverPCovOverlay1) +
                                 pow(qOverPSingle2 - qOverPOverlay2, 2) / (qOverPCovSingle2 + qOverPCovOverlay2);
        double agreementSwap   = pow(qOverPSingle1 - qOverPOverlay2, 2) / (qOverPCovSingle1 + qOverPCovOverlay2) +
                                 pow(qOverPSingle2 - qOverPOverlay1, 2) / (qOverPCovSingle2 + qOverPCovOverlay1);

        const Trk::Track* firstSingle {nullptr};
        const Trk::Track* firstOverlay {nullptr};
        const Trk::Track* secondSingle {nullptr};
        const Trk::Track* secondOverlay {nullptr};
        if (agreementDirect <= agreementSwap)
        {
            firstSingle = (*trackCollection1)[0];
            firstOverlay = (*trackCollection2)[0];
            secondSingle = (*trackCollection1)[1];
            secondOverlay = (*trackCollection2)[1];
            ATH_MSG_VERBOSE("Matched q/p = " << qOverPSingle1 << " with " << qOverPOverlay1 << ", and q/p = " << qOverPSingle2 << " with " << qOverPOverlay2);
        }
        else
        {
            firstSingle = (*trackCollection1)[0];
            firstOverlay = (*trackCollection2)[1];
            secondSingle = (*trackCollection1)[1];
            secondOverlay = (*trackCollection2)[0];
            ATH_MSG_VERBOSE("Matched q/p = " << qOverPSingle1 << " with " << qOverPOverlay2 << ", and q/p = " << qOverPSingle2 << " with " << qOverPOverlay1);
        }
        int nZMatch {0};
        auto singleParameters = firstSingle->trackParameters();
        auto overlayParameters = firstOverlay->trackParameters();
        for (size_t iSingle = 0, iOverlay = 0; (iSingle < singleParameters->size()) && (iOverlay < overlayParameters->size());)
        {
            auto singleState = (*singleParameters)[iSingle];
            auto overlayState = (*overlayParameters)[iOverlay];
            auto singleCov = singleState->covariance();
            auto overlayCov = overlayState->covariance();
            if (abs(singleState->position().z() - overlayState->position().z()) < 0.03)
            {
                nZMatch++;
                m_yAgreement += pow(singleState->position().y() - overlayState->position().y(), 2)/((*singleCov)(1,1) + (*overlayCov)(1,1));
                iSingle++;
                iOverlay++;
                ATH_MSG_VERBOSE("Matched (1): " << singleState->position().z() << " and " << overlayState->position().z());
            }
            else if (singleState->position().z() < overlayState->position().z())
            {
                ATH_MSG_VERBOSE("z not matched (1): " << singleState->position().z() << " vs " << overlayState->position().z());
                iSingle++;
            }
            else if (overlayState->position().z() < singleState->position().z())
            {
                ATH_MSG_VERBOSE("z not matched (1): " << singleState->position().z() << " vs " << overlayState->position().z());
                iOverlay++;
            }
        }
        double matchFract1 { ((double) nZMatch)/singleParameters->size() };
        ATH_MSG_VERBOSE("MatchFract1: " << matchFract1);
        nZMatch = 0;
        singleParameters = secondSingle->trackParameters();
        overlayParameters = secondOverlay->trackParameters();
        for (size_t iSingle = 0, iOverlay = 0; (iSingle < singleParameters->size()) && (iOverlay < overlayParameters->size());)
        {
            auto singleState = (*singleParameters)[iSingle];
            auto overlayState = (*overlayParameters)[iOverlay];
            auto singleCov = singleState->covariance();
            auto overlayCov = overlayState->covariance();
            if (abs(singleState->position().z() - overlayState->position().z()) < 0.03)
            {
                nZMatch++;
                m_yAgreement += pow(singleState->position().y() - overlayState->position().y(), 2)/((*singleCov)(1,1) + (*overlayCov)(1,1));
                iSingle++;
                iOverlay++;
                ATH_MSG_VERBOSE("Matched (2): " << singleState->position().z() << " and " << overlayState->position().z());
            }
            else if (singleState->position().z() < overlayState->position().z())
            {
                ATH_MSG_VERBOSE("z not matched (2): " << singleState->position().z() << " vs " << overlayState->position().z());
                iSingle++;
            }
            else if (overlayState->position().z() < singleState->position().z())
            {
                ATH_MSG_VERBOSE("z not matched (2): " << singleState->position().z() << " vs " << overlayState->position().z());
                iOverlay++;
            }
        }
        double matchFract2 { ((double) nZMatch)/singleParameters->size() };
        ATH_MSG_VERBOSE("MatchFract2: " << matchFract2);
        m_matchFraction = std::min(matchFract1, matchFract2);
        ATH_MSG_VERBOSE("MatchFraction: " << m_matchFraction);
    }

    m_tree->Fill();
    return StatusCode::SUCCESS;
}

StatusCode HistoRDOAlg::finalize()
{
    return StatusCode::SUCCESS;
}