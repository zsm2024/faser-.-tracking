from numpy import log10,logspace
from cmcrameri import cm
import matplotlib.pyplot as plt
from ROOT import TEfficiency, TH1F

def PlotConfigs(xscale='linear',yscale='linear',xlim=None,ylim=None,title=None,show=False,xticks=None,xlabel=None,ylabel='#Events',legend=False,legloc='best'):
    plt.xscale(xscale)
    plt.yscale(yscale)
    if xticks: plt.xticks(xticks)
    if xlabel: plt.xlabel(xlabel)
    if ylabel: plt.ylabel(ylabel)
    if legend: plt.legend(loc=legloc)
    if title:plt.title(title)
    if show: plt.show()

def configHitMap(arrays,bins=(50,50),pckwargs={'xlabel':"X (mm)",'ylabel':"Y (mm)"},counts=False,kwargs={}):
    #2D Histogram with colors
    arrayx=arrays[0]
    arrayy=arrays[1]
    plt.hist2d(arrayx, arrayy, bins=bins, cmap=cm.imola,**kwargs)
    plt.colorbar()
    if counts: plt.text(50,-185,"#EVENTS: {}".format(len(arrayx)))
    PlotConfigs(**pckwargs)

def configBasicHist(arr,bins=25,pckwargs={'xscale':'linear'},kwargs={}):
    #1D Histogram
    if 'log' in pckwargs['xscale']: bins=CorrectBinsForLog(arr,bins,pckwargs['xscale'])
    n, binsOut, patches = plt.hist(arr,bins=bins,align="left",histtype=u'step',**kwargs)
    PlotConfigs(**pckwargs)

    return n,binsOut

def configMultiHist(arrlist,bins=25,label=[],pckwargs={'xscale':'linear','legloc':'upper left'},kwargs={}):
    #1D Histogram for comparison
    megarr=[]
    for a in arrlist: megarr+=a

    if 'log' in pckwargs['xscale']: bins=CorrectBinsForLog(megarr,bins,pckwargs['xscale'])
    for iarr in range(len(arrlist)):
        arr=arrlist[iarr]
        labelused=''
        if len(arr)==0:
            print(f"Empty array.")
        else: 
            if len(label)==len(arrlist): labelused=label[iarr]
            else: 
                print("Insufficient labels given.")
            plt.hist(arr,bins=bins,align="left",histtype=u'step',label=labelused,**kwargs)
        
    PlotConfigs(**pckwargs)

def config2DScatter(arrlist,pckwargs=dict(),kwargs={}):
    x=arrlist[0]
    y=arrlist[1]
    # plt.ylim(-25,25)
    # plt.xlim(-10**-5,10**-5)
    plt.scatter(x,y,**kwargs)
    PlotConfigs(**pckwargs)

def PrintBoolPercent(arr,kwargs={}):
    #For an array of boolians, print out the % of elements that are "true".
    n=0.0
    d=len(arr)
    for x in arr:
        if x: n+=1.0
    
    finalval= (n/d)*100
    print("Calculating...")
    print(f"{n}/{d}")
    print('Percent:'+'{:.2f}'.format(finalval)+'%')

def PlotRootRatio(arrlist,bins=15,hmin=0,hmax=10000,title=None):

    narray,darray=arrlist[0],arrlist[1]
    h_numerator=TH1F("num","num",bins,hmin,hmax)
    h_denominator=TH1F("den","den",bins,hmin,hmax)
    for i in narray: h_numerator.Fill(i)
    for j in darray: h_denominator.Fill(j)

    h_ratio = TEfficiency(h_numerator,h_denominator)
    c = h_ratio.CreateGraph()
    c.GetYaxis().SetLimits(0.0, 1.05)
    if title: h_ratio.SetTitle(title)
    h_ratio.Draw("ep")
    input()

#Utilities
def CorrectBinsForLog(arr,bins,xscale):
    minx,maxx = (min(arr),max(arr))
    if minx==maxx: 
        maxx+=1
        bins=1
    bins = logspace(start=log10(minx), stop=log10(maxx), num=bins)
    if xscale=='symlog':
        bins = [0]+list(logspace(start=log10(1), stop=log10(maxx), num=24))
        if minx>1: bins = logspace(start=log10(minx), stop=log10(maxx), num=25)

    return bins


    

