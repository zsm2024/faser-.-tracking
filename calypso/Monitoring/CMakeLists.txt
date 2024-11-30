################################################################################
# Package: Monitoring
# Contains python code to implement offline monitoring
################################################################################


# Declare the package name:
atlas_subdir( Monitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/*.py )
