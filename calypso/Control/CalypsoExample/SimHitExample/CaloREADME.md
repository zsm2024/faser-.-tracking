
# Running the ECAL simulation  

Use runEcal.py to specify particle type, energy, mass, z position in calo, theta and phi ranges etc
runEcal.py location: calypso/Simulation/G4Faser/G4FaserAlg/test/runEcal.py
 - ConfigFlags.Output.HITSFileName = ""
specifies the output name of the HITS file
 - pg.sampler.pid = -11
particle ID (-11 = electron)
 - pg.sampler.mom = PG.EThetaMPhiSampler(energy=100*GeV, theta=[0,pi/20], phi=[0,2*pi], mass=0.511)
set particle energy, theta, phi and mass
 - pg.sampler.pos = PG.PosSampler(x=[-5.0, 5.0], y=[-5.0, 5.0], z=2744.1, t=0.0)
set position of beam in terms of x and y coordinates 
z = 2744.1 (mm) corresponds to calorimeter face 
x=60.9 and y=49.6 corresponds to centre of top left module
 - sys.exit(int(acc.run(maxEvents=1000).isFailure()))
set the number of events to simulate
1000 event simulations at 1 TeV for electrons and photons need to be split
change the random seed in this case
 - pg.randomSeed = 123456

Run this file in Calypso's run directory to genetate HITS file of simulation.

# Plotting Histograms in CaloSimHitAlg

Once the HITS file has been generated from runEcal.py:
 - use CaloSimHitExample_jobOptions.py to loop over HITS and generate HistoFile
 - the histogram plotting is specified in CaloSimHitAlg.h and CaloSimHitAlg.cxx

CaloSimHitAlg.cxx plots variations of 6 histograms:
 - eLoss - energy deposited per hit in the 4 ECAL modules (GeV)
    - eLossBR - eLoss in bottom right ECAL module
    - eLossTR - eLoss in top right ECAL module
    - eLossBL - eLoss in bottom left ECAL module
    - eLossTL - eLoss in top left ECAL module
 - eLossTot - total energy deposited by particle across all 4 ECAL modules (GeV)
    - eLossBRTot - eLossTot in bottom right ECAL module
    - eLossTRTot - eLossTot in top right ECAL module
    - eLossBLTot - eLossTot in bottom left ECAL module
    - eLossTLTot - eLossTot in top left ECAL module
 - module - cross section of hits in the calo, where (0,0), (1,0), (0,1) and (1,1) correspond to ECAL module centres
 - modulePos - cross section of hits in the calo, showing their position in mm
 - meanTime - events in the calo as a function of mean time (ns)
 - weightedTime - energy weighted events in the calo as a function of mean time (ns)

There are also fractional histograms for eLoss and eLossTot that give the deposited energy as a fraction of injected energy.
Due to the histograms not depending on injected energy of simulated particle, may need to Rebin() when drawing eLoss and eLossTot histograms.
