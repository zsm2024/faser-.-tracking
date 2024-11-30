atlas_subdir( GenEventExample )

atlas_add_component( GenEventExample
                    src/GenEventReadAlg.cxx
                    src/components/GenEventExample_entries.cxx
                    LINK_LIBRARIES AthenaBaseComps GeneratorObjects 
        )

atlas_install_python_modules( python/*.py )
