atlas_subdir(OverlayRDO)

atlas_add_component(
        OverlayRDO
        src/OverlayRDOAlg.h src/SelectRDOAlg.h src/BinRDOAlg.h src/HistoRDOAlg.h
        src/OverlayRDOAlg.cxx src/SelectRDOAlg.cxx src/BinRDOAlg.cxx src/HistoRDOAlg.cxx
        src/component/OverlayRDO_entries.cxx
        LINK_LIBRARIES AthenaBaseComps StoreGateLib TrackerRawData TrackerIdentifier TrackerReadoutGeometry TrkTrack GeoPrimitives TrackerRIO_OnTrack xAODFaserWaveform
)

atlas_install_python_modules(python/*.py)
# atlas_install_scripts(test/*.py)
