add_executable(Callgrind "${CMAKE_CURRENT_SOURCE_DIR}/callgrind_benchmark/main.cpp")
target_compile_features(Callgrind PRIVATE cxx_std_20)

# Our benchmark executable also wants to know about our plugin code...
target_include_directories(Callgrind PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)

# Copy over compile definitions from our plugin target so it has all the JUCEy goodness
target_compile_definitions(Callgrind PRIVATE $<TARGET_PROPERTY:${PROJECT_NAME},COMPILE_DEFINITIONS>)

# And give tests access to our shared code
target_link_libraries(Callgrind PRIVATE SharedCode Catch2::Catch2WithMain)

# Make an Xcode Scheme for the test executable so we can run tests in the IDE
set_target_properties(Callgrind PROPERTIES XCODE_GENERATE_SCHEME ON)

# When running Tests we have specific needs
target_compile_definitions(Callgrind PUBLIC
        JUCE_MODAL_LOOPS_PERMITTED=1 # let us run Message Manager in tests
        RUN_PAMPLEJUCE_TESTS=1 # also run tests in module .cpp files guarded by RUN_PAMPLEJUCE_TESTS
)

