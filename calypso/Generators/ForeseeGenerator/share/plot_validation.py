import ROOT as R
from collections import namedtuple

Hist = namedtuple("Hist", "name, xtitle, ytitle, xlo, xhi, ylo, yhi, r, d, logx, logy, ndiv",
                  defaults = [None, None, None, None, None, None, 1, "hist", False, False, None])

def plot(f, name, xtitle, ytitle, xlo = None, xhi = None, ylo = None, yhi = None,
         r = 1, d = "hist", logx = False, logy = False, ndiv = None):

    h = f.Get(name)

    if xlo is not None and xhi is not None:
        h.SetAxisRange(xlo, xhi)

    if ylo is not None and yhi is not None:
        h.SetAxisRange(ylo, yhi, "Y")
    elif not logy:
        h.SetMinimum(0)

    if isinstance(r, tuple):
        h.Rebin2D(r[0], r[1])        
    elif r != 1:
        h.Rebin(r)

    if xtitle is not None:
        h.GetXaxis().SetTitle(xtitle)

    if ytitle is not None:
        h.GetYaxis().SetTitle(ytitle)

    if logx:
        R.gPad.SetLogx()

    if logy:
        R.gPad.SetLogy()

    if ndiv is not None:
        h.SetNdivisions(ndiv)

    h.SetLabelSize(0.05, "X")
    h.SetTitleSize(0.05, "X")
    h.SetLabelSize(0.05, "Y")
    h.SetTitleSize(0.05, "Y")

    h.GetXaxis().SetTitleOffset(1.2)

    R.gPad.SetBottomMargin(0.15)
    R.gPad.SetLeftMargin(0.12)
    R.gPad.SetRightMargin(0.2)        

    h.Draw(d)
    return h

def plotn(f, args, configs, x, y, outname):

    c = R.TCanvas()
    c.Divide(x, y)
    c._objs = []

    if isinstance(configs, tuple):
        configs = [configs]
        
    for i, cfg in enumerate(configs):
        c.cd(i+1)
        c._objs.append(plot(f, *cfg))
        
    c.Print(f"{args.output}/{outname}.eps")
    c.Print(f"{args.output}/{outname}.png")    

    return

if __name__ == "__main__":

    
    R.gROOT.SetBatch(True)
    R.gStyle.SetOptStat(0)

    import argparse, sys, os
    parser = argparse.ArgumentParser(description="Run gen-level validation plotting")
    parser.add_argument("file", help = "full path to imput file")
    parser.add_argument("--output", "-o",  default = "valplot", help = "Name of output directory")
    parser.add_argument("--ndaughters", "-d", default = 2, type = int, help = "Number of daugthers to plot")
    args = parser.parse_args()

    if not os.path.exists(args.output):
        os.mkdir(args.output)

    print (args.file)
    f = R.TFile.Open(args.file)

    for i in range(args.ndaughters):
        config = [Hist(f"P_d{i}", logy = True, xtitle = "p^{0} [GeV]", ndiv = 5, r = 5),
                  Hist(f"Theta_d{i}", xtitle = "#theta [rad]", ndiv = -4),
                  Hist(f"Mass_d{i}", xtitle = "m^{0} [GeV]", xlo = 0, xhi = 0.1, ndiv = 4),               
                  Hist(f"Pt_d{i}", logy = True, xtitle = "p_{T}^{0} [GeV]", ndiv = 10, r = 5),
                  Hist(f"Phi_d{i}", xtitle = "#phi [rad]"),
                  Hist(f"ThetaVsP_d{i}", ytitle = "p^{0} [GeV]", xtitle = "#theta [rad]", logx = True, logy = True, d = "colz")
                  ]

        plotn(f, args, config, 3, 2, f"daug{i}")
        
    config = [Hist("P_M", logy = True, xtitle = "p^{0} [GeV]", ndiv = 5, xlo = 0, xhi = 10000, r=10),
              Hist("Theta_M", xtitle = "#theta [rad]", ndiv = -4, r = 1),
              Hist("Mass_M", xtitle = "m^{0} [GeV]", xlo = 0, xhi = 1., ndiv = 5),               
              Hist("Pt_M", logy = True, xtitle = "p_{T}^{0} [GeV]", ndiv = 10, r = 50),
              Hist("Phi_M", xtitle = "#phi [rad]", r = 2),
              Hist("ThetaVsP_M", ytitle = "p^{0} [GeV]", xtitle = "#theta [rad]", logx = True, logy = True, d = "colz")
              ]

    plotn(f, args, config, 3, 2, "mother")

    plotn(f, args, Hist("PIDs", xtitle="PDG Id"), 1, 1, "pid") 

    
    config = [Hist("ThetaVsP_M", ytitle = "p^{0} [GeV]", xtitle = "#theta [rad]", logx = True, logy = True, d = "colz"),
              Hist("ThetaVsP_d0", ytitle = "p^{0} [GeV]", xtitle = "#theta [rad]", logx = True, logy = True, d = "colz"),
              Hist("ThetaVsP_d1", ytitle = "p^{0} [GeV]", xtitle = "#theta [rad]", logx = True, logy = True, d = "colz")
              ]                   

    plotn(f, args, config, 2, 2, "twod")

    config = [Hist("Vtx_X_LLP", xtitle = "x [mm]", r = 1),
              Hist("Vtx_Y_LLP", xtitle = "y [mm]", r = 1),
              Hist("Vtx_Z_LLP", xtitle = "z [mm]", r = 1, ndiv = 5),
              Hist("Vtx_XY_LLP", xtitle = "x [mm]", ytitle = "y [mm]", d = "colz", r = (1,1)),
              Hist("Vtx_R_LLP", xtitle = "r [mm]", r = 1, ndiv = 5)
              ]

    plotn(f, args, config, 3, 2, "vtx_llp")


#     config = [Hist("Vtx_X_All", xtitle = "x [mm]", r = 5),
#               Hist("Vtx_Y_All", xtitle = "y [mm]", r = 5),
#               Hist("Vtx_Z_All", xtitle = "z [mm]", r = 5, ndiv = 5),
#               Hist("Vtx_XY_All", xtitle = "x [mm]", ytitle = "y [mm]", d = "colz", r = (5,5)),
#               Hist("Vtx_R_All", xtitle = "r [mm]", r = 5, ndiv = 5)
#               ]
# 
#     plotn(f, args, config, 3, 2, "vtx_all")


#     config = [Hist("Vtx_X", xtitle = "x [mm]", r = 5),
#               Hist("Vtx_Y", xtitle = "y [mm]", r = 5),
#               Hist("Vtx_Z", xtitle = "z [mm]", r = 5, ndiv = 5),
#               Hist("Vtx_XY", xtitle = "x [mm]", ytitle = "y [mm]", d = "colz", r = (5,5)),
#               Hist("Vtx_R", xtitle = "r [mm]", r = 5, ndiv = 5)
#               ]
# 
#     plotn(f, args, config, 3, 2, "vtx")


    
