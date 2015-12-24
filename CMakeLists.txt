cmake_minimum_required (VERSION 2.6)

project(assettool)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file (GLOB MAIN_SOURCES "*.cpp" "*.h")
file (GLOB GENERIC_SOURCES "Generic/*.cpp" "Generic/*.h")
file (GLOB EXTERNAL_SOURCES "External/*.cpp" "External/*.h")
file (GLOB THPS_SOURCES "THPS/*.cpp" "THPS/*.h")
file (GLOB GTASA_SOURCES "GTASA/*.cpp" "GTASA/*.h")
file (GLOB ASSIMP_SOURCES "Assimp/*.cpp" "Assimp/*.h")
file (GLOB CHCENGINE_SOURCES "CHCEngine/*.cpp" "CHCEngine/*.h")
file (GLOB XML_SOURCES "XML/*.cpp" "XML/*.h")

set (ALL_SRCS ${MAIN_SOURCES} ${GENERIC_SOURCES} ${EXTERNAL_SOURCES} ${THPS_SOURCES} ${GTASA_SOURCES} ${ASSIMP_SOURCES} ${CHCENGINE_SOURCES} ${XML_SOURCES})

include_directories (. External)

source_group("" FILES ${MAIN_SOURCES})
source_group("Generic" FILES ${GENERIC_SOURCES})
source_group("External" FILES ${EXTERNAL_SOURCES})
source_group("THPS" FILES ${THPS_SOURCES})
source_group("GTASA" FILES ${GTASA_SOURCES})
source_group("Assimp" FILES ${ASSIMP_SOURCES})
source_group("CHCEngine" FILES ${CHCENGINE_SOURCES})
source_group("XML" FILES ${XML_SOURCES})

#OpenGL specific stuff
IF(WIN32)

add_executable (assettool WIN32 ${ALL_SRCS})
#target_compile_definitions(assettool)
#target_link_libraries(assettool)

ELSE() #unix

ENDIF()