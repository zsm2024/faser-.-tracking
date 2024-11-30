#!/usr/bin/env python

import glob
import os
import sys

print("NtupleDumping all condor jobs that produced non-empty error logs")

# make a liist of all runs and batch job numbers that failed and thus have error logs that are not empty
run_list = []
allErrorLogs_list = glob.glob('/afs/cern.ch/user/d/dfellers/faser/ntuple-dumper/run/logs/*/*err')
for error_log in allErrorLogs_list:
    if os.path.getsize(error_log) != 0:
        print('Error Log is not empty: ', error_log)
        run_num = int(error_log.split('/')[-2].split('-')[-1])
        bath_num = int(error_log.split('.')[-2])
        run_list.append([run_num,bath_num])

print("list to be re-submitted:", run_list)

ptag="p0008"
filesPerJob=100

for i,run_info in enumerate(run_list):
    runno = run_info[0]
    batch_number = run_info[1]

    print("%i of %i runs processed. Currently processing run %i-%i (%s)"%(i,len(run_list),runno,batch_number,runconfig))

    batchFile=f"batch/Run-{runno:06d}-{batch_number}.sub"
    fh=open(batchFile,"w")
    pwd=os.getenv("PWD")
    fh.write(f"""
    executable              = {pwd}/analyzeRun.sh
    arguments               = {runno} {batch_number} {filesPerJob}
    output                  = {pwd}/logs/Run-{runno:06d}/batch.{batch_number}.out
    error                   = {pwd}/logs/Run-{runno:06d}/batch.{batch_number}.err
    log                     = {pwd}/logs/Run-{runno:06d}/batch.log
    requirements            = (Arch == "X86_64" && OpSysAndVer =?= "CentOS7")
    getenv                  = False
    transfer_output_files   = ""
    +JobFlavour             = "workday"
    queue 1
    """)
    fh.close()
    os.system(f"echo condor_submit {batchFile}")
    os.system(f"condor_submit {batchFile}")
