################################################################################
# Package: FaserSiDigitization
################################################################################

# Declare the package name:
atlas_subdir( FaserSiDigitization )

# Component(s) in the package:
atlas_add_library( FaserSiDigitization
                   src/SiChargedDiode.cxx
                   src/SiChargedDiodeCollection.cxx
                   src/SiSurfaceCharge.cxx
                   PUBLIC_HEADERS FaserSiDigitization
                   LINK_LIBRARIES AthenaKernel AthAllocators Identifier GaudiKernel TrackerReadoutGeometry TrackerSimEvent )

