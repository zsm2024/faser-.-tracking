from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

def DerivationAlgCfg(flags, stream, tools, **kwargs):

    acc = ComponentAccumulator()
    
    kwargs.setdefault("Tools", tools)
    ##print ("SETTING", kwargs["Tools"], type(kwargs["Tools"][0]))
    acc.addEventAlgo(CompFactory.Derive(stream + "_DerivationAlg", **kwargs))

    return acc

def DerivationOutputCfg(flags, stream, accept, items = [], extra_items = [], exclude_items = [], **kwargs):

    acc = ComponentAccumulator()

    if not items:
        items = [ "xAOD::EventInfo#*"
                  , "xAOD::EventAuxInfo#*"
                  , "xAOD::FaserTriggerData#*"
                  , "xAOD::FaserTriggerDataAux#*"
                  , "FaserSiHitCollection#*"  # Strip hits, do we want this?
                  , "FaserSCT_RDO_Container#*" 
                  , "xAOD::WaveformHitContainer#*"
                  , "xAOD::WaveformHitAuxContainer#*"
                  , "xAOD::WaveformClock#*"
                  , "xAOD::WaveformClockAuxInfo#*"
                  ]

    if not items and extra_items:
        items.append(extra_items)

    # from PrimaryDPDMaker/python/Primary_DPD_OutputDefinitions.py in athena
    # Once can use TakeItemsFromInput = True probably need to use
    # acc.getEventAlgo(f"OutputStream{stream}").RemoveItem(exclude_list)
    exclude_list = []
    if exclude_items:
        for ex in exclude_items:
            if ex in items:
                exclude_list.append(ex)

            if ex.endswith("*"):
                for it in items:
                    if it.startswith(ex.rstrip("*")):
                        exclude_list.append(it)

    # items = list(set(items) - set(exclude_list)) 
                        

    #flags.unlock()
    #flags.addFlag(f"Output.AOD{stream}FileName", f"my.{stream}.xAOD.root")
    #flags.lock()

    acc.merge(OutputStreamCfg(flags, stream, items))
    acc.getEventAlgo(f"OutputStream{stream}").AcceptAlgs = [accept]
    ## if not items:
    ##acc.getEventAlgo(f"OutputStream{stream}").TakeItemsFromInput = True    # crashes
    ## if extra_items:
    ##cc.getEventAlgo(f"OutputStream{stream}").RemoveItem(extra_items)
    ## if exclude_list:
    ##acc.getEventAlgo(f"OutputStream{stream}").AddItem(exclude_list)

    return acc

def FullyConfiguredStream(flags, stream, tools, items = [], extra_items = [], **kwargs):
    # TODO:
    # - get items from input + why crash

    acc = ComponentAccumulator()

    acc.merge(DerivationAlgCfg(flags, stream, tools, **kwargs))
    acc.merge(DerivationOutputCfg(flags, stream, stream + "_DerivationAlg", items, extra_items))
              
    return acc


    
