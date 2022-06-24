file(GLOB
    tests
    attributes.cpp
)

foreach(file ${tests})
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
        COMMAND Index.test-${bin}
    )
endforeach(file)
