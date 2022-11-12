set(DCOMMONTEST_SOURCES
	"AMFDeserializeTests.cpp"
	"TestLDFFormat.cpp"
	"TestNiPoint3.cpp"
	"TestEncoding.cpp"
)

# Set our executable
add_executable(dCommonTests ${DCOMMONTEST_SOURCES})

# Link needed libraries
target_link_libraries(dCommonTests ${COMMON_LIBRARIES} GTest::gtest_main)

# Copy test files to testing directory
add_subdirectory(TestBitStreams)
file(COPY ${TESTBITSTREAMS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Discover the tests
gtest_discover_tests(dCommonTests)