#!/usr/bin/env python
from PlotHelper import *
from Utilities import *


#Q/P METHODS
def GetPBounds(dcuts=[],lcuts=[]):
    TotalTracks=0
    PArray=[]
    for event in t:
        if ApplyAllCuts(event,dcuts,lcuts):
            TotalTracks+=event.TrackCount
            PArray+=list(event.tracksTruthPmag)
    Ilims=[int(i*TotalTracks/10) for i in range(11)]
    PArray.sort()
    Plims=[0]+[PArray[i] for i in Ilims]
    return Plims

#Q/P P bounds entered manually because storing them was weird in dictionary generation
PH = [191909.7203710638, 248751.19099289246]
PHr=0.02
def MakeNewQPDicts(Plims=None):
    newdict=dict()
    basename='AllTracksQP'
    dictbase={
        'array':[],
        'lambda': GetAllTrackQPReco,
        'plotterfunc': configBasicHist,
        'plotkwargs' :{'bins':100,'kwargs':{'range':[-1*PHr,PHr]}}} #[2*-10**-3,2*10**-3] 
    
    #for i in range(len(Plims)-1):
    name=f'{basename}_{int(PH[0]*10**-3)}-{int(PH[1]*10**-3)}GeV'
    newdict[name]=dict(dictbase)
    newdict[name]['lkwargs']={'tar':[PH[0],PH[1]]}
     #   break #TEMP

    return newdict

#DATA LAMBDAS (for retrieving values in complicated ways)
def RecoVsTruthMomentumMatched(event):
    return [event.tracksPmag[event.trackD2i-1],event.tracksTruthPmag[event.trackD2i-1]]
    
def RecoVsTruthQPMatched(event,charge=1):
    return [RecoVsMCQP(event,charge,'qopr'),RecoVsMCQP(event,charge,'qopm')]

def ResidualVSQP_truth(event):
    return [RecoVsMCQP(event,1,'qopm'),RecoVsMCQP(event,1),RecoVsMCQP(event,-1,'qopm'),RecoVsMCQP(event,-1)]

def RecoVsMCQP(event,charge=-1,param=None):
    i=event.trackD2i-1
    if event.tracksTruthCharge[i]!=charge: i=event.trackD3i-1

    q_r=event.tracksCharge[i]
    p_r=event.tracksPmag[i]
    q_m=event.tracksTruthCharge[i]
    p_m=event.tracksTruthPmag[i]
    
    if param=='qr': return q_r
    if param=='pr': return p_r
    if param=='qm': return q_m
    if param=='pm': return p_m
    if param=='qopr': return q_r/p_r
    if param=='qopm': return q_m/p_m
    return ((q_r/p_r)-(q_m/p_m))/event.tracksSigmaQovP[i]

def NormalizedResidual2(event):
    return [RecoVsMCQP(event,1),RecoVsMCQP(event,-1)]

def GetAllTrackQPReco(event,tar=1000):
    measarr = []                           
    for ti in range(event.TrackCount):
        q_r=event.tracksCharge[ti]
        p_r=event.tracksPmag[ti]
        p_m=event.tracksTruthPmag[ti]
        if type(tar)==list: 
            tar=PH
            if tar[0]<p_m and p_m<tar[1]: measarr+=[(q_r/p_r)*10**3]
            #if tar[0]<p_m and p_m<tar[1]: measarr+=[(q_r/p_r)*10**3]
        #elif p_m>tar*0.9 and p_m<tar*1.2: measarr+=[(q_r/p_r)*10**3]

    return measarr

def GetAllTrackQPTruth(event):
    measarr = []
    for ti in range(len(event.truthd0_charge)):
        q_0=event.truthd0_charge[ti]
        p_0=event.truthd0_P[ti]
        measarr+=[(q_0/p_0)*10**3]

    for ti in range(len(event.truthd1_charge)):
        q_1=event.truthd1_charge[ti]
        p_1=event.truthd1_P[ti]
        measarr+=[(q_1/p_1)*10**3]

    return measarr

def GetAllTrackMomentum(event):
    measarr = []
                            
    for ti in range(event.TrackCount):
        p_r=event.tracksPmag[ti]
        measarr+=[p_r*10**-3]

    return finalval


#For storing different plot configurations
MainArrDict={
    'RecoX':{
        'array':[],
        'lambda': lambda event: event.tracksX[0],
        'plotterfunc': configBasicHist,
        'plotkwargs' : {}
    },
    'RecoMaxMomentum':{
        'array':[],
        'lambda': lambda event:max(event.tracksPmag),
        'plotterfunc': configBasicHist,
        'plotkwargs' : {'pckwargs':{'xscale':'log'}}
    },
    'TruthMaxMomentum':{
        'array':[],
        'lambda': lambda event:max(event.tracksTruthPmag),
        'plotterfunc': configBasicHist,
        'plotkwargs' : {'pckwargs':{'xscale':'log'}}
    },
    'RecoVsTruthMaxMomentum':{
        'array':[[],[]],
        'lambda': lambda event:[max(event.tracksPmag),max(event.tracksTruthPmag)],
        'plotterfunc': configMultiHist,
        'plotkwargs' : {'label':['Reco','Truth'],'pckwargs':{'xscale':'log','legend':True}}
    },
    'RecoVsTruthMaxMomentumScatter':{
        'array':[[],[]],
        'lambda': lambda event:[max(event.tracksPmag),max(event.tracksTruthPmag)],
        'plotterfunc': config2DScatter,
        'plotkwargs' : {}
    },
    'QP_reco Vs QP_truth':{
        'array':[[],[],[],[]],
        'lambda': RecoVsTruthQPMatched,
        'plotterfunc': configMultiHist,
        'plotkwargs' : {'label':['Reco +','Truth +','Reco -','Truth -'],'kwargs':{'range':[-10**-5,10**-5]},'pckwargs':{'xscale':'linear','xlabel':"Q/P",'legend':True}} 
    },
    'QP_truth_vs_Residual':{
        'array':[[],[]],
        'lambda': ResidualVSQP_truth,
        'plotterfunc': config2DScatter,
        'plotkwargs' : {'kwargs':{},'pckwargs':{'xlabel':"Q/P_truth",'ylabel':"Residual"}}
    },
    'NormalizedResidual':{
        'array':[[],[]],
        'lambda': NormalizedResidual2,
        'plotterfunc': configMultiHist,
        'plotkwargs' : {'bins':20,'kwargs':{'range':[-10,11]},'label':['Positive','Negative'],'pckwargs':{'xscale':'linear','xlabel':"Residual",'legend':True}} 
    },
    'AllTracksQP_reco':{
        'array':[],
        'lambda': GetAllTrackQPReco,
        'plotterfunc': configBasicHist,
        'plotkwargs' :{'bins':100,'kwargs':{'range':[2*-10**-3,2*10**-3]}},
    },
    'AllTracksQP_truth':{
        'array':[],
        'lambda': GetAllTrackQPTruth,
        'plotterfunc': configBasicHist,
        'plotkwargs' :{'kwargs':{'range':[2*-10**-3,2*10**-3]}},
    },
    'AllMomentum_Reco':{
        'array':[],
        'lambda': GetAllTrackMomentum,
        'plotterfunc': configBasicHist,
        'plotkwargs' : {'kwargs':{'range':[0,4000]}}
    },
    'AllTracksQP_reco':{
        'array':[],
        'lambda': GetAllTrackQPReco,
        'plotterfunc': configBasicHist,
        'plotkwargs' :{'bins':50,'kwargs':{'range':[2*-10**-3,2*10**-3]}},
        'lkwargs' : {'tar':2000*10**3} 
    },
    'AllTracksTruthPmag':{
        'array':[],
        'lambda': lambda event: list(event.tracksTruthPmag),
        'plotterfunc': configBasicHist,
        'plotkwargs' :{'bins':50,'pckwargs':{'xscale':'log'}},
    },
    'NormalizedResidualSum':{
        'array':[],
        'lambda': NormalizedResidual2,
        'plotterfunc': configBasicHist,
        'plotkwargs' : {'bins':20,'kwargs':{'range':[-10,11]},'pckwargs':{'xscale':'linear','xlabel':"Residual"}} 
    },
    }
    


TestArrDict=dict()
testname='NormalizedResidualSum' #Any key from MainArrDict
TestArrDict[testname]=MainArrDict[testname]
# for k in MainArrDict.keys():
#     if 'Check' in k:
#         TestArrDict[k]=MainArrDict[k]

def GeneralPlotter(arrDict,dcuts=[],lcuts=[],showplot=True,savefig=False):
    nans=0

    for event in t:
        if ApplyAllCuts(event,dcuts,lcuts):
            for name in arrDict.keys():
                value=nan
                if 'lkwargs' in list(arrDict[name].keys()): 
                    value = arrDict[name]['lambda'](event,**arrDict[name]['lkwargs'])
                else: value = arrDict[name]['lambda'](event)
                checkReal=True
                if type(value)==list: checkReal = all([v != inf and v !=nan and v !=-1*inf for v in value])
                else: checkReal = value != inf and value !=nan and value !=-1*inf

                if checkReal and type(value)==list:
                    if arrDict[name]['plotterfunc'] in [configBasicHist]: arrDict[name]['array']+=value
                    else:
                        for iv in range(len(value)):
                            arrDict[name]['array'][iv]+=[value[iv]]
                elif checkReal: arrDict[name]['array']+=[value]
                else: nans+=1

    print(f"# of NaNs:{nans}")
    for name in arrDict.keys():
        isPrintout= arrDict[name]['plotterfunc'] in [PrintBoolPercent]
        print(name)
        print('LEN:',len(arrDict[name]['array']))
        arrDict[name]['plotterfunc']( arrDict[name]['array'], **arrDict[name]['plotkwargs'])
        if not isPrintout:
            plt.title(user_input+name)
            if savefig: plt.savefig(user_input+name)
            if showplot: plt.show()

#EXECUTE
if __name__ == "__main__":
    DP_fnames=['Ntuple_Aee_100MeV_1Em5_']#  Ntuple_Aee_10MeV_1Em5_ Ntuple_Amm_316MeV_2Em6_ Ntuple_Aee_10MeV_1Em4_
    for f in DP_fnames:
        user_input=f
        t = TChain("events") 
        nfiles = 0
        nfiles += t.Add("/eos/home-s/sshively/CALYPZONE/run/BackUp2/"+user_input+".root") 
        print(f)
        
        GeneralPlotter(TestArrDict,dcuts=[{}],lcuts=[BoolDaughterCut])
        #print(GetPBounds(lcuts=[BoolDaughterCut]))
