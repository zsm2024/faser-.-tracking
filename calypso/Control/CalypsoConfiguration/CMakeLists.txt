################################################################################
# Package: CalypsoConfiguration
################################################################################

# Declare the package name:
atlas_subdir( CalypsoConfiguration )

# External dependencies:
find_package( six )

# Install files from the package:
atlas_install_python_modules( python/*.py  )

