#include "GeoModelTestAlg.h"

#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GeoModelFaserUtilities/GeoModelExperiment.h"

#include "ScintReadoutGeometry/VetoDetectorManager.h"
#include "ScintReadoutGeometry/VetoNuDetectorManager.h"
#include "ScintReadoutGeometry/TriggerDetectorManager.h"
#include "ScintReadoutGeometry/PreshowerDetectorManager.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"

#include "ScintReadoutGeometry/ScintDetectorElement.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

#include "ScintIdentifier/VetoID.h"
#include "ScintIdentifier/VetoNuID.h"
#include "ScintIdentifier/TriggerID.h"
#include "ScintIdentifier/PreshowerID.h"
#include "TrackerIdentifier/FaserSCT_ID.h"

#include "GaudiKernel/PhysicalConstants.h"

GeoModelTestAlg::GeoModelTestAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthReentrantAlgorithm(name, pSvcLocator), m_numSctStations {0}
{ }

GeoModelTestAlg::~GeoModelTestAlg() { }

StatusCode GeoModelTestAlg::initialize()
{
    m_numSctStations = std::max(0, m_lastSctStation - m_firstSctStation + 1);
    // ATH_CHECK(m_field.retrieve());
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ATH_CHECK( m_fieldCondObjInputKey.initialize() );
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ATH_CHECK(m_alignTool.retrieve());
    return StatusCode::SUCCESS;
}

StatusCode GeoModelTestAlg::execute(const EventContext& ctx) const
{   
    GeoModelExperiment* theExpt = nullptr;
    std::vector<std::string> listOfManagers;
    ATH_CHECK(detStore()->retrieve(theExpt, "FASER"));
    if (theExpt != nullptr)
    {
        ATH_MSG_ALWAYS("Retrieved top-level FASER experiment geometry from DetStore.");
        listOfManagers = theExpt->getListOfManagers();
        ATH_MSG_ALWAYS("Experiment has defined " << listOfManagers.size() << " detector managers:");
        for (auto mgr : listOfManagers)
        {
            const GeoVDetectorManager* pMgr = nullptr;
            pMgr = theExpt->getManager(mgr);
            if (pMgr != nullptr)
            {
                ATH_MSG_ALWAYS("Retrieved (generic) manager " << mgr << " from top-level experiment.");
            }
            else
            {
                ATH_MSG_FATAL("Failed to retrieve manager " << mgr);
                return StatusCode::FAILURE;
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve top-level FASER experiment geometry from DetStore.");
        return StatusCode::FAILURE;
    }

    ATH_CHECK(testVeto());

    if (m_numVetoNuStations > 0) ATH_CHECK(testVetoNu());

    ATH_CHECK(testTrigger());

    ATH_CHECK(testPreshower());

    ATH_CHECK(testSCT());

    ATH_CHECK(testField(ctx));

    // ATH_CHECK(m_alignTool->createDB());
    // ATH_CHECK(m_alignTool->sortTrans());
    // ATH_CHECK(m_alignTool->outputObjs());
    // ATH_CHECK(m_alignTool->fillDB("",1,0,9999999,9999999));

    return StatusCode::SUCCESS;
}

StatusCode GeoModelTestAlg::testField(const EventContext& ctx) const
{
    MagField::FaserFieldCache    fieldCache;
    // Get field cache object
    SG::ReadCondHandle<FaserFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
    const FaserFieldCacheCondObj* fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve FaserFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
        return StatusCode::FAILURE;
    }
    fieldCondObj->getInitializedCache (fieldCache);


    int iPhi = 0;
    const double r = 15.0; // millimeters
    for (int z = -1700; z <= 2600; z+=50)
    {
        double phi = 15.0 * iPhi++;
        double x = r * cos(phi * Gaudi::Units::pi/180.0);
        double y = r * sin(phi * Gaudi::Units::pi/180.0);

        double pos[] { x , y, (double)z };
        double field[] { 0., 0., 0. };
        fieldCache.getField(pos, field);
        ATH_MSG_ALWAYS("Field at (" << (int) x << "," << (int) y << "," << z << ") = (" << field[0]*1000 << ", " << field[1]*1000 << ", " << field[2]*1000 << ") Tesla");
    }
    return StatusCode::SUCCESS;
}

StatusCode GeoModelTestAlg::testSCT() const
{
    // Test retrieval of helper object directly from store
    const FaserSCT_ID* helper = nullptr;
    ATH_CHECK(detStore()->retrieve(helper, "FaserSCT_ID"));
    if (helper != nullptr)
    {
        // Test neighbors with helper function
        const IdContext& context = helper->wafer_context();
        ATH_MSG_ALWAYS("Retrieved FaserSCT_ID helper from DetStore.");

        // Print list of identifiers
        if (m_printSctIdentifiers)
        {
            for (FaserSCT_ID::const_id_iterator it = helper->wafer_begin(); it != helper->wafer_end(); ++it)
            {
                ATH_MSG_ALWAYS("Wafer ID: " << (it->get_compact() >>32));
            }
        }
        for (int iStation = m_firstSctStation; iStation <= m_lastSctStation; iStation++)
        {
            // if (m_numSctStations%2 == 0 && iStation == 0) continue;
            for (int iPlane = 0; iPlane < m_numSctPlanesPerStation; iPlane++)
            {
                for (int iRow = 0; iRow < m_numSctRowsPerPlane; iRow++)
                {
                    for (int iModule = -m_numSctModulesPerRow/2; iModule <= m_numSctModulesPerRow/2; iModule++)
                    {
                        if (m_numSctModulesPerRow%2 == 0 && iModule == 0) continue;
                        for (int iSensor = 0; iSensor < 2; iSensor++)
                        {
                            Identifier thisId = helper->wafer_id(iStation, iPlane, iRow, iModule, iSensor, true);
                            IdentifierHash thisHash = helper->wafer_hash(thisId);
                            IdentifierHash prevHash;
                            IdentifierHash nextHash;
                            IdentifierHash flipHash;
                            Identifier prevId;
                            Identifier nextId;
                            Identifier flipId;
                            const int INVALID = std::numeric_limits<int>::min();
                            int prevPhiStation { INVALID };
                            int nextPhiStation { INVALID };
                            int prevPhiPlane   { INVALID };
                            int nextPhiPlane   { INVALID };
                            int prevPhiRow     { INVALID };
                            int nextPhiRow     { INVALID };
                            int prevPhiModule  { INVALID };
                            int nextPhiModule  { INVALID };
                            int prevPhiSensor  { INVALID };
                            int nextPhiSensor  { INVALID };
                            int prevPhiStat = helper->get_prev_in_phi(thisHash, prevHash);
                            if (prevPhiStat == 0) 
                            {
                                IdentifierHash testHash;
                                int nextPhiStat = helper->get_next_in_phi(prevHash, testHash);
                                if (nextPhiStat != 0 || testHash != thisHash)
                                {
                                    ATH_MSG_FATAL("Next SCT module in phi (" << testHash << ") of previous in phi (" << prevHash << ") is not the original (" << thisHash <<")" );
                                    return StatusCode::FAILURE;
                                }
                                prevPhiStat = helper->get_id(prevHash, prevId, &context);
                                if (prevPhiStat == 0)
                                {
                                    prevPhiStation = helper->station(prevId);
                                    prevPhiPlane   = helper->layer(prevId);
                                    prevPhiRow     = helper->phi_module(prevId);
                                    prevPhiModule  = helper->eta_module(prevId);
                                    prevPhiSensor  = helper->side(prevId);
                                }
                            }
                            int nextPhiStat = helper->get_next_in_phi(thisHash, nextHash);
                            if (nextPhiStat == 0) 
                            {
                                IdentifierHash testHash;
                                prevPhiStat = helper->get_prev_in_phi(nextHash, testHash);
                                if (prevPhiStat != 0 || testHash != thisHash)
                                {
                                    ATH_MSG_FATAL("Previous SCT Module in phi (" << testHash << ") of next in phi (" << nextHash << ") is not the original (" << thisHash <<")" );
                                    return StatusCode::FAILURE;
                                }
                                nextPhiStat = helper->get_id(nextHash, nextId, &context);
                                if (nextPhiStat == 0)
                                {
                                    nextPhiStation = helper->station(nextId);
                                    nextPhiPlane   = helper->layer(nextId);
                                    nextPhiRow     = helper->phi_module(nextId);
                                    nextPhiModule  = helper->eta_module(nextId);
                                    nextPhiSensor  = helper->side(nextId);
                                }
                            }
                            int prevEtaStation { INVALID };
                            int nextEtaStation { INVALID };
                            int prevEtaPlane   { INVALID };
                            int nextEtaPlane   { INVALID };
                            int prevEtaRow     { INVALID };
                            int nextEtaRow     { INVALID };
                            int prevEtaModule  { INVALID };
                            int nextEtaModule  { INVALID };
                            int prevEtaSensor  { INVALID };
                            int nextEtaSensor  { INVALID };
                            int prevEtaStat = helper->get_prev_in_eta(thisHash, prevHash);
                            if (prevEtaStat == 0) 
                            {
                                IdentifierHash testHash;
                                int nextEtaStat = helper->get_next_in_eta(prevHash, testHash);
                                if (nextEtaStat != 0 || testHash != thisHash)
                                {
                                    ATH_MSG_FATAL("Next SCT module in eta (" << testHash << ") of previous in eta (" << prevHash << ") is not the original (" << thisHash <<")" );
                                    return StatusCode::FAILURE;
                                }
                                prevEtaStat = helper->get_id(prevHash, prevId, &context);
                                if (prevEtaStat == 0)
                                {
                                    prevEtaStation = helper->station(prevId);
                                    prevEtaPlane   = helper->layer(prevId);
                                    prevEtaRow     = helper->phi_module(prevId);
                                    prevEtaModule  = helper->eta_module(prevId);
                                    prevEtaSensor  = helper->side(prevId);
                                }
                            }
                            int nextEtaStat = helper->get_next_in_eta(thisHash, nextHash);
                            if (nextEtaStat == 0) 
                            {
                                IdentifierHash testHash;
                                prevEtaStat = helper->get_prev_in_eta(nextHash, testHash);
                                if (prevEtaStat != 0 || testHash != thisHash)
                                {
                                    ATH_MSG_FATAL("Previous SCT Module in eta (" << testHash << ") of next in eta (" << nextHash << ") is not the original (" << thisHash <<")" );
                                    return StatusCode::FAILURE;
                                }
                                nextEtaStat = helper->get_id(nextHash, nextId, &context);
                                if (nextEtaStat == 0)
                                {
                                    nextEtaStation = helper->station(nextId);
                                    nextEtaPlane   = helper->layer(nextId);
                                    nextEtaRow     = helper->phi_module(nextId);
                                    nextEtaModule  = helper->eta_module(nextId);
                                    nextEtaSensor  = helper->side(nextId);
                                }
                            }
                            int flipStation { INVALID };
                            int flipPlane   { INVALID };
                            int flipRow     { INVALID };
                            int flipModule  { INVALID };
                            int flipSensor  { INVALID };
                            int flipStat = helper->get_other_side(thisHash, flipHash);
                            if (flipStat == 0) 
                            {
                                IdentifierHash testHash;
                                int flipStat = helper->get_other_side(flipHash, testHash);
                                if (flipStat != 0 || testHash != thisHash)
                                {
                                    ATH_MSG_FATAL("Flip side of SCT sensor (" << testHash << ") of flip side (" << flipHash << ") is not the original (" << thisHash <<")" );
                                    return StatusCode::FAILURE;
                                }
                                flipStat = helper->get_id(flipHash, prevId, &context);
                                if (flipStat == 0)
                                {
                                    flipStation = helper->station(prevId);
                                    flipPlane   = helper->layer(prevId);
                                    flipRow     = helper->phi_module(prevId);
                                    flipModule  = helper->eta_module(prevId);
                                    flipSensor  = helper->side(prevId);
                                }
                            }
                            else
                            {
                                ATH_MSG_FATAL("SCT sensor " << thisHash << " has no other side!");
                                return StatusCode::FAILURE;
                            }
                            ATH_MSG_ALWAYS("Sct Sensor " << iStation << "/" << iPlane << "/" << iRow << "/" << iModule << "/" << iSensor <<
                                        " (" << thisHash << ") " <<
                                         ": prev phi = " << (prevPhiStation == INVALID ? "-" : std::to_string(prevPhiStation)) << "/" << 
                                                        (prevPhiPlane == INVALID ? "-" : std::to_string(prevPhiPlane)) << "/" << 
                                                        (prevPhiRow == INVALID ? "-": std::to_string(prevPhiRow)) << "/" << 
                                                        (prevPhiModule == INVALID ? "-" : std::to_string(prevPhiModule)) << "/" <<
                                                        (prevPhiSensor == INVALID ? "-" : std::to_string(prevPhiSensor)) <<
                                        " , next phi = " << (nextPhiStation == INVALID ? "-" : std::to_string(nextPhiStation)) << "/" << 
                                                        (nextPhiPlane == INVALID ? "-" : std::to_string(nextPhiPlane)) << "/" << 
                                                        (nextPhiRow == INVALID ? "-": std::to_string(nextPhiRow)) << "/" << 
                                                        (nextPhiModule == INVALID ? "-" : std::to_string(nextPhiModule)) << "/" <<
                                                        (nextPhiSensor == INVALID ? "-" : std::to_string(nextPhiSensor)) <<
                                       " , prev eta = " << (prevEtaStation == INVALID ? "-" : std::to_string(prevEtaStation)) << "/" << 
                                                        (prevEtaPlane == INVALID ? "-" : std::to_string(prevEtaPlane)) << "/" << 
                                                        (prevEtaRow == INVALID ? "-": std::to_string(prevEtaRow)) << "/" << 
                                                        (prevEtaModule == INVALID ? "-" : std::to_string(prevEtaModule)) << "/" <<
                                                        (prevEtaSensor == INVALID ? "-" : std::to_string(prevEtaSensor)) <<
                                        " , next eta = " << (nextEtaStation == INVALID ? "-" : std::to_string(nextEtaStation)) << "/" << 
                                                        (nextEtaPlane == INVALID ? "-" : std::to_string(nextEtaPlane)) << "/" << 
                                                        (nextEtaRow == INVALID ? "-": std::to_string(nextEtaRow)) << "/" << 
                                                        (nextEtaModule == INVALID ? "-" : std::to_string(nextEtaModule)) << "/" <<
                                                        (nextEtaSensor == INVALID ? "-" : std::to_string(nextEtaSensor)) <<
                                        " , other side = " << flipStation << "/" <<
                                                              flipPlane << "/" <<
                                                              flipRow << "/" <<
                                                              flipModule << "/" <<
                                                              flipSensor
                                                        );

                        }
                    }
                }
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve FaserSCT_ID helper from DetStore.");
        return StatusCode::FAILURE;
    }

    // Test direct retrieval of typed managers from DetStore
    const TrackerDD::SCT_DetectorManager* sctMgr = nullptr;
    ATH_CHECK(detStore()->retrieve(sctMgr, "SCT"));
    if (sctMgr != nullptr)
    {
        ATH_MSG_ALWAYS("Retrieved (typed) SCT detector manager with " << sctMgr->getNumTreeTops() << " treetops directly from DetStore.");
        // Compare numerology with the "right" answers from our properties
        if (sctMgr->numerology().numStations() != m_numSctStations || sctMgr->getNumTreeTops() != (size_t) m_numSctStations)
        {
            ATH_MSG_FATAL("Disagreement in number of SCT stations.");
            return StatusCode::FAILURE;
        }
        if (sctMgr->numerology().numLayers() != m_numSctPlanesPerStation)
        {
            ATH_MSG_FATAL("Disagreement in number of layers per SCT station.");
            return StatusCode::FAILURE;
        }
        if (sctMgr->numerology().numPhiModulesForLayer() != m_numSctRowsPerPlane)
        {
            ATH_MSG_FATAL("Disagreement in number of rows per SCT plane.");
            return StatusCode::FAILURE;
        }
        if (sctMgr->numerology().numEtaModulesForLayer() != m_numSctModulesPerRow)
        {
            ATH_MSG_FATAL("Disagreement in number of modules per SCT row.");
            return StatusCode::FAILURE;
        }
        if (sctMgr->numerology().maxNumStrips() != m_numSctStripsPerSensor)
        {
            ATH_MSG_FATAL("Disagreement in number of strips per SCT sensor.");
            return StatusCode::FAILURE;
        }

        // Test detector elements
        const TrackerDD::SiDetectorElementCollection* elements = sctMgr->getDetectorElementCollection();
        if (elements == nullptr)
        {
            ATH_MSG_FATAL("No detector element collection for SCT");
            return StatusCode::FAILURE;
        }
        for (int station = m_firstSctStation; station <= m_lastSctStation; station++)
        {
            // if (m_numSctStations%2 == 0 && station == 0) continue;
            for (int plane = 0; plane < m_numSctPlanesPerStation; plane++)
            {
                for (int row = 0; row < m_numSctRowsPerPlane; row++)
                {
                    for (int module = -m_numSctModulesPerRow/2; module <= m_numSctModulesPerRow/2; module++)
                    {
                        if (m_numSctModulesPerRow%2 == 0 && module == 0) continue;
                        for (int sensor = 0; sensor < 2; sensor++)
                        {
                            Identifier id = helper->wafer_id(station, plane, row, module, sensor, true);
                            if (!sctMgr->identifierBelongs(id))
                            {
                                ATH_MSG_FATAL("Valid SCT sensor identifier does not pass identifierBelongs.");
                                return StatusCode::FAILURE;
                            }
                            IdentifierHash hash = helper->wafer_hash(id);
                            TrackerDD::SiDetectorElement* elementByLevels = sctMgr->getDetectorElement(station, plane, row, module, sensor);
                            TrackerDD::SiDetectorElement* elementById = sctMgr->getDetectorElement(id);
                            TrackerDD::SiDetectorElement* elementByHash = sctMgr->getDetectorElement(hash);
                            TrackerDD::SiDetectorElement* element = (*elements)[hash];
                            if (element == nullptr || elementByLevels != element || elementById != element || elementByHash != element)
                            {
                                ATH_MSG_FATAL("Inconsistent retrieval of SCT detector elements");
                                return StatusCode::FAILURE;
                            }
                            ATH_MSG_ALWAYS("Found SCT sensor (" << station << "/" << plane << "/" << row << "/" << module << "/" << sensor << ") with global center at (" << 
                                element->center().x() << ", " <<
                                element->center().y() << ", " <<
                                element->center().z() << ")."
                                );
                            const TrackerDD::SiDetectorElement* nextPhi = element->nextInPhi();
                            if (nextPhi != nullptr)
                            {
                                if (nextPhi->prevInPhi() != element)
                                {
                                    ATH_MSG_FATAL("Previous phi neighbor of next SCT element is not this element.");
                                    return StatusCode::FAILURE;
                                }
                            }
                            const TrackerDD::SiDetectorElement* prevPhi = element->prevInPhi();
                            if (prevPhi != nullptr)
                            {
                                if (prevPhi->nextInPhi() != element)
                                {
                                    ATH_MSG_FATAL("Next phi neighbor of previous SCT element is not this element.");
                                    return StatusCode::FAILURE;
                                }
                            }
                            if (nextPhi == nullptr && prevPhi == nullptr)
                            {
                                ATH_MSG_FATAL("SCT element " << hash << " has no previous OR next phi neighbor.");
                                return StatusCode::FAILURE;
                            }
                            const TrackerDD::SiDetectorElement* nextEta = element->nextInEta();
                            if (nextEta != nullptr)
                            {
                                if (nextEta->prevInEta() != element)
                                {
                                    ATH_MSG_FATAL("Previous eta neighbor of next SCT element is not this element.");
                                    return StatusCode::FAILURE;
                                }
                            }
                            const TrackerDD::SiDetectorElement* prevEta = element->prevInEta();
                            if (prevEta != nullptr)
                            {
                                if (prevEta->nextInEta() != element)
                                {
                                    ATH_MSG_FATAL("Next eta neighbor of previous SCT element is not this element.");
                                    return StatusCode::FAILURE;
                                }
                            }
                            if (nextEta == nullptr && prevEta == nullptr)
                            {
                                ATH_MSG_FATAL("SCT element " << hash << " has no previous OR next eta neighbor.");
                                return StatusCode::FAILURE;
                            }
                            const TrackerDD::SiDetectorElement* flip = element->otherSide();
                            if (flip == nullptr)
                            {
                                ATH_MSG_FATAL("SCT element " << hash << " has no other side.");
                                return StatusCode::FAILURE;
                            }
                            if (flip->otherSide() != element)
                            {
                                ATH_MSG_FATAL("Other side of other side of SCT sensor is not this sensor.");
                                return StatusCode::FAILURE;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve (typed) SCT detector manager directly from DetStore.");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

StatusCode GeoModelTestAlg::testVetoNu() const
{
    // Test retrieval of helper object directly from store
    const VetoNuID* helper = nullptr;
    ATH_CHECK(detStore()->retrieve(helper, "VetoNuID"));
    if (helper != nullptr)
    {
        // Test neighbors with helper function
        const IdContext& context = helper->plate_context();
        ATH_MSG_ALWAYS("Retrieved VetoNuID helper from DetStore.");
        for (int iStation = 0; iStation < m_numVetoNuStations; iStation++)
        {
            for (int iPlate = 0; iPlate < m_numVetoNuPlatesPerStation; iPlate++)
            {
                Identifier thisId = helper->plate_id(iStation, iPlate, true);
                IdentifierHash thisHash = helper->plate_hash(thisId);
                IdentifierHash prevHash;
                IdentifierHash nextHash;
                Identifier prevId;
                Identifier nextId;
                int prevStation {-1};
                int prevPlate {-1};
                int nextStation {-1};
                int nextPlate {-1};
                int prevStat = helper->get_prev_in_z(thisHash, prevHash);
                if (prevStat == 0) 
                {
                    IdentifierHash testHash;
                    int nextStat = helper->get_next_in_z(prevHash, testHash);
                    if (nextStat != 0 || testHash != thisHash)
                    {
                        ATH_MSG_FATAL("Next VetoNu plate (" << testHash << ") of previous (" << prevHash << ") is not the original (" << thisHash <<")" );
                        return StatusCode::FAILURE;
                    }
                    prevStat = helper->get_id(prevHash, prevId, &context);
                    if (prevStat == 0)
                    {
                        prevStation = helper->station(prevId);
                        prevPlate   = helper->plate(prevId);
                    }
                }
                int nextStat = helper->get_next_in_z(thisHash, nextHash);
                if (nextStat == 0) 
                {
                    IdentifierHash testHash;
                    prevStat = helper->get_prev_in_z(nextHash, testHash);
                    if (prevStat != 0 || testHash != thisHash)
                    {
                        ATH_MSG_FATAL("Previous veto plate (" << testHash << ") of next (" << nextHash << ") is not the original (" << thisHash <<")" );
                        return StatusCode::FAILURE;
                    }
                    nextStat = helper->get_id(nextHash, nextId, &context);
                    if (nextStat == 0)
                    {
                        nextStation = helper->station(nextId);
                        nextPlate   = helper->plate(nextId);
                    }
                }
                ATH_MSG_ALWAYS("Station/Plate " << iStation << "/" << iPlate << 
                               " (" << thisHash << ") " <<
                               " : prev = " << prevStation << "/" << prevPlate <<
                               " , next = " << nextStation << "/" << nextPlate );
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve VetoNuID helper from DetStore.");
        return StatusCode::FAILURE;
    }

    // Test direct retrieval of typed managers from DetStore
    const ScintDD::VetoNuDetectorManager* vetoMgr = nullptr;
    ATH_CHECK(detStore()->retrieve(vetoMgr, "VetoNu"));
    if (vetoMgr != nullptr)
    {
        ATH_MSG_ALWAYS("Retrieved (typed) VetoNu detector manager with " << vetoMgr->getNumTreeTops() << " treetops directly from DetStore.");
        // Compare numerology with the "right" answers from our properties
        if (vetoMgr->numerology().numStations() != m_numVetoNuStations)
        {
            ATH_MSG_FATAL("Disagreement in number of veto stations.");
            return StatusCode::FAILURE;
        }
        if (m_numVetoNuStations > 0 && vetoMgr->numerology().numPlatesPerStation() != m_numVetoNuPlatesPerStation)
        {
            ATH_MSG_FATAL("Disagreement in number of plates per veto station.");
            return StatusCode::FAILURE;
        }
        if (m_numVetoNuStations > 0 && m_numVetoNuPlatesPerStation > 0 && vetoMgr->numerology().numPmtsPerPlate() != m_numVetoNuPmtsPerPlate)
        {
            ATH_MSG_FATAL("Disagreement in number of pmts per veto plate.");
            return StatusCode::FAILURE;
        }
        // Test detector elements
        const ScintDD::ScintDetectorElementCollection* elements = vetoMgr->getDetectorElementCollection();
        for (int station = 0; station < m_numVetoNuStations; station++)
        {
            for (int plate = 0; plate < m_numVetoNuPlatesPerStation; plate++)
            {
                Identifier id = helper->plate_id(station, plate, true);
                if (!vetoMgr->identifierBelongs(id))
                {
                    ATH_MSG_FATAL("Valid VetoNu identifier does not pass identifierBelongs.");
                    return StatusCode::FAILURE;
                }
                IdentifierHash hash = helper->plate_hash(id);
                ScintDD::ScintDetectorElement* elementByLevels = vetoMgr->getDetectorElement(station, plate);
                ScintDD::ScintDetectorElement* elementById = vetoMgr->getDetectorElement(id);
                ScintDD::ScintDetectorElement* elementByHash = vetoMgr->getDetectorElement(hash);
                ScintDD::ScintDetectorElement* element = (*elements)[hash];
                if (elementByLevels != element || elementById != element || elementByHash != element)
                {
                    ATH_MSG_FATAL("Inconsistent retrieval of VetoNu detector elements");
                    return StatusCode::FAILURE;
                }
                ATH_MSG_ALWAYS("Found VetoNu plate (" << station << ", " << plate << ") with global center at (" << 
                    element->center().x() << ", " <<
                    element->center().y() << ", " <<
                    element->center().z() << ")."
                    );
                const ScintDD::ScintDetectorElement* next = element->nextInZ();
                if (next != nullptr)
                {
                    if (next->prevInZ() != element)
                    {
                        ATH_MSG_FATAL("Previous neighbor of next VetoNu element is not this element.");
                        return StatusCode::FAILURE;
                    }
                }
                const ScintDD::ScintDetectorElement* prev = element->prevInZ();
                if (prev != nullptr)
                {
                    if (prev->nextInZ() != element)
                    {
                        ATH_MSG_FATAL("Next neighbor of previous VetoNu element is not this element.");
                        return StatusCode::FAILURE;
                    }
                }
                if (next == nullptr && prev == nullptr && m_numVetoPlatesPerStation > 1)
                {
                    ATH_MSG_FATAL("Veto element " << hash << " has no previous OR next neighbor.");
                    return StatusCode::FAILURE;
                }
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve (typed) Veto detector manager directly from DetStore.");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

StatusCode GeoModelTestAlg::testVeto() const
{
    // Test retrieval of helper object directly from store
    const VetoID* helper = nullptr;
    ATH_CHECK(detStore()->retrieve(helper, "VetoID"));
    if (helper != nullptr)
    {
        // Test neighbors with helper function
        const IdContext& context = helper->plate_context();
        ATH_MSG_ALWAYS("Retrieved VetoID helper from DetStore.");
        for (int iStation = 0; iStation < m_numVetoStations; iStation++)
        {
            for (int iPlate = 0; iPlate < m_numVetoPlatesPerStation; iPlate++)
            {
                Identifier thisId = helper->plate_id(iStation, iPlate, true);
                IdentifierHash thisHash = helper->plate_hash(thisId);
                IdentifierHash prevHash;
                IdentifierHash nextHash;
                Identifier prevId;
                Identifier nextId;
                int prevStation {-1};
                int prevPlate {-1};
                int nextStation {-1};
                int nextPlate {-1};
                int prevStat = helper->get_prev_in_z(thisHash, prevHash);
                if (prevStat == 0) 
                {
                    IdentifierHash testHash;
                    int nextStat = helper->get_next_in_z(prevHash, testHash);
                    if (nextStat != 0 || testHash != thisHash)
                    {
                        ATH_MSG_FATAL("Next Veto plate (" << testHash << ") of previous (" << prevHash << ") is not the original (" << thisHash <<")" );
                        return StatusCode::FAILURE;
                    }
                    prevStat = helper->get_id(prevHash, prevId, &context);
                    if (prevStat == 0)
                    {
                        prevStation = helper->station(prevId);
                        prevPlate   = helper->plate(prevId);
                    }
                }
                int nextStat = helper->get_next_in_z(thisHash, nextHash);
                if (nextStat == 0) 
                {
                    IdentifierHash testHash;
                    prevStat = helper->get_prev_in_z(nextHash, testHash);
                    if (prevStat != 0 || testHash != thisHash)
                    {
                        ATH_MSG_FATAL("Previous veto plate (" << testHash << ") of next (" << nextHash << ") is not the original (" << thisHash <<")" );
                        return StatusCode::FAILURE;
                    }
                    nextStat = helper->get_id(nextHash, nextId, &context);
                    if (nextStat == 0)
                    {
                        nextStation = helper->station(nextId);
                        nextPlate   = helper->plate(nextId);
                    }
                }
                ATH_MSG_ALWAYS("Station/Plate " << iStation << "/" << iPlate << 
                               " (" << thisHash << ") " <<
                               " : prev = " << prevStation << "/" << prevPlate <<
                               " , next = " << nextStation << "/" << nextPlate );
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve VetoID helper from DetStore.");
        return StatusCode::FAILURE;
    }

    // Test direct retrieval of typed managers from DetStore
    const ScintDD::VetoDetectorManager* vetoMgr = nullptr;
    ATH_CHECK(detStore()->retrieve(vetoMgr, "Veto"));
    if (vetoMgr != nullptr)
    {
        ATH_MSG_ALWAYS("Retrieved (typed) Veto detector manager with " << vetoMgr->getNumTreeTops() << " treetops directly from DetStore.");
        // Compare numerology with the "right" answers from our properties
        if (vetoMgr->numerology().numStations() != m_numVetoStations)
        {
            ATH_MSG_FATAL("Disagreement in number of veto stations.");
            return StatusCode::FAILURE;
        }
        if (m_numVetoStations > 0 && vetoMgr->numerology().numPlatesPerStation() != m_numVetoPlatesPerStation)
        {
            ATH_MSG_FATAL("Disagreement in number of plates per veto station.");
            return StatusCode::FAILURE;
        }
        if (m_numVetoStations > 0 && m_numVetoPlatesPerStation > 0 && vetoMgr->numerology().numPmtsPerPlate() != m_numVetoPmtsPerPlate)
        {
            ATH_MSG_FATAL("Disagreement in number of pmts per veto plate.");
            return StatusCode::FAILURE;
        }
        // Test detector elements
        const ScintDD::ScintDetectorElementCollection* elements = vetoMgr->getDetectorElementCollection();
        for (int station = 0; station < m_numVetoStations; station++)
        {
            for (int plate = 0; plate < m_numVetoPlatesPerStation; plate++)
            {
                Identifier id = helper->plate_id(station, plate, true);
                if (!vetoMgr->identifierBelongs(id))
                {
                    ATH_MSG_FATAL("Valid Veto identifier does not pass identifierBelongs.");
                    return StatusCode::FAILURE;
                }
                IdentifierHash hash = helper->plate_hash(id);
                ScintDD::ScintDetectorElement* elementByLevels = vetoMgr->getDetectorElement(station, plate);
                ScintDD::ScintDetectorElement* elementById = vetoMgr->getDetectorElement(id);
                ScintDD::ScintDetectorElement* elementByHash = vetoMgr->getDetectorElement(hash);
                ScintDD::ScintDetectorElement* element = (*elements)[hash];
                if (elementByLevels != element || elementById != element || elementByHash != element)
                {
                    ATH_MSG_FATAL("Inconsistent retrieval of Veto detector elements");
                    return StatusCode::FAILURE;
                }
                ATH_MSG_ALWAYS("Found Veto plate (" << station << ", " << plate << ") with global center at (" << 
                    element->center().x() << ", " <<
                    element->center().y() << ", " <<
                    element->center().z() << ")."
                    );
                const ScintDD::ScintDetectorElement* next = element->nextInZ();
                if (next != nullptr)
                {
                    if (next->prevInZ() != element)
                    {
                        ATH_MSG_FATAL("Previous neighbor of next Veto element is not this element.");
                        return StatusCode::FAILURE;
                    }
                }
                const ScintDD::ScintDetectorElement* prev = element->prevInZ();
                if (prev != nullptr)
                {
                    if (prev->nextInZ() != element)
                    {
                        ATH_MSG_FATAL("Next neighbor of previous Veto element is not this element.");
                        return StatusCode::FAILURE;
                    }
                }
                if (next == nullptr && prev == nullptr && m_numVetoPlatesPerStation > 1)
                {
                    ATH_MSG_FATAL("Veto element " << hash << " has no previous OR next neighbor.");
                    return StatusCode::FAILURE;
                }
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve (typed) Veto detector manager directly from DetStore.");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}





StatusCode GeoModelTestAlg::testTrigger() const
{
    // Test retrieval of helper object directly from store
    const TriggerID* helper = nullptr;
    ATH_CHECK(detStore()->retrieve(helper, "TriggerID"));
    if (helper != nullptr)
    {
        // Test neighbors with helper function
        const IdContext& context = helper->plate_context();
        ATH_MSG_ALWAYS("Retrieved TriggerID helper from DetStore.");
        for (int iStation = 0; iStation < m_numTriggerStations; iStation++)
        {
            for (int iPlate = 0; iPlate < m_numTriggerPlatesPerStation; iPlate++)
            {
                Identifier thisId = helper->plate_id(iStation, iPlate, true);
                IdentifierHash thisHash = helper->plate_hash(thisId);
                IdentifierHash prevHash;
                IdentifierHash nextHash;
                Identifier prevId;
                Identifier nextId;
                int prevStation {-1};
                int prevPlate {-1};
                int nextStation {-1};
                int nextPlate {-1};
                int prevStat = helper->get_prev_in_z(thisHash, prevHash);
                if (prevStat == 0) 
                {
                    IdentifierHash testHash;
                    int nextStat = helper->get_next_in_z(prevHash, testHash);
                    if (nextStat != 0 || testHash != thisHash)
                    {
                        ATH_MSG_FATAL("Next Trigger plate (" << testHash << ") of previous (" << prevHash << ") is not the original (" << thisHash <<")" );
                        return StatusCode::FAILURE;
                    }
                    prevStat = helper->get_id(prevHash, prevId, &context);
                    if (prevStat == 0)
                    {
                        prevStation = helper->station(prevId);
                        prevPlate   = helper->plate(prevId);
                    }
                }
                int nextStat = helper->get_next_in_z(thisHash, nextHash);
                if (nextStat == 0) 
                {
                    IdentifierHash testHash;
                    prevStat = helper->get_prev_in_z(nextHash, testHash);
                    if (prevStat != 0 || testHash != thisHash)
                    {
                        ATH_MSG_FATAL("Previous Trigger plate (" << testHash << ") of next (" << nextHash << ") is not the original (" << thisHash <<")" );
                        return StatusCode::FAILURE;
                    }
                    nextStat = helper->get_id(nextHash, nextId, &context);
                    if (nextStat == 0)
                    {
                        nextStation = helper->station(nextId);
                        nextPlate   = helper->plate(nextId);
                    }
                }
                ATH_MSG_ALWAYS("Station/Plate " << iStation << "/" << iPlate << 
                               " (" << thisHash << ") " <<
                               " : prev = " << prevStation << "/" << prevPlate <<
                               " , next = " << nextStation << "/" << nextPlate );
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve TriggerID helper from DetStore.");
        return StatusCode::FAILURE;
    }

    // Test direct retrieval of typed managers from DetStore
    const ScintDD::TriggerDetectorManager* triggerMgr = nullptr;
    ATH_CHECK(detStore()->retrieve(triggerMgr, "Trigger"));
    if (triggerMgr != nullptr)
    {
        ATH_MSG_ALWAYS("Retrieved (typed) Trigger detector manager with " << triggerMgr->getNumTreeTops() << " treetops directly from DetStore.");
        // Compare numerology with the "right" answers from our properties
        if (triggerMgr->numerology().numStations() != m_numTriggerStations || triggerMgr->getNumTreeTops() != m_numTriggerStations)
        {
            ATH_MSG_FATAL("Disagreement in number of Trigger stations.");
            return StatusCode::FAILURE;
        }
        if (m_numTriggerStations > 0 && triggerMgr->numerology().numPlatesPerStation() != m_numTriggerPlatesPerStation)
        {
            ATH_MSG_FATAL("Disagreement in number of plates per Trigger station.");
            return StatusCode::FAILURE;
        }
        if (m_numTriggerStations > 0 && m_numTriggerPlatesPerStation > 0 && triggerMgr->numerology().numPmtsPerPlate() != m_numTriggerPmtsPerPlate)
        {
            ATH_MSG_FATAL("Disagreement in number of pmts per Trigger plate.");
            return StatusCode::FAILURE;
        }
        // Test detector elements
        const ScintDD::ScintDetectorElementCollection* elements = triggerMgr->getDetectorElementCollection();
        for (int station = 0; station < m_numTriggerStations; station++)
        {
            for (int plate = 0; plate < m_numTriggerPlatesPerStation; plate++)
            {
                Identifier id = helper->plate_id(station, plate, true);
                if (!triggerMgr->identifierBelongs(id))
                {
                    ATH_MSG_FATAL("Valid Trigger identifier does not pass identifierBelongs.");
                    return StatusCode::FAILURE;
                }
                IdentifierHash hash = helper->plate_hash(id);
                ScintDD::ScintDetectorElement* elementByLevels = triggerMgr->getDetectorElement(station, plate);
                ScintDD::ScintDetectorElement* elementById = triggerMgr->getDetectorElement(id);
                ScintDD::ScintDetectorElement* elementByHash = triggerMgr->getDetectorElement(hash);
                ScintDD::ScintDetectorElement* element = (*elements)[hash];
                if (elementByLevels != element || elementById != element || elementByHash != element)
                {
                    ATH_MSG_FATAL("Inconsistent retrieval of Trigger detector elements");
                    return StatusCode::FAILURE;
                }
                ATH_MSG_ALWAYS("Found Trigger plate (" << station << ", " << plate << ") with global center at (" << 
                    element->center().x() << ", " <<
                    element->center().y() << ", " <<
                    element->center().z() << ")."
                    );
                const ScintDD::ScintDetectorElement* next = element->nextInZ();
                if (next != nullptr)
                {
                    if (next->prevInZ() != element)
                    {
                        ATH_MSG_FATAL("Previous neighbor of next Trigger element is not this element.");
                        return StatusCode::FAILURE;
                    }
                }
                const ScintDD::ScintDetectorElement* prev = element->prevInZ();
                if (prev != nullptr)
                {
                    if (prev->nextInZ() != element)
                    {
                        ATH_MSG_FATAL("Next neighbor of previous Trigger element is not this element.");
                        return StatusCode::FAILURE;
                    }
                }
                if (next == nullptr && prev == nullptr && m_numTriggerPlatesPerStation > 1)
                {
                    ATH_MSG_FATAL("Trigger element " << hash << " has no previous OR next neighbor.");
                    return StatusCode::FAILURE;
                }
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve (typed) Trigger detector manager directly from DetStore.");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

StatusCode GeoModelTestAlg::testPreshower() const
{
    // Test retrieval of helper object directly from store
    const PreshowerID* helper = nullptr;
    ATH_CHECK(detStore()->retrieve(helper, "PreshowerID"));
    if (helper != nullptr)
    {
        // Test neighbors with helper function
        const IdContext& context = helper->plate_context();
        ATH_MSG_ALWAYS("Retrieved PreshowerID helper from DetStore.");
        for (int iStation = 0; iStation < m_numPreshowerStations; iStation++)
        {
            for (int iPlate = 0; iPlate < m_numPreshowerPlatesPerStation; iPlate++)
            {
                Identifier thisId = helper->plate_id(iStation, iPlate, true);
                IdentifierHash thisHash = helper->plate_hash(thisId);
                IdentifierHash prevHash;
                IdentifierHash nextHash;
                Identifier prevId;
                Identifier nextId;
                int prevStation {-1};
                int prevPlate {-1};
                int nextStation {-1};
                int nextPlate {-1};
                int prevStat = helper->get_prev_in_z(thisHash, prevHash);
                if (prevStat == 0) 
                {
                    IdentifierHash testHash;
                    int nextStat = helper->get_next_in_z(prevHash, testHash);
                    if (nextStat != 0 || testHash != thisHash)
                    {
                        ATH_MSG_FATAL("Next Preshower plate (" << testHash << ") of previous (" << prevHash << ") is not the original (" << thisHash <<")" );
                        return StatusCode::FAILURE;
                    }
                    prevStat = helper->get_id(prevHash, prevId, &context);
                    if (prevStat == 0)
                    {
                        prevStation = helper->station(prevId);
                        prevPlate   = helper->plate(prevId);
                    }
                }
                int nextStat = helper->get_next_in_z(thisHash, nextHash);
                if (nextStat == 0) 
                {
                    IdentifierHash testHash;
                    prevStat = helper->get_prev_in_z(nextHash, testHash);
                    if (prevStat != 0 || testHash != thisHash)
                    {
                        ATH_MSG_FATAL("Previous veto plate (" << testHash << ") of next (" << nextHash << ") is not the original (" << thisHash <<")" );
                        return StatusCode::FAILURE;
                    }
                    nextStat = helper->get_id(nextHash, nextId, &context);
                    if (nextStat == 0)
                    {
                        nextStation = helper->station(nextId);
                        nextPlate   = helper->plate(nextId);
                    }
                }
                ATH_MSG_ALWAYS("Station/Plate " << iStation << "/" << iPlate << 
                               " (" << thisHash << ") " <<
                               " : prev = " << prevStation << "/" << prevPlate <<
                               " , next = " << nextStation << "/" << nextPlate );
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve PreshowerID helper from DetStore.");
        return StatusCode::FAILURE;
    }

    // Test direct retrieval of typed managers from DetStore
    const ScintDD::PreshowerDetectorManager* preshowerMgr = nullptr;
    ATH_CHECK(detStore()->retrieve(preshowerMgr, "Preshower"));
    if (preshowerMgr != nullptr)
    {
        ATH_MSG_ALWAYS("Retrieved (typed) Preshower detector manager with " << preshowerMgr->getNumTreeTops() << " treetops directly from DetStore.");
        // Compare numerology with the "right" answers from our properties
        if (preshowerMgr->numerology().numStations() != m_numPreshowerStations || preshowerMgr->getNumTreeTops() != m_numPreshowerStations)
        {
            ATH_MSG_FATAL("Disagreement in number of preshower stations.");
            return StatusCode::FAILURE;
        }
        if (m_numPreshowerStations > 0 && preshowerMgr->numerology().numPlatesPerStation() != m_numPreshowerPlatesPerStation)
        {
            ATH_MSG_FATAL("Disagreement in number of plates per preshower station.");
            return StatusCode::FAILURE;
        }
        if (m_numPreshowerStations > 0 && m_numPreshowerPlatesPerStation > 0 && preshowerMgr->numerology().numPmtsPerPlate() != m_numPreshowerPmtsPerPlate)
        {
            ATH_MSG_FATAL("Disagreement in number of pmts per preshower plate.");
            return StatusCode::FAILURE;
        }
        // Test detector elements
        const ScintDD::ScintDetectorElementCollection* elements = preshowerMgr->getDetectorElementCollection();
        for (int station = 0; station < m_numPreshowerStations; station++)
        {
            for (int plate = 0; plate < m_numPreshowerPlatesPerStation; plate++)
            {
                Identifier id = helper->plate_id(station, plate, true);
                if (!preshowerMgr->identifierBelongs(id))
                {
                    ATH_MSG_FATAL("Valid Preshower identifier does not pass identifierBelongs.");
                    return StatusCode::FAILURE;
                }
                IdentifierHash hash = helper->plate_hash(id);
                ScintDD::ScintDetectorElement* elementByLevels = preshowerMgr->getDetectorElement(station, plate);
                ScintDD::ScintDetectorElement* elementById = preshowerMgr->getDetectorElement(id);
                ScintDD::ScintDetectorElement* elementByHash = preshowerMgr->getDetectorElement(hash);
                ScintDD::ScintDetectorElement* element = (*elements)[hash];
                if (elementByLevels != element || elementById != element || elementByHash != element)
                {
                    ATH_MSG_FATAL("Inconsistent retrieval of Preshower detector elements");
                    return StatusCode::FAILURE;
                }
                ATH_MSG_ALWAYS("Found Preshower plate (" << station << ", " << plate << ") with global center at (" << 
                    element->center().x() << ", " <<
                    element->center().y() << ", " <<
                    element->center().z() << ")."
                    );
                const ScintDD::ScintDetectorElement* next = element->nextInZ();
                if (next != nullptr)
                {
                    if (next->prevInZ() != element)
                    {
                        ATH_MSG_FATAL("Previous neighbor of next Preshower element is not this element.");
                        return StatusCode::FAILURE;
                    }
                }
                const ScintDD::ScintDetectorElement* prev = element->prevInZ();
                if (prev != nullptr)
                {
                    if (prev->nextInZ() != element)
                    {
                        ATH_MSG_FATAL("Next neighbor of previous Preshower element is not this element.");
                        return StatusCode::FAILURE;
                    }
                }
                if (next == nullptr && prev == nullptr && m_numPreshowerPlatesPerStation > 1)
                {
                    ATH_MSG_FATAL("Preshower element " << hash << " has no previous OR next neighbor.");
                    return StatusCode::FAILURE;
                }
            }
        }
    }
    else
    {
        ATH_MSG_FATAL("Failed to retrieve (typed) Preshower detector manager directly from DetStore.");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

StatusCode GeoModelTestAlg::finalize()
{

    return StatusCode::SUCCESS;
}
