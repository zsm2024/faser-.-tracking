#!/usr/bin/env python                                                                                                                         

# Set up (Py)ROOT.                                                                                                                            
import ROOT
import datetime
from array import array
import math
import datetime
import pickle
import pandas
import uproot3
import uproot
import numpy as np
import awkward as ak
t1 = uproot3.open("/eos/project/f/faser-preshower/simulations/H2_beamline/1M_events/FASER_e-_v46_80GeV.root")["NTuples/GoodParticle"]


FASERdf= t1.pandas.df(["FASER_Px", "FASER_Py","FASER_Pz" ,"FASER_PDGid", "FASER_EventID", "FASER_x", "FASER_y"] )
pandas.set_option("display.max_rows", None, "display.max_columns", None)


FASERdf_new = FASERdf.groupby('FASER_EventID').agg(list)


FASERdf_new =FASERdf_new.drop(FASERdf_new.index[0])


data = {key: FASERdf_new[key].values for key in [ "FASER_Px", "FASER_Py","FASER_Pz" ,"FASER_PDGid", "FASER_x", "FASER_y"]}

a = ak.Array(data["FASER_Px"])
print(a)
#ta =a 
b = ak.Array(data["FASER_Py"])
print(b)
c = ak.Array(data["FASER_Pz"])
print(c)
d = ak.Array(data["FASER_PDGid"])
print(d)
e = ak.Array(data["FASER_x"])
print(e)
f = ak.Array(data["FASER_y"])
print(f)

with uproot.recreate("80GeV_first.root") as rootfile:
     rootfile["tree1"] = ak.zip({"FASER_Px": a, "FASER_Py": b, "FASER_Pz": c, "FASER_PDGid":d, "FASER_x":e, "FASER_y": f})

f = ROOT.TFile.Open("80GeV_e-_calypso_H2input.root", "recreate")
t = ROOT.TTree("t","t")

f1 = ROOT.TFile("80GeV_first.root")
tree1 = f1.Get("tree1")




vecPx = ROOT.std.vector(float)()
vecPy = ROOT.std.vector(float)()
vecPz = ROOT.std.vector(float)()
vecPDG = ROOT.std.vector(float)()
Num = ROOT.std.vector(int)()
t.Branch("vecPx", vecPx)
t.Branch("vecPy", vecPy)
t.Branch("vecPz", vecPz)
t.Branch("vecPDG", vecPDG)
t.Branch("N", Num)
print(tree1.GetEntries())
for x in range(tree1.GetEntries()):
     tree1.GetEntry(x)
     px= list(tree1.FASER_Px)
     py= list(tree1.FASER_Py)
     pz=list(tree1.FASER_Pz)
     pdg=list(tree1.FASER_PDGid)
     n = (tree1.n)
     j = 0
     vecPx.clear()
     vecPy.clear()
     vecPz.clear()
     vecPDG.clear()
     Num.clear()
     for j in range(n):
      vecPx.push_back(px[j])
      vecPy.push_back(py[j])
      vecPz.push_back(pz[j])
      vecPDG.push_back(pdg[j])
      Num.push_back(n)                                                                                          
     t.Fill()


f.Write()
f.Close()
