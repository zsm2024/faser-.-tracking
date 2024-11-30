# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GeneratorModules.EvgenAlg import EvgenAlg
from AthenaPython.PyAthena import StatusCode, EventInfo, EventID, EventType
from AthenaCommon.SystemOfUnits import GeV, m
import ROOT
import math


class TestBeamReader(EvgenAlg):
    def __init__(self, name="TestBeamReader", MCEventKey="BeamTruthEvent"):
        super(TestBeamReader,self).__init__(name=name)
        self.McEventKey = MCEventKey
        return

    def fillEvent(self, evt):
        try:
          from AthenaPython.PyAthena import HepMC3  as HepMC
        except ImportError:
          from AthenaPython.PyAthena import HepMC   as HepMC
        evt.weights().push_back(1.0)
        eventNumber = 0
        treeEventId = 0
        runNumber = 100
        mcEventType = EventType()
        mcEventType.add_type(EventType.IS_SIMULATION)

        mcEventId = EventID(run_number = runNumber, event_number = eventNumber)
        mcEventInfo = EventInfo(id = mcEventId, type = mcEventType)
        self.evtStore.record(mcEventInfo, "McEventInfo", True, False)
        ROOT.SetOwnership(mcEventType, False)
        ROOT.SetOwnership(mcEventId, False)
        ROOT.SetOwnership(mcEventInfo, False)
        pos = HepMC.FourVector(13.8*0.001*m, 22.7*0.001*m, 0*m, 0)
        gv = HepMC.GenVertex(pos)
        ROOT.SetOwnership(gv, False)
        evt.add_vertex(gv)

        nParticles = list(self.evtStore["N"])

        pdgclol = list(self.evtStore["vecPDG"])
        px =list(self.evtStore["vecPx"])
        py = list(self.evtStore["vecPy"])
        pz = list(self.evtStore["vecPz"])
       
        for i in range(nParticles[0]):
            gp = HepMC.GenParticle()
            pdgc = int()

            pdgc = math.floor(pdgclol[i])
            if (pdgc ==-11 or pdgc ==11):
                 M = (0.511*0.001)
            elif (pdgc == 22):
                 M = 0.0
            else:
                 M = 0.0
            E = math.sqrt(((px[i])*(px[i]))+((py[i])*(py[i]))+((pz[i])*(pz[i]))+((M)*(M)))  
            mom = HepMC.FourVector(px[i]*0.001*GeV, py[i]*0.001*GeV, pz[i]*0.001*GeV, E*0.001*GeV)            
            gp.set_momentum(mom)
            gp.set_generated_mass(M*GeV)
            gp.set_pdg_id(pdgc)

            hepmc_status = 4
            gp.set_status(hepmc_status)
            ROOT.SetOwnership(gp, False)
            gv.add_particle_in(gp)
            
         

        for i in range(nParticles[0]):
            gp = HepMC.GenParticle()

            pdgc = int()
            pdgc = math.floor(pdgclol[i])
            if (pdgc ==-11 or pdgc ==11):
                 M = (0.511*0.001)
            elif (pdgc == 22):
                 M = 0.0
            else:
                 M = 0.0
            E = math.sqrt(((px[i])*(px[i]))+((py[i])*(py[i]))+((pz[i])*(pz[i]))+((M)*(M)))
            mom = HepMC.FourVector(px[i]*0.001*GeV, py[i]*0.001*GeV, pz[i]*0.001*GeV, E*0.001*GeV)
            gp.set_momentum(mom)
            gp.set_generated_mass(M*GeV)
            gp.set_pdg_id(pdgc)
                                                                       
            hepmc_status = 1
            gp.set_status(hepmc_status)
            ROOT.SetOwnership(gp, False)
                                                                                           
            gv.add_particle_out(gp)    


        return StatusCode.Success
