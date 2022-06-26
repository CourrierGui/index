file(GLOB
    unit_tests
    attributes.cpp
    cli.cpp
)

foreach(file ${unit_tests})
    get_filename_component(bin ${file} NAME_WE)

    add_executable(Index.test-${bin} ${file})
    target_link_libraries(Index.test-${bin}
        PRIVATE
            compilation_flags
            CleanTest::main-automatic
            index_utils
    )
    add_test(
        NAME    Index.test-${bin}
        COMMAND Index.test-${bin} -j 1
    )
endforeach(file)

add_test(
    NAME    acceptance
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/acceptance.bash ${CMAKE_SOURCE_DIR}
)
