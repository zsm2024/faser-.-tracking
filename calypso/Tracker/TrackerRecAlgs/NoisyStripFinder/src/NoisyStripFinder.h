/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

#ifndef NoisyStripFinder_H
#define NoisyStripFinder_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

////Next contains a typedef so cannot be fwd declared
#include "TrackerRawData/FaserSCT_RDO_Container.h"
#include "xAODFaserTrigger/FaserTriggerData.h"

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "AthenaKernel/IOVRange.h"
#include "xAODEventInfo/EventInfo.h"

//STL
#include <map>
#include <TH1.h>
#include <TFile.h>
#include <TParameter.h>

#include <string>

class FaserSCT_ID;
class ISvcLocator;
class StatusCode;

namespace Tracker
{

/**
 *    @class NoisyStripFinder
 *    @brief Creates histograms with strip occupancy data from SCT Raw Data Objects
 *    Creates histograms with strip occupancy data from SCT Raw Data Objects. Root files containing strip occupancy histograms can then be combined and analyzed (example pyROOT script in share folder) in order to make an XML database of noisy strips.
 */
class NoisyStripFinder : public AthReentrantAlgorithm {
  public:
    /// Constructor with parameters:
    NoisyStripFinder(const std::string& name, ISvcLocator* pSvcLocator);

    /**    @name Usual algorithm methods */
    //@{
    ///Retrieve the tools used and initialize variables
    virtual StatusCode initialize() override;
    ///Form clusters and record them in StoreGate (detector store)
    virtual StatusCode execute(const EventContext& ctx) const override;
    ///Clean up and release the collection containers
    virtual StatusCode finalize() override;
    //Make this algorithm clonable.
    virtual bool isClonable() const override { return true; };
    //@}

  private:
    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    //NoisyStripFinder() = delete;
    //NoisyStripFinder(const NoisyStripFinder&) = delete;
    //NoisyStripFinder &operator=(const NoisyStripFinder&) = delete;
    //@}

    StringProperty m_OutputRootName{this, "OutputHistRootName", "NoisyStripFinderHist.root", "Name of output histogram root file for NoisyStripFinder"};

    UnsignedIntegerProperty m_triggerMask{this, "TriggerMask", 0x10, "Trigger mask (0x10 = random trig)"};

    const FaserSCT_ID* m_idHelper;

    SG::ReadHandleKey<FaserSCT_RDO_Container> m_rdoContainerKey{this, "DataObjectName", "FaserSCT_RDOs", "FaserSCT RDOs"};
    SG::ReadHandleKey<xAOD::FaserTriggerData> m_FaserTriggerData{ this, "FaserTriggerDataKey", "FaserTriggerData", "ReadHandleKey for xAOD::FaserTriggerData"};
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{ this, "EventInfoKey", "EventInfo", "ReadHandleKey for xAOD::EventInfo"};

    mutable int m_numberOfEvents{0};
    mutable std::atomic<int> m_numberOfRDOCollection{0};
    mutable std::atomic<int> m_numberOfRDO{0};

    mutable std::map<int,TH1D*> NoisyStrip_histmap;

    // Keep track of first/last IOV seen
    // Stored as (run << 32) + lumi block
    mutable IOVRange m_iovrange;
};
}
#endif // NoisyStripFinder_H

