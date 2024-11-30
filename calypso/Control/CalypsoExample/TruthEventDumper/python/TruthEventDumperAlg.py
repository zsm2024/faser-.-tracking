# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode, McEventCollection, HepMC, CLHEP
import McParticleEvent.Pythonizations

__author__ = "Dave Caser <dcasper@uci.edu>"

class TruthEventDumperAlg(PyAthena.Alg):
    def __init__(self, name="TruthEventDumper", MCEventKey="TruthEvent"):
        super(TruthEventDumperAlg,self).__init__(name=name)
        self.MCEventKey = MCEventKey
        return

    def initialize(self):
        # self.maxLow = 0
        # self.maxMid = 0
        # self.maxHi = 0
        return StatusCode.Success


    def execute(self):
        evtCollection = self.evtStore[self.MCEventKey] 
        for mcEvt in evtCollection:
            mcEvt.print()
            # for mcParticle in mcEvt.particles:
            #     barCode = mcParticle.barcode()
            #     self.maxLow = max(self.maxLow, barCode%200000)
            #     if barCode%1000000 > 200000:
            #         self.maxMid = max(self.maxMid, barCode%1000000 - 200000)
            #     self.maxHi = max(self.maxHi, barCode//1000000)
        return StatusCode.Success

    def finalize(self):
        # print("Low part: ", self.maxLow, " out of 200000 (",100*self.maxLow/200000,"% of overflow)")
        # print("Mid part: ", self.maxMid, " out of ", 1000000 - 200000, " (",100*self.maxMid/(1000000-200000),"% of overflow")
        # print("Hi  part: ", self.maxHi, " out of ", (1<<31)//1000000, " (", 100*self.maxHi/((1<<31)//1000000),"% of overflow")

        return StatusCode.Success
