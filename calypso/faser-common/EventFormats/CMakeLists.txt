if (${CMAKE_PROJECT_NAME} STREQUAL "Calypso")
  # Calypso/Athena build

  message("EventFormats: Calypso")

  atlas_subdir(EventFormats)
 # Include the CMake module to use ExternalProject_Add
  include(ExternalProject)

  ExternalProject_Add( proj_ZStd
    INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
    URL https://github.com/facebook/zstd/archive/refs/tags/v1.5.5.tar.gz
    PREFIX  ${CMAKE_CURRENT_BINARY_DIR}/ZStd
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE   true
    DOWNLOAD_EXTRACT_TIMESTAMP false
    BUILD_COMMAND     make lib
    INSTALL_COMMAND   make prefix=${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM} install
    )
  ExternalProject_Get_Property(proj_ZStd install_dir)
  message("install dir of projZStd = ${install_dir}")
  message("destination dir of projZStd = ${CMAKE_CURRENT_BINARY_DIR}ZStd")


  include_directories(${install_dir}/include)

  set_target_properties(proj_ZStd PROPERTIES IMPORTED_LOCATION ${install_dir})
  set_target_properties(proj_ZStd PROPERTIES IMPORTED_LOCATION_RelWithDebInfo ${install_dir})

  set_target_properties(proj_ZStd PROPERTIES LOCATION ${install_dir}/lib)
  set_target_properties(proj_ZStd PROPERTIES LOCATION_RelWithDebInfo ${install_dir}/lib)

  set( ZSTD_LIBRARIES ${install_dir}/lib/libzstd.so )

  add_library(ZSTD SHARED IMPORTED)
  set_target_properties(ZSTD PROPERTIES IMPORTED_LOCATION ${install_dir}/lib)
  set_target_properties(ZSTD PROPERTIES IMPORTED_LOCATION_RelWithDebInfo ${install_dir}/lib)
  set_target_properties(ZSTD PROPERTIES LOCATION ${install_dir}/lib)
  set_target_properties(ZSTD PROPERTIES LOCATION_RelWithDebInfo ${install_dir}/lib)
  add_dependencies(ZSTD proj_ZStd)

  find_package(PkgConfig QUIET)
  pkg_check_modules(LZ4 liblz4)

  if(NOT LZ4_FOUND)
    message("Locating lz4 library in LCG release instead...")
    find_path(LZ4_INCLUDE_DIR lz4.h
      PATHS "${LZ4_LCGROOT}"
      PATH_SUFFIXES include)
    
    find_library(LZ4_LIBRARY NAMES lz4 lz4_static
      PATHS "${LZ4_LCGROOT}"
      PATH_SUFFIXES lib
      REQUIRED)

    if(LZ4_LIBRARY)
        set(LZ4_LIBRARIES ${LZ4_LIBRARY})
	message("lz4 located: ${LZ4_LIBRARIES}")
        get_filename_component(LZ4_LIBRARY_DIRS ${LZ4_LIBRARY} DIRECTORY)
    endif()

  else()
      find_library(LZ4_LIBRARIES
          NAMES lz4 lz4_static
          PATHS ${LZ4_LIBRARY_DIRS}
          NO_DEFAULT_PATH)
  endif()

  pkg_check_modules(ZLIB libzlib)

  if(NOT ZLIB_FOUND)
    message("Locating zlib library in LCG release instead...")
    find_path(ZLIB_INCLUDE_DIR zlib.h
      PATHS "${ZLIB_LCGROOT}"
      PATH_SUFFIXES include)

    find_library(ZLIB_LIBRARY NAMES zlib
      PATHS "${ZLIB_LCGROOT}"
      PATH_SUFFIXES lib
      REQUIRED)

    if(ZLIB_LIBRARY)
        set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
	message("zlib located: ${ZLIB_LIBRARIES}")
        get_filename_component(ZLIB_LIBRARY_DIRS ${ZLIB_LIBRARY} DIRECTORY)
    endif()

  else()
      find_library(ZLIB_LIBRARIES
          NAMES zlib
          PATHS ${ZLIB_LIBRARY_DIRS}
          NO_DEFAULT_PATH)
  endif()

  mark_as_advanced(LZ4_INCLUDE_DIR LZ4_LIBRARY_DIRS LZ4_LIBRARIES LZ4_LIBRARY)
  mark_as_advanced(ZSTD_LIBRARIES ZSTD_INCLUDE_DIR)
  mark_as_advanced(ZLIB_INCLUDE_DIR ZLIB_LIBRARY_DIRS ZLIB_LIBRARIES ZLIB_LIBRARY)

  atlas_add_library ( EventFormats
       	  EventFormats/*.hpp EventFormats/*.icc
          INTERFACE
       	  PUBLIC_HEADERS EventFormats
	  INCLUDE_DIRS ${LZ4_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR}
	  LINK_LIBRARIES Exceptions ${ZSTD_LIBRARIES} ${LZ4_LIBRARIES} pthread ${ZLIB_LIBRARIES}
       	  )
  add_dependencies(EventFormats ZSTD lz4)

  target_include_directories( EventFormats INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../Logging/include;${install_dir}/include>"
  )


  atlas_add_executable( eventDump apps/eventDump.cxx
     LINK_LIBRARIES EventFormats
  )

  target_include_directories( eventDump PUBLIC
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../Logging/include>
  )

  atlas_add_executable( eventFilter apps/eventFilter.cxx
     LINK_LIBRARIES EventFormats
  )

  target_include_directories( eventFilter PUBLIC
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../Logging/include>
  )
else()
  # Online build

  find_package(ZLIB 1.2.11 REQUIRED)
  include_directories(${ZLIB_INCLUDE_DIRS})

  find_package(PkgConfig REQUIRED)
  # If found, LZ$_* variables will be defined
  pkg_check_modules(LZ4 REQUIRED liblz4)

  find_library(LZ4_LIBRARY
    NAMES lz4 lz4_static
    PATHS ${LZ4_LIBRARY_DIRS}
    NO_DEFAULT_PATH)

  pkg_check_modules(ZSTD REQUIRED libzstd)

  find_library(LZSTD_LIBRARY
    NAMES zstd
    PATHS ${ZSTD_LIBRARY_DIRS}
    NO_DEFAULT_PATH)

  mark_as_advanced(LZ4_LIBRARY LZ4_INCLUDE_DIRS LZ4_LIBRARY_DIRS LZ4_LIBRARIES)
  mark_as_advanced(ZSTD_LIBRARY ZSTD_INCLUDE_DIRS ZSTD_LIBRARY_DIRS ZSTD_LIBRARIES)

  add_library(EventFormats INTERFACE)
  target_include_directories(EventFormats SYSTEM INTERFACE ./ ${ZSTD_INCLUDE_DIRS} ${LZ4_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
  target_link_libraries(EventFormats INTERFACE Exceptions Logging ${ZSTD_LIBRARIES} pthread ${ZLIB_LIBRARIES} ${LZ4_LIBRARIES})

  add_faser_executable(eventDump apps/eventDump.cxx)
  add_faser_executable(eventFilter apps/eventFilter.cxx)

  target_link_libraries(eventDump EventFormats)
  target_link_libraries(eventFilter EventFormats)
  if (${CMAKE_PROJECT_NAME} STREQUAL "daqling_top")
    target_link_libraries(eventDump ers)
    target_link_libraries(eventFilter ers)
  endif()

endif()
