from PlotHelper import *
from Utilities import *

#Efficiency Plotters Seperate from General Plotter system. Maybe put this in its own script
def CutEfficiencyArrays(lmbda,dcutN=[],dcutD=[],lcutN=[],lcutD=[],title=None,bins=15,hmin=0,hmax=10000):
    Func = lmbda # (Returns 1 value)
    narray=[]
    darray=[]
    for event in t:
        if ApplyAllCuts(event,dcutN,lcutN): narray+=[Func(event)]
        if ApplyAllCuts(event,dcutD,lcutD): darray+=[Func(event)]

    PlotRootRatio([narray,darray],title=f"{user_input} {title}",bins=bins,hmin=hmin,hmax=hmax)

def FiducialEff_vs_Rho2():
    l= lambda event: (event.vertexPos[0]**2)+(event.vertexPos[1]**2)
    CutEfficiencyArrays(l,title='BothDFiducial vs TRho^2',bins=20,hmin=0,hmax=10000,lcutN=[lambda event: event.truthd0_IsFiducial[0] and event.truthd1_IsFiducial[0]])

def FiducialEff_vs_VertexZ():
    l= lambda event: event.vertexPos[2]
    CutEfficiencyArrays(l,title='BothDFiducial vs TVertexZ',bins=20,hmin=-1600,hmax=0,lcutN=[lambda event: event.truthd0_IsFiducial[0] and event.truthd1_IsFiducial[0]])

EfficiencyDict={
    'FiducialEff_vs_Rho2':{
        'lambda': lambda event: (event.vertexPos[0]**2)+(event.vertexPos[1]**2),
        'kwargs':{'title':'BothDFiducial vs TRho^2',
        'bins':20,'hmin':0,'hmax':10000,
        'lcutN':[lambda event: event.truthd0_IsFiducial[0] and event.truthd1_IsFiducial[0]]}
    },
    'FiducialEff_vs_Rho2':{
        'lambda': lambda event: event.vertexPos[2],
        'kwargs':{'title':'BothDFiducial vs TVertexZ',
        'bins':20,'hmin':-1600,'hmax':0,
        'lcutN':[lambda event: event.truthd0_IsFiducial[0] and event.truthd1_IsFiducial[0]]}
    }
}

def DoEfficiencyPlots():
    for k in EfficiencyDict.keys():
        print(k)
        CutEfficiencyArrays(EfficiencyDict[k]['lambda'],**EfficiencyDict[k]['kwargs'])

#EXECUTE
if __name__ == "__main__":
    DP_fnames=['Ntuple_Aee_100MeV_1Em5_']#  Ntuple_Aee_10MeV_1Em5_ Ntuple_Amm_316MeV_2Em6_ Ntuple_Aee_10MeV_1Em4_
    for f in DP_fnames:
        user_input=f
        t = TChain("events") 
        nfiles = 0
        nfiles += t.Add("/eos/home-s/sshively/CALYPZONE/run/BackUp2/"+user_input+".root") 
        print(f)
        
        DoEfficiencyPlots()