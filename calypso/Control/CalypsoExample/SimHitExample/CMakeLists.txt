atlas_subdir( SimHitExample )

atlas_add_component( SimHitExample
                    src/*.cxx
                    src/components/SimHitExample_entries.cxx
                    LINK_LIBRARIES AthenaBaseComps GeneratorObjects TrackerSimEvent ScintSimEvent FaserCaloSimEvent NeutrinoSimEvent
        )

atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )