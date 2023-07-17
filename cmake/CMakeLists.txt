cmake_minimum_required(VERSION 3.0)

project(ydibgtch)

file(GLOB SOURCES_LEVEL1 "${PROJECT_SOURCE_DIR}/../src/*.cpp")
file(GLOB SOURCES_LEVEL2 "${PROJECT_SOURCE_DIR}/../src/*/*.cpp")
# file(GLOB SOURCES_LEVEL3 "${PROJECT_SOURCE_DIR}/../src/*/*/*.cpp")

file(GLOB HEADERS_LEVEL1 "${PROJECT_SOURCE_DIR}/../include/*.h")
file(GLOB HEADERS_LEVEL2 "${PROJECT_SOURCE_DIR}/../include/*/*.h")
# file(GLOB HEADERS_LEVEL3 "${PROJECT_SOURCE_DIR}/../include/*/*/*.h")

# add_executable(ydibgtch ../src/main.cpp)
add_executable(ydibgtch
						${SOURCES_LEVEL1} ${HEADERS_LEVEL1}
						${SOURCES_LEVEL2} ${HEADERS_LEVEL2}
						# ${SOURCES_LEVEL3} ${HEADERS_LEVEL3}
				)

# VS doesn't recofnize /../ path in include libs, solve this
target_include_directories(ydibgtch PUBLIC "${PROJECT_SOURCE_DIR}/../include")