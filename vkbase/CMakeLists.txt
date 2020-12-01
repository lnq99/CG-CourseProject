file(GLOB BASE_SRC "*.cpp")
file(GLOB BASE_HEADERS "*.hpp")

if(WIN32)
    add_library(vkbase STATIC ${BASE_SRC})
    target_link_libraries(vkbase
        ${Vulkan_LIBRARY}
        ${WINLIBS}
    )

else(WIN32)
    add_library(vkbase STATIC ${BASE_SRC})
    target_link_libraries(vkbase
        ${Vulkan_LIBRARY}
        ${XCB_LIBRARY}
    )

endif(WIN32)