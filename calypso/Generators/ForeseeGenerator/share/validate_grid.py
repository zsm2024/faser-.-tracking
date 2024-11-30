import json
import subprocess as proc
from glob import glob
import os, sys

grid = "../calypso/Generators/ForeseeGenerator/share/points_ALP_W.json"

with open(grid) as f:
    data = json.load(f)

name = data["name"]
path = f"/bundle/data/FASER/Carl_Output/Foresee/{name}/"
ecom = 13.6
pid1 = data["pid1"]
pid2 = data["pid2"]

for coup, masses in data["samples"].items():
    for m in masses:
        print(coup, m, f"{path}events_{ecom}TeV_m{m}GeV_c{float(coup):.1e}to_{pid1}_{pid2}_{name}*.hepmc")
        files = glob(f"{path}events_{ecom}TeV_m{m}GeV_c{float(coup):.1e}to_{pid1}_{pid2}_{name}*.hepmc")

        if len(files) != 1:
            continue
        
        infile = files[0]
        outfile = infile.replace(".hepmc", ".EVNT.pool.root")
        valfile = infile.replace(".hepmc", ".VAL.pool.root")
        valdir = infile.replace(".hepmc", "")

        #if str(coup) in coup_fix:
        #    infile = infile.replace(str(coup), coup_fix[str(coup)])

        cmd = f"python ../calypso/Generators/ForeseeGenerator/share/convert_hepmc_to_evnt.py --filesInput {infile} Output.EVNTFileName={outfile}"
        print(">>>", cmd, "\n")
        os.system(cmd)

        cmd2 = f"python ../calypso/Generators/ForeseeGenerator/python/Validate.py -d 2 -m -n 10000 -o {valfile} {outfile}"
        print(">>>", cmd2, "\n")
        os.system(cmd2)

        cmd3 = f"python ../calypso/Generators/ForeseeGenerator/share/plot_validation.py -d 2 -o {valdir} {valfile}"
        print(">>>", cmd3, "\n")
        os.system(cmd3)        
