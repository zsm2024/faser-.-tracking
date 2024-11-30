# Copyright (C) 2021 CERN for the benefit of the FASER collaboration

import ROOT,math,random
from random import random
from math import pi,sin,cos,acos,asin,sqrt
import FaserCosmicGenerator.Range as r
from numpy import array,add
import numpy as np


PI = pi
TWOPI = 2*pi

from ParticleGun.samplers import Sampler, SampledParticle

class CosmicSampler(Sampler):
    "Cosmic ray sampler"

    def __init__(self, chargeRatio = 1.27, maxMuonEnergyGeV = 10000, thetaMinDegree = 0, thetaMaxDegree = 80, targetDepthMeters = 85, targetDxMm = 600, targetDyMm = 600, targetDzMm = 7000,  x0Mm = 0, y0Mm = 0, z0Mm = 150):
        if thetaMinDegree > thetaMaxDegree :
            return RuntimeError("thetaMin must be less than or equal to thetaMax")

        self.maxEnergy = maxMuonEnergyGeV
        self.depth = targetDepthMeters
        # self.radiusMm = targetRadiusMm        # used for generating start position
        # self.radiusCm = self.radiusMm/10      # used for calculating rates
        self.cosThMax = math.cos(thetaMinDegree*PI/180)
        self.cosThMin = math.cos(thetaMaxDegree*PI/180)
        self.x0 = x0Mm
        self.y0 = y0Mm
        self.z0 = z0Mm
        self.dx = targetDxMm
        self.dy = targetDyMm
        self.dz = targetDzMm
        self.areaXZ = self.dx * self.dz  # top
        self.areaXY = 2 * self.dx * self.dy  # front/back
        self.areaYZ = 2 * self.dy * self.dz  # sides
        # Accumulate, including constant solid-angle factors
        self.weightedXZ = PI * self.areaXZ
        self.weightedXY = self.weightedXZ + 2 * self.areaXY
        self.weightedYZ = self.weightedXY + 2 * self.areaYZ
        self.chargeRatio = chargeRatio
        # read range data
        # compute rate

    def shoot(self): #don't, though :(
        # generate one event here
        self.genPosition = ROOT.TLorentzVector()
        self.genMomentum = ROOT.TLorentzVector()
        self.pdgCode = 13

        CR=CosmicRay(sampler = self)

        x, y, z    = CR.pos
        px, py, pz = CR.mom

        # impose vertical timing constraint
        self.genPosition.SetXYZT(x + self.x0, y + self.y0, z + self.z0, -(y + self.y0)/math.fabs(CR.costh))
        self.genMomentum.SetPxPyPzE(px,py,pz,CR.Efinal)

        particles = []
        p = SampledParticle(CR.pid)
        p.mom = self.genMomentum
        p.pos = self.genPosition
        particles.append(p)
        return particles

    def __call__(self):
        return self.shoot()

class CosmicRay:
    def __init__(self, sampler):
        self.pid=0
        self.sampler=sampler #stores settings from Sampler class

        self.mass=105.658 #MeV (muon)
        self.Einit=0 #GeV
        self.EiMeV=0 #MeV
        self.Efinal=0 #GeV
        self.EfMeV= 0 #MeV
        self.pos=(0,0,0)
        self.direction=(0,0,0) #
        self.mom=(0,0,0)
        self.th=0 #theta, radians
        self.phi=0 #angle that particle approaches z (vertical) axis
        self.costh=0 #cos(th)
        self.mc_valid=False #monte carlo/keepit bool
        self.deepEnough=False #Does particle reach ddepth?
        self.top = False
        self.frontback = False
        self.sides = False

        self.fill()

    def fill(self):
        #Generate theta/energy combinations until a combo has enough energy to reach detector
        while not self.deepEnough or not self.mc_valid:
            self.genEntry()
            self.genCosTh()
            Emin = r.muEnergy(self.sampler.depth)
            self.genEnergy(Emin, self.sampler.maxEnergy)
            self.deepEnough = self.checkDepth()

            if self.top :
                geoWeight = self.costh
            else:
                geoWeight = sqrt(1 - self.costh**2)

            if r.keepit(geoWeight * r.getValforMC(self.Einit, self.costh), random()) and self.deepEnough:
                self.Efinal = r.muSlant(self.Einit, self.sampler.depth, self.costh)
                self.EfMeV = self.Efinal*1000
                P = r.getMom(self.EfMeV + self.mass, self.mass)
                self.genCoords()
                self.mom=P*array(self.direction)
                if random() < self.sampler.chargeRatio / (self.sampler.chargeRatio + 1):
                    self.pid = -13  # mu+
                else:
                    self.pid = 13   # mu-
                self.mc_valid=True
        
            
    def genEntry(self):
        # Generate entry surface to try
        r = random()
        if r < self.sampler.weightedXZ/self.sampler.weightedYZ :
            self.top = True
            self.frontback = False
            self.sides = False
        elif r < self.sampler.weightedXY/self.sampler.weightedYZ :
            self.frontback = True
            self.top = False
            self.sides = False
        else :
            self.sides = True
            self.top = False
            self.frontback = False

    def genCosTh(self):
        cosmin,cosmax=self.sampler.cosThMin,self.sampler.cosThMax
        maxdiff=cosmax-cosmin
        self.costh=cosmin+(maxdiff*random())
        self.th=acos(self.costh)

    def genEnergy(self, Emin, Emax, gamma=2.7):
        #Generates initial kinetic energy of muon
        self.Einit=(Emin**(1-gamma)+random()*(Emax**(1-gamma)-Emin**(1-gamma)))**(1/(1-gamma))
        self.EiMeV = self.Einit*1000

    def getPtleDepth(self):
        #Calculates how deep(vertically) a particle with Einit penetrates into earth
        mulength=r.muRange(self.Einit)
        return mulength/self.costh

    def setDepth(self):
        #Calculates how deep(vertically) a particle with Einit penetrates into earth
        mulength=r.muRange(self.Einit)
        return mulength*self.costh

    def checkDepth(self):
        mulength=r.muRange(self.Einit)
        mudepth=self.setDepth()
        return mudepth>=self.sampler.depth

    def genCoords(self):
        face = 1
        if self.top :
            phi = TWOPI*random()
            self.pos = ( (random()-0.5)*self.sampler.dx, self.sampler.dy/2, (random()-0.5)*self.sampler.dz )
        else:
            if random() < 0.5 :
                face = -1
            if self.frontback :
                phi = asin(2 * random() - 1)
                self.pos = ( (random()-0.5)*self.sampler.dx, (random()-0.5)*self.sampler.dy, -face * self.sampler.dz/2 )
            else :
                phi = acos(1 - 2 * random())
                self.pos = ( -face * self.sampler.dx/2, (random()-0.5)*self.sampler.dy, (random()-0.5)*self.sampler.dz )



        costheta = self.costh
        sintheta = sqrt(1 - costheta**2)
        ## Do rotation by hand
        self.direction = array((face*sin(phi)*sintheta, -costheta, face*cos(phi)*sintheta))


    
