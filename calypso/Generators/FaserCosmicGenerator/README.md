#Faser cosmic-ray generator

## Setup

In Athena 22.0.49, the scipy module is missing from the LCG environment for some reason.

To use the generator, the following command is required after the usual steps to setup the release:

`export PYTHONPATH=/cvmfs/sft.cern.ch/lcg/releases/LCG_100_ATLAS_5/scipy/1.5.1/x86_64-centos7-gcc8-opt/lib/python3.8/site-packages:${PYTHONPATH}`

## Usage

The `RunG4Cosmics.py` file shows a default configuration, appropriate for generating cosmic rays in all of FASER.

Options available through the `CosmicSampler` constructor include:

* maxMuonEnergyGeV - the maximum muon energy to sample at the surface (default: 10000 = 10 TeV; setting this higher does not result in appreciably higher rate)
* targetDepthMeters - the depth of the detector in meters (default: 85)
* thetaMinDegree - the minimum zenith angle to sample (default: 0 = vertically downward)
* thetaMaxDegree - the maximum zenith angle to sample (default: 80 = almost horizontal)
* targetDxMm - the width of the box to generate muons on (default: 7000 = 7m, sufficient to enclose all of FASER including FASERnu and ECAL)
* targetDyMm - the height of the box to generate muons on (default: 600 = 0.6m, sufficient to enclose all of FASER)
* targetDzMm - the length of the box to generate muons on (default: 600 = 0.6m, sufficient to enclose all of FASER)
* x0Mm - X offset of the target box from the origin in detector coordinates (default: 0)
* y0Mm - Y offset of the target box from the origin in detector coordinates (default: 0)
* z0Mm - Z offset of the target box from the origin in detector coordinates (default: 150 = 15 cm)
* chargeRatio - the mu+:mu- ratio (default: 1.27; energy and angle dependence not yet implemented)

The Z offset is chosen to make the box required to enclose all of FASER (including FASERnu and ECAL) as small as possible.  This improves the efficiency for making cosmic rays that hit something.  For single-detector studies (tracker, ECAL, FASERnu) it will be more efficient to reduce the target length and shift the center appropriately.

The calculated rate of cosmic rays at 85 meter depth is 3.41 Hz for a 1 meter radius target sphere, and 5.8 Hz for the default 7000 * 600 * 600 mm^3 box.
