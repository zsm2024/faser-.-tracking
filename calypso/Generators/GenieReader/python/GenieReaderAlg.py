# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GeneratorModules.EvgenAlg import EvgenAlg
from AthenaPython.PyAthena import StatusCode, EventInfo, EventID, EventType
from AthenaCommon.SystemOfUnits import GeV, m, nanosecond
import ROOT

__author__ = "Dave Caser <dcasper@uci.edu>"

class GenieReader(EvgenAlg):
    def __init__(self, name="GenieReader", MCEventKey="BeamTruthEvent"):
        super(GenieReader,self).__init__(name=name)
        self.McEventKey = MCEventKey
        return

    def fillEvent(self, evt):
        try:
          from AthenaPython.PyAthena import HepMC3  as HepMC
        except ImportError:
          from AthenaPython.PyAthena import HepMC   as HepMC
        evt.weights().push_back(1.0)

        treeEventInfo = self.evtStore["TTreeEventInfo"]
        treeEventId = treeEventInfo.event_ID()
        runNumber = treeEventId.run_number()
        eventNumber = treeEventId.event_number()
        # print("found run/event number ", runNumber, "/", eventNumber)

        mcEventType = EventType()
        mcEventType.add_type(EventType.IS_SIMULATION)

        mcEventId = EventID(run_number = runNumber, event_number = eventNumber)
        mcEventInfo = EventInfo(id = mcEventId, type = mcEventType)
        self.evtStore.record(mcEventInfo, "McEventInfo", True, False)
        ROOT.SetOwnership(mcEventType, False)
        ROOT.SetOwnership(mcEventId, False)
        ROOT.SetOwnership(mcEventInfo, False)

        # impose axial timing constraint - time is expected in units of length, so just use z position
        pos = HepMC.FourVector(self.evtStore["vx"]*m, self.evtStore["vy"]*m, self.evtStore["vz"]*m, self.evtStore["vz"]*m)
        gv = HepMC.GenVertex(pos)
        ROOT.SetOwnership(gv, False)
        evt.add_vertex(gv)

        nParticles = self.evtStore["n"]
        pdgc = list(self.evtStore["pdgc"])
        status = list(self.evtStore["status"])
        px = list(self.evtStore["px"])
        py = list(self.evtStore["py"])
        pz = list(self.evtStore["pz"])
        E = list(self.evtStore["E"])
        M = list(self.evtStore["M"])    

        for i in range(nParticles):
            gp = HepMC.GenParticle()
            mom = HepMC.FourVector(px[i]*GeV, py[i]*GeV, pz[i]*GeV, E[i]*GeV)            
            gp.set_momentum(mom)
            gp.set_generated_mass(M[i]*GeV)
            gp.set_pdg_id(pdgc[i])
            genie_status = status[i]
            if (genie_status == 0):   # initial particle
                hepmc_status = 4
            elif (genie_status == 1): # stable final particle
                hepmc_status = 1
            elif (genie_status == 3): # decayed particle
                hepmc_status = 2
            else:                     # catch-all informational particle
                hepmc_status = 3
            gp.set_status(hepmc_status)
            ROOT.SetOwnership(gp, False)
            if (hepmc_status == 4):
                gv.add_particle_in(gp)
            else:
                gv.add_particle_out(gp)    

        return StatusCode.Success
