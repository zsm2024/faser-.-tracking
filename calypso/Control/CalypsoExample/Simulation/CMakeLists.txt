################################################################################
# Package: Simulation
################################################################################

# Declare the package name:
atlas_subdir( Simulation )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/*.sh scripts/*.py )


