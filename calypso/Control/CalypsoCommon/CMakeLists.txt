################################################################################
# Package: CalypsoCommon
################################################################################

# Declare the package name:
atlas_subdir( CalypsoCommon )

# External dependencies:
find_package( six )

# Install files from the package:
atlas_install_python_modules( python/*.py )
#atlas_install_joboptions( share/Preparation.py share/Execution.py share/Atlas.UnixStandardJob.py test/*.py 
#                          share/zeroJO.py share/Atlas_Gen.UnixStandardJob.py share/MemTraceInclude.py share/runbatch.py)
#atlas_install_scripts( share/athena.py share/athena3.py share/athena_preload.sh share/chappy.py share/find_cfg_dups.py share/test_cfg_pickling.py )
#atlas_install_runtime(share/*.pkl)

# Aliases:
atlas_add_alias( calypso "athena.py" )

