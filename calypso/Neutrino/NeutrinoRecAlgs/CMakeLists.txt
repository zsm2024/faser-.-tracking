atlas_subdir( NeutrinoRecAlgs )

atlas_add_component(NeutrinoRecAlgs
                    src/*.cxx
                    src/components/NeutrinoRecAlgs_entries.cxx
                    LINK_LIBRARIES AthenaBaseComps GeneratorObjects TrackerSimEvent ScintSimEvent FaserCaloSimEvent NeutrinoSimEvent Identifier NeutrinoIdentifier NeutrinoReadoutGeometry StoreGateLib SGtests
        )

atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )