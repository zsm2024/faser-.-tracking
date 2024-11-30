# $Id: CMakeLists.txt 749562 2016-05-25 04:45:43Z krasznaa $
################################################################################
# Package: WaveEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( WaveEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( WaveEventAthenaPoolPoolCnv
   WaveEventAthenaPool/*.h src/*.h src/*.cxx
   FILES WaveRawEvent/RawWaveform.h
   WaveRawEvent/RawWaveformContainer.h
   LINK_LIBRARIES Identifier GeneratorObjectsTPCnv AthAllocators AthContainers
   AthenaBaseComps AthenaKernel SGTools StoreGateLib AthenaPoolCnvSvcLib
   AthenaPoolUtilities GaudiKernel WaveRawEvent
   # AtlasSealCLHEP 
   )

atlas_add_dictionary( WaveEventAthenaPoolCnvDict
   WaveEventAthenaPool/WaveEventAthenaPoolCnvDict.h
   WaveEventAthenaPool/selection.xml
   LINK_LIBRARIES Identifier GeneratorObjectsTPCnv )

# Install files from the package:
# atlas_install_headers( WaveEventAthenaPool )



