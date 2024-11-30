#include "LHCDataAlg.h"

#include "xAODFaserLHC/FaserLHCDataAux.h"

LHCDataAlg::LHCDataAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { 
}

StatusCode
LHCDataAlg::initialize() {
  ATH_MSG_INFO(name() << "::initalize()" );

  // Initalize tools
  ATH_CHECK( m_lhcTool.retrieve() );

  // Read handle
  ATH_CHECK( m_eventInfo.initialize() );

  // Write handle
  ATH_CHECK( m_lhcDataKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode
LHCDataAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode
LHCDataAlg::execute(const EventContext& ctx) const {

  ATH_MSG_DEBUG("Executing");

  SG::WriteHandle<xAOD::FaserLHCData> lhcDataHandle(m_lhcDataKey, ctx);
  ATH_CHECK( lhcDataHandle.record( std::make_unique<xAOD::FaserLHCData>(),
				   std::make_unique<xAOD::FaserLHCDataAux>() ) );

  ATH_MSG_DEBUG("FaserLHCData " << lhcDataHandle.name() << " initialized");

  lhcDataHandle->set_fillNumber(m_lhcTool->getFillNumber(ctx));
  lhcDataHandle->set_machineMode(m_lhcTool->getMachineMode(ctx));
  lhcDataHandle->set_beamMode(m_lhcTool->getBeamMode(ctx));
  lhcDataHandle->set_beamType1(m_lhcTool->getBeamType1(ctx));
  lhcDataHandle->set_beamType2(m_lhcTool->getBeamType2(ctx));

  lhcDataHandle->set_betaStar(m_lhcTool->getBetaStar(ctx));
  lhcDataHandle->set_crossingAngle(m_lhcTool->getCrossingAngle(ctx));

  lhcDataHandle->set_stableBeams(m_lhcTool->getStableBeams(ctx));
  lhcDataHandle->set_injectionScheme(m_lhcTool->getInjectionScheme(ctx));

  // Slight naming mismatch here, but oh well
  lhcDataHandle->set_numBunchBeam1(m_lhcTool->getBeam1Bunches(ctx));
  lhcDataHandle->set_numBunchBeam2(m_lhcTool->getBeam2Bunches(ctx));
  lhcDataHandle->set_numBunchColliding(m_lhcTool->getCollidingBunches(ctx));

  ATH_MSG_DEBUG("FaserLHCData B1: " << m_lhcTool->getBeam1Bunches(ctx)
		<< " B2: " << m_lhcTool->getBeam2Bunches(ctx)
		<< " Coll: " << m_lhcTool->getCollidingBunches(ctx));

  // Fill BCID information

  // Get the BCID mask
  std::vector<unsigned char> bcid_mask = m_lhcTool->getBCIDMasks(ctx);

  // Get the event bcid value
  SG::ReadHandle<xAOD::EventInfo> xevt(m_eventInfo, ctx);
  unsigned int bcid = xevt->bcid();

  int nearest;
  if (m_lhcTool->getCollidingBunches(ctx) == 0) {
    ATH_MSG_INFO("No colliding bunches, can't set nearest");
    nearest = -3564;
  } else {
    nearest = findNearest(bcid, bcid_mask, 3);  // Colliding beams
  }
  lhcDataHandle->set_distanceToCollidingBCID(nearest);
  ATH_MSG_DEBUG("Found distance of " << nearest << " from BCID " << bcid 
		<< " to the nearest colliding BCID ");

  if (m_lhcTool->getBeam1Bunches(ctx) == 0) {
    ATH_MSG_INFO("No beam 1 bunches, can't set nearest");
    nearest = -3564;
  } else {
    nearest = findNearest(bcid, bcid_mask, 1);  // Beam1 unpaired
  }
  lhcDataHandle->set_distanceToUnpairedB1(nearest);
  ATH_MSG_DEBUG("Found distance of " << nearest << " from BCID " << bcid 
		<< " to the nearest unpaired B1 ");

  if (m_lhcTool->getBeam2Bunches(ctx) == 0) {
    ATH_MSG_INFO("No beam 2 bunches, can't set nearest");
    nearest = -3564;
  } else {
    nearest = findNearest(bcid, bcid_mask, 2);  // Beam2 unpaired
  }
  lhcDataHandle->set_distanceToUnpairedB2(nearest);
  ATH_MSG_DEBUG("Found distance of " << nearest << " from BCID " << bcid 
		<< " to the nearest unpaired B2 ");

  // Add 127 to current BCID to check if inbound B1 is nearby FASER
  if (m_lhcTool->getBeam1Bunches(ctx) == 0) {
    ATH_MSG_INFO("No beam 1 bunches, can't set nearest");
    nearest = -3564;
  } else {
    int offset_bcid = (bcid + 127) % 3564;
    nearest = findNearest(offset_bcid, bcid_mask, 1);  // Inbound B1
    int nearest2 = findNearest(offset_bcid, bcid_mask, 3);  // Inbound B1
    if (abs(nearest2) < abs(nearest)) nearest = nearest2;
  }
  lhcDataHandle->set_distanceToInboundB1(nearest);
  ATH_MSG_DEBUG("Found distance of " << nearest << " from BCID " << bcid 
  		<< " to the nearest inbound B1 ");

  unsigned int previous;
  if (m_lhcTool->getCollidingBunches(ctx) == 0) {
    ATH_MSG_INFO("No colliding bunches, can't set nearest");
    previous = 9999;
  } else {
    previous = previousColliding(bcid, bcid_mask);
  }
  lhcDataHandle->set_distanceToPreviousColliding(previous);
  ATH_MSG_DEBUG("Found distance of " << previous << " from BCID " << bcid 
		<< " to the previous colliding bunch ");

  if (m_lhcTool->getCollidingBunches(ctx) == 0) {
    ATH_MSG_INFO("No colliding bunches, can't set nearest");
    previous = 9999;
  } else {
    previous = previousTrain(bcid, bcid_mask);
  }
  lhcDataHandle->set_distanceToTrainStart(previous);
  ATH_MSG_DEBUG("Found distance of " << previous << " from BCID " << bcid 
		<< " to the start of the previous train ");

  return StatusCode::SUCCESS;
}

// Function to find distance to nearest BCID
// mask indicates the condition: 1 - unpaired B1, 2 - unpaired B2, 3 - colliding
int
LHCDataAlg::findNearest(unsigned int bcid, const std::vector<unsigned char>& bcid_mask, unsigned char mask) const {

  // Does the BCID make sense?
  if ((bcid > 3564) || (bcid <= 0)) {
    ATH_MSG_WARNING("Requested distance from invalid BCID " << bcid << "!");
    return -3565;
  }

  unsigned int test_bcid;

  // Move outwards from the bcid (starting at offset of 0)
  for (unsigned int i=0; i < 3564/2; i++) {

    // Try positive offsets
    test_bcid = (bcid + i) % 3564;

    // BCID 0 doesn't exist in the real machine
    // BCID 3564 doesn't exist in our array (but will always be empty in real machine)
    // So avoid these pathologies
    if (test_bcid != 0) 
      if (mask == bcid_mask[test_bcid]) return i;

    if(i == 0) continue;  // No need to check -0

    // Try negative offsets
    test_bcid = (bcid - i) % 3564;

    // BCID 0 doesn't exist in the real machine
    // BCID 3564 doesn't exist in our array (but will always be empty)
    // So avoid these pathologies
    if (test_bcid != 0) 
      if (mask == bcid_mask[test_bcid]) return -i;

  }

  // If we got to here, there was no match
  // This is possible if for instance all BCIDs are colliding and mask = 0x01
  
  // Does the BCID make sense?
  ATH_MSG_WARNING("Couldn't find distance from BCID " << bcid << " and pattern " << int(mask) << "!");

  // Only print this out in debug, otherwise floods log file
  int b1=0;
  int b2=0;
  int col = 0;
  for (int i=0; i<3564; i++) {
    if (bcid_mask[i] & 0x01) b1++;
    if (bcid_mask[i] & 0x02) b2++;
    if (bcid_mask[i] & 0x03) col++;
    if (bcid_mask[i] > 0)
      ATH_MSG_DEBUG("BCID " << i << " - " << int(bcid_mask[i]));
  }

  //ATH_MSG_WARNING(bcid_mask);
  ATH_MSG_WARNING("Found B1: " << b1 << " B2: " << b2 << " Coll: " << col);
  return -3565;

}

// Function to find previous colliding BCID
unsigned int
LHCDataAlg::previousColliding(unsigned int bcid, const std::vector<unsigned char>& bcid_mask) const {

  // Does the BCID make sense?
  if ((bcid > 3564) || (bcid <= 0)) {
    ATH_MSG_WARNING("Requested distance from invalid BCID " << bcid << "!");
    return 9999;
  }

  unsigned int test_bcid;

  // Move backwards from the bcid (starting at offset of 0)
  for (unsigned int i=0; i < 3564; i++) {

    // Try positive offsets
    test_bcid = (bcid - i) % 3564;

    // BCID 3564 doesn't exist in our array (but will always be empty)
    // So avoid these pathologies
    if (test_bcid != 0) 
      if (0x03 == bcid_mask[test_bcid]) return i;

  }

  // If we got to here, there was no match
  // Does the BCID make sense?
  ATH_MSG_WARNING("Couldn't find colliding from BCID " << bcid << "!");
  ATH_MSG_WARNING(bcid_mask);
  return 9999;

}

unsigned int
LHCDataAlg::previousTrain(unsigned int bcid, const std::vector<unsigned char>& bcid_mask) const {

  // Does the BCID make sense?
  if ((bcid > 3564) || (bcid <= 0)) {
    ATH_MSG_WARNING("Requested distance from invalid BCID " << bcid << "!");
    return 9999;
  }

  unsigned int test_bcid;

  bool in_train = false;
  // Move backwards from the bcid (starting at offset of 0)
  for (unsigned int i=0; i < 3564; i++) {

    // BCID to test
    test_bcid = (bcid - i) % 3564;

    // BCID 3564 doesn't exist in our array (but will always be empty)
    // So avoid these pathologies
    ATH_MSG_DEBUG("Test " << test_bcid << " In train: " << in_train << " mask[test_bcid]");

    // If we are not in a train before, lets see if we are in a train now
    if (!in_train) {

      // If we found a colliding bunch, set in_train, otherwise we are still not in a train
      if ((test_bcid != 0) && (0x03 == bcid_mask[test_bcid]))
	in_train = true;

      // Either way, move to next BCID
      continue;

    } else {

      // Currently in a train, lets see if now we are not
      if ((test_bcid == 0) || (0x03 != bcid_mask[test_bcid])) {
	in_train = false;
	// Previous BCID was first in train
	return i-1;
      } else {
	in_train = true;
      }
	
    }

  }

  // If we got to here, there was no match
  // Does the BCID make sense?
  ATH_MSG_WARNING("Couldn't find first in train from BCID " << bcid << "!");
  ATH_MSG_WARNING(bcid_mask);
  return 9999;

}
