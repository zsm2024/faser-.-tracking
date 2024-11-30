#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// #include "MagFieldInterfaces/IMagFieldSvc.h"
// #include "TrackerAlignGenTools/ITrackerAlignDBTool.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MagField cache
#include "MagFieldConditions/FaserFieldCacheCondObj.h"
#include "MagFieldElements/FaserFieldCache.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Minimalist algorithm to test retrieval of constructed geometry from DetStore

class GeoModelTestAlg : public AthReentrantAlgorithm
{
    public:
    GeoModelTestAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~GeoModelTestAlg();

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

    private:  
    StatusCode testVeto() const;
    StatusCode testVetoNu() const;
    StatusCode testTrigger() const;
    StatusCode testPreshower() const;
    StatusCode testSCT() const;
    StatusCode testField(const EventContext& ctx) const;


    // Independently specify the "right" number of elements, for comparison with what we get from detector store
    Gaudi::Property<int>            m_numVetoStations           {this, "NumVetoStations", 2, "Number of stations in the Veto detector"};
    Gaudi::Property<int>            m_numVetoPlatesPerStation   {this, "NumVetoPlatesPerStation", 2, "Number of plates per station in the Veto detector"};
    Gaudi::Property<int>            m_numVetoPmtsPerPlate       {this, "NumVetoPmtsPerPlate", 1, "Number of pmts per plate in the Veto detector"};

    Gaudi::Property<int>            m_numTriggerStations           {this, "NumTriggerStations", 1, "Number of stations in the Trigger detector"};
    Gaudi::Property<int>            m_numTriggerPlatesPerStation   {this, "NumTriggerPlatesPerStation", 2, "Number of plates per station in the Trigger detector"};
    Gaudi::Property<int>            m_numTriggerPmtsPerPlate       {this, "NumTriggerPmtsPerPlate", 2, "Number of pmts per plate in the Trigger detector"};

    Gaudi::Property<int>            m_numPreshowerStations           {this, "NumPreshowerStations", 1, "Number of stations in the Preshower detector"};
    Gaudi::Property<int>            m_numPreshowerPlatesPerStation   {this, "NumPreshowerPlatesPerStation", 2, "Number of plates per station in the Preshower detector"};
    Gaudi::Property<int>            m_numPreshowerPmtsPerPlate       {this, "NumPreshowerPmtsPerPlate", 1, "Number of pmts per plate in the Preshower detector"};

    Gaudi::Property<int>            m_numVetoNuStations           {this, "NumVetoNuStations", 1, "Number of stations in the VetoNu detector"};
    Gaudi::Property<int>            m_numVetoNuPlatesPerStation   {this, "NumVetoNuPlatesPerStation", 2, "Number of plates per station in the VetoNu detector"};
    Gaudi::Property<int>            m_numVetoNuPmtsPerPlate       {this, "NumVetoNuPmtsPerPlate", 1, "Number of pmts per plate in the VetoNu detector"};

    Gaudi::Property<int>            m_firstSctStation           {this, "FirstSCTStation", 1, "Identifier of the first SCT station (0 w/FaserNu, 1 otherwise)"};
    Gaudi::Property<int>            m_lastSctStation            {this, "LastSCTStation", 3, "Identifier of the last SCT station (normally 3)"};
    Gaudi::Property<int>            m_numSctPlanesPerStation    {this, "NumSCTPlanesPerStation", 3, "Number of planes per station in the SCT detector"};
    Gaudi::Property<int>            m_numSctRowsPerPlane        {this, "NumSCTRowsPerPlane", 4, "Number of rows of modules per plane in the SCT detector"};
    Gaudi::Property<int>            m_numSctModulesPerRow       {this, "NumSCTModulesPerRow", 2, "Number of modules per row in the SCT detector"};
    Gaudi::Property<int>            m_numSctStripsPerSensor     {this, "NumSCTStripsPerSensor", 768,"Number of readout strips per sensor in the SCT detector"};
    Gaudi::Property<bool>           m_printSctIdentifiers       {this, "PrintSctIDs", false, "Flag to print raw wafer identifiers"};

    int m_numSctStations;
    // ServiceHandle<MagField::IMagFieldSvc> m_field { this, "FieldService", "FaserFieldSvc" };
    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<FaserFieldCacheCondObj> m_fieldCondObjInputKey {this, "FaserFieldCacheCondObj", "fieldCondObj",
                                                                        "Name of the Magnetic Field conditions object key"};


    // ToolHandle<ITrackerAlignDBTool> m_alignTool { this, "AlignDbTool", "TrackerAlignDBTool" };
};