#UTILITY
from ROOT import TChain, TH2F, TH1F
print("Root loaded.")
import matplotlib.pyplot as plt
print("MPL loaded.")
from operator import lt,eq,ne,gt
from numpy import nan, seterr, inf,arctan, array
from numpy import sum as npsum
import numpy as np
from math import sqrt
print("Other stuff loaded.")

user_input=''

seterr(all='ignore')
missingattr=[]

#CUTS
#attr: [>,<,==,!=] if none --> skip
BASICCUT={
    'TrackCount':[0,None,None,None]
    }

def BoolDaughterCut(event):
    if event.TrackCount>=2 and len(event.tracksTruthBarcode)>=2:
        return 2 in event.tracksTruthBarcode and 3 in event.tracksTruthBarcode
    else: return False



def ApplyCut(event, cut):
    tests=[gt,lt,eq,ne]
    bool1=[]
    global missingattr
    for attr in cut.keys():
        for i in range(len(tests)):
            try:
                if cut[attr][i]!=None: 
                    value = getattr(event,attr)
                    bool1 += [tests[i](value,cut[attr][i]) and value != inf and value !=nan and value !=-1*inf]
                    if not all(bool1): break
            except AttributeError:
                if attr not in missingattr:
                    print("No attribute: ",attr)
                    missingattr+=[attr]
            
        if not all(bool1):
            break
    
    return all(bool1)

def ApplyAllCuts(event,dcuts=[],lcuts=[]): #Requires all True to be True
    #cutList = list of dictionaries
    bool1=True
    boollist=[]
    for cut in dcuts:
        bool1 = ApplyCut(event,cut)
        if not bool1: break
    if bool1:
        for lcut in lcuts:
            boollist+=[lcut(event)]
            if not all(boollist): break
    
    return all(boollist) and bool1

