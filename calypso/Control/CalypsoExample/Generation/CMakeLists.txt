################################################################################
# Package: Generation
################################################################################

# Declare the package name:
atlas_subdir( Generation )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/*.sh scripts/*.py )


