################################################################################
# Package: FaserISF_HepMC_Interfaces
################################################################################

# Declare the package name:
atlas_subdir( FaserISF_HepMC_Interfaces )

atlas_add_library( FaserISF_HepMC_Interfaces
                   INTERFACE
                   PUBLIC_HEADERS FaserISF_HepMC_Interfaces
                   LINK_LIBRARIES AthenaKernel GaudiKernel )