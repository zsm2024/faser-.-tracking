#!/usr/bin/env python

#TODO: exponential decay lifetime?

import ParticleGun as PG
from math import sqrt, sin, cos, acos
from random import uniform
from AthenaCommon.SystemOfUnits import TeV, GeV, MeV
from AthenaCommon.PhysicalConstants import pi
from ROOT import TLorentzVector

class CylinderSampler(PG.Sampler):
    """
    Sampler of position 4-vectors within a cylindrical volume.
    """

    def __init__(self, rsq, phi, z, t=0, axialTiming = True):
        self.rsq   = rsq
        self.phi = phi
        self.z   = z
        self.t   = t
        self.axialTiming = axialTiming

    @property
    def rsq(self):
        "rsq position sampler"
        return self._rsq
    @rsq.setter
    def rsq(self, rsq):
        self._rsq = PG.mksampler(rsq)

    @property
    def phi(self):
        "phi position sampler"
        return self._phi
    @phi.setter
    def phi(self, phi):
        self._phi = PG.mksampler(phi)

    @property
    def z(self):
        "z position sampler"
        return self._z
    @z.setter
    def z(self, z):
        self._z = PG.mksampler(z)

    @property
    def t(self):
        "Time sampler"
        return self._t
    @t.setter
    def t(self, t):
        self._t = PG.mksampler(t)

    def shoot(self):
        r   = sqrt(self.rsq())
        phi = self.phi()
        z   = self.z()
        t   = self.t()
        if self.axialTiming:
            t += z
        #print "POS =", x, y, z, t
        return TLorentzVector(r * cos(phi), r * sin(phi), z, t)

class DIFSampler(PG.ParticleSampler):
    """
    A particle gun which models 2 particle decay.
    """
    class particle:
        """
        A helper class to hold information about a particle.
        """
        def __init__(self,pid = None,mass = None,mom=None,theta=None,phi=None,pos=None):
            self.pid = pid
            if mass is None:
                self.mass = PG.MASSES[abs(pid)]
            else:
                self.mass = mass

            self.mom_mag = mom
            self.theta = theta
            self.phi = phi
            self.pos = pos

            if self.mass is not None and self.mom_mag is not None:
                self.E = sqrt(self.mom_mag**2 + self.mass**2)
            else:
                self.E = None

            if self.theta is not None and self.phi is not None and self.mass is not None and self.E is not None:
                self.mom = PG.EThetaMPhiSampler(self.E,self.theta,self.mass,self.phi).shoot()
            else:
                self.mom = None

    # def __init__(self, daughter1_pid = 11, daughter2_pid = -11, mother_pid = None, mother_mom = PG.EThetaMPhiSampler(sqrt((1*TeV)**2 + (10*MeV)**2),0,10*MeV,0),mother_pos = CylinderSampler([0, 100**2],[0, 2*pi],[-1500, 0],0)):
    def __init__(self, daughter1_pid=13, daughter2_pid=-13, mother_pid=None,
                 mother_mom=PG.EThetaMPhiSampler(sqrt((1*TeV)**2 + (500*MeV)**2), [0, 0.0002], 500*MeV, [0, 2*pi]),
                 mother_pos=CylinderSampler([0, 100**2], [0, 2*pi], [-1500, 0], 0)):
        self._mother_sampler = PG.ParticleSampler(pid = mother_pid, mom = mother_mom, pos=mother_pos)

        self.daughter1 = self.particle(daughter1_pid)
        self.daughter2 = self.particle(daughter2_pid)

    @property
    def mother_sampler(self):
        return self._mother_sampler
    
    @mother_sampler.setter
    def mother_sampler(self,sampler):
        if isinstance(sampler,PG.ParticleSampler) or issubclass(type(sampler),PG.ParticleSampler):
            self._mother_sampler = sampler
        else:
            raise AttributeError("DIFSampler: Trying to set mother_sampler to something other than a ParticleSampler.")

    def calculate_decay_p(self,m0,m1,m2):
        return sqrt(m0**4 - (2*m0**2 * m1**2) + (m1**4) - (2*m0**2 * m2**2) - (2*m1**2 *m2**2) + (m2**4)) / (2*m0) 

    def lorentz_transformation(self):
        #self.mother_mom = self.mother.mom.shoot()
        Bx = self.mother_mom.Px() / self.mother_mom.E()
        By = self.mother_mom.Py() / self.mother_mom.E()
        Bz = self.mother_mom.Pz() / self.mother_mom.E()
        self.daughter1.mom.Boost(Bx,By,Bz)
        self.daughter2.mom.Boost(Bx,By,Bz)

    def shoot(self):
        "Mother particle decays into two daughter particles"
        ## Shoot daughter 1 in a random direction in CM frame
        self.daughter1.phi = uniform(0,2 * pi)

        ## ensure cos(theta) is uniform from -1 to 1
        cos_theta = uniform(-1,1)
        self.daughter1.theta = acos(cos_theta)

        self.daughter2.phi = self.daughter1.phi + pi
        self.daughter2.theta = acos(-cos_theta)

        ## The magnitude of the momentum will be equal and opposite
        self.mother = self.mother_sampler
        self.mother.mass_override = False
        mother_part = self.mother.shoot()[0]
        self.mother_mom = mother_part.mom
        self.mother_pos = mother_part.pos
        
        p = self.calculate_decay_p(self.mother.mom.mass(),self.daughter1.mass,self.daughter2.mass)

        self.daughter1.E = sqrt(self.daughter1.mass**2 + p**2)
        self.daughter2.E = sqrt(self.daughter2.mass**2 + p**2)
        
        self.daughter1.mom = PG.EThetaMPhiSampler(self.daughter1.E,self.daughter1.theta,self.daughter1.mass,self.daughter1.phi).shoot()
        self.daughter2.mom = PG.EThetaMPhiSampler(self.daughter2.E,self.daughter2.theta,self.daughter2.mass,self.daughter2.phi).shoot()

        ## boost into lab frame
        self.lorentz_transformation()

        self.mother_pos = self.mother.pos.shoot()
        d1 = PG.SampledParticle(self.daughter1.pid,self.daughter1.mom,self.mother_pos)
        d2 = PG.SampledParticle(self.daughter2.pid,self.daughter2.mom,self.mother_pos)

        return [d1,d2]

if __name__ == "__main__":
    import matplotlib.pyplot as plt
    import sys

    show_graphs = False
    if len(sys.argv) > 1 and sys.argv[1] == "-g":
        show_graphs = True
    
    epsilon = 10**-6 #account for round off error

    ## test motionless mother particle
    mother_pid = 321 #K+
    mother_mass = PG.MASSES[mother_pid]
    daughter0_pid = 211 #pi+
    daughter1_pid = 111 #pi0

    ## check uniformifty of angles
    if show_graphs:
        cos_thetas = [[],[]]
        thetas = [[],[]]
        phis = [[],[]]
    
    n = 100
    for i in range(n):
        try:
            DIFS = DIFSampler(daughter0_pid,daughter1_pid, mother_pid) # K+ -> pi+ and pi0
            DIFS.mother_sampler.mom = PG.EThetaMPhiSampler(mother_mass,0,mother_mass,0)
            daughters = DIFS.shoot()

            mother_mom = DIFS.mother_sampler.mom.shoot()

            assert daughters[0].mom.E() + daughters[1].mom.E() == mother_mass
            assert (daughters[0].mom.E() + daughters[1].mom.E())**2 - (daughters[0].mom.P() - daughters[1].mom.P())**2 == (PG.MASSES[mother_pid])**2
            assert abs(daughters[0].mom.P() - daughters[1].mom.P()) <= epsilon 
            assert daughters[0].mom.Theta() - epsilon <= abs(pi - daughters[1].mom.Theta()) <= daughters[0].mom.Theta() + epsilon
            assert abs(daughters[0].mom.Px() + daughters[1].mom.Px()) <= epsilon
            assert abs(daughters[0].mom.Py() + daughters[1].mom.Py()) <= epsilon
            assert abs(daughters[0].mom.Pz() + daughters[1].mom.Pz()) <= epsilon
            assert daughters[0].pos.X() == daughters[1].pos.X() == DIFS.mother_pos.X()
            assert daughters[0].pos.Y() == daughters[1].pos.Y() == DIFS.mother_pos.Y()
            assert daughters[0].pos.Z() == daughters[1].pos.Z() == DIFS.mother_pos.Z()

            if show_graphs:
                cos_thetas[0].append(cos(daughters[0].mom.Theta()))
                cos_thetas[1].append(cos(daughters[1].mom.Theta()))
                thetas[0].append(daughters[0].mom.Theta())
                thetas[1].append(daughters[1].mom.Theta())
                phis[0].append(daughters[0].mom.Phi())
                phis[1].append(daughters[1].mom.Phi())

        except AssertionError:
            print("Error on run " + str(i))

            print("mother particle:")
            print(" E = "     + str(mother_mom.E()))
            print(" M = "     + str(mother_mom.M()))
            print(" P = "     + str(mother_mom.P()))
            print(" Px = "    + str(mother_mom.Px()))
            print(" Py = "    + str(mother_mom.Py()))
            print(" Pz = "    + str(mother_mom.Pz()))
            print(" theta = " + str(mother_mom.Theta()))
            print(" phi = "   + str(mother_mom.Phi()))
            print(" x = "     + str(DIFS.mother_pos.X()))
            print(" y = "     + str(DIFS.mother_pos.Y()))
            print(" z = "     + str(DIFS.mother_pos.Z()))

            print("daughter 0 particle:")
            print(" E = "     + str(daughters[0].mom.E()))
            print(" M = "     + str(daughters[0].mom.M()))
            print(" P = "     + str(daughters[0].mom.P()))
            print(" Px = "    + str(daughters[0].mom.Px()))
            print(" Py = "    + str(daughters[0].mom.Py()))
            print(" Pz = "    + str(daughters[0].mom.Pz()))
            print(" theta = " + str(daughters[0].mom.Theta()))
            print(" phi = "   + str(daughters[0].mom.Phi()))
            print(" x = "     + str(daughters[0].pos.X()))
            print(" y = "     + str(daughters[0].pos.Y()))
            print(" z = "     + str(daughters[0].pos.Z()))

            print("daughter 1 particle:")
            print(" E = "     + str(daughters[1].mom.E()))
            print(" M = "     + str(daughters[1].mom.M()))
            print(" P = "     + str(daughters[1].mom.P()))
            print(" Px = "    + str(daughters[1].mom.Px()))
            print(" Py = "    + str(daughters[1].mom.Py()))
            print(" Pz = "    + str(daughters[1].mom.Pz()))
            print(" theta = " + str(daughters[1].mom.Theta()))
            print(" phi = "   + str(daughters[1].mom.Phi()))
            print(" x = "     + str(daughters[1].pos.X()))
            print(" y = "     + str(daughters[1].pos.Y()))
            print(" z = "     + str(daughters[1].pos.Z()))

            raise

    if show_graphs:
        fig,axs = plt.subplots(2,3)
        axs[0,0].hist(thetas[0])
        axs[0,0].set_title("daughter 0: theta")
        axs[0,1].hist(cos_thetas[1])
        axs[0,1].set_title("daughter 0: cos(theta)")
        axs[0,2].hist(phis[1])
        axs[0,2].set_title("daughter 0: phi")
        axs[1,0].hist(thetas[1])
        axs[1,0].set_title("daughter 1: theta")
        axs[1,1].hist(cos_thetas[0])
        axs[1,1].set_title("daughter 1, cos(theta)")
        axs[1,2].hist(phis[0])
        axs[1,2].set_title("daughter 1, phi")
        plt.show()

    # test moving mother particle
    mother_pid = 15 #tau
    daughter0_pid = 16 #nu_tau
    daughter1_pid = 211 #pi+-

    n = 100
    for i in range(n):
        M = PG.MASSES[mother_pid]
        theta = acos(uniform(-1,1))
        mother_mom = PG.EThetaMPhiSampler([M,3*M],theta,M,[0,2*pi])
        mother_pos = PG.PosSampler(x=0,y=0,z=[-1500,0],t=0) 

        DIFS = DIFSampler(daughter0_pid,daughter1_pid,mother_pid) # tau -> nu_tau and pi+-
        DIFS.mother_sampler = PG.ParticleSampler(pid=mother_pid,mom=mother_mom,n=1,pos=mother_pos)
        daughters = DIFS.shoot()

        try:
            mother_mom = DIFS.mother_mom
            s = daughters[0].mom+daughters[1].mom

            assert mother_mom.E() - epsilon <= s.E() <= mother_mom.E() + epsilon
            assert mother_mom.P() - epsilon <= s.P()<= mother_mom.P() + epsilon 
            assert mother_mom.Px() - epsilon <= s.Px() <= mother_mom.Px() + epsilon
            assert mother_mom.Py() - epsilon <= s.Py() <= mother_mom.Py() + epsilon
            assert mother_mom.Pz() - epsilon <= s.Pz() <= mother_mom.Pz() + epsilon
            assert daughters[0].pos.X() == daughters[1].pos.X() == DIFS.mother_pos.X()
            assert daughters[0].pos.Y() == daughters[1].pos.Y() == DIFS.mother_pos.Y()
            assert daughters[0].pos.Z() == daughters[1].pos.Z() == DIFS.mother_pos.Z()

        except AssertionError:
            print("Error on run " + str(i))

            print("mother particle:")
            print(" E = "     + str(mother_mom.E()))
            print(" M = "     + str(mother_mom.M()))
            print(" P = "     + str(mother_mom.P()))
            print(" Px = "    + str(mother_mom.Px()))
            print(" Py = "    + str(mother_mom.Py()))
            print(" Pz = "    + str(mother_mom.Pz()))
            print(" theta = " + str(mother_mom.Theta()))
            print(" phi = "   + str(mother_mom.Phi()))
            print(" x = "     + str(DIFS.mother_pos.X()))
            print(" y = "     + str(DIFS.mother_pos.Y()))
            print(" z = "     + str(DIFS.mother_pos.Z()))

            print("daughter 0 particle:")
            print(" E = "     + str(daughters[0].mom.E()))
            print(" M = "     + str(daughters[0].mom.M()))
            print(" P = "     + str(daughters[0].mom.P()))
            print(" Px = "    + str(daughters[0].mom.Px()))
            print(" Py = "    + str(daughters[0].mom.Py()))
            print(" Pz = "    + str(daughters[0].mom.Pz()))
            print(" theta = " + str(daughters[0].mom.Theta()))
            print(" phi = "   + str(daughters[0].mom.Phi()))
            print(" x = "     + str(daughters[0].pos.X()))
            print(" y = "     + str(daughters[0].pos.Y()))
            print(" z = "     + str(daughters[0].pos.Z()))

            print("daughter 1 particle:")
            print(" E = "     + str(daughters[1].mom.E()))
            print(" M = "     + str(daughters[1].mom.M()))
            print(" P = "     + str(daughters[1].mom.P()))
            print(" Px = "    + str(daughters[1].mom.Px()))
            print(" Py = "    + str(daughters[1].mom.Py()))
            print(" Pz = "    + str(daughters[1].mom.Pz()))
            print(" theta = " + str(daughters[1].mom.Theta()))
            print(" phi = "   + str(daughters[1].mom.Phi()))
            print(" x = "     + str(daughters[1].pos.X()))
            print(" y = "     + str(daughters[1].pos.Y()))
            print(" z = "     + str(daughters[1].pos.Z()))

            raise
    
    # Test default arguments
    epsilon = 2 # increase round off error because the numbers are much larger

    n = 100
    for i in range(n):
        DIFS = DIFSampler() 
        daughters = DIFS.shoot()

        mother_mom = DIFS.mother_mom
        s = daughters[0].mom+daughters[1].mom

        try:
            assert DIFS.mother_sampler.pid() is None
            assert mother_mom.E()**2 -  mother_mom.P()**2 - epsilon <= s.E()**2 - s.P()**2 <= mother_mom.E()**2 - mother_mom.P()**2 + epsilon
            assert mother_mom.Px() - epsilon <= s.Px() <= mother_mom.Px() + epsilon
            assert mother_mom.Py() - epsilon <= s.Py() <= mother_mom.Py() + epsilon
            assert mother_mom.Pz() - epsilon <= s.Pz() <= mother_mom.Pz() + epsilon
            assert daughters[0].pos.X() == daughters[1].pos.X() == DIFS.mother_pos.X()
            assert daughters[0].pos.Y() == daughters[1].pos.Y() == DIFS.mother_pos.Y()
            assert daughters[0].pos.Z() == daughters[1].pos.Z() == DIFS.mother_pos.Z()

        except AssertionError:
            print("Error on run " + str(i))

            print("mother particle:")
            print(" E = "     + str(mother_mom.E()))
            print(" M = "     + str(mother_mom.M()))
            print(" P = "     + str(mother_mom.P()))
            print(" Px = "    + str(mother_mom.Px()))
            print(" Py = "    + str(mother_mom.Py()))
            print(" Pz = "    + str(mother_mom.Pz()))
            print(" theta = " + str(mother_mom.Theta()))
            print(" phi = "   + str(mother_mom.Phi()))
            print(" x = "     + str(DIFS.mother_pos.X()))
            print(" y = "     + str(DIFS.mother_pos.Y()))
            print(" z = "     + str(DIFS.mother_pos.Z()))

            print("daughter 0 particle:")
            print(" E = "     + str(daughters[0].mom.E()))
            print(" M = "     + str(daughters[0].mom.M()))
            print(" P = "     + str(daughters[0].mom.P()))
            print(" Px = "    + str(daughters[0].mom.Px()))
            print(" Py = "    + str(daughters[0].mom.Py()))
            print(" Pz = "    + str(daughters[0].mom.Pz()))
            print(" theta = " + str(daughters[0].mom.Theta()))
            print(" phi = "   + str(daughters[0].mom.Phi()))
            print(" x = "     + str(daughters[0].pos.X()))
            print(" y = "     + str(daughters[0].pos.Y()))
            print(" z = "     + str(daughters[0].pos.Z()))

            print("daughter 1 particle:")
            print(" E = "     + str(daughters[1].mom.E()))
            print(" M = "     + str(daughters[1].mom.M()))
            print(" P = "     + str(daughters[1].mom.P()))
            print(" Px = "    + str(daughters[1].mom.Px()))
            print(" Py = "    + str(daughters[1].mom.Py()))
            print(" Pz = "    + str(daughters[1].mom.Pz()))
            print(" theta = " + str(daughters[1].mom.Theta()))
            print(" phi = "   + str(daughters[1].mom.Phi()))
            print(" x = "     + str(daughters[1].pos.X()))
            print(" y = "     + str(daughters[1].pos.Y()))
            print(" z = "     + str(daughters[1].pos.Z()))

            raise

   # pass invalid type to mother_sampler 
    try:
        DIFS = DIFSampler() 
        DIFS.mother_sampler = PG.SampledParticle()
        raise AssertionError("mother_sampler set to improper type")
    except AttributeError:
        pass

    # pass derived type to mother_sampler
    class SP(PG.ParticleSampler):
        pass

    DIFS = DIFSampler() 
    DIFS.mother_sampler = SP()

    print("DIFSampler: All unit tests passed")
