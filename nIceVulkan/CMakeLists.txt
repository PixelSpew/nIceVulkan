set(RES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res/")
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/inc/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/inc/config.h")

include_directories(. inc ../external/tinyobjloader ${CMAKE_CURRENT_BINARY_DIR}/inc)
file(GLOB_RECURSE SRC ./*.cpp src/*.cpp)

add_definitions(-DUNICODE -DVK_USE_PLATFORM_WIN32_KHR)
if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
	add_executable(nIceFramework WIN32 ${SRC})
else()
	add_executable(nIceFramework ${SRC})
endif()

target_link_libraries (nIceFramework tinyobjloader ${CMAKE_CURRENT_SOURCE_DIR}/lib/vulkan-1.lib)
