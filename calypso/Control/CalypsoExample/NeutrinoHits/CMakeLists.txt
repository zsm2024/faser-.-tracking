################################################################################
# Package: NeutrinoHits
################################################################################

# Declare the package name:
atlas_subdir( NeutrinoHits )

# Install files from the package:
atlas_install_scripts( scripts/*.sh scripts/*.py )

atlas_add_test( ProdNeutrinoHits
    SCRIPT scripts/faser_dumpnu.py ${CMAKE_CURRENT_SOURCE_DIR}/../rawdata/FaserMC-MC22_PG_muon_fasernu_100GeV-100012-00000-HITS.root
    PROPERTIES TIMEOUT 300 )

