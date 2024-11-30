################################################################################
# Package: Digitization
################################################################################

# Declare the package name:
atlas_subdir( Digitization )

# Install files from the package:
atlas_install_scripts( scripts/*.sh scripts/*.py )

atlas_add_test( ProdDigi
    SCRIPT scripts/faser_digi.py ${CMAKE_CURRENT_SOURCE_DIR}/../rawdata/FaserMC-MC22_PG_muon_fasernu_100GeV-100012-00000-HITS.root
    PROPERTIES TIMEOUT 300 )


