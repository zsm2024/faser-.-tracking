from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GeneratorModules.EvgenAlg import EvgenAlg
from AthenaPython.PyAthena import StatusCode, EventInfo, EventID, EventType
from AthenaCommon.SystemOfUnits import GeV, MeV, cm
from AthenaCommon.Constants import DEBUG

from FaserCosmicGenerator import Range

import ROOT

import numpy as np
import math

# TODO: correct angle for beam crossing angle: both in angel itself and position

class FlukaReader(EvgenAlg):
    def __init__(self, name="FlukaReader", MCEventKey="BeamTruthEvent", file_name = "", dist = 0, z = -1.5, randomSeed = None, nsamples = 1, test = False):
        super(FlukaReader,self).__init__(name=name)
        self.McEventKey = MCEventKey
        self.file_name = file_name
        self.dist = dist * 100 # cm
        self.z = z * 1000 # mm
        self.isample = 0
        self.nsamples = nsamples
        self.test = test

        self.columns = ["run", "event", "type", "gen", "E", "w", "x", "y", "cosX", "cosY", "age", "_"]

        if randomSeed is not None:
            self.msg.info(f"Setting seed to {randomSeed}")
            self.rng = np.random.default_rng(randomSeed)
        else:
            self.rng = np.random.default_rng()        

        self.file = open(self.file_name)

        if self.test:
            self.before = dict(zip(self.columns, [[] for i in range(len(self.columns))]))
            self.after = dict(zip(self.columns, [[] for i in range(len(self.columns))]))

               
        return

    def genFinalize(self):

        if self.test:
            self.plot()
        
        self.file.close()
        return StatusCode.Success
    

    def fillEvent(self, evt):
        "This is called for every real event * the number of samplings"

        # If the sample gets to the number requested, then reset to 0
        if self.isample == self.nsamples:
            self.msg.debug("Reseting samples")
            self.isample = 0

        # Only if the sample is 0 load the new fluka entry
        if self.isample == 0:
            self.msg.debug("Loading new fluka event")
            try:
                l = next(self.file)
            except StopIteration:
                return StatusCode.Success
            
            entry =  dict(zip(self.columns, l.strip("\n").split()))
            for i,c in enumerate(self.columns):
                if i < 4:
                    entry[c] = int(entry[c])
                else:
                    entry[c] = float(entry[c])

            self.entry = entry

        # Call for each sample of each event
        self.msg.debug(f"Processing sample {self.isample}")
        self.process(self.entry, evt)
        self.isample += 1

        return StatusCode.Success

    def angle(self, cosTheta):
        "Convert cos(theta) wrt x or y axis to theta wrt z axis"
        return np.pi/2 - np.arccos(cosTheta)

    def pid(self, ftype):
        "Convert fluka particle type to PID"
        if ftype == 10:  # mu+
            return -13
        elif ftype == 11: # mu-
            return 13
        else:
            return 0

    def path_length(self, z, cosThetaX, cosThetaY):
        "Get path length traversed in the material, taking into account incident angles"
        
        # Convert theta wrt x and y axis to wrt z axis
        thetaX = self.angle(cosThetaX)
        thetaY = self.angle(cosThetaY)        
        
        # Correct z for angle 
        zcorr = z / np.abs(np.cos(thetaX)) / np.abs(np.cos(thetaY))
        
        return zcorr

    def energy_after_loss_exact(self, e, zcorr):
        "Calculate exact energy after loss in material"
        return Range.muPropagate(e, zcorr/100.) # meters

    def energy_after_loss(self, e, cosThetaX, cosThetaY, zcorr, a = 2e-3, b = 4e-6):
        "Calculate approximate energy after loss in material"
    
        # Based on
        # http://www.bartol.udel.edu/~stanev/lectures/apr17.pdf
        # and PDG chapter 27 fig 27.1
        # https://www.google.co.uk/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&cad=rja&uact=8&ved=2ahUKEwiG9YjNtvr2AhUbiVwKHdQfD9sQFnoECAsQAQ&url=https%3A%2F%2Fpdg.lbl.gov%2F2005%2Freviews%2Fpassagerpp.pdf&usg=AOvVaw1HGA5PZtC2UiqA6B7_C5dz        
                
        eps = a/b
        return (e + eps) * np.exp(-b * zcorr) - eps

    def mean_scattering_angle(self, e, cosThetaX, cosThetaY, zcorr, X0 = 10.02, m = 105.66e-3, charge = 1, beta = 1):
        "Calculate mean scattering angle over many scatters for given energy and length z"
        
        # Based on PDG chapter 27 eqns 27.10, 27.16, 27.17
        # https://www.google.co.uk/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&cad=rja&uact=8&ved=2ahUKEwiG9YjNtvr2AhUbiVwKHdQfD9sQFnoECAsQAQ&url=https%3A%2F%2Fpdg.lbl.gov%2F2005%2Freviews%2Fpassagerpp.pdf&usg=AOvVaw1HGA5PZtC2UiqA6B7_C5dz        
        # and
        # https://pdg.lbl.gov/2014/AtomicNuclearProperties/HTML/standard_rock.html
        
        # Convert E to momentum [GeV]
        p = np.sqrt(e**2 - m**2)  
        
        # Mean angle [GeV and cm]
        c = 1 # n.u
        theta0 = 13.6e-3 / (p * c * beta) * charge *  np.sqrt(zcorr/X0) * (1 + 0.38 * math.log(zcorr, X0))
        return theta0

    def scattering_angle(self, cosTheta, theta0, rand1):
        "Calculate actual scattering angle over many scatters for given start angle and mean angle"
        
        # Convert theta wrt x or y axis to wrt z axis
        theta = self.angle(cosTheta)
        
        # Add random scattering angle
        return theta + rand1 * theta0

    def scattering_postition(self, x, cosThetaX, cosThetaY, zcorr, theta0, rand1, rand2):
        "Calculate transverse scattering position over many scatters for given start angle and mean angle + length z"
        
        # Convert theta wrt x to wrt z axis
        thetaX = self.angle(cosThetaX)
        
        xout = np.copy(x)
        if xout.ndim == 0:
            xout = float(xout)
        
        # Add displacement due to initial angle
        #xang = z * np.tan(thetaX)
        xang = zcorr * np.sin(thetaX)            
        xout += xang
        
        # Add displacement due to multiple scattering
        dx = rand1 * zcorr * theta0 / np.sqrt(12) + rand2 * zcorr * theta0/2
        xout += dx
    
        return xout

    def propagate(self, entry):
        "Propagate the particle through a given distance of standard rock using the small-angle approxiumation"

        if self.dist == 0:
            return entry

        # Random numbers
        rand1 = self.rng.normal(0, 1)
        rand2 = self.rng.normal(0, 1)        

        # Get entry info
        e = entry["E"]
        x = entry["x"]
        y = entry["y"]        
        cosX = entry["cosX"]
        cosY = entry["cosY"]

        # Correct path length for angles
        z = self.path_length(self.dist, cosX, cosY)

        # Account for energy loss
        #eout = self.energy_after_loss(e, cosX, cosY, z)        
        eout = self.energy_after_loss_exact(e, z) 

        # Account for scattering on angle and position
        theta0 = self.mean_scattering_angle(e, cosX, cosY, z)
        thetaXout = self.scattering_angle(cosX, theta0, rand1)
        thetaYout = self.scattering_angle(cosY, theta0, rand1)
        xout = self.scattering_postition(x, cosX, cosY, z, theta0, rand1, rand2)
        yout = self.scattering_postition(y, cosY, cosX, z, theta0, rand1, rand2)        

        # Update entry info using copy for cases when resample so don't change the original
        newentry = entry.copy()
        newentry["E"] = eout
        newentry["x"] = xout
        newentry["y"] = yout        
        newentry["cosX"] =  np.cos(np.pi/2 + thetaXout)
        newentry["cosY"] =  np.cos(np.pi/2 + thetaYout)
        
        return newentry


    def process(self, entry, evt):

        if self.test:
            for k,v in entry.items():
                self.before[k].append(float(v))

        if self.msg.level > DEBUG: 
            print("Original Entry", entry)

        if self.dist != 0:
            # Propoagate to FASER            
            newentry = self.propagate(entry)
        elif self.nsamples != 1:
            # Else smear if sampling more than once, using copy to avoid changing original
            newentry = entry.copy()
            newentry["E"] *= self.rng.normal(1, 0.05)
            newentry["x"] *= self.rng.normal(1, 0.05)
            newentry["y"] *= self.rng.normal(1, 0.05)            
            newentry["cosX"] = np.cos(np.arccos(entry["cosX"]) * self.rng.normal(1, 0.05))
            newentry["cosY"] = np.cos(np.arccos(entry["cosY"]) * self.rng.normal(1, 0.05))
        else:
            # No propagation or smearing
            newentry = entry

        if self.msg.level > DEBUG: 
            print("Propagated/Smeared Entry", newentry)

            
        if self.test:
            for k,v in newentry.items():
                self.after[k].append(float(v))
        
        try:
          from AthenaPython.PyAthena import HepMC3  as HepMC
        except ImportError:
          from AthenaPython.PyAthena import HepMC   as HepMC

        # Add weight, correcting for mutliple sampling
        evt.weights().push_back(newentry["w"] / self.nsamples)


        # Setup MC event
        mcEventType = EventType()
        mcEventType.add_type(EventType.IS_SIMULATION)

        mcEventId = EventID(run_number = newentry["run"], event_number = newentry["event"])

        mcEventInfo = EventInfo(id = mcEventId, type = mcEventType)

        self.evtStore.record(mcEventInfo, "McEventInfo", True, False)

        ROOT.SetOwnership(mcEventType, False)
        ROOT.SetOwnership(mcEventId, False)
        ROOT.SetOwnership(mcEventInfo, False)

        # Create HepMC Vertex
        # Impose axial timing constraint
        pos = HepMC.FourVector(newentry["x"] * cm, newentry["y"] * cm, self.z, self.z)
        gv = HepMC.GenVertex(pos)

        ROOT.SetOwnership(gv, False)
        evt.add_vertex(gv)

        # TODO: skip event if below a certain energy

        # Create HepMC particle
        gp = HepMC.GenParticle()

        m = 105.66
        e = newentry["E"] * 1000.  #MeV

        # If the energy is less than mass then skip the event
        if e < m:
            self.setFilterPassed(False)
            self.msg.debug("Event failed energy cut")
            return False
        else:
            self.setFilterPassed(True)
        
        p = np.sqrt(e**2 - m**2)

        thetaX = self.angle(newentry["cosX"])        
        thetaY = self.angle(newentry["cosY"])

        # theta: just above z axis as phi deals with negative 
        theta = np.abs(thetaY)
        # phi: 0 - 2pi
        phi = np.arctan2(newentry["cosY"], newentry["cosX"])
        #phi = np.arctan(newentry["cosY"] / newentry["cosX"])
        if phi < 0: phi += 2*np.pi
        if phi == 2*np.pi: phi = 0

        #self.msg.debug(f"INPUT: {e}, {m}, {p}, {theta}, {phi}, {np.sin(theta)}, {np.cos(theta)}, {np.sin(phi)}, {np.cos(phi)}")
                    
        px = p * np.sin(theta) * np.cos(phi)
        py = p * np.sin(theta) * np.sin(phi)        
        pz = p * np.cos(theta) 

        mom = HepMC.FourVector(px, py, pz, e)

        gp.set_momentum(mom)
        gp.set_generated_mass(m)
        gp.set_pdg_id(self.pid(newentry["type"]))
        gp.set_status(1)

        #self.msg.debug(f"HEPMC:{px, py, pz, e}")
        #gp.print()
        
        ROOT.SetOwnership(gp, False)
        gv.add_particle_out(gp)

        return True

    def plot(self):
        "Plot entries before and after propagation/smeating for tests"

        if not self.test:
            return

        import matplotlib.pyplot as plt
        
        plt.figure()
        ebins = np.linspace(0, 5000, 50)
        plt.xlabel("Energy")
        plt.hist(self.before["E"], bins=ebins, weights = np.array(self.before["w"])/self.nsamples, histtype='step', color = "g", fill = False, label = "before")
        plt.hist(self.after["E"], bins = ebins, weights = np.array(self.after["w"])/self.nsamples, histtype='step', color = "r", fill = False, label = "after")
        plt.gca().set_yscale('log')
        plt.legend()
        plt.savefig("energy.png")

        plt.figure()
        plt.xlabel("Angle to beam in X dir")        
        thetaX =  np.pi/2. - np.arccos(np.array(self.before["cosX"]))
        thetaXout =  np.pi/2. - np.arccos(np.array(self.after["cosX"]))        
        tbins = np.linspace(-0.5, 0.5, 100)
        plt.hist(thetaX, bins = tbins, weights = np.array(self.before["w"])/self.nsamples, histtype='step', color = "g", fill = False, label = "before")
        plt.hist(thetaXout, bins = tbins, weights = np.array(self.after["w"])/self.nsamples, histtype='step', color = "r", fill = False, label = "after")
        plt.gca().set_yscale('log')
        plt.legend()
        plt.savefig("thetaX.png")

        plt.figure()
        plt.xlabel("Angle to beam in Y dir")        
        thetaY =  np.pi/2. - np.arccos(np.array(self.before["cosY"]))
        thetaYout =  np.pi/2. - np.arccos(np.array(self.after["cosY"]))        
        plt.hist(thetaY, bins = tbins, weights = np.array(self.before["w"])/self.nsamples, histtype='step', color = "g", fill = False, label = "before")
        plt.hist(thetaYout, bins = tbins, weights = np.array(self.after["w"])/self.nsamples, histtype='step', color = "r", fill = False, label = "after")
        plt.gca().set_yscale('log')
        plt.legend()
        plt.savefig("thetaY.png")

        plt.figure()
        plt.xlabel("Dispacement in X dir")        
        xbins = np.linspace(-300, 300, 100)
        plt.hist(self.before["x"], bins = xbins, weights = np.array(self.before["w"])/self.nsamples, histtype='step', color = "g", fill = False, label = "before")
        plt.hist(self.after["x"], bins = xbins, weights = np.array(self.after["w"])/self.nsamples, histtype='step', color = "r", fill = False, label = "after")
        plt.gca().set_yscale('log')
        plt.legend()
        plt.savefig("x.png")

        plt.figure() 
        plt.xlabel("Dispacement in Y dir")               
        plt.hist(self.before["y"], bins = xbins, weights = np.array(self.before["w"])/self.nsamples, histtype='step', color = "g", fill = False, label = "before")
        plt.hist(self.after["y"], bins = xbins, weights = np.array(self.after["w"])/self.nsamples, histtype='step', color = "r", fill = False, label = "after")        
        plt.gca().set_yscale('log')
        plt.legend()
        plt.savefig("y.png")

        return

def getNEvents(fname, maxEvents):
    "Work out how many events are in the file"

    n = 0
    with open(fname) as f:
        n = sum(1 for _ in f)

    if maxEvents != -1 and n > maxEvents:
        n = maxEvents

    print(">>> Setting number of real events to", n)

    return n

if __name__ == "__main__":

#     from AthenaCommon.AlgSequence import AlgSequence
#     job = AlgSequence()
#     job += FlukaReader(file_name = "/user/gwilliam/unit30_Nm", dist = (480-409))
# 
#     from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#     ostream = AthenaPoolOutputStream( "StreamEVGEN" , "evgen.pool.root", noTag=True )    
#     ostream.ItemList.remove("EventInfo#*")
#     ostream.ItemList += [ "EventInfo#McEventInfo", 
#                            "McEventCollection#*" ]
#                 
#     theApp.EvtMax = 1000


    import argparse, sys    
    parser = argparse.ArgumentParser(description="Run Fluka reader")
    parser.add_argument("file", help = "Path to fluka file")
    parser.add_argument("--dist", "-d", default = 0, type = float, help = "depth of standard rock to propagate through [m]")
    parser.add_argument("--pos", "-z", default = -3.75, type = float, help = "Position in z in FASER coordinate system [m]")    
    parser.add_argument("--output", "-o",  default = "evgen.pool.root", help = "Name of output file")
    parser.add_argument("--mcEventKey", "-k",  default = "BeamTruthEvent", help = "Name of MC collection")
    parser.add_argument("--nevents", "-n", default = -1, type = int, help = "Number of events to process")
    parser.add_argument("--randomSeed", "-r", default=12345, type=int, help = "Seed for random number generator")
    parser.add_argument("--nsamples", "-s", default = 1, type = int, help = "Number of times to sample each event")
    parser.add_argument("--test", "-t", action = "store_true", help = "Make test plots")    
    args = parser.parse_args()


    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG, INFO
    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    configFlags = initConfigFlags()
    configFlags.Input.isMC = True
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
    configFlags.GeoModel.FaserVersion     = "FASER-01"           # Default FASER geometry
    configFlags.Detector.EnableFaserSCT = True
    configFlags.Output.EVNTFileName = args.output    
    configFlags.lock()

    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(configFlags)
    
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    acc = ComponentAccumulator()
    reader = FlukaReader("FlukaReader", MCEventKey=args.mcEventKey, file_name = args.file, dist = args.dist, z = args.pos, randomSeed = args.randomSeed, nsamples = args.nsamples, test = args.test)
    reader.OutputLevel = INFO
    acc.addEventAlgo(reader)    
    cfg.merge(acc)

    itemList = [ "EventInfo#McEventInfo", "McEventCollection#*" ]
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(configFlags, "EVNT", itemList, disableEventTag = True))
    cfg.getEventAlgo("OutputStreamEVNT").AcceptAlgs = ["FlukaReader"]
    sc = cfg.run(maxEvents = getNEvents(args.file, args.nevents) * args.nsamples)
    sys.exit(not sc.isSuccess())    
