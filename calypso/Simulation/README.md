To generate Calypso MC data from an installation (run) directory:

1) edit the G4FaserAlg/test/G4FaserAlgConfig_Test.py file to setup the job
....the default generator is particle-gun; it can also read EVNT files but note that the Veto is far away from (0,0,0) where most ATLAS generators put things by default
....there are examples of single-particle (muon or electron), cosmic-rays and decay-in-flight.  The parameters of each can be changed by passing keyword arguments
....read the comments carefully as a few other things have to be changed in the job options to switch between internal generator and generator data-file
....the geometry can be changed on the command line, using the GeoModel.FaserVersion flag and IOVDb.GlobalTag flags.  See the tests in the CMakeLists.txt file for examples

2) source ./setup.sh
3) G4FaserAlgConfig_Test.py

(setup.sh will put the script in your path)


