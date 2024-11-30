Calypso is the FASER offline software system, based on the ATLAS Athena and LHCb GAUDI frameworks

Start by creating a personal fork of https://gitlab.cern.ch/faser/calypso.git (using the gitlab web interface)

Then the following sequence will allow you to compile and run Calypso on any machine with cvmfs access.
```
#clone the (forked) project to your local machine
git clone --recursive https://:@gitlab.cern.ch:8443/$USERNAME/calypso.git 

#The next three lines are used to setup the ATLAS release environment
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase 
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
asetup --input=calypso/asetup.faser Athena,24.0.41

#create build directory
mkdir build
cd build

#build calypso
cmake -DCMAKE_INSTALL_PREFIX=../run ../calypso ; make ; make install
```

Before running anything, it is MANDATORY to do:

```
cd ../run
source ./setup.sh
```

Don't omit the dot in the `source ./setup.sh` command! 

It can be convenient to alias the command "asetup --input=calypso/asetup.faser" to something like "fsetup"

**Lxplus/afs setup**

If afs is used to host and run calypso, the user should do so with an afs workspace.

Instructions for setting up a workspace can be found here: https://resources.web.cern.ch/resources/Help/?kbid=067040

When compiling, CERN IT recommends using condor to submit batch jobs. The basics of running condor are found here: https://batchdocs.web.cern.ch/local/quick.html 


## Known issues:

* It is now essential to use the tags `ConfigFlags.GeoModel.FaserVersion` and `ConfigFlags.IOVDb.GlobalTag` in a consistent way.  If nothing is specified the first option (baseline) should be chosen by default.

** `ConfigFlags.GeoModel.FaserVersion = "FASER-01"` and `ConfigFlags.IOVDb.GlobalTag = OFLCOND-FASER-01` enables the baseline TI-12 detector (obsolete and no longer supported)

** `ConfigFlags.GeoModel.FaserVersion = "FASER-02"` and `ConfigFlags.IOVDb.GlobalTag = OFLCOND-FASER-02` enables the interface tracker and repositioned Veto (obsolete and no longer supported)

** `ConfigFlags.GeoModel.FaserVersion = "FASERNU-03"` and `ConfigFlags.IOVDb.GlobalTag = OFLCOND-FASER-02` enables the full FaserNu (IFT + emulsion) setup

** `ConfigFlags.GeoModel.FaserVersion = "FASERNU-03"` and `ConfigFlags.IOVDb.GlobalTag = OFLCOND-FASER-03` enables the full FaserNu (IFT + emulsion) setup with updated (10Nov22) magnetic field map

** `ConfigFlags.GeoModel.FaserVersion = "FASERNU-03"` and `ConfigFlags.IOVDb.GlobalTag = OFLCOND-FASER-04` enables the full FaserNu (IFT + emulsion) setup with updated (2023) tracker alignment

** `ConfigFlags.GeoModel.FaserVersion = "FASERNU-04"` and `ConfigFlags.IOVDb.GlobalTag = OFLCOND-FASER-04` enables the full FaserNu (IFT + emulsion) setup with updated (2023) geometry

** `ConfigFlags.GeoModel.FaserVersion = "FASER-TB00"` and `ConfigFlags.IOVDb.GlobalTag = OFLCOND-FASER-TB00` enables the 2021 Test-beam setup.

** `ConfigFlags.GeoModel.FaserVersion = "FASER-TB01"` and `ConfigFlags.IOVDb.GlobalTag = OFLCOND-FASER-TB01` enables the 2021 Test-beam setup with updated (2023) geometry.

* The command `source /cvmfs/sft.cern.ch/lcg/releases/LCG_104d_ATLAS_7/sqlite/3320300/x86_64-el9-gcc13-opt/sqlite-env.sh` may be necessary to avoid errors when generating a database
