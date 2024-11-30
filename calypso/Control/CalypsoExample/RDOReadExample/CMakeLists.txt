atlas_subdir( RDOReadExample )

atlas_add_component( RDOReadExample
                    src/RDOReadAlg.cxx
                    src/components/RDOReadExample_entries.cxx
                    LINK_LIBRARIES AthenaBaseComps GeneratorObjects TrackerSimEvent TrackerRawData TrackerSimData TrackerIdentifier
        )

#atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
