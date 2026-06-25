cmake_minimum_required ( VERSION 3.16...3.21 )
project ( WSMDSim )

set ( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set ( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

# ----------------------------------------------------------------------------
# Find Geant4 package
# Set WITH_GEANT4_UIVIS=OFF for batch-only builds
#
option ( WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON )

if ( WITH_GEANT4_UIVIS )
  find_package ( Geant4 REQUIRED ui_all vis_all )
else ()
  find_package ( Geant4 REQUIRED )
endif ()

# ----------------------------------------------------------------------------
# Geant4 setup + project include directory
#
include ( ${Geant4_USE_FILE} )
include_directories ( ${PROJECT_SOURCE_DIR}/include )

# ----------------------------------------------------------------------------
# Find ROOT
#
find_package ( ROOT REQUIRED COMPONENTS Core Hist RIO Tree )
include ( ${ROOT_USE_FILE} )

# ----------------------------------------------------------------------------
# Sources and headers
#
file ( GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc )
file ( GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh )

# ----------------------------------------------------------------------------
# Generate ROOT Dictionary
#
ROOT_GENERATE_DICTIONARY ( WCMDSimDict
  MCEventInfo.hh MCPhotonHit.hh MCPMT.hh MCPMTData.hh MCPrimary.hh MCPrimaryData.hh
  MCStep.hh MCTrack.hh MCTrackData.hh PMTHit.hh
  LINKDEF include/LinkDef.h
  OPTIONS -I${Geant4_INCLUDE_DIRS} -I${PROJECT_SOURCE_DIR}/include
)

# ----------------------------------------------------------------------------
# Standalone data library — loadable from ROOT macros without Geant4
#
set ( data_sources
  ${PROJECT_SOURCE_DIR}/src/MCEventInfo.cc
  ${PROJECT_SOURCE_DIR}/src/MCPhotonHit.cc
  ${PROJECT_SOURCE_DIR}/src/MCPMT.cc
  ${PROJECT_SOURCE_DIR}/src/MCPMTData.cc
  ${PROJECT_SOURCE_DIR}/src/MCPrimary.cc
  ${PROJECT_SOURCE_DIR}/src/MCPrimaryData.cc
  ${PROJECT_SOURCE_DIR}/src/MCStep.cc
  ${PROJECT_SOURCE_DIR}/src/MCTrack.cc
  ${PROJECT_SOURCE_DIR}/src/MCTrackData.cc
)

ROOT_GENERATE_DICTIONARY ( WCMDSimDataDict
  MCEventInfo.hh MCPhotonHit.hh MCPMT.hh MCPMTData.hh MCPrimary.hh MCPrimaryData.hh
  MCStep.hh MCTrack.hh MCTrackData.hh
  LINKDEF include/DataLinkDef.h
  OPTIONS -I${PROJECT_SOURCE_DIR}/include
)

add_library ( WCMDSimData SHARED WCMDSimDataDict.cxx ${data_sources} )
target_include_directories ( WCMDSimData PUBLIC ${PROJECT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR} )
target_link_libraries ( WCMDSimData ROOT::Core ROOT::RIO ROOT::Tree )

# ----------------------------------------------------------------------------
# Executable
#
add_executable ( WCMDSim WCMDSim.cc WCMDSimDict.cxx ${sources} )
target_include_directories ( WCMDSim PRIVATE ${PROJECT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR} )
target_link_libraries ( WCMDSim ${Geant4_LIBRARIES} ROOT::Core ROOT::Hist ROOT::RIO ROOT::Tree )

# ----------------------------------------------------------------------------
# Install
#
include ( GNUInstallDirs )

configure_file ( setup_wcmdsim.sh.in setup_wcmdsim.sh @ONLY )
install ( PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/setup_wcmdsim.sh
  DESTINATION .
)

install ( TARGETS WCMDSim
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install ( TARGETS WCMDSimData
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# ROOT dictionary support files (needed for TClass lookup)
install ( FILES
  ${CMAKE_CURRENT_BINARY_DIR}/libWCMDSimDataDict.rootmap
  ${CMAKE_CURRENT_BINARY_DIR}/libWCMDSimDataDict_rdict.pcm
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Data class headers (for use from ROOT macros without Geant4)
install ( FILES
  include/MCEventInfo.hh
  include/MCPhotonHit.hh
  include/MCPMT.hh
  include/MCPMTData.hh
  include/MCPrimary.hh
  include/MCPrimaryData.hh
  include/MCStep.hh
  include/MCTrack.hh
  include/MCTrackData.hh
  include/PMTHit.hh
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Macros and input flux file
install ( FILES run.mac vis.mac muon_flux.root
  DESTINATION ${CMAKE_INSTALL_DATADIR}
)
