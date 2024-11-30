#!/usr/bin/env python3
#
import os
import sys
import math
import array
import itertools

# Triggers: 0x01 - calo, 0x02 - veto, 0x03 - timing, 0x10 - random

def usage():
    print("Usage: timing_check.py <filename>|<dirname> [triggermask]")

if len(sys.argv) == 1:
    usage()
    sys.exit(-1)

# Extract tracker station requirements
if len(sys.argv) == 3:
    trigmask = int(sys.argv[2])
    extra = f"_{triggermask}"
else:
    trigmask = 0xFF
    extra = ''

from pathlib import Path

import ROOT
ROOT.xAOD.Init().ignore()
ROOT.xAOD.AuxContainerBase()
os.environ["XAOD_ACCESSTRACER_FRACTION"] = "0.0"

#
# Open file or files
pathname = Path(sys.argv[1])

# Is this a directory?
if pathname.is_dir():
    print(f"Opening files in directory {pathname.name}")

    t2 = ROOT.TChain("CollectionTree")
    nfiles = t2.Add(str(pathname)+'/Faser-Physics*.root')

    if (nfiles == 0):
        print(f"TChain found no files!")
        usage()
        sys.exit(0)

    # Make transient tree
    t1 = ROOT.xAOD.MakeTransientTree(t2)

    # Make output file name
    outfile = pathname.name + "_timing"+extra+".pdf"

    print(f"TChain found {nfiles} files with {t2.GetEntries()} events")

    avperfile = t2.GetEntries() / nfiles

# Is this a file?
elif pathname.is_file():
    print(f"Opening file {pathname.name}")

    t2 = ROOT.TChain("CollectionTree")
    nfiles = t2.Add(str(pathname))

    if (nfiles != 1):
        print(f"TChain error opening file!")
        usage()
        sys.exit(0)

    print(f"Opened file with {t2.GetEntries()} events")

    avperfile = t2.GetEntries()

    # Make transient tree
    t1 = ROOT.xAOD.MakeTransientTree(t2)

    # Make outfile name from input
    outfile = pathname.stem + "_timing"+extra+".pdf"

# Neither?
else:
    print(f"Can't understand {pathname.name}")
    usage()
    sys.exit(-1)

class ClockPlots:

    def __init__(self):

        # Ranges for plots
        self.freq_bins = 80
        self.freq_lo = 40.0
        self.freq_hi = 40.2

        self.th_bins = 100

    def init(self, tree):

        self.h_freq = ROOT.TH1I("", "Clock Frequency", self.freq_bins, self.freq_lo, self.freq_hi)
        self.h_freq.GetXaxis().SetTitle("Clock Frequency (MHz)")
        self.h_freq.GetYaxis().SetTitle("Events")
        #self.h_freq.Sumw2()

        self.h_phase = ROOT.TH1I("", "Clock Phase", 60, 2*(-3.1416), 2*3.1416)
        self.h_phase.GetXaxis().SetTitle("Clock Phase")
        self.h_phase.GetYaxis().SetTitle("Events")

        self.h_amp = ROOT.TH1I("", "Amplitude", 50, 0, 2000.)
        self.h_amp.GetXaxis().SetTitle("Clock Amplitude (mV)")
        self.h_amp.GetYaxis().SetTitle("Events")

        self.h_off = ROOT.TH1I("", "Offset", 50, 0, 2000.)
        self.h_off.GetXaxis().SetTitle("Clock Offset (mV)")
        self.h_off.GetYaxis().SetTitle("Events")

    def fill(self, tree):

        # First, create the histograms
        self.init(tree)

        # Iterate over all entries
        nev = tree.GetEntries()
        iev = 0
        for ev in tree:
            self.h_freq.Fill(ev.WaveformClock.frequency())
            self.h_phase.Fill(ev.WaveformClock.phase())
            self.h_amp.Fill(ev.WaveformClock.amplitude())
            self.h_off.Fill(ev.WaveformClock.dc_offset())

            # Protect against reading off the end
            iev += 1
            if iev == nev: break

    def draw(self, canvas, outfile):

        # Under/overflows, mean, rms, and entries
        ROOT.gStyle.SetOptStat(111110)

        canvas.Clear()
        canvas.Divide(2,2)
        canvas.cd(1)
        self.h_freq.Draw()
        canvas.cd(2)
        self.h_phase.Draw()
        canvas.cd(3)
        self.h_amp.Draw()
        canvas.cd(4)
        self.h_off.Draw()
        canvas.Update()
        canvas.Print(outfile)

    def print_stats(self):

        freq_mean = self.h_freq.GetMean()
        freq_rms = self.h_freq.GetStdDev()
        freq_n = self.h_freq.GetEntries()
        print(f"LHC Clock: {freq_mean:.6} +/- {freq_rms/math.sqrt(freq_n):.6}")

class WavePlots:

    def __init__(self, triggerMask=0xFF):

        # Number of waveforms channels
        self.nchan = 15

        # Trigger mask
        self.mask = triggerMask

        self.chan_hist_list = []
        self.log_list = []

        # Maaximum peak value
        self.peak_max = 16000.

    def init(self, tree):

        # Keyed by channel
        self.createChannelHist('h_localtime', 40, 750, 950, "Local Time")
        self.createChannelHist('h_triggertime', 40, -80, 80, "Trigger Time")
        self.createChannelHist('h_bcidtime', 50, -10, 40, "BCID Time")

    def createChannelHist(self, name, nbins, xlo, xhi, xtitle='', ytitle='Waveforms', stats=True, log=False):

        setattr(self, name, dict())
        x = getattr(self, name)
        for chan in range(self.nchan):
            x[chan] = ROOT.TH1I("", "", nbins, xlo, xhi)
            if len(xtitle) > 0:
                x[chan].GetXaxis().SetTitle(f"Ch {chan} {xtitle}")
            if len(ytitle) > 0:
                x[chan].GetYaxis().SetTitle(ytitle)
            x[chan].SetStats(stats)

        self.chan_hist_list.append(name)
        if log:
            self.log_list.append(name)

    def fill(self, tree):

        # First, create the histograms
        self.init(tree)

        # Iterate over all entries
        nev = tree.GetEntries()
        iev = 0
        for ev in tree:

            time = ev.EventInfo.timeStamp()
            trig = ev.FaserTriggerData.tap()

            if not (trig & self.mask): 
                iev += 1
                if iev == nev: 
                    break
                else: 
                    continue

            # Process waveforms
            try:
                wave_list = itertools.chain(ev.CaloWaveformHits, ev.PreshowerWaveformHits, ev.TriggerWaveformHits, ev.VetoWaveformHits, ev.VetoNuWaveformHits)
            except:
                wave_list = itertools.chain(ev.CaloWaveformHits, ev.PreshowerWaveformHits)

            for wave in wave_list:

                channel = wave.channel()

                # Check if failed threshold
                if wave.status_bit(0): continue

                # Fill fit parameters
                self.h_localtime[channel].Fill(wave.localtime())
                self.h_triggertime[channel].Fill(wave.trigger_time())
                self.h_bcidtime[channel].Fill(wave.bcid_time())

            # End of loop over waveforms

            # Protect against reading off the end
            iev+=1
            if iev == nev: break

        # End of loop over events

        # Put overflows in last bin of plots
        self.fixOverflow(self.h_localtime)
        self.fixOverflow(self.h_triggertime)

    def fixOverflow(self, hdict):

        for h in hdict.values():

            if h.GetNbinsY() == 1:
                self.fixOverflow1D(h)
            else:
                self.fixOverflow2D(h)

    def fixOverflow1D(self, hist):
        nbins = hist.GetNbinsX()
        nlast = hist.GetBinContent(nbins)
        nover = hist.GetBinContent(nbins+1)
        hist.SetBinContent(nbins, nlast+nover)

    def fixOverflow2D(self, hist):
        nbx = hist.GetNbinsX()
        nby = hist.GetNbinsY()

        for ibinx in range(nbx+1):
            nlast = hist.GetBinContent(ibinx, nby)
            nover = hist.GetBinContent(ibinx, nby+1)
            hist.SetBinContent(ibinx, nby, nlast+nover)

        for ibiny in range(nby+1):
            nlast = hist.GetBinContent(nbx, ibiny)
            nover = hist.GetBinContent(nbx+1, ibiny)
            hist.SetBinContent(nbx, ibiny, nlast+nover)

        # Also the double overflow
        nlast = hist.GetBinContent(nbx, nby)
        nover = hist.GetBinContent(nbx+1, nby+1)
        hist.SetBinContent(nbx, nby, nlast+nover)


    def draw(self, canvas, outfile):

        #
        # Plot channel plots
        for name in self.chan_hist_list:
            canvas.Clear()
            canvas.Divide(4,4)

            if name in self.log_list:
                setlog = True
            else:
                setlog = False

            for chan in range(self.nchan):
                canvas.cd(chan+1)
                x = getattr(self, name)
                x[chan].Draw()
                if setlog: 
                    ROOT.gPad.SetLogy(True)
                else:
                    ROOT.gPad.SetLogy(False)

            canvas.Print(outfile)

    def print_stats(self):

        for chan in range(self.nchan):
            local_mean = self.h_localtime[chan].GetMean()
            trig_mean  = self.h_triggertime[chan].GetMean()
            bcid_mean  = self.h_bcidtime[chan].GetMean()
            print(f"Chan {chan:2}: Entries {int(self.h_localtime[chan].GetEntries()):8} Local {local_mean:6.1f} Trigger {trig_mean:6.2f} BCID {bcid_mean:6.2f}")

#print("xAOD tree")
#t1.Print()
#print("non xAOD tree")
#t2.Print()

cp = ClockPlots()
cp.fill(t1)

# Triggers: 0x01 - calo, 0x02 - veto, 0x03 - timing, 0x10 - random
wp = WavePlots(triggerMask=trigmask)
wp.fill(t1)

c = ROOT.TCanvas()
c.Print(outfile+"[")

cp.draw(c, outfile)
wp.draw(c, outfile)

c.Print(outfile+"]")

cp.print_stats()
wp.print_stats()
