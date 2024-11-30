# Decay In Flight Sampler

The DIFSampler models a two-body decay.

## Usage

The `runG4DecayInFlight.py` file shows a default configuration.

Options available through the `DIFSampler` constructor include:

* daughter1_pid - The pid of the first decay particle. The default valid pids are listed [here](https://gitlab.cern.ch/atlas/athena/-/blob/master/Generators/ParticleGun/python/samplers.py#L825). (default: 11) 
* daughter2_pid - The pid of the second decay particle. The default valid pids are listed [here](https://gitlab.cern.ch/atlas/athena/-/blob/master/Generators/ParticleGun/python/samplers.py#L825). (default: -11)
* mother_pid - The pid of the mother particle. Only set this if you need to get the mother's pid from the mother_sampler property (default: None)
* mother_mom - A momentum sampler representing the mother particle's momentum. (default: PG.EThetaMPhiSampler(sqrt((1 * TeV)**2 + (10 * MeV)**2),0,10*MeV,0))
* mother_pos - A position sampler representing the mother particle's position. (default: DIFGenerator.CylinderSampler(rsq = [0, (100*mm)**2], phi = [0,2*pi], z = [-1500*mm,0]))

The pid, momentum sampler, and position sampler of the mother particle can be modified after initialization by setting the `mother_sampler` property of the `DIFSampler`. The `mother_sampler` property should be a [ParticleSampler](https://gitlab.cern.ch/atlas/athena/-/blob/master/Generators/ParticleGun/python/samplers.py#L858).

