#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "GeneratorObjects/McEventCollection.h"
#include <TH1.h>
#include <math.h>
// #include <TProfile.h>

/* GenEvent reading example */

class GenEventReadAlg : public AthHistogramAlgorithm
{
    public:
    GenEventReadAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~GenEventReadAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    private:
    TH1* m_cosThetaHist;  
    TH1* m_rVertexHist;
    TH1* m_phiHist;
    TH1* m_rPerpHist;
    // TProfile* m_hprof;

    DoubleProperty m_zOffset { this, "ZOffsetMm", 150.0, "Offset of the generation sphere center" };
    DoubleProperty m_radius  { this, "TargetRadiusMm", 3500.0, "Radius of target sphere in mm" };

    // Read handle keys for data containers
    // Any other event data can be accessed identically
    // Note the key names ("BeamTruthEvent" or "SCT_Hits") are Gaudi properties and can be configured at run-time
    SG::ReadHandleKey<McEventCollection> m_mcEventKey       { this, "McEventCollection", "BeamTruthEvent" };
};