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

# TODO: VS doesn't recofnize /../ path in include libs, solve this

target_include_directories(ydibgtch PUBLIC "${PROJECT_SOURCE_DIR}/../include")

# TODO: should also add cmake/src/* to includes
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\include
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\.pio\libdeps\esp32\EncButton\src
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\.pio\libdeps\esp32\TimerMs\src
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\.pio\libdeps\esp32\Unity\src
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\cmake\src\cores
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\cmake\src\libraries\WiFi
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\cmake\src\libraries\WebServer
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\cmake\src\libraries\DNSServer
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\cmake\src\libraries\EEPROM
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\cmake\src\tools\sdk\include\mdns
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\cmake\src\libdeps\FastLED
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\cmake\src\libdeps\VirtualButton

# These libs are excluded:
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\.pio\libdeps\esp32\FastLED\src
# D:\prj\chess_maxon\dgt_chess_board\ydibdgtch\.pio\libdeps\esp32\VirtualButton\src


# # file(GLOB LIBS_INCLUDE "${PROJECT_SOURCE_DIR}/../.pio/esp32/*")
# file(GLOB LIBS_INCLUDE "${PROJECT_SOURCE_DIR}/../.pio/esp32/*")
# foreach(dir ${LIBS_INCLUDE})
# 	variable_watch(dir)
#     IF(IS_DIRECTORY "${dir}/src")
#         target_include_directories(ydibgtch PUBLIC "${dir}/src")
#     ELSE()
#         CONTINUE()
#     ENDIF()
# endforeach()