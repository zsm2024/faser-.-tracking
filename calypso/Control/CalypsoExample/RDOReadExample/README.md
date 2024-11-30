Example algorithm to access RDO data & its corresponding simulation data (given generated from simulation)

For each RDO, uses the identifier and TrackerSimDataCollection to get the TrackerSimData associated with it

Then loops through the energy deposits of that TrackerSimData to find the particle barcode of the highest energy deposit

Then find the FaserSiHit corresponding to the particle with the same barcode collision at the same place as the RDO

And plots the RDO GroupSize versus the incident angle the particle made when it was detected

Currently reads from a RDO root file named my.RDO.pool.root, which can be generated from the digitization:
https://gitlab.cern.ch/faser/calypso/tree/master/Tracker/TrackerDigitization/FaserSCT_Digitization

After calypso is installed and compiled & digitization generated

run > python python/RDOReadExample/RDOReadExampleConfig.py