atlas_subdir( WaveformAnalysisExample )

atlas_add_component( WaveformAnalysisExample
                    src/*.cxx
		            src/components/WaveformAnalysisExample_entries.cxx
                    LINK_LIBRARIES AthenaBaseComps xAODFaserWaveform
        )

