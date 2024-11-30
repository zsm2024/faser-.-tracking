#!/usr/bin/env python

# Set up (Py)ROOT.
import ROOT
import glob
import sys
import pandas as pd
import pickle

# Define a Landau convoluted with a gaussian for MIP fitting
landguas_conv = ROOT.TF1Convolution("landau","gaus",-10,200,True) # the functions have to be close to zero at min and max bin of convolution or else circular Fourier transform will move convolve values at max and min
landguas_conv.SetNofPointsFFT(10000)
landgaus = ROOT.TF1("landgaus",landguas_conv, -10, 200, landguas_conv.GetNpar())
landgaus2 = ROOT.TF1("landgaus2",landguas_conv, -10, 200, landguas_conv.GetNpar())
landgaus.SetParNames("Landau constant","Landau MPV","Landau width","Gaussian mean","Gaussian width")
landgaus2.SetParNames("Landau constant","Landau MPV","Landau width","Gaussian mean","Gaussian width")

def FitDualLandgaus(hist,noise_smearing):
    hist.Fit("landau")
    landgaus.SetParameters(hist.GetFunction("landau").GetParameter(0),hist.GetFunction("landau").GetParameter(1),hist.GetFunction("landau").GetParameter(2),0.0,noise_smearing)
    landgaus.SetParLimits(0,0.1*hist.GetFunction("landau").GetParameter(0),20.0*hist.GetFunction("landau").GetParameter(0))
    landgaus.SetParLimits(1,0.5*hist.GetFunction("landau").GetParameter(1),1.2*hist.GetFunction("landau").GetParameter(1))
    landgaus.SetParLimits(2,0.1*hist.GetFunction("landau").GetParameter(2),1.2*hist.GetFunction("landau").GetParameter(2))
    landgaus.FixParameter(3,0.0)
    landgaus.FixParameter(4,noise_smearing) # fix gaussian smearing to the noise seen in randomly triggered events
    hist.Fit(landgaus,"+")
    # fit with a second landgaus function closer to the peak
    landgaus2.SetParameters(hist.GetFunction("landau").GetParameter(0),hist.GetFunction("landau").GetParameter(1),hist.GetFunction("landau").GetParameter(2),0.0,noise_smearing)
    landgaus2.SetParLimits(0,0.1*hist.GetFunction("landau").GetParameter(0),20.0*hist.GetFunction("landau").GetParameter(0))
    landgaus2.SetParLimits(1,0.5*hist.GetFunction("landau").GetParameter(1),1.2*hist.GetFunction("landau").GetParameter(1))
    landgaus2.SetParLimits(2,0.1*hist.GetFunction("landau").GetParameter(2),1.2*hist.GetFunction("landau").GetParameter(2))
    landgaus2.FixParameter(3,0.0)
    landgaus2.FixParameter(4,noise_smearing) # fix gaussian smearing to the noise seen in randomly triggered events
    # check if fit range is outside hist x axis range
    fit_min = hist.GetFunction("landgaus").GetParameter(1) - 0.5*hist.GetStdDev()
    fit_max = hist.GetFunction("landgaus").GetParameter(1) + hist.GetStdDev()
    if hist.GetXaxis().GetXmin() < fit_min and hist.GetXaxis().GetXmax() > fit_max:
        hist.Fit(landgaus2,"+","",fit_min,fit_max)
    else:
        hist.Fit(landgaus2,"+")

    hist.GetFunction("landau").SetLineColorAlpha(0,0.0) # dont show the original landau fit
    hist.GetFunction("landgaus").SetLineColor(2)
    hist.GetFunction("landgaus2").SetLineColor(4)

# create a dictionary that holds the rough center point of each calo channel
calo_center = [{"x":-60,"y":-60},{"x":60,"y":-60},{"x":-60,"y":60},{"x":60,"y":60}]

user_input = str(sys.argv[1]) # set to either 'all_high', 'all_low', or a run number

t = ROOT.TChain("nt")
nfiles = 0
all_run_paths = glob.glob("/eos/project/f/faser-commissioning/DeionsNtuples/*")

if user_input=="test":
    runconfig = "test"
    gain = 1.0
    nfiles = t.Add(str(sys.argv[2])) 
    rootFile = ROOT.TFile(str(sys.argv[2])) # to get noise histograms
elif user_input=="all_high":
    runconfig = "High_gain"
    print("processing high-gain runs")
    gain = 30.0
    for run_path in all_run_paths:
        nfiles += t.Add(run_path+"/Data-tuple-High_gain*.root") # chain all ntuples from all runs that are high gain
    rootFile = ROOT.TFile("/eos/project/f/faser-commissioning/DeionsNtuples/7930/Data-tuple-High_gain-007930-00000-100.root") # load file from largest high gain run to get noise histograms
elif user_input=="all_low":
    runconfig = "Low_gain"
    print("processing low-gain runs")
    gain = 1.0
    for run_path in all_run_paths:
        nfiles += t.Add(run_path+"/Data-tuple-Low_gain*.root") # chain all ntuples from all runs that are high gain
    rootFile = ROOT.TFile("/eos/project/f/faser-commissioning/DeionsNtuples/8137/Data-tuple-Low_gain-008137-00000-100.root") # load file from largest low gain run to get noise histograms
elif user_input=="high_filter":
    runconfig = "null"
    print("processing High gain with filter runs")
    gain = 3.0
    for run in ["8715","8717"]:
        nfiles += t.Add("/eos/project/f/faser-commissioning/DeionsNtuples/"+run+"/*.root") # chain all ntuples from all runs that are high gain
    rootFile = ROOT.TFile("/eos/project/f/faser-commissioning/DeionsNtuples/8717/Data-tuple-"+runconfig+"-008717-00000-100.root") # load file from largest low gain run to get noise histograms
elif user_input=="high_filter2":
    runconfig = "High_gain"
    print("processing High gain with filter runs 2")
    gain = 3.0
    for run in ["8932","8933"]:
        nfiles += t.Add("/eos/project/f/faser-commissioning/DeionsNtuples/"+run+"/*.root") # chain all ntuples from all runs that are high gain
    rootFile = ROOT.TFile("/eos/project/f/faser-commissioning/DeionsNtuples/8933/Data-tuple-"+runconfig+"-008933-00000-100.root") # load file from largest low gain run to get noise histograms
else: # assume user_input is a run number
    # get run configuration from table oon Brian's website
    table_runs = pd.read_html('http://aagaard.web.cern.ch/aagaard/FASERruns.html') # load in run tables from website
    df = table_runs[0] # table_runs is a list of all tables on the website, we only want the first one
    try:
        runconfig=str(df.at[df.loc[df['Run'] == int(user_input)].index[0],'Configuration'].replace(' ','_')) # get config from website run log telling if run is High_gain or Low_gain calo 
    except: # if run is not in Brian's website table, just set config to null, will result in calibrated depoosited energy being zero
        runconfig="null"
    print("processing run "+runconfig+" ("+runconfig+")")
    if runconfig=="High_gain":
        gain = 30.0
    elif runconfig=="Low_gain":
        gain = 1.0
    else:
        print("run config is neither 'High_gain' nor 'Low_gain', calo histogram ranges may be messed up")
        gain = 3.0 # assume filter and high gain
        if int(user_input) > 8717:
            gain = 0.5
    nfiles += t.Add("/eos/project/f/faser-commissioning/DeionsNtuples/"+user_input+"/*.root") # chain all ntuples from all runs that are high gain
    rootFile = ROOT.TFile("/eos/project/f/faser-commissioning/DeionsNtuples/"+user_input+"/Data-tuple-"+runconfig+"-00"+user_input+"-00000-100.root") # load file from largest low gain run to get noise histograms




print("number of files chained together = ",nfiles)

#ROOT.gROOT.SetStyle("ATLAS")
#ROOT.gStyle.SetOptStat(111110) #take away option box in histograms
#ROOT.gStyle.SetOptTitle(1)
#ROOT.gStyle.SetOptFit(1)

# Define histograms here
hCaloCharge = []
hCaloPeak = []
hCaloBaselineRMS = []
hXYvsEcalo = []
hXYCharge = []
hXYPeak = []
for chan in range(4):
    hCaloCharge.append(ROOT.TH1F("hCalo"+str(chan)+"charge", "Charge in calo ch"+str(chan)+";Q (pC);# of events",100,0.2*gain,2.0*gain))
    hCaloPeak.append(ROOT.TH1F("hCalo"+str(chan)+"peak", "Peak in calo ch"+str(chan)+";peak (mV);# of events",100,1.0*gain,5.0*gain))
    hCaloBaselineRMS.append(ROOT.TH1F("hCalo"+str(chan)+"baseline_rms", "Baseline_RMS in calo ch"+str(chan)+";baseline_rms (mV);# of events",100,0.1,0.7))
    hXYvsEcalo.append(ROOT.TProfile2D("hXYvsEcalo"+str(chan) , "Calo ch"+str(chan)+" Charge vs Pos;X pos (mm);Y pos (mm)",26, -130.0, 130.0, 26, -130.0, 130.0))
    hXYCharge.append(ROOT.TH3F("hXYCharge"+str(chan) , "Calo ch"+str(chan)+" Charge vs Pos;X pos (mm);Y pos (mm);Calo ch"+str(chan)+" Charge (pC)",65, -130.0, 130.0, 65, -130.0, 130.0,100,0.2*gain,2.0*gain))
    hXYPeak.append(ROOT.TH3F("hXYPeak"+str(chan) , "Calo ch"+str(chan)+" Peak vs Pos;X pos (mm);Y pos (mm);Calo ch"+str(chan)+" Peak (mV)",26, -130.0, 130.0, 26, -130.0, 130.0,100,1.0*gain,5.0*gain))


hCaloEdep = ROOT.TH1F("hCaloEdep", "Edep in Calo;Edep (GeV);# of events",100,0.0,1.8)
hCaloE_EM = ROOT.TH1F("hCaloE_EM", "EM E in Calo;EM E (GeV);# of events",100,0.0,5.0)
hCalo_nMIP = ROOT.TH1F("hCalo_nMIP", "nMIP in Calo;n MIP ;# of events",100,0.0,10.)

hCaloThetaX = ROOT.TH1F("hCaloThetaX", "Track #theta_{x} at Calo face;#theta_{x} (radians);# of tracks",100,-0.1,0.1)
hCaloThetaY = ROOT.TH1F("hCaloThetaY", "Track #theta_{y} at Calo face;#theta_{y} (radians);# of tracks",100,-0.1,0.1)

hTrackPvsPYdiff = ROOT.TProfile("hTrackPvsPYdiff" , "Track #Deltap_{Y}/p vs p;Track p (MeV);(pY_{upstream} - pY_{downstream}) / p_{total}",100, 1000.0, 200000.0)
hTrackPvsPXdiff = ROOT.TProfile("hTrackPvsPXdiff" , "Track #Deltap_{X}/p vs p;Track p (MeV);(pX_{upstream} - pX_{downstream}) / p_{total}",100, 1000.0, 200000.0)

#t.Print() # will show you all variables in ttree

i = 0
for event in t:
    i += 1

    if i%1000 == 0:
        print( "Processing event #%i of %i" % (i, t.GetEntries() ) )

    if event.longTracks == 1: # only process events with at least one track that has hits in last 3 tracking stations
        for j in range(event.longTracks): # loop over all long tracks in the event (long = has hits in last 3 tracking stations)
            if event.Track_p0[j] != 0.0:
                hTrackPvsPYdiff.Fill(event.Track_p0[j],(event.Track_py0[j] - event.Track_py1[j])/event.Track_p0[j])
                hTrackPvsPXdiff.Fill(event.Track_p0[j],(event.Track_px0[j] - event.Track_px1[j])/event.Track_p0[j])

            #print("track charge = %i and nLayers = %i" % (event.Track_charge[j],event.Track_nLayers[j]))
            #print("track upstream   (x,y,z) (px,py,pz) = (%f,%f,%f) (%f,%f,%f)" % (event.Track_x0[j],event.Track_y0[j],event.Track_z0[j],event.Track_px0[j],event.Track_py0[j],event.Track_pz0[j]))
            #print("track downstream (x,y,z) (px,py,pz) = (%f,%f,%f) (%f,%f,%f)" % (event.Track_x1[j],event.Track_y1[j],event.Track_z1[j],event.Track_px1[j],event.Track_py1[j],event.Track_pz1[j]))

            #print("track at vetoNu (x,y) (thetaX,thetaY) = (%f,%f) (%f,%f)" % (event.Track_X_atVetoNu[j],event.Track_Y_atVetoNu[j],event.Track_ThetaX_atVetoNu[j],event.Track_ThetaY_atVetoNu[j]))
            #print("track at Calo (x,y) (thetaX,thetaY) = (%f,%f) (%f,%f)" % (event.Track_X_atCalo[j],event.Track_Y_atCalo[j],event.Track_ThetaX_atCalo[j],event.Track_ThetaY_atCalo[j]))

        #print("number of track segments = ",event.TrackSegments)
        #for j in range(event.TrackSegments):
            #print("trackseg (x,y,z) (px,py,pz) = (%f,%f,%f) (%f,%f,%f)" % (event.TrackSegment_x[j],event.TrackSegment_y[j],event.TrackSegment_z[j],event.TrackSegment_px[j],event.TrackSegment_py[j],event.TrackSegment_pz[j]))
            #print("trackseg chi2 = %i and ndof = %i" % (event.TrackSegment_Chi2[j],event.TrackSegment_nDoF[j]))

        #print("number of SpacePoints = ",event.SpacePoints)
        #for j in range(event.SpacePoints):
            #print("Spacepoint #",j)
            #print("SpacePoint (x,y,z) = (%f,%f,%f)" % (event.SpacePoint_x[j],event.SpacePoint_y[j],event.SpacePoint_z[j]))

        hCaloEdep.Fill(event.Calo_total_E_dep / 1000.0)
        hCaloE_EM.Fill(event.Calo_total_E_EM / 1000.0)
        hCalo_nMIP.Fill(event.Calo_total_nMIP)

        x_calo = event.Track_X_atCalo[0]
        y_calo = event.Track_Y_atCalo[0]

        hCaloThetaX.Fill(event.Track_ThetaX_atCalo[0])
        hCaloThetaY.Fill(event.Track_ThetaY_atCalo[0])


        hCaloBaselineRMS[0].Fill(event.Calo0_baseline_rms)
        hCaloBaselineRMS[1].Fill(event.Calo1_baseline_rms)
        hCaloBaselineRMS[2].Fill(event.Calo2_baseline_rms)
        hCaloBaselineRMS[3].Fill(event.Calo3_baseline_rms)

        if abs(event.Track_ThetaX_atCalo[0]) > 0.1 or abs(event.Track_ThetaX_atCalo[0]) > 0.1: continue

        for chan,peak in enumerate([event.Calo0_raw_peak,event.Calo1_raw_peak,event.Calo2_raw_peak,event.Calo3_raw_peak]):
            hXYPeak[chan].Fill(x_calo,y_calo,peak)

        for chan,charge in enumerate([event.Calo0_raw_charge,event.Calo1_raw_charge,event.Calo2_raw_charge,event.Calo3_raw_charge]):
            hXYCharge[chan].Fill(x_calo,y_calo,charge)

            if charge > 0.2*gain and charge < 2.0*gain:
                hXYvsEcalo[chan].Fill(x_calo,y_calo,charge)

        if x_calo > -60.0 and x_calo < -20.0 and y_calo > -80.0 and y_calo < -10.0:
            hCaloCharge[0].Fill(event.Calo0_raw_charge)
            hCaloPeak[0].Fill(event.Calo0_raw_peak)
        elif x_calo > 70.0 and x_calo < 100.0 and y_calo > -90.0 and y_calo < -10.0:
            hCaloCharge[1].Fill(event.Calo1_raw_charge)
            hCaloPeak[1].Fill(event.Calo1_raw_peak)
        elif x_calo > -60.0 and x_calo < -20.0 and y_calo > 20.0 and y_calo < 110.0:
            hCaloCharge[2].Fill(event.Calo2_raw_charge)
            hCaloPeak[2].Fill(event.Calo2_raw_peak)
        elif x_calo > 70.0 and x_calo < 100.0 and y_calo > 20.0 and y_calo < 110.0:
            hCaloCharge[3].Fill(event.Calo3_raw_charge)
            hCaloPeak[3].Fill(event.Calo3_raw_peak)

    #if i > 10000:
    #    break

# create a list of histograms of random event integrals
hRandomCharge = []
for chan in range(15):
    hRandomCharge.append(rootFile.Get("hRandomCharge"+str(chan)))

# Now make some plots
filename = "analyze-"+user_input+"-Ntuples.pdf"

c = ROOT.TCanvas()
c.Print(filename+'[')
c.Divide(2,2)
c.cd(1)
hCaloEdep.Draw()
ROOT.gPad.SetLogy()
c.cd(2)
hCaloE_EM.Draw()
ROOT.gPad.SetLogy()
c.cd(3)
hCalo_nMIP.Draw()
ROOT.gPad.SetLogy()
c.Print(filename)

c = ROOT.TCanvas()
c.Divide(2,2)
for chan in range(4):
    c.cd(1+chan)
    hXYvsEcalo[chan].GetZaxis().SetRangeUser(hCaloCharge[chan].GetMean() - 0.2*hCaloCharge[chan].GetStdDev(),hCaloCharge[chan].GetMean() + 0.35*hCaloCharge[chan].GetStdDev())
    hXYvsEcalo[chan].Draw('COLZ')
c.Print(filename)

leg = []
c = ROOT.TCanvas()
c.Divide(2,2)
for chan in range(4):
    c.cd(1+chan)
    FitDualLandgaus(hCaloCharge[chan], hRandomCharge[chan].GetStdDev())
    hCaloCharge[chan].Draw()

    leg.append( ROOT.TLegend(0.55,0.55,0.89,0.75) )
    leg[chan].AddEntry(hCaloCharge[chan].GetFunction("landgaus"),"Landgaus MPV = "+str(hCaloCharge[chan].GetFunction("landgaus").GetParameter(1))[:6]+" #pm "+str(hCaloCharge[chan].GetFunction("landgaus").GetParError(1))[:6],"L")
    leg[chan].AddEntry(hCaloCharge[chan].GetFunction("landgaus2"),"Landguas MPV = "+str(hCaloCharge[chan].GetFunction("landgaus2").GetParameter(1))[:6]+" #pm "+str(hCaloCharge[chan].GetFunction("landgaus2").GetParError(1))[:6],"L")
    leg[chan].AddEntry(hCaloCharge[chan].GetFunction("landgaus"),"Landguas gaussian width = "+str(hCaloCharge[chan].GetFunction("landgaus").GetParameter(4))[:6],"")
    leg[chan].SetBorderSize(0)
    leg[chan].Draw()
c.Print(filename)

cuts = [[5.1,39.9,-19.9,49.9],[-39.9,39.9,-29.9,-0.1],[-39.9,39.9,35.1,54.9]] # [xLeft,xRight,yDown,yUp] shift by 0.1 so not right on boundary of bin
calo_charge_MPV_dict = [[],[],[],[]]
for chan in range(4):
    hist = []
    leg = []
    line_yDown = []
    line_yUp = []
    line_xLeft = []
    line_xRight = []
    c = ROOT.TCanvas()
    c.Divide(len(cuts),2)
    hist2D = hXYCharge[chan].Project3DProfile("yx")

    for cut_num,cut in enumerate(cuts):
        c.cd(1+cut_num)
        hist.append(hXYCharge[chan].Clone("hXYCharge_ch"+str(chan)+"_cut"+str(cut_num)).ProjectionZ("hXYChargeProjections_ch"+str(chan)+"_cut"+str(cut_num),hXYCharge[chan].GetXaxis().FindBin(calo_center[chan]["x"]+cut[0]),hXYCharge[chan].GetXaxis().FindBin(calo_center[chan]["x"]+cut[1]),hXYCharge[chan].GetYaxis().FindBin(calo_center[chan]["y"]+cut[2]),hXYCharge[chan].GetYaxis().FindBin(calo_center[chan]["y"]+cut[3]),"" ))
        FitDualLandgaus(hist[cut_num], hRandomCharge[cut_num].GetStdDev())
        hist[cut_num].Draw()

        if chan == 0 and cut_num == 0 :
            # store histograms in pickle files
            pickle.dump( hist[cut_num], open( "./hCalo0_MIP_"+str(user_input)+".p", "wb" ) )
 

        leg.append( ROOT.TLegend(0.55,0.55,0.89,0.75) )
        leg[cut_num].AddEntry(hist[cut_num].GetFunction("landgaus"),"Landgaus MPV = "+str(hist[cut_num].GetFunction("landgaus").GetParameter(1))[:6]+" #pm "+str(hist[cut_num].GetFunction("landgaus").GetParError(1))[:6],"L")
        leg[cut_num].AddEntry(hist[cut_num].GetFunction("landgaus2"),"Landguas MPV = "+str(hist[cut_num].GetFunction("landgaus2").GetParameter(1))[:6]+" #pm "+str(hist[cut_num].GetFunction("landgaus2").GetParError(1))[:6],"L")
        leg[cut_num].AddEntry(hist[cut_num].GetFunction("landgaus"),"Landguas gaussian width = "+str(hist[cut_num].GetFunction("landgaus").GetParameter(4))[:6],"")
        leg[cut_num].SetBorderSize(0)
        leg[cut_num].Draw()

        # add MIP MPV to dict to print in table
        calo_charge_MPV_dict[chan].append({'MPV':str(hist[cut_num].GetFunction("landgaus2").GetParameter(1))[:6] ,'MPV_error':str(hist[cut_num].GetFunction("landgaus2").GetParError(1))[:6]})

        # draw 2d heat plot showing positional cuts
        c.cd(1+cut_num+len(cuts))
        if cut_num == 0:
            tempHist = hist2D.Clone("hist2D_ch"+str(cut_num)+"_cut"+str(cut_num))
            tempHist.GetZaxis().SetRangeUser(hist[cut_num].GetMean() - 0.2*hist[cut_num].GetStdDev(),hist[cut_num].GetMean() + 0.35*hist[cut_num].GetStdDev())
        tempHist.Draw("COLZ")
        line_yDown.append(ROOT.TLine(calo_center[chan]["x"]+cut[0],calo_center[chan]["y"]+cut[2],calo_center[chan]["x"]+cut[1],calo_center[chan]["y"]+cut[2]));
        line_yUp.append(ROOT.TLine(calo_center[chan]["x"]+cut[0],calo_center[chan]["y"]+cut[3],calo_center[chan]["x"]+cut[1],calo_center[chan]["y"]+cut[3]));
        line_xLeft.append(ROOT.TLine(calo_center[chan]["x"]+cut[0],calo_center[chan]["y"]+cut[2],calo_center[chan]["x"]+cut[0],calo_center[chan]["y"]+cut[3]));
        line_xRight.append(ROOT.TLine(calo_center[chan]["x"]+cut[1],calo_center[chan]["y"]+cut[2],calo_center[chan]["x"]+cut[1],calo_center[chan]["y"]+cut[3]));
        line_yDown[cut_num].Draw()
        line_yUp[cut_num].Draw()
        line_xLeft[cut_num].Draw()
        line_xRight[cut_num].Draw()

    c.Print(filename)

# make a table of MIP MPV values for different positiion cuts
c = ROOT.TCanvas()
header = ROOT.TPaveText(0.2,0.8,0.9,0.2)
header.SetTextSize(0.03)
header.SetFillColor(0)
header.SetTextAlign(32)
header.AddText("Cut #,Calo0_Charge_MPV,Calo0_Charge_err,Calo1_Charge_MPV,Calo1_Charge_Charge_err,Calo2_Charge_MPV,Calo2_Charge_err,Calo3_Charge_MPV,Calo3_Charge_err")
for i in range(len(cuts)):
    header.AddText("cut "+str(i)+","+calo_charge_MPV_dict[0][i]['MPV']+","+calo_charge_MPV_dict[0][i]['MPV_error']+","+calo_charge_MPV_dict[1][i]['MPV']+","+calo_charge_MPV_dict[1][i]['MPV_error']+","+calo_charge_MPV_dict[2][i]['MPV']+","+calo_charge_MPV_dict[2][i]['MPV_error']+","+calo_charge_MPV_dict[3][i]['MPV']+","+calo_charge_MPV_dict[3][i]['MPV_error'])
header.Draw()
c.Print(filename)


c = ROOT.TCanvas()
c.Divide(2,2)
for chan in range(4):
    c.cd(1+chan)
    hRandomCharge[chan].Draw()
c.Print(filename)

leg = []
c = ROOT.TCanvas()
c.Divide(2,2)
for chan in range(4):
    c.cd(1+chan)
    FitDualLandgaus(hCaloPeak[chan], hCaloBaselineRMS[chan].GetMean())
    hCaloPeak[chan].Draw()

    leg.append( ROOT.TLegend(0.55,0.55,0.89,0.75) )
    leg[chan].AddEntry(hCaloPeak[chan].GetFunction("landgaus"),"Landgaus MPV = "+str(hCaloPeak[chan].GetFunction("landgaus").GetParameter(1))[:6]+" #pm "+str(hCaloPeak[chan].GetFunction("landgaus").GetParError(1))[:6],"L")
    leg[chan].AddEntry(hCaloPeak[chan].GetFunction("landgaus2"),"Landguas MPV = "+str(hCaloPeak[chan].GetFunction("landgaus2").GetParameter(1))[:6]+" #pm "+str(hCaloPeak[chan].GetFunction("landgaus2").GetParError(1))[:6],"L")
    leg[chan].AddEntry(hCaloPeak[chan].GetFunction("landgaus"),"Landguas gaussian width = "+str(hCaloPeak[chan].GetFunction("landgaus").GetParameter(4))[:6],"")
    leg[chan].SetBorderSize(0)
    leg[chan].Draw()
c.Print(filename)


calo_peak_MPV_dict = [[],[],[],[]]
for chan in range(4):
    hist = []
    leg = []
    line_yDown = []
    line_yUp = []
    line_xLeft = []
    line_xRight = []
    c = ROOT.TCanvas()
    c.Divide(len(cuts),2)
    hist2D = hXYPeak[chan].Project3DProfile("yx")

    for cut_num,cut in enumerate(cuts):
        c.cd(1+cut_num)
        hist.append(hXYPeak[chan].Clone("hXYPeak_ch"+str(chan)+"_cut"+str(cut_num)).ProjectionZ("hXYPeakProjections_ch"+str(chan)+"_cut"+str(cut_num),hXYPeak[chan].GetXaxis().FindBin(calo_center[chan]["x"]+cut[0]),hXYPeak[chan].GetXaxis().FindBin(calo_center[chan]["x"]+cut[1]),hXYPeak[chan].GetYaxis().FindBin(calo_center[chan]["y"]+cut[2]),hXYCharge[chan].GetYaxis().FindBin(calo_center[chan]["y"]+cut[3]),"" ))
        FitDualLandgaus(hist[cut_num], hCaloBaselineRMS[chan].GetMean())
        hist[cut_num].Draw()

        leg.append( ROOT.TLegend(0.55,0.55,0.89,0.75) )
        leg[cut_num].AddEntry(hist[cut_num].GetFunction("landgaus"),"Landgaus MPV = "+str(hist[cut_num].GetFunction("landgaus").GetParameter(1))[:6]+" #pm "+str(hist[cut_num].GetFunction("landgaus").GetParError(1))[:6],"L")
        leg[cut_num].AddEntry(hist[cut_num].GetFunction("landgaus2"),"Landguas MPV = "+str(hist[cut_num].GetFunction("landgaus2").GetParameter(1))[:6]+" #pm "+str(hist[cut_num].GetFunction("landgaus2").GetParError(1))[:6],"L")
        leg[cut_num].AddEntry(hist[cut_num].GetFunction("landgaus"),"Landguas gaussian width = "+str(hist[cut_num].GetFunction("landgaus").GetParameter(4))[:6],"")
        leg[cut_num].SetBorderSize(0)
        leg[cut_num].Draw()

        # add MIP MPV to dict to print in table
        calo_peak_MPV_dict[chan].append({'MPV':str(hist[cut_num].GetFunction("landgaus2").GetParameter(1))[:6] ,'MPV_error':str(hist[cut_num].GetFunction("landgaus2").GetParError(1))[:6]})


        # draw 2d heat plot showing positional cuts
        c.cd(1+cut_num+len(cuts))
        if cut_num == 0:
            tempHist = hist2D.Clone("hist2D_ch"+str(cut_num)+"_cut"+str(cut_num))
            tempHist.GetZaxis().SetRangeUser(hist[cut_num].GetMean() - 0.2*hist[cut_num].GetStdDev(),hist[cut_num].GetMean() + 0.35*hist[cut_num].GetStdDev())
        tempHist.Draw("COLZ")
        line_yDown.append(ROOT.TLine(calo_center[chan]["x"]+cut[0],calo_center[chan]["y"]+cut[2],calo_center[chan]["x"]+cut[1],calo_center[chan]["y"]+cut[2]));
        line_yUp.append(ROOT.TLine(calo_center[chan]["x"]+cut[0],calo_center[chan]["y"]+cut[3],calo_center[chan]["x"]+cut[1],calo_center[chan]["y"]+cut[3]));
        line_xLeft.append(ROOT.TLine(calo_center[chan]["x"]+cut[0],calo_center[chan]["y"]+cut[2],calo_center[chan]["x"]+cut[0],calo_center[chan]["y"]+cut[3]));
        line_xRight.append(ROOT.TLine(calo_center[chan]["x"]+cut[1],calo_center[chan]["y"]+cut[2],calo_center[chan]["x"]+cut[1],calo_center[chan]["y"]+cut[3]));
        line_yDown[cut_num].Draw()
        line_yUp[cut_num].Draw()
        line_xLeft[cut_num].Draw()
        line_xRight[cut_num].Draw()

    c.Print(filename)

# make a table of MIP MPV values for different positiion cuts
c = ROOT.TCanvas()
header = ROOT.TPaveText(0.2,0.8,0.9,0.2)
header.SetTextSize(0.03)
header.SetFillColor(0)
header.SetTextAlign(32)
header.AddText("Cut #,Calo0_Peak_MPV,Calo0_Peak_err,Calo1_Peak_MPV,Calo1_Peak_err,Calo2_Peak_MPV,Calo2_Peak_err,Calo3_Peak_MPV,Calo3_Peak_err")
for i in range(len(cuts)):
    header.AddText("cut "+str(i)+","+calo_peak_MPV_dict[0][i]['MPV']+","+calo_peak_MPV_dict[0][i]['MPV_error']+","+calo_peak_MPV_dict[1][i]['MPV']+","+calo_peak_MPV_dict[1][i]['MPV_error']+","+calo_peak_MPV_dict[2][i]['MPV']+","+calo_peak_MPV_dict[2][i]['MPV_error']+","+calo_peak_MPV_dict[3][i]['MPV']+","+calo_peak_MPV_dict[3][i]['MPV_error'])
header.Draw()
c.Print(filename)

c = ROOT.TCanvas()
c.Divide(2,2)
for chan in range(4):
    c.cd(1+chan)
    hCaloBaselineRMS[chan].Draw()
c.Print(filename)

c = ROOT.TCanvas()
c.Divide(1,2)
c.cd(1)
hCaloThetaX.Draw()
c.cd(2)
hCaloThetaY.Draw()
c.Print(filename)

c = ROOT.TCanvas()
c.Divide(1,2)
c.cd(1)
hTrackPvsPYdiff.GetYaxis().SetRangeUser(hTrackPvsPYdiff.GetMean(2) - hTrackPvsPYdiff.GetStdDev(2), hTrackPvsPYdiff.GetMean(2) + hTrackPvsPYdiff.GetStdDev(2))
hTrackPvsPYdiff.Draw()
c.cd(2)
hTrackPvsPXdiff.GetYaxis().SetRangeUser(hTrackPvsPXdiff.GetMean(2) - hTrackPvsPXdiff.GetStdDev(2), hTrackPvsPXdiff.GetMean(2) + hTrackPvsPXdiff.GetStdDev(2))
hTrackPvsPXdiff.Draw()
c.Print(filename)

c = ROOT.TCanvas()
c.Divide(4,4)
for chan in range(15):
    c.cd(1+chan)
    hRandomCharge[chan].Draw()
c.Print(filename)

# Must close file at the end
c.Print(filename+']')

