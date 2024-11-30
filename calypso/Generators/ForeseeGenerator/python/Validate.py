from AthenaPython.PyAthena import StatusCode, McEventCollection, HepMC, CLHEP
from GeneratorModules.EvgenAnalysisAlg import EvgenAnalysisAlg

import ROOT as R
import numpy as np
import os
from math import sqrt

def fix():
    "Python Fixes for HepMC"
    def add(self, other):
        self.set(self.x() + other.x(), self.y() + other.y(),
                 self.z() + other.z(), self.t() + other.t())
        return self

    HepMC.FourVector.__iadd__ = add
    del add

    return

class HistSvc(object):
    "Class to deal with histograms"

    def __init__(self):
        self.hists = {}

    def add(self, name, nbinsX = None, loX = None, hiX = None, nbinsY = None, loY = None, hiY = None, title = None, arrayX = None, arrayY = None):
        hname = os.path.basename(name)

        if title is None:  title = hname

        if nbinsY is not None:
            self.hists[name] = R.TH2F(hname, title, nbinsX, loX, hiX, nbinsY, loY, hiY)
        elif arrayX is not None and arrayY is not None:
            self.hists[name] = R.TH2F(hname, title, len(arrayX) - 1, arrayX, len(arrayY) - 1, arrayY)
        elif arrayX is not None and arrayY is None and nbinsY is not None:
            self.hists[name] = R.TH2F(hname, title, len(arrayX) - 1, arrayX, nbinsY, loY, hiY)
        elif arrayX is None and arrayY is not None:
            self.hists[name] = R.TH2F(hname, title, nbinsX, loX, hiX, len(arrayY) - 1, arrayY)            
        elif arrayX is not None:
            self.hists[name] = R.TH1F(hname, title, len(arrayX) - 1, arrayX)
        else:
            self.hists[name] = R.TH1F(hname, title, nbinsX, loX, hiX)                

    def __getitem__(self, name):
        return self.hists[name]

    def write(self, name):

        f = R.TFile.Open(name, "RECREATE")
    
        for n, h in self.hists.items():
            path = os.path.dirname(n)
            if path and not f.GetDirectory(path):
                f.mkdir(path)
            
            f.cd(path)
            h.Write()

        f.Close()

        return

class EvgenValidation(EvgenAnalysisAlg):
    "Gen-level validation"

    def __init__(self, name = "EvgenValidation", ndaughters = 2, outname = "validation.root", mother_stored = False):
        super(EvgenValidation, self).__init__(name=name)
        self.hists = HistSvc()
        self.mother_stored = mother_stored
        self.ndaughters = ndaughters
        self.outname = outname

    def binning(self):
        "binning for theta vs phi plot"
        tmin, tmax, tnum = [-6, 0, 24]
        pmin, pmax, pnum = [ 0, 5, 10]
        t_edges = np.logspace(tmin, tmax, num=tnum+1)
        p_edges = np.logspace(pmin, pmax, num=pnum+1)
        return t_edges, p_edges

    def initialize(self):

        # All daughters
        self.hists.add("PIDs", 600, -300, 300)

        # Daughter i
        tbins, pbins = self.binning()
        for i in range(self.ndaughters):
            self.hists.add(f"E_d{i}", 100, 0, 10000)                        
            self.hists.add(f"P_d{i}", 100, 0, 10000)
            self.hists.add(f"Pz_d{i}", 100, 0, 10000)
            self.hists.add(f"Pt_d{i}", 100, 0, 1)
            self.hists.add(f"Theta_d{i}", 20, 0, 0.001)
            self.hists.add(f"Phi_d{i}", 16, -3.2, 3.2)
            self.hists.add(f"ThetaVsP_d{i}", arrayX = tbins, arrayY = pbins)
            self.hists.add(f"Mass_d{i}", 5000, 0, 1)            

        # Mother
        self.hists.add("E_M", 1000, 0, 10000)
        self.hists.add("P_M", 1000, 0, 10000)
        self.hists.add("Pz_M", 1000, 0, 10000)         
        self.hists.add("Pt_M", 1000, 0, 1)       
        self.hists.add("Theta_M", 20, 0, 0.001)
        self.hists.add("Phi_M", 16, -3.2, 3.2)
        self.hists.add("Mass_M", 2000, 0, 2)
        self.hists.add("ThetaVsP_M", arrayX = tbins, arrayY = pbins)

        # Vertex
        self.hists.add("Vtx_X_LLP", 50, -100, 100)
        self.hists.add("Vtx_Y_LLP", 50, -100, 100)
        self.hists.add("Vtx_Z_LLP", 500, -5000, 5000)
        self.hists.add("Vtx_R_LLP", 20, 0, 200)        
        self.hists.add("Vtx_XY_LLP", 50, -100, 100, 50, -100, 100)

        self.hists.add("Vtx_X_All", 50, -100, 100)
        self.hists.add("Vtx_Y_All", 50, -100, 100)
        self.hists.add("Vtx_Z_All", 500, -5000, 5000)
        self.hists.add("Vtx_R_All", 20, 0, 200)        
        self.hists.add("Vtx_XY_All", 50, -100, 100, 50, -100, 100)      
        
        return StatusCode.Success


    def fillKin(self, label, p, mass = True, twoD = True):

        self.hists[f"E_{label}"].Fill(p.t()/1000, self.weight)        
        self.hists[f"P_{label}"].Fill(p.rho()/1000, self.weight)
        self.hists[f"Pz_{label}"].Fill(p.pz()/1000, self.weight)                
        self.hists[f"Pt_{label}"].Fill(p.perp()/1000, self.weight)
        self.hists[f"Theta_{label}"].Fill(p.theta(), self.weight)
        self.hists[f"Phi_{label}"].Fill(p.phi(), self.weight)

        if mass:
            self.hists[f"Mass_{label}"].Fill(p.m()/1000, self.weight)

        if twoD:
            self.hists[f"ThetaVsP_{label}"].Fill(p.theta(), p.rho()/1000, self.weight)

        return

    def fillDaughter(self, p):
        self.hists["PIDs"].Fill(p.pdg_id(), self.weight)
        return

    def fillVertex(self, label, v):
        self.hists[f"Vtx_X_{label}"].Fill(v.x(), self.weight)
        self.hists[f"Vtx_Y_{label}"].Fill(v.y(), self.weight)
        self.hists[f"Vtx_Z_{label}"].Fill(v.z(), self.weight)
        self.hists[f"Vtx_XY_{label}"].Fill(v.x(), v.y(), self.weight)
        self.hists[f"Vtx_R_{label}"].Fill(sqrt(v.x()**2 + v.y()**2), self.weight)                
        return
    

    def execute(self):
        evt = self.events()[0]
        self.weight = evt.weights()[0] if evt.weights() else 1

        # Loop over all particles in events 
        momenta = []
        vertices = []
        mother = HepMC.FourVector(0,0,0,0)
        llp_vtx = None
        
        for i, p in enumerate(evt.particles):
            #print("--- ", i)
            p.print()
            self.fillDaughter(p)

            if self.mother_stored:
                if i == 0:
                    mother = p.momentum()
                else:
                    momenta.append(p.momentum()) 
            else:
                momenta.append(p.momentum())    
                mother += p.momentum()

            if i == 0 and p.production_vertex():
                #p.production_vertex().print()
                llp_vtx = p.production_vertex().point3d()
            elif i == 0 and p.end_vertex():
                llp_vtx = p.end_vertex().point3d()                            

            if p.production_vertex():
                vertices.append(p.production_vertex().point3d())

            if p.production_vertex():
                vertices.append(p.production_vertex().point3d())

        # Fill daughter plots
        for i in range(self.ndaughters):
            if i >= len(momenta): continue
            self.fillKin(f"d{i}", momenta[i])

        # Fill mother plots
        self.fillKin("M", mother, mass = True)

        # Fill all vertices
        for v in vertices:
            self.fillVertex("All", v)

        # Fill LLP vertex plots
        if llp_vtx:
            self.fillVertex("LLP", llp_vtx)
            
        return StatusCode.Success

    def finalize(self):
        self.hists.write(self.outname)
        return StatusCode.Success

        
if __name__ == "__main__":

    import argparse, sys
    parser = argparse.ArgumentParser(description="Run gen-level validation")
    parser.add_argument("file", nargs="+", help = "full path to imput file")
    parser.add_argument("--ndaughters", "-d", default = 2, type = int, help = "Number of daugthers to plot")
    parser.add_argument("--mother_stored", "-m", default = False, action = "store_true", help = "Is mother stored in input?")    
    parser.add_argument("--output", "-o",  default = "validation.root", help = "Name of output file")
    parser.add_argument("--mcEventKey", "-k",  default = "BeamTruthEvent", help = "Name of MC collection")
    parser.add_argument("--nevents", "-n", default = -1, type = int, help = "Number of events to process")    
    args = parser.parse_args()    

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)
    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    configFlags = initConfigFlags()
    configFlags.Input.isMC = True
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
    configFlags.GeoModel.FaserVersion     = "FASER-01"           # Default FASER geometry
    configFlags.Detector.EnableFaserSCT = True
    configFlags.Input.Files = args.file
    configFlags.lock()

    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(configFlags)
    
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(configFlags))

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    import McParticleEvent.Pythonizations
    fix()
    
    acc = ComponentAccumulator()
    valid = EvgenValidation("EvgenValidation", ndaughters =  args.ndaughters, outname = args.output, mother_stored = args.mother_stored)
    valid.McEventKey = args.mcEventKey
    acc.addEventAlgo(valid)    
    cfg.merge(acc)

    sc = cfg.run(maxEvents = args.nevents)
    sys.exit(not sc.isSuccess())
