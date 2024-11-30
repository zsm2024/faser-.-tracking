# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode, McEventCollection, CLHEP
from AthenaCommon.SystemOfUnits import m
import ROOT

try:
    from AthenaPython.PyAthena import HepMC3  as HepMC
except ImportError:
    from AthenaPython.PyAthena import HepMC   as HepMC

class ShiftLOS(PyAthena.Alg):
    def __init__(self, name="ShiftLOS", InputMCEventKey="BeamTruthEvent", OutputMCEventKey="BeamTruthEventShifted", xcross = 0, ycross = 0, xshift = 0, yshift = 0):
        super(ShiftLOS,self).__init__(name=name)
        self.InputMCEventKey = InputMCEventKey
        self.OutputMCEventKey = OutputMCEventKey
        self.xcross = xcross 
        self.ycross = ycross 
        self.xshift = xshift 
        self.yshift = yshift 
        self.distance = 480 * m # Assumes 480m is 0 of FASER coordinate system
        return

    def shift_vertices(self, evt):

        # Don't need to shift if at IP unless request explicit shift
        if not self.distance and not self.xshift and not self.yshift:
            return evt

        # Loop over all vertices
        for v in evt.vertices:
            # Get position
            pos = v.position()
            x = pos.x()
            y = pos.y()
            z = pos.z()            
            dz = self.distance + z

            # Shift x or y by appropriate crossing angle
            if self.xcross:                
                x += dz * self.xcross 
                self.msg.debug(f"Shifting x by {self.xcross*1E6} urad over {dz/1E3:.2} m: {pos.x()} -> {x} mm ")
            elif self.ycross:
                y += dz * self.ycross
                self.msg.debug(f"Shifting y by {self.ycross*1E6} urad over {dz/1E3:.2} m: {pos.y()} -> {y} mm ")

            if self.xshift:
                x += self.xshift
                self.msg.debug(f"Shifting x by {self.xshift} mm: {pos.x()} -> {x} mm ")                
            elif self.yshift:
                y += self.yshift
                self.msg.debug(f"Shifting y by {self.yshift} mm: {pos.y()} -> {y} mm ")                                
                            
            v.set_position(HepMC.FourVector(x, y, z, pos.t()))

        return evt

        
    def boost_particles(self, evt):

        if self.xcross == self.ycross == 0:
            return evt

        pxsum, pysum = 0,0
        pxsum_orig, pysum_orig = 0,0

        # Loop over all particles
        for p in evt.particles:
            # Get momentum
            mom = p.momentum()

            pxsum_orig += mom.x()
            pysum_orig += mom.y()            

            # Boost in x or y using CLHEP
            boost = CLHEP.Hep3Vector(self.xcross, self.ycross, 0.0)
            tmp = CLHEP.HepLorentzVector(mom.px(), mom.py(), mom.pz(), mom.e())
            tmp.boost(boost)
            
            pxsum += tmp.x() - mom.x()
            pysum += tmp.y() - mom.y()            

            # Convert back to HepMC
            p.set_momentum(HepMC.FourVector(tmp.px(), tmp.py(), tmp.pz(), tmp.e()))
            
            self.msg.debug(f"Change in total px = {pxsum:.1f} MeV ({pxsum/pxsum_orig * 100: .3f} %), change in total py = {pysum:.1f} MeV ({pysum/pysum_orig * 100: .3f} %)")

        return evt

    def execute(self):
        self.msg.debug(f"Exectuing {self.getName()}")

        if not self.xcross and not self.ycross and not self.xshift and not self.yshift:
            return StatusCode.Success

        self.msg.debug(f"Reading {self.InputMCEventKey}")
        inevt = self.evtStore[self.InputMCEventKey][0]

        self.msg.debug("Creating output event and collection")
        outcoll = McEventCollection()
        ROOT.SetOwnership(outcoll, False)        

        # Clone input event
        outevt = HepMC.GenEvent(inevt.__follow__()) # go from ElementProxy to element itself

        # Modify
        outevt = self.shift_vertices(outevt)
        outevt = self.boost_particles(outevt)        

        # Write output
        outcoll.push_back(outevt)
        ROOT.SetOwnership(outevt, False)                
        
        self.msg.debug(f"Recording {self.OutputMCEventKey}")
        self.evtStore.record(outcoll, self.OutputMCEventKey, True, False)

        return StatusCode.Success

if __name__ == "__main__":
    import argparse, sys    
    parser = argparse.ArgumentParser(description="Run ShiftLOS")
    parser.add_argument("infile", help = "Path to input EVNT file")
    parser.add_argument("outfile", help = "Path to output EVNT file")    
    parser.add_argument("--InputMCEventKey", "-i",  default = "BeamTruthEvent", help = "Name of Input MC collection")
    parser.add_argument("--OutputMCEventKey", "-o",  default = "BeamTruthEventShifted", help = "Name of Output MC collection")    
    parser.add_argument("--xcross", "-x", default = 0, type = float, help = "Crossing angle of LHC beam in x [urad]")
    parser.add_argument("--ycross", "-y", default = 0, type = float, help = "Crossing angle of LHC beam in y [urad]")
    parser.add_argument("--xshift",  default = 0, type = float, help = "Shift of LHC beam in x [mm]")
    parser.add_argument("--yshift",  default = 0, type = float, help = "Shift of LHC beam in y [mm]")    
    parser.add_argument("--nevents", "-n", default = -1, type = int, help = "Number of events to process")    
    args = parser.parse_args()

    # from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    configFlags = initConfigFlags()
    configFlags.Input.isMC = True
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
    configFlags.GeoModel.FaserVersion     = "FASER-01"           # Default FASER geometry
    configFlags.Detector.EnableFaserSCT = True
    configFlags.Input.Files = [ args.infile ]    
    configFlags.Output.EVNTFileName = args.outfile
    configFlags.lock()

    # Configure components
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(configFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    # from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    
    cfg = MainServicesCfg(configFlags)
    cfg.merge(PoolReadCfg(configFlags))

    # import McParticleEvent.Pythonizations
    
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    # from AthenaConfiguration.ComponentFactory import CompFactory
    
    acc = ComponentAccumulator()
    alg = ShiftLOS("ShiftLOS", InputMCEventKey=args.InputMCEventKey, OutputMCEventKey=args.OutputMCEventKey,
                   xcross = args.xcross * 1e-6, ycross = args.ycross * 1e-6, xshift = args.xshift, yshift = args.yshift)
    alg.OutputLevel = INFO
    acc.addEventAlgo(alg)    
    cfg.merge(acc)

    itemList = [ "EventInfo#McEventInfo", "McEventCollection#*" ]
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(configFlags, "EVNT", itemList, disableEventTag = True))

    sc = cfg.run(maxEvents = args.nevents)
    sys.exit(not sc.isSuccess())

    
