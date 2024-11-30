#!/usr/bin/env python3
#
# Simple utility to run as a post script
# after the noisyStripFinder
#
# Usage: checkNoisyStripHist.py <histfile> [return code]
#
import sys
if len(sys.argv) <= 1:
    sys.exit(1)
filename = sys.argv[1]
#
# Check previous return code if it is provided
if len(sys.argv) >= 3:
    rc = int(sys.argv[2])
    if rc: sys.exit(rc)
#
# Check histogram entries
# This causes a segfault.  Lets try without ROOT
#from ROOT import TFile
if False:
    import ROOT
    try:
        f = ROOT.TFile.Open(filename, 'r')
    except Exception as e:
        print(e)
        sys.exit(1)
    else:
        n = f.Get("numEvents").GetVal()
        print(f"Found {filename} with {n} entries")
        sys.exit(n==0)

from pathlib import Path
f = Path(filename)
if not f.is_file(): sys.exit(1)

size = f.stat().st_size

if size < 1000:   # Almost certainly empty
    print(f"Found {f} with size {size}!")
    sys.exit(1)

sys.exit(0)
