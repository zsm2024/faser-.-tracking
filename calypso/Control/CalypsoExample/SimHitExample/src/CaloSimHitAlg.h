#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "GeneratorObjects/McEventCollection.h"
#include "FaserCaloSimEvent/CaloHitCollection.h"
#include <TH1.h>

/* CaloSimHit reading example - Carl Gwilliam + Lottie Cavanagh, Liverpool */

class CaloSimHitAlg : public AthHistogramAlgorithm
{
    public:
    CaloSimHitAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~CaloSimHitAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    private:
    TH1* m_eloss;    
    TH2* m_module;
    TH2* m_modulePos;
    TH1* m_elossTot; 
 
    TH1* m_meanTime;
    TH1* m_weightedTime;

    // add new histograms
    // TL = top left calorimeter module
    // TR = top right calorimeter module
    // BL = bottom left calorimeter module
    // BR = bottom right calorimeter module 

    TH1* m_elossTL; // TL
    TH1* m_elossTR; // TR
    TH1* m_elossBR; // BR
    TH1* m_elossBL; // BL
    TH1* m_elossTLTot;
    TH1* m_elossTRTot;
    TH1* m_elossBRTot;
    TH1* m_elossBLTot;    

    // add in fractional histograms

    TH1* m_eloss_frac;
    TH1* m_elossTL_frac;
    TH1* m_elossTR_frac;
    TH1* m_elossBR_frac; 
    TH1* m_elossBL_frac; 
    TH1* m_elossTot_frac;
    TH1* m_elossTLTot_frac;
    TH1* m_elossTRTot_frac;
    TH1* m_elossBRTot_frac;
    TH1* m_elossBLTot_frac;  

    // Read handle keys for data containers
    // Any other event data can be accessed identically
    // Note the key names ("BeamTruthEvent" or "SCT_Hits") are Gaudi properties and can be configured at run-time
    SG::ReadHandleKey<McEventCollection> m_mcEventKey       { this, "McEventCollection", "BeamTruthEvent" };
    SG::ReadHandleKey<CaloHitCollection> m_faserCaloHitKey { this, "CaloHitCollection", "EcalHits" };
};
