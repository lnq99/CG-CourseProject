function(build)
    SET(FOLDER ${CMAKE_CURRENT_SOURCE_DIR})

    SET(NAME app)

    # Main
    file(GLOB SOURCE *.cpp ${BASE_HEADERS} ${FOLDER}/*.cpp Model/*.cpp)

    SET(MAIN_CPP ${FOLDER}/main.cpp)

    # imgui example requires additional source files
    file(GLOB ADD_SOURCE "../lib/imgui/*.cpp")
    SET(SOURCE ${SOURCE} ${ADD_SOURCE})

    add_executable(${NAME} ${MAIN_CPP} ${SOURCE})
    target_link_libraries(${NAME} vkbase )

    set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

    if(RESOURCE_INSTALL_DIR)
        install(TARGETS ${NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endfunction(build)


build()
