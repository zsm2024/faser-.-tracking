# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FaserGeant4 )

# Generations correspond to topologically sorted list:
# - https://its.cern.ch/jira/secure/attachment/282935/topologically_sorted_package_generations.txt
set( FaserGeant4_TARGET_OBJECTS
    # Generation 0
    # G4AtlasInterfaces
    # SimHelpers
    G4FaserInterfaces
    # Generation 1
    G4AtlasToolsLib
    # MCTruth
    FaserMCTruth
    # Generation 2
    # CaloG4SimLib
    # ISF_Geant4Event
    FaserISF_Geant4Event
    # Generation 3
    # LArG4Code
    # MCTruthBaseLib
    FaserMCTruthBaseLib
    # Generation 4
    # LArG4ShowerLib
    # G4AtlasAlgLib
    # TileG4InterfacesLib
    G4FaserAlgLib
    # Generation 5
    # TRT_G4Utilities
    # LArG4ShowerLibSvcLib
    G4PhysicsLists
    # Pythia8DecayerLib
    # GeoMaterial2G4
    # TrackWriteFastSimLib
    # ISF_Geant4ToolsLib
    # TileGeoG4SDLib
    FaserGeoMaterial2G4
    # Generation 6
    # BCM_G4_SDLib
    # BLM_G4_SDLib
    # PixelG4_SDLib
    # SCT_G4_SDLib
    # TRT_G4_SDLib
    # MuonG4SDLib
    # G4DebuggingHelperLib
    # G4ProfilingToolsLib
    # Geo2G4Lib 
    EcalG4_SDLib
    EmulsionG4_SDLib
    FaserSCT_G4_SDLib
    PreshowerG4_SDLib
    TriggerG4_SDLib
    VetoG4_SDLib
    VetoNuG4_SDLib
    FaserGeo2G4Lib
    )

# Components
# Generations correspond to topologically sorted list:
# - https://its.cern.ch/jira/secure/attachment/282935/topologically_sorted_package_generations.txt
set( FaserGeant4Component_TARGET_OBJECTS
    # Generation 1
    #G4AtlasTools
    # Generation 2
    #CaloG4Sim
    #ISF_Geant4CommonTools
    # Generation 3
    #MCTruthBase
    # FaserMCTruthBaseLib
    # Generation 4
    #G4AtlasAlg
    G4FaserAlg
    # Generation 5
    # LArG4ShowerLibSvc
    # TrackWriteFastSim
    # ISF_Geant4Tools
    # TileGeoG4SD
    # Generation 6
    # HGTD_G4_SD
    # BCM_G4_SD
    # BLM_G4_SD
    # PixelG4_SD
    # SCT_G4_SD
    # TRT_G4_SD
    EcalG4_SD
    EmulsionG4_SD
    FaserSCT_G4_SD
    PreshowerG4_SD
    TriggerG4_SD
    VetoG4_SD
    VetoNuG4_SD
    # TRT_TR_Process
    # LArG4Barrel
    # LArG4EC
    # LArG4FCAL
    # LArG4FastSimulation
    # LArG4HEC
    # LArG4SD
    # MagFieldUtils
    # MuonG4SD
    # G4AtlasServices
    G4FaserServices
    # Charginos
    # Pythia8Decayer
    # ExtraParticles
    # G4CosmicFilter
    # G4ExtraProcesses
    # G4ScoringProcess
    # Gauginos
    # Monopole
    # Quirks
    # Sleptons
    # G4HiveEx
    # G4DebuggingTools
    # G4FastSimulation
    # G4ProfilingTools
    # G4StepLimitation
    # G4UserActions
    G4FaserUserActions
    # Geo2G4
    FaserGeo2G4
    # ISF_Services
    FaserISF_Services
    # ISF_Geant4CommonServices
    # ISF_Geant4Services
    # DeadMaterial
    # MinBiasScintillator
    # TileGeoG4Calib 
    )


# Add additional libs and components used only in Athena
# if( NOT SIMULATIONBASE )
#     list( APPEND AtlasGeant4_TARGET_OBJECTS
#         # Generation 5
#         ForwardTransportSvcLib
#         G4ExternalDecay
#         # Generation 6
#         AFP_G4_SDLib
#         ALFA_G4_SDLib
#         LUCID_G4_SDLib
#         ZDC_SDLib
#         LArG4GenShowerLibLib
#         ISF_FastCaloSimParametrizationLib )

#     list( APPEND AtlasGeant4Component_TARGET_OBJECTS
#         # Generation 2
#         ActsGeantFollowing
# 	# Generation 5
# 	ForwardTransportSvc
# 	# Generation 6
#         AFP_G4_SD
# 	ALFA_G4_SD
# 	ForwardRegionMgField
# 	ForwardTransport
# 	LUCID_G4_SD
# 	LUCID_OpProcess
# 	ZDC_SD
# 	LArG4FastSimSvc
# 	LArG4GenShowerLib
# 	LArG4H6SD
# 	G4AtlasTests
# 	G4HitFilter
# 	RHadrons
# 	ISF_FastCaloSimParametrization
# 	ISF_FastCaloSimSD
# 	ISF_FastCaloSimServices
# 	ISF_FatrasToolsG4
# 	TBDetDescrCnv
# 	CombinedScintillator
# 	LarCrate
# 	MuonWall
# 	PhantomCalorimeter
# 	TrkG4UserActions )
# endif()

# Need to disable as-needed, as libraries are not necessarily
# ordered correctly with respect to objects.
atlas_disable_as_needed()

# Shared library that only tests should link to, composed of library OBJECTs
atlas_add_library( FaserGeant4Lib
                   dummy.cc
                   SHARED
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES ${FaserGeant4_TARGET_OBJECTS} )


# Primary "big" component composed of all library/component OBJECT libs
atlas_add_component( FaserGeant4
                     dummy.cc
                     LINK_LIBRARIES "-Wl,--exclude-libs,ALL" ${FaserGeant4Component_TARGET_OBJECTS} ${FaserGeant4_TARGET_OBJECTS} )
