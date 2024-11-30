#!/usr/bin/env python

import glob
import os
import sys

ptag="p0008"
filesPerJob=100

runno=int(sys.argv[1])

os.system(f"mkdir -p logs/Run-{runno:06d}")
os.system(f"mkdir -p batch")
os.system(f"mkdir -p /eos/project/f/faser-commissioning/DeionsNtuples/{runno}")

dataDir=f"/eos/experiment/faser/rec/2022/{ptag}/{runno:06d}"
files=glob.glob(f"{dataDir}/Faser-Physics*")
numFiles=len(files)
numJobs=numFiles//filesPerJob+(numFiles%filesPerJob!=0)
batchFile=f"batch/Run-{runno:06d}.sub"
fh=open(batchFile,"w")
pwd=os.getenv("PWD")
fh.write(f"""
executable              = {pwd}/analyzeRun.sh
arguments               = {runno} $(ProcId) {filesPerJob} {ptag}
output                  = {pwd}/logs/Run-{runno:06d}/batch.$(ProcId).out
error                   = {pwd}/logs/Run-{runno:06d}/batch.$(ProcId).err
log                     = {pwd}/logs/Run-{runno:06d}/batch.log
requirements            = (Arch == "X86_64" && OpSysAndVer =?= "CentOS7")
getenv                  = False
transfer_output_files   = ""
+JobFlavour             = "workday"
queue {numJobs}
""")
fh.close()
os.system(f"echo condor_submit {batchFile}")

