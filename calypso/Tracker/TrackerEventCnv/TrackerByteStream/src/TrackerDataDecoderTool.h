//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef TRACKERBYTESTREAM_TRACKERDATADECODERTOOL_H
#define TRACKERBYTESTREAM_TRACKERDATADECODERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "EventFormats/DAQFormats.hpp"
#include "TrackerRawData/FaserSCT_RDO_Container.h"

#include "TrackerIdentifier/FaserSCT_ID.h"


// This class provides conversion between bytestream and Tracker RDOs

class TrackerDataDecoderTool : public AthAlgTool {

 public:
  TrackerDataDecoderTool(const std::string& type, const std::string& name, 
			  const IInterface* parent);

  virtual ~TrackerDataDecoderTool();

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode convert(const DAQFormats::EventFull* re, FaserSCT_RDO_Container* cont, std::string key,
                     const std::map<int, std::pair<int, int> >& cableMapping);

private:
  const FaserSCT_ID*                      m_sctID{nullptr};
  IdContext                               m_sctContext;
  std::vector<bool>                       m_phiReversed;

  // Gaudi::Property<uint32_t>               m_trb0Station { this, "Trb0StationNumber", 1, "Station number for TRB #0" };
};

#endif  /* TRACKERBYTESTREAM_TRACKERDATADECODERTOOL_H */
 
