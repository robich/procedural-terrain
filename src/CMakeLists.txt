FILE(GLOB_RECURSE APP app/*)
FILE(GLOB_RECURSE BUFFERS buffers/*)
FILE(GLOB_RECURSE CAMERA camera/*)
FILE(GLOB_RECURSE NOISE noise/*)
FILE(GLOB_RECURSE SCENE scene/*)
FILE(GLOB_RECURSE TEXTURES textures/*)
FILE(GLOB_RECURSE TW tw/*)

source_group(Source\\app FILES ${APP})
source_group(Source\\buffers FILES ${BUFFERS})
source_group(Source\\camera FILES ${CAMERA})
source_group(Source\\noise FILES ${NOISE})
source_group(Source\\scene FILES ${SCENE})
source_group(Source\\textures FILES ${TEXTURES})
source_group(Source\\tw FILES ${TW})

add_executable(Mountains ${APP} ${BUFFERS} ${CAMERA} ${NOISE} ${SCENE} ${TEXTURES} ${TW})

target_link_libraries(Mountains ${COMMON_LIBS})
target_deploy_file(Mountains ../external/AntTweakBar/lib/x64/AntTweakBar64.dll)

