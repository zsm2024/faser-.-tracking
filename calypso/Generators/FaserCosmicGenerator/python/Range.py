#!/usr/bin/env python
# coding: utf-8

import numpy as np

from math import sqrt,cos,sin
import scipy.interpolate
from FaserCosmicGenerator.rockdata import rockdata as rockData

rockDensity = 2.65 # gm/cm^3 for standard rock
##with open('stdRockData.csv',newline='') as csvFile:
##    reader=csv.DictReader(csvFile)
##    for row in reader:
##        rockData.append(row)
#print("Read ",len(rockData)," entries")
# Splines are log(X) vs. log(T), and vice-versa
# Expect data file has energies in MeV and depth in gm/cm^2
logE = [np.log(np.float64(d[0])) for d in rockData[1:]]
logX = [np.log(np.float64(d[3])) for d in rockData[1:]]
xFromE = scipy.interpolate.CubicSpline(logE, logX, bc_type='natural')
eFromX = scipy.interpolate.CubicSpline(logX, logE, bc_type='natural')

def muRange(eKin):
    # returns range in meters of standard rock for specified muon kinetic energy in GeV
    return np.exp(xFromE(np.log(eKin*1000)))/rockDensity/100

def muEnergy(xRange):
    # returns muon kinetic energy in GeV to penetrate specified range in meters of standard rock
    #print('xr:',xRange)
    return np.exp(eFromX(np.log(xRange*100*rockDensity)))/1000

def muPropagate(eStart, xDepth):
    # returns the kinetic energy of a muon which starts with kinetic energy eStart after traveling xDepth meters of standard rock
    fullRange = muRange(eStart)
    if (fullRange <= xDepth):
        return 0
    return muEnergy(fullRange - xDepth)

def muSlant(eStart, xDepth, cosTheta):
    # returns the kinetic energy of a muon which starts with kinetic energy eStart after penetrating to a depth xDepth in meters of standard rock
    # cosTheta is the cosine(zenith angle); 1 = vertical downward, 0 = horizontal
    if (cosTheta <= 0 or cosTheta > 1):
        print("muSlant: cosTheta (", cosTheta, ") out of range; must be less than or equal to 1 and greater than 0")
        return 0
    fullRange = muRange(eStart)
    slantDepth = xDepth/cosTheta
    if (fullRange <= slantDepth):
        return 0
    return muEnergy(fullRange - slantDepth)

def getCosThetaCorrected(costh,P1=0.102573,P2=-0.068287,P3=0.958633,P4=0.0407253,P5=0.817285):
    tn=(costh**2)+(P1**2)+(P2*(costh**P3))+(P4*(costh**P5))
    td=1+(P1**2+P2)+P4
    return sqrt(tn/td)

def dIdE(costh,Emu):
    #From DayaBay paper, eqn2
    #Takes cos(angle of incidence), a muon energy, and returns Flux(?)
    costhetaS=getCosThetaCorrected(costh)

    B1=(1.1*Emu*costhetaS)/115
    B2=(1.1*Emu*costhetaS)/850
    B3=Emu*(costhetaS**1.29)
    
    A1=Emu*(1+(3.64/B3))
    A2=(1/(1+B1))+(0.054/(1+B2))

    final=0.14*(A1**-2.7)*A2
    return final

def keepit(val1,val2,disable=False): #For monte carlo reasons.
    if disable: return True
    else: return val1>val2

def getminE(costh): #gfun(x)
    fdepth=85
    return muEnergy(fdepth/costh)

def dIdE_swap(Emu,costh): #func(y,x)
    #From DayaBay paper, eqn2
    #Takes cos(angle of incidence), a muon energy, and returns Flux(?)
    costhetaS=getCosThetaCorrected(costh)

    B1=(1.1*Emu*costhetaS)/115
    B2=(1.1*Emu*costhetaS)/850
    B3=Emu*(costhetaS**1.29)
    
    A1=Emu*(1+(3.64/B3))
    A2=(1/(1+B1))+(0.054/(1+B2))

    final=0.14*(A1**-2.7)*A2
    return final

def getValforMC(Einit,costh,Edenom=0.09):
    return (Einit**2.7)*dIdE(costh,Einit)/Edenom

def Rx(th=0):
    #Rotate on x axis
    Rx=np.array(((1,0,0),
             (0,cos(th),-sin(th)),
             (0,sin(th), cos(th))))
    return Rx

def Rz(phi=0):
    #Rotate on x axis
    Rz = np.array(((cos(phi),-sin(phi),0),
                  (sin(phi), cos(phi),0),
                  (0,0,1)))
    return Rz

def Ry(psi=0):
    #Rotate on x axis
    Ry = np.array(((cos(psi),0,sin(psi)),
                  (0,1,0),
                  (-sin(psi),0,cos(psi))))
    return Ry


def dot(M1,M2):
    return np.dot(M1,M2)

def getMom(E,m):
    #assuming c=1
    return sqrt(E**2-(m**2))

#def getRate(R,cthmin=0,cthmax=1,Emax=100000):
#    a1,a2=dblquad(dIdE_swap,cthmin,cthmax,getminE,Emax)
#    return a1*3.14*(R**2)*2*3.14


