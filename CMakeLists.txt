cmake_minimum_required(VERSION 3.6)

set(project_name point_cloud)
project(${project_name})

# GLFW
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

#GLU
#pkg_search_module(GLU REQUIRED glu)

# NUKLEAR GUI
# set(nuklear lib/nuklear)

#CIMGUI
set(cimgui lib/cimgui)
set(imgui_impl ${cimgui}/imgui/examples)

set(gl3w lib/gl3w)
set(klib lib/klib)
set(linmath lib/linmath/)

# library search path
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig")
set(ENV{LD_LIBRARY_PATH} "ENV{LD_LIBRARY_PATH}:/usr/local/lib")

# build dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -Wall -g")

# Include dirs
include_directories(
	src 
	${cimgui} 
	${cimgui}/imgui ${imgui_impl}
    ${gl3w}
    ${klib}
    ${linmath})

file(GLOB source_files "src/*.c")
file(GLOB gl3w_files "${gl3w}/GL/*.c")

# cimgui compilation
file(GLOB 
	cimgui_files 
	"${cimgui}/*.cpp" 
	"${cimgui}/imgui/*.cpp"
	"${imgui_impl}/imgui_impl_glfw.cpp"
	"${imgui_impl}/imgui_impl_opengl3.cpp")
add_library(cimgui ${cimgui_files})
if (WIN32)
	target_link_libraries(cimgui ${GLFW_LIBRARIES})
endif (WIN32)

target_compile_definitions(cimgui PRIVATE 
	IMGUI_IMPL_API=extern\ \"C\"
	IMGUI_IMPL_OPENGL_LOADER_GL3W)

add_executable(
	${project_name} 
	${source_files}
	${gl3w_files})

if (WIN32)
	target_link_libraries(
		${project_name}
		${GLFW_LIBRARIES}
		m opengl32 dl cimgui)	
endif (WIN32)

if (LINUX)
	target_link_libraries(
		${project_name}
		${GLFW_LIBRARIES}
		m GL dl cimgui)	
endif (LINUX)



