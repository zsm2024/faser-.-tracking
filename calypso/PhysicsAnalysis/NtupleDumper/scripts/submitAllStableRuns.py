#!/usr/bin/env python

import glob
import os
import sys
import pandas as pd

print("NtupleDumping all stable-beam runs found at http://aagaard.web.cern.ch/aagaard/FASERruns.html")

table_runs = pd.read_html('http://aagaard.web.cern.ch/aagaard/FASERruns.html') # load in run tables from website
df = table_runs[0] # table_runs is a list of all tables on the website, we only want the first table
df.columns = [c.replace(' ', '_') for c in df.columns] # rename the columns such that names with spaces are replaced with '_' (needed to access 'Stable_Beam' column)
df.drop(df[df.Stable_Beam != 'Yes'].index, inplace=True) # drop all runs that are not stable beam runs
df.drop(df[(df.Configuration != 'Low gain') & (df.Configuration != 'High gain')].index, inplace=True) # drop all runs that are not 'Low gain' or 'High gain'

run_list = df['Run'].tolist()

ptag="p0008"
filesPerJob=100

for i,runno in enumerate(run_list):
    print("%i of %i runs processed. Currently processing run %i (%s)"%(i,len(run_list),runno,runconfig))

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
    arguments               = {runno} $(ProcId) {filesPerJob} 
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
    os.system(f"condor_submit {batchFile}")

