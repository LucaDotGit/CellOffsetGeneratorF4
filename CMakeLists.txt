# --- Project ---

cmake_minimum_required(VERSION 4.5.0)

set(COMMONLIB_PLUGIN_NAME "CellOffsetGenerator")
set(COMMONLIB_PLUGIN_AUTHOR "Luca")
set(COMMONLIB_PLUGIN_VERSION "2.0.0.0")
set(COMMONLIB_PLUGIN_LICENSE "GPL-3.0-or-later")
set(COMMONLIB_PLUGIN_DESCRIPTION "${COMMONLIB_PLUGIN_NAME}")
set(COMMONLIB_PLUGIN_COMMENTS "")

string(TIMESTAMP COMMONLIB_PLUGIN_COPYRIGHT_YEAR "%Y" UTC)

set(COMMONLIB_FEATURE_INI FALSE)
set(COMMONLIB_FEATURE_TOML TRUE)
set(COMMONLIB_FEATURE_JSON FALSE)
set(COMMONLIB_FEATURE_YAML FALSE)
set(COMMONLIB_FEATURE_XBYAK FALSE)

set(COMMONLIB_RUNTIME_VR FALSE)
set(COMMONLIB_RUNTIME_OG TRUE)
set(COMMONLIB_RUNTIME_NG TRUE)
set(COMMONLIB_RUNTIME_AE TRUE)

project(
	"${COMMONLIB_PLUGIN_NAME}"
	VERSION "${COMMONLIB_PLUGIN_VERSION}"
	DESCRIPTION "${COMMONLIB_PLUGIN_DESCRIPTION}"
	SPDX_LICENSE "${COMMONLIB_PLUGIN_LICENSE}"
	LANGUAGES CXX
)

# --- Dependencies ---

find_package(
	xxhash
	REQUIRED
	CONFIG
)

# --- Source Files ---

set(COMMONLIB_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/CommonLibF4")

if(NOT TARGET CommonLibF4)
	add_subdirectory(
		"${COMMONLIB_ROOT_DIR}"
		CommonLibF4
	)
endif()

add_library(
	"${PROJECT_NAME}"
	SHARED
)

file(
	GLOB_RECURSE
	_headers
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

file(
	GLOB_RECURSE
	_modules
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cppm"
)

file(
	GLOB_RECURSE
	_sources
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

target_sources(
	"${PROJECT_NAME}"
	PRIVATE
	FILE_SET HEADERS
	FILES ${_headers}

	PRIVATE
	FILE_SET CXX_MODULES
	FILES ${_modules}

	PRIVATE
	${_sources}
)

target_precompile_headers(
	"${PROJECT_NAME}"
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.hpp"
)

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# --- Plugin Info ---

include("${COMMONLIB_ROOT_DIR}/assets/cmake/ConfigureXsePluginInfo.cmake")

configure_xse_plugin_info("${COMMONLIB_ROOT_DIR}")

# --- Output Path ---

include("${COMMONLIB_ROOT_DIR}/assets/cmake/ConfigureXseOutputPath.cmake")

configure_xse_output_path()

# --- Compilation ---

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
	"XXH_INLINE_ALL=1"
	"XXH_ENABLE_AUTOVECTORIZE=1"
	"$<$<CONFIG:Debug>:XXH_DEBUGLEVEL=1>"
)

# --- Linkage ---

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
	"CommonLibF4::CommonLibF4"
	"xxHash::xxhash"
)
