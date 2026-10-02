# Each small check is a standalone executable with its own main().
file(GLOB small_test_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_custom_target(SmallTests)

foreach(source IN LISTS small_test_sources)
    get_filename_component(test_name "${source}" NAME_WE)
    set(target "Small_${test_name}")
    add_executable("${target}" "${source}")
    target_compile_features("${target}" PRIVATE cxx_std_14)
    add_dependencies(SmallTests "${target}")
    add_test(NAME "Small.${test_name}" COMMAND "$<TARGET_FILE:${target}>")
    set_tests_properties("Small.${test_name}" PROPERTIES LABELS "small" TIMEOUT 30)
endforeach()
