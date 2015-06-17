#file(GLOB_RECURSE BUFFERS "*")
#target_deploy_file(${PROJECTNAME} ${BUFFERS})

add_executable(Buffers framebuffer.cpp)
target_link_libraries(Buffers LINK_PUBLIC ${CMALE_CURRENT_SOURCE_DIR})

#add_executable(buffers framebuffer.cpp)
#target_link_libraries(buffers Moutains)