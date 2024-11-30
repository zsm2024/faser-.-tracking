/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

#include "NoisyStripFinder.h"

#include "FaserDetDescr/FaserDetectorID.h"    
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"
#include "TrackerRawData/FaserSCT_RDORawData.h"
#include "StoreGate/WriteHandle.h"
#include "xAODEventInfo/EventInfo.h"

#include <sstream>
#include <string.h>
#include <list>
#include <TH1.h>
#include <TFile.h>
#include <TParameter.h>

namespace Tracker
{
// Constructor with parameters:
NoisyStripFinder::NoisyStripFinder(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator),
  m_idHelper{nullptr}
{
  m_iovrange = IOVRange(IOVTime(), IOVTime());  // Make sure this starts undefined
}

// Initialize method:
StatusCode NoisyStripFinder::initialize() {
  ATH_MSG_INFO("NoisyStripFinder::initialize()!");

  ATH_CHECK(m_rdoContainerKey.initialize());
  ATH_CHECK(m_FaserTriggerData.initialize());
  ATH_CHECK(m_eventInfo.initialize());

  // Get the SCT ID helper
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode NoisyStripFinder::execute(const EventContext& ctx) const {

  SG::ReadHandle<xAOD::FaserTriggerData> xaod(m_FaserTriggerData, ctx);
 
  int trig_int = xaod->tap();
  int trigmask_int = m_triggerMask.value();

  ATH_MSG_DEBUG("Found FaserTriggerData");
  ATH_MSG_DEBUG("trigger = " << trig_int);
  ATH_MSG_DEBUG("mask = " << trigmask_int);

  if (!(xaod->tap() & m_triggerMask.value())) return StatusCode::SUCCESS; // only process events that pass the trigger mask

  ATH_MSG_DEBUG("trigger passed mask");
  ++m_numberOfEvents;

  // Keep track of run
  SG::ReadHandle<xAOD::EventInfo> xevt(m_eventInfo, ctx);
  ATH_MSG_DEBUG("Found run number: " << xevt->runNumber());
  IOVTime iov(xevt->runNumber(), xevt->lumiBlock());
  ATH_MSG_DEBUG("IOV: " << iov);

  if (!m_iovrange.start().isValid()) 
    m_iovrange = IOVRange(iov, iov);
   
  if (iov > m_iovrange.stop())
    m_iovrange = IOVRange(m_iovrange.start(), iov);

  ATH_MSG_DEBUG("Range: " << m_iovrange);

  // First, we have to retrieve and access the container, not because we want to 
  // use it, but in order to generate the proxies for the collections, if they 
  // are being provided by a container converter.
  SG::ReadHandle<FaserSCT_RDO_Container> rdoContainer{m_rdoContainerKey, ctx};
  ATH_CHECK(rdoContainer.isValid());

  // Anything to dereference the DataHandle will trigger the converter
  FaserSCT_RDO_Container::const_iterator rdoCollections{rdoContainer->begin()};
  FaserSCT_RDO_Container::const_iterator rdoCollectionsEnd{rdoContainer->end()};
  for (; rdoCollections != rdoCollectionsEnd; ++rdoCollections) {
    ++m_numberOfRDOCollection;
    const TrackerRawDataCollection<FaserSCT_RDORawData>* rd{*rdoCollections};
    ATH_MSG_DEBUG("RDO collection size=" << rd->size() << ", Hash=" << rd->identifyHash());
    for (const FaserSCT_RDORawData* pRawData: *rd) {
      ++m_numberOfRDO;
      const Identifier      firstStripId(pRawData->identify());
      const int             thisStrip(m_idHelper->strip(firstStripId));
      ATH_MSG_DEBUG( "---------- new rdo ----------------------------- " );
      ATH_MSG_DEBUG( "rd->identifyHash() = " << rd->identifyHash() );
      ATH_MSG_DEBUG( "strip = " << thisStrip );

      if ( NoisyStrip_histmap.count(rd->identifyHash()) == 0 ) { // map.count(key) returns 1 if the key exists and 0 if it does not
        std::string histname_str = std::to_string(rd->identifyHash());
        char const *histname = histname_str.c_str();
        std::string station_num = std::to_string(m_idHelper->station(firstStripId));
        std::string layer_num = std::to_string(m_idHelper->layer(firstStripId));
        std::string phi_num = std::to_string(m_idHelper->phi_module(firstStripId));
        std::string eta_num = std::to_string(m_idHelper->eta_module(firstStripId));
        std::string side_num = std::to_string(m_idHelper->side(firstStripId));
        std::string hist_title_str = "Station " + station_num + " | Layer " + layer_num + " | phi-module " + phi_num + " | eta-module " + eta_num + " | Side " + side_num + "; strip number; # of events";
        char const *hist_title = hist_title_str.c_str();
        NoisyStrip_histmap[rd->identifyHash()] = new TH1D(histname, hist_title, 768, -0.5, 767.5);
      } 
      NoisyStrip_histmap[rd->identifyHash()]->Fill(thisStrip); // increase the value by one so as to count the number of times the strip was active    
    }
  }
  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode NoisyStripFinder::finalize() 
{
  ATH_MSG_INFO("NoisyStripFinder::finalize()");
  ATH_MSG_INFO( m_numberOfEvents << " events found" );
  ATH_MSG_INFO( m_numberOfRDOCollection << " RDO collections processed" );
  ATH_MSG_INFO( m_numberOfRDO<< " RawData" );
  ATH_MSG_INFO( "Number of sensors found = " << NoisyStrip_histmap.size() << " out of 192" );

  for (int ihash = 0; ihash < 192; ++ihash){ // print out the sensors that are missing 
    if ( NoisyStrip_histmap.count(ihash) == 0 ){
      ATH_MSG_INFO("missing sensor # " << ihash);
    }
  }

  ATH_MSG_INFO("IOV range found = " << m_iovrange);

  const char *outputname = m_OutputRootName.value().c_str();

  TFile* outputfile = new TFile(outputname,"RECREATE");

  int trigmask_int = m_triggerMask.value();

  TParameter("numEvents", m_numberOfEvents).Write();
  TParameter("trigger", trigmask_int).Write();

  // Write IOV range so we can save this to the DB 
  if (m_iovrange.start().isValid()) {
    long run = m_iovrange.start().run();
    long lb = m_iovrange.start().event();
    TParameter("IOVLoRun", run).Write();  // re_time()
    TParameter("IOVLoLB",  lb).Write();
    ATH_MSG_INFO("IOV Lo: " << run << "," << lb );
  }
  else
    ATH_MSG_WARNING("Starting IOV time invalid");

  if (m_iovrange.stop().isValid()) {
    long run = m_iovrange.stop().run();
    long lb = m_iovrange.stop().event();
    TParameter("IOVHiRun", run).Write();
    TParameter("IOVHiLB", lb).Write();
    ATH_MSG_INFO("IOV Hi: " << run << "," << lb );
  }
  else
    ATH_MSG_WARNING("Ending IOV time invalid");

  std::map<int,TH1D*>::iterator it = NoisyStrip_histmap.begin();
  // Iterate over the map using Iterator till end.
  while (it != NoisyStrip_histmap.end()){
    ATH_MSG_INFO( "" );
    ATH_MSG_INFO( "---------- hot strip occupancy >= 0.1 for Tracker Sensor hash = "<< it->first <<" ----------" );
    int i = 1;
    while (i <= 768){
      // This is only for information
      if ( it->second->GetBinContent(i)/(double)m_numberOfEvents >= 0.01 ){
        ATH_MSG_INFO( "hot strip # = " << i-1 << ", hit occupancy = " << it->second->GetBinContent(i)/(double)m_numberOfEvents ); // print out hot strips
      }
      i++;
    }
    it->second->Write();
    it++;
  }

  outputfile->Close();

  return StatusCode::SUCCESS;
}
}

