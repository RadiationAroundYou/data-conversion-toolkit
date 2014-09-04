#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(data-conversion-toolkit)

#----------------------------------------------------------------------------
# Find ROOT variables if the variable G4ANALYSIS_USE_ROOT is set
#
find_package(ROOT)

# Get the curl library.
find_package(CURL)
#find_library(CURL_LIBRARY NAMES curl)
#message(STATUS ${CURL_LIBRARY})

# Get the curlpp library.
find_library(CURLPP_LIBRARY NAMES curlpp)
message(STATUS ${CURLPP_LIBRARY})

# Get the json-c library.
find_library(JSONC_LIBRARY NAMES json-c)
message(STATUS ${JSONC_LIBRARY})

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
if(ROOT_FOUND)
include_directories(${ROOT_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/include)
message(STATUS ${ROOT_INCLUDE_DIR})
else()
include_directories(${PROJECT_SOURCE_DIR}/include)
endif()

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

#----------------------------------------------------------------------------
# Add the executables, and link it to the Geant4 libraries
#
add_executable(Cl2Mf-converter Cl2Mf-converter.cpp ${sources} ${headers})
add_executable(Mo2Mf-converter Mo2Mf-converter.cpp ${sources} ${headers})
add_executable(Px2Mf-converter Px2Mf-converter.cpp ${sources} ${headers}) 
add_executable(Lu2Mf-converter Lu2Mf-converter.cpp ${sources} ${headers}) 
add_executable(Mf-updater Mf-updater.cpp ${sources} ${headers}) 
add_executable(Mf-filter Mf-filter.cpp ${sources} ${headers}) 

if(ROOT_FOUND)
target_link_libraries(Cl2Mf-converter ${ROOT_LIBRARIES} -lXMLParser -lGeom -lXMLIO ${CURL_LIBRARY}  ${CURLPP_LIBRARY} ${JSONC_LIBRARY})
target_link_libraries(Px2Mf-converter ${ROOT_LIBRARIES} -lXMLParser -lGeom -lXMLIO ${CURL_LIBRARY}  ${CURLPP_LIBRARY} ${JSONC_LIBRARY})
target_link_libraries(Mo2Mf-converter ${ROOT_LIBRARIES} -lXMLParser -lGeom -lXMLIO ${CURL_LIBRARY}  ${CURLPP_LIBRARY} ${JSONC_LIBRARY})
target_link_libraries(Lu2Mf-converter ${ROOT_LIBRARIES} -lXMLParser -lGeom -lXMLIO ${CURL_LIBRARY}  ${CURLPP_LIBRARY} ${JSONC_LIBRARY})
target_link_libraries(Mf-updater      ${ROOT_LIBRARIES} -lXMLParser -lGeom -lXMLIO ${CURL_LIBRARY}  ${CURLPP_LIBRARY} ${JSONC_LIBRARY})
target_link_libraries(Mf-filter       ${ROOT_LIBRARIES} -lXMLParser -lGeom -lXMLIO ${CURL_LIBRARY}  ${CURLPP_LIBRARY} ${JSONC_LIBRARY})
message(STATUS ${ROOT_LIBRARIES})
endif()


#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS Cl2Mf-converter DESTINATION bin)
install(TARGETS Px2Mf-converter DESTINATION bin)
install(TARGETS Mo2Mf-converter DESTINATION bin)
install(TARGETS Lu2Mf-converter DESTINATION bin)
install(TARGETS Mf-updater DESTINATION bin)
install(TARGETS Mf-filter DESTINATION bin)
