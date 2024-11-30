import random
from math import pi, sin, cos, sqrt
from ParticleGun.samplers import Sampler, mksampler
import ROOT

class RadialPosSampler(Sampler):
    """
    Sampler of Position 3-vectors, for modelling a beamspot, based on radius.
    """

    def __init__(self, r, x, y, z, t = 0, axialTiming = True):
        self.radius = r
        self.x = x
        self.y = y
        self.z = z
        self.t = t
        self.axialTiming = axialTiming

    @property
    def z(self):
        "z position sampler"
        return self._z
    @z.setter
    def z(self, z):
        self._z = mksampler(z)

    @property
    def t(self):
        "Time sampler"
        return self._t
    @t.setter
    def t(self, t):
        self._t = mksampler(t)

    @property
    def r(self):
        "r position sampler"

        #fwhm = 2*self.radius
        #sig = fwhm/(2 * sqrt(2 * log(2)))

        if isinstance(self.radius, list) and len(self.radius) == 2:
            # If a list of length 2, generate uniformally over an annulus from r[0] to r[1]
            return sqrt(random.uniform(abs(self.radius[0]**2), abs(self.radius[1]**2)))
        elif self.radius < 0:
            # Generate uniformally up to |r| if r is < 0
            return sqrt(random.uniform(0, abs(self.radius**2)))        
        else:
            # Else generate as a Gaussian with widht r
            x = random.gauss(0, self.radius)
            y = random.gauss(0, self.radius)
            return sqrt(x**2 + y**2)

    @property 
    def phi(self):
        "phi position sampler"
        return random.uniform(0, 2*pi)

    def shoot(self):
        r = self.r
        phi = self.phi
        x = self.x + r * cos(phi)
        y = self.y + r * sin(phi)
        z = self.z()
        t = self.t()
        if self.axialTiming:
            t += z

        return ROOT.TLorentzVector(x, y, z, t)

if __name__ == "__main__":
    # test when run from command line

    import numpy as np
    import matplotlib.pyplot as plt

    xlist = []
    ylist = []

    r = RadialPosSampler(r = -10, x = 0, y =  0, z = 10)
    for i in range (100000):
        res = r.shoot()
        xlist.append(res.X())
        ylist.append(res.Y())

    xarr = np.array(xlist)
    yarr = np.array(ylist)

    plt.figure(figsize = (5,5))
    plt.subplot(2,2,1)
    plt.hist2d(xarr, yarr)
    plt.subplot(2,2,2)
    plt.hist(yarr)
    plt.subplot(2,2,3)
    plt.hist(xarr)
    plt.subplot(2,2,4)
    plt.hist(np.sqrt(xarr**2 + yarr**2))    
    plt.tight_layout()
    plt.show()

