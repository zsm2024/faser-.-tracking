atlas_subdir(TrackSeedPerformanceWriter)

atlas_add_component(
        TrackSeedPerformanceWriter
        src/TrackSeedPerformanceWriter.h
        src/TrackSeedPerformanceWriter.cxx
        src/components/TrackSeedPerformanceWriter_entries.cxx
        LINK_LIBRARIES AthenaBaseComps StoreGateLib TrkTrack TrackerSimData TrackerPrepRawData TrkRIO_OnTrack TrackerRIO_OnTrack
)

atlas_install_python_modules(python/*.py)
atlas_install_scripts(test/*.py)
