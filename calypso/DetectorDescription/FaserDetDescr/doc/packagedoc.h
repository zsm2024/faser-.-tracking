/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@page FaserDetDescr_page 

This package contains an implentation base class for all identifier
helper classes - FaserDetectorID. This class contains the information
of the "upper levels" of identifiers, i.e. which detector system
(Scint, Tracker, Calorimeter) and which subdetector. All concrete helpers
derive from FaserDetectorID and thus each one can respond to the
generic queries on the upper levels of identifiers. 

FaserDetectorID also provides facilities for printing out identifiers,
e.g. show_to_string, and maintains the version tags.






*/
