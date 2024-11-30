# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addService,addAlgorithm
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.FaserSCT_DigitizationTool"          , "FaserSCT_DigitizationTool")
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.FaserSCT_GeantinoTruthDigitizationTool"          , "FaserSCT_GeantinoTruthDigitizationTool")
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.FaserSCT_DigitizationToolHS"        , "FaserSCT_DigitizationToolHS")
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.FaserSCT_DigitizationToolPU"        , "FaserSCT_DigitizationToolPU")
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.FaserSCT_DigitizationToolSplitNoMergePU", "FaserSCT_DigitizationToolSplitNoMergePU")
addAlgorithm("FaserSCT_Digitization.FaserSCT_DigitizationConfig.FaserSCT_DigitizationHS"       , "FaserSCT_DigitizationHS")
addAlgorithm("FaserSCT_Digitization.FaserSCT_DigitizationConfig.FaserSCT_DigitizationPU"       , "FaserSCT_DigitizationPU")
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.getFaserSiliconRange"               , "FaserSiliconRange" )
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.getFaserSCT_RandomDisabledCellGenerator", "FaserSCT_RandomDisabledCellGenerator")
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.getFaserSCT_Amp", "FaserSCT_Amp" )
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.getFaserSCT_FrontEnd"               , "FaserSCT_FrontEnd" )
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.getFaserPileupSCT_FrontEnd"         , "PileupFaserSCT_FrontEnd" )
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.getFaserSCT_SurfaceChargesGenerator", "FaserSCT_SurfaceChargesGenerator" )
addTool("FaserSCT_Digitization.FaserSCT_DigitizationConfig.FaserSCT_OverlayDigitizationTool", "FaserSCT_OverlayDigitizationTool")
addAlgorithm("FaserSCT_Digitization.FaserSCT_DigitizationConfig.FaserSCT_OverlayDigitization", "FaserSCT_OverlayDigitization")
