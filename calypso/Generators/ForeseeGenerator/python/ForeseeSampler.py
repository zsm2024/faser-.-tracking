import os, sys
import random
import numpy as np

import ParticleGun as PG

from DIFGenerator.DIFSampler import CylinderSampler

class ForeseeNumpySampler(PG.ParticleSampler):
    """ Sample from the output of Foresee generation in numpy format with columns E, theta, weight"""
    def __init__(self, model_path = ".", model_name = "DarkPhoton",  com_energy = "14", mother_mass = 0.01,
                 coupling = None, mother_pid = None, daughter1_pid = 11, daughter2_pid = -11, randomSeed = None):

        self.path = os.path.expanduser(os.path.expandvars(model_path))
        self.modelname = model_name
        self.energy = com_energy
        self.mass = mother_mass
        self.coupling = coupling
        self.pid = mother_pid
        self.daughter1_pid = daughter1_pid
        self.daughter2_pid = daughter2_pid
        self.n = 1
        self.distance = 480 # m
        self.mass_override = False

        if randomSeed is not None:
            print(f"Setting seed to {randomSeed}")
            self.rng = np.random.default_rng(randomSeed)
        else:
            self.rng = np.random.default_rng()        

        self.xs = 0

        self.read()

    def read(self):
        "Read data from numpy file in format E, theta, weight"

        if self.path.endswith(".npy"):
            filename = self.path
        elif self.coupling is None:
            filename = f"{self.path}/files/models/{self.modelname}/events/events_{self.energy}TeV_m{self.mass}GeV_to_{self.daughter1_pid}_{self.daughter2_pid}.npy"
        else:
            filename = f"{self.path}/files/models/{self.modelname}/events/events_{self.energy}TeV_m{self.mass}GeV_c{self.coupling}to_{self.daughter1_pid}_{self.daughter2_pid}.npy"

        print(f"Reading data from file: {filename}")
        self.data = np.load(filename)

        # Create probablity for each mode as weight / sum(weights)
        self.prob = self.data[2]/np.sum(self.data[2])
        
        return

#     def mass(self):
#         "Mass converted from GeV to MeV"
#         return self._mass * 1000

    def shoot(self):
        "Choose a random item from the data, based on the probability"

        energy, theta, weight = self.rng.choice(self.data, axis = 1, p = self.prob)

        self.xs += weight

        # Convert mass to MeV
        mass = self.mass * 1000

        # Sample phi
        phi = self.rng.uniform(0, 2*np.pi)

        # Sample z
        z = self.rng.uniform(-1500, 0)        

        # Create mother momentum, randomly sampling phi
        self.mom = PG.EThetaMPhiSampler(energy, theta, mass, phi)

        # Create mother pos, randomly sampling phi
        r = (self.distance * 1000 + abs(z))  * np.tan(theta)
        
        self.pos = CylinderSampler(r**2, phi, z, 0)

        # Create particle
        p = PG.SampledParticle(self.pid)
        p.mom = self.mom.shoot()
        p.pos = self.pos.shoot()
        p.mass = mass
        #self.mom.mass = mass        

        return [p]

class ForeseeSampler(PG.MomSampler):
    """Create events from foresee directly on the fly

    Requires:
        * foresee to be downloaded and in python path

        cd <PATH>
        git clone https://github.com/KlingFelix/FORESEE.git
        export PYTHONPATH=$PYTHONPATH:<PATH>/FORESEE/src/        
        
        * scikit-hep installed

        pip install scikit-hep --user
        
        * forsee files dir symlinked to the run dir

        ln -s <PATH>/FORESEE/files .

    """
    
    
    def __init__(self, modelname, energy, mass, couplings, daughter1_pid, daughter2_pid, mother_pid = None):
        self.modelname = modelname
        self.model = Model(self.modelname)
        self.energy = energy
        self._mass = mass
        self.couplings = [couplings] if isinstance(couplings, str) else couplings
        self.mother_pid = mother_pid
        self.daughter1_pid = daughter1_pid
        self.daughter2_pid = daughter2_pid

        self.rng = np.random.default_rng()
        self.xs = 0

        if not os.path.exists("files"):
            os.symlink(os.path.expandvars("$Calypso_DIR/../calypso/Generators/foresee/files"), "files")

        self.pid_map = { 
            (11, 11) : "e_e",           
            (13, 13) : "mu_mu",
            (22, 22) : "gamma_gamma",
            }

        self.mode = self.pid_map.get((self.daughter1_pid, self.daughter2_pid), None)
        if self.mode is None:
            sys.exit(f"Undefined decay to {self.daughter1_pid} + {self.daughter2_pid} for {self.modelname}")

        from foresee import Foresee, Model, Utility
        self.foresee = Foresee()
        self.foresee.set_detector(selection="np.sqrt(x.x**2 + x.y**2)< 0.1", channels=[self.mode], distance=480, length=1.5 , luminosity=150)        


        if self.modelname == "DarkPhoton":
            self.data = self.darkphoton()
        elif self.modelname == "ALP-W":
            self.data = self.alps()
        else:
            sys.exit(f"Unknown model {self.modelname}")

        return


    def mass(self):
        return self._mass * 1000

    def darkphoton(self):

        # Production modes
        self.model.add_production_2bodydecay(
            pid0 =  "111",
            pid1 = "22",
            br = "2.*0.99 * coupling**2 * pow(1.-pow(mass/self.masses('111'),2),3)",
            generator = "EPOSLHC",
            energy = self.energy,
            nsample = 10)
    
        self.model.add_production_2bodydecay(
            pid0 = "221",
            pid1 = "22",
            br = "2.*0.39 * coupling**2 * pow(1.-pow(mass/self.masses('221'),2),3)",
            generator = "EPOSLHC",
            energy = self.energy,
            nsample = 10)

        # Handwavey 
        self.model.add_production_mixing(
            pid = "113",
            mixing = "coupling * 0.3/5. * 0.77545**2/abs(mass**2-0.77545**2+0.77545*0.147*1j)",
            generator = "EPOSLHC",
            energy = self.energy,
            )

        # Question on validity as FASER gets larger
        self.model.add_production_direct(
            label = "Brem",
            energy = self.energy,
            condition = "p.pt<1",
            coupling_ref=1,
            )
        
        self.model.add_production_direct(
            label = "DY",
            energy = self.energy,
            coupling_ref=1,
            massrange=[1.5, 10.]
            )

        return self.decays()


    def alps(self):

        self.model.add_production_2bodydecay(
            pid0 = "5",
            pid1 = "321",
            br = "2.2e4 * coupling**2 * np.sqrt((1-(mass+0.495)**2/5.279**2)*(1-(mass-0.495)**2/5.279**2))",
            generator = "Pythia8",
            energy = self.energy,
            nsample = 20, # Vary over out of phi and theta 
            )
    
        
        self.model.add_production_2bodydecay(
            pid0 = "-5",
            pid1 = "321",
            br = "2.2e4 * coupling**2 * np.sqrt((1-(mass+0.495)**2/5.279**2)*(1-(mass-0.495)**2/5.279**2))",
            generator = "Pythia8",
            energy = self.energy,
            nsample = 20,
            )
        
        self.model.add_production_2bodydecay(
            pid0 = "130",
            pid1 = "111",
            br = "4.5 * coupling**2 * np.sqrt((1-(mass+0.135)**2/0.495**2)*(1-(mass-0.135)**2/0.495**2))",
            generator = "EPOSLHC",
            energy = self.energy,
            nsample = 10,
            )

        self.model.add_production_2bodydecay(
            pid0 = "321",
            pid1 = "211",
            br = "10.5 * coupling**2 * np.sqrt((1-(mass+0.135)**2/0.495**2)*(1-(mass-0.135)**2/0.495**2))",
            generator = "EPOSLHC",
            energy = self.energy,
            nsample = 10,
            )

        return self.decays()


    def decays(self):
        # Decays
        self.model.set_ctau_1d(
            filename=f"files/models/{self.modelname}/ctau.txt", 
            coupling_ref=1
            )

        # TODO take into account BR
        self.model.set_br_1d(
            modes = [self.mode],
            filenames=[f"files/models/{self.modelname}/br/{self.mode}.txt"] 
            )

        # LLP spectrum
        self.foresee.set_model(model=self.model)
        
        plt = self.foresee.get_llp_spectrum(self._mass, coupling=1, do_plot=True)  # This is just a reference coupling 
        plt.savefig(f"{self.modelname}.png")

        def flatten(l):
            return [i for sublist in l for i in sublist]

        coups, ctaus, nsigs, energies, weights, thetas = self.foresee.get_events(mass=self._mass, energy=self.energy, couplings=self.couplings)

        return [flatten(thetas), flatten(energies), flatten(weights)] 

    def shoot(self):
        # Create probablity for each mode as weight / sum(weights)
        prob = self.data[2]/np.sum(self.data[2])

        # Choose a random item from the data, base on the probability
        # TODO: what about reuse of events?
        theta_mother, e_mother, w = self.rng.choice(self.data, axis = 1, p = prob)

        self.xs += w

        # Create other momentum
        mother_mom = PG.EThetaMPhiSampler(e_mother*1000, theta_mother, self.mass(), [0,2*np.pi])

        return mother_mom.shoot()

if __name__ == "__main__":

    # Testing ...

    from math import sqrt, log10
    import matplotlib.pyplot as plt
    import matplotlib
    from DIFGenerator import DIFSampler
    
    
    path = os.path.expandvars("$Calypso_DIR/../calypso/Generators/ForeseeGenerator/data/events_14TeV_m0.1GeV_c1e-05to_11_-11.npy")
    path = "files/models/DarkPhoton/events/events_14TeV_m0.1GeV_c1e-05to_11_-11.npy"

    modelname = "DarkPhoton"
    mass = 0.1

    theta = []
    mom = []

    d0theta = []
    d0mom = []

    d1theta = []
    d1mom = []

    # Accounting for rounding
    epsilon = 6

    # Create mother sampler reading data from foresee
    mother_sampler = ForeseeNumpySampler(model_path = path, model_name = modelname, com_energy = "14", mother_mass = 0.1, coupling = 1e-5,  mother_pid = None, daughter1_pid = 11, daughter2_pid = -11)
    
    # Create decay-in-flight
    d = DIFSampler(11, -11, None)
    d.mother_sampler = mother_sampler
    
    # Loop over a range of events
    for i in range(100000):        

        # Shoot the decay in flight
        daughters = d.shoot()

        # Get mother and sum of daugthers and check these make sense.
        mother_mom = d.mother_mom
        s = daughters[0].mom+daughters[1].mom

        try:            
            assert mother_mom.E() - epsilon <= s.E() <= mother_mom.E() + epsilon
            assert mother_mom.P() - epsilon <= s.P()<= mother_mom.P() + epsilon 
            assert mother_mom.Px() - epsilon <= s.Px() <= mother_mom.Px() + epsilon
            assert mother_mom.Py() - epsilon <= s.Py() <= mother_mom.Py() + epsilon
            assert mother_mom.Pz() - epsilon <= s.Pz() <= mother_mom.Pz() + epsilon
            assert daughters[0].pos.X() == daughters[1].pos.X() == d.mother_pos.X()
            assert daughters[0].pos.Y() == daughters[1].pos.Y() == d.mother_pos.Y()
            assert daughters[0].pos.Z() == daughters[1].pos.Z() == d.mother_pos.Z()
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
            print(" x = "     + str(d.mother_pos.X()))
            print(" y = "     + str(d.mother_pos.Y()))
            print(" z = "     + str(d.mother_pos.Z()))

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

        # Store mother info to plot
        theta.append(log10(mother_mom.Theta()))
        mom.append(log10(mother_mom.P()/1000.))

        # Store mother info to plot
        d0theta.append(log10(daughters[0].mom.Theta()))
        d0mom.append(log10(daughters[0].mom.P()/1000.))
        d1theta.append(log10(daughters[1].mom.Theta()))
        d1mom.append(log10(daughters[1].mom.P()/1000.))
        

 
    # Plot mother from sampling events
    prange=[[-6, 0, 120],[ 0, 5, 50]]
    tmin, tmax, tnum = prange[0]
    pmin, pmax, pnum = prange[1]
    t_edges = np.logspace(tmin, tmax, num=tnum+1)
    p_edges = np.logspace(pmin, pmax, num=pnum+1)   

    ticks = np.array([[np.linspace(10**(j),10**(j+1),9)] for j in range(-7,6)]).flatten()
    ticks = [np.log10(x) for x in ticks]
    ticklabels = np.array([[r"$10^{"+str(j)+"}$","","","","","","","",""] for j in range(-7,6)]).flatten()
    matplotlib.rcParams.update({'font.size': 15})

    
    fig = plt.figure(figsize=(8,5.5))
    ax = plt.subplot(1,1,1)
    h=ax.hist2d(x=theta,y=mom,
                bins=[tnum,pnum],range=[[tmin,tmax],[pmin,pmax]],
                norm=matplotlib.colors.LogNorm(), cmap="hsv",
                )
    fig.colorbar(h[3], ax=ax)
    ax.set_xlabel(r"angle wrt. beam axis $\theta$ [rad]")
    ax.set_ylabel(r"momentum $p$ [GeV]")
    ax.set_xticks(ticks)
    ax.set_xticklabels(ticklabels)
    ax.set_yticks(ticks)
    ax.set_yticklabels(ticklabels)
    ax.set_xlim(tmin, tmax)
    ax.set_ylim(pmin, pmax)
    plt.savefig(f"{modelname}_PG_m{mass}.png")

    fig = plt.figure(figsize=(8,5.5))
    ax = plt.subplot(1,1,1)
    h=ax.hist2d(x=d0theta,y=d0mom,
                bins=[tnum,pnum],range=[[tmin,tmax],[pmin,pmax]],
                norm=matplotlib.colors.LogNorm(), cmap="hsv",
                )
    fig.colorbar(h[3], ax=ax)
    ax.set_xlabel(r"angle wrt. beam axis $\theta$ [rad]")
    ax.set_ylabel(r"momentum $p$ [GeV]")
    ax.set_xticks(ticks)
    ax.set_xticklabels(ticklabels)
    ax.set_yticks(ticks)
    ax.set_yticklabels(ticklabels)
    ax.set_xlim(tmin, tmax)
    ax.set_ylim(pmin, pmax)
    plt.savefig(f"{modelname}_PG_d0_m{mass}.png")

    fig = plt.figure(figsize=(8,5.5))
    ax = plt.subplot(1,1,1)
    h=ax.hist2d(x=d1theta,y=d1mom,
                bins=[tnum,pnum],range=[[tmin,tmax],[pmin,pmax]],
                norm=matplotlib.colors.LogNorm(), cmap="hsv",
                )
    fig.colorbar(h[3], ax=ax)
    ax.set_xlabel(r"angle wrt. beam axis $\theta$ [rad]")
    ax.set_ylabel(r"momentum $p$ [GeV]")
    ax.set_xticks(ticks)
    ax.set_xticklabels(ticklabels)
    ax.set_yticks(ticks)
    ax.set_yticklabels(ticklabels)
    ax.set_xlim(tmin, tmax)
    ax.set_ylim(pmin, pmax)
    plt.savefig(f"{modelname}_PG_d1_m{mass}.png")
    

    print (f"x-sect = {mother_sampler.xs} pb")
    


        
