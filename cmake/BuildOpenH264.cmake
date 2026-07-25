# Build OpenH264 (static) via meson/ninja for optional screen capture.
# Soft-disables on missing tools, download failure, or build failure.
# Override:
#   -DOPENH264_VERSION=v2.4.1
#   -DOPENH264_DOWNLOAD_URL=<full tarball url>
#
# OpenH264's meson build hard-requires nasm on x86/x86_64.

set(OPENH264_VERSION "v2.4.1" CACHE STRING "OpenH264 release tag")
set(OPENH264_DOWNLOAD_URL "" CACHE STRING
    "Optional full URL to openh264 source archive (overrides version)")

set(AKHENATEN_HAS_OPENH264 FALSE)
set(OPENH264_INCLUDE_DIR "")
set(MESON_PYTHON_MODULE FALSE)

find_program(MESON_EXECUTABLE NAMES meson meson.py)
find_program(NINJA_EXECUTABLE NAMES ninja ninja-build)

# Windows pip --user often installs meson outside PATH; fall back to python -m meson.
if(NOT MESON_EXECUTABLE)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        execute_process(
            COMMAND "${Python3_EXECUTABLE}" -m meson --version
            RESULT_VARIABLE _oh_meson_mod_rc
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(_oh_meson_mod_rc EQUAL 0)
            set(MESON_EXECUTABLE "${Python3_EXECUTABLE}" CACHE FILEPATH "meson via python -m" FORCE)
            set(MESON_PYTHON_MODULE TRUE)
            message(STATUS "openh264: using `${Python3_EXECUTABLE} -m meson`")
        endif()
    endif()
endif()

if(NOT MESON_EXECUTABLE)
    message(WARNING "openh264: meson not found — video recording disabled")
    return()
endif()
if(NOT MSVC AND NOT NINJA_EXECUTABLE)
    message(WARNING "openh264: ninja not found — video recording disabled")
    return()
endif()

if(MESON_PYTHON_MODULE)
    set(_OH_MESON_CMD "${MESON_EXECUTABLE}" -m meson)
else()
    set(_OH_MESON_CMD "${MESON_EXECUTABLE}")
endif()

# --- nasm (required on x86/x86_64) --------------------------------------------
find_program(NASM_EXECUTABLE NAMES nasm)
if(NOT NASM_EXECUTABLE AND WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_nasm_ver "2.16.03")
    set(_nasm_root "${CMAKE_BINARY_DIR}/_deps/nasm")
    set(_nasm_zip "${_nasm_root}/nasm-${_nasm_ver}-win64.zip")
    set(_nasm_url "https://www.nasm.us/pub/nasm/releasebuilds/${_nasm_ver}/win64/nasm-${_nasm_ver}-win64.zip")
    file(MAKE_DIRECTORY "${_nasm_root}")
    if(NOT EXISTS "${_nasm_root}/nasm-${_nasm_ver}/nasm.exe")
        message(STATUS "openh264: downloading nasm ${_nasm_ver}…")
        file(DOWNLOAD "${_nasm_url}" "${_nasm_zip}"
            SHOW_PROGRESS STATUS _nasm_dl TLS_VERIFY ON)
        list(GET _nasm_dl 0 _nasm_code)
        if(_nasm_code EQUAL 0)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xf "${_nasm_zip}"
                WORKING_DIRECTORY "${_nasm_root}"
                RESULT_VARIABLE _nasm_tar_rc
            )
            if(NOT _nasm_tar_rc EQUAL 0)
                message(WARNING "openh264: nasm extract failed")
            endif()
        else()
            list(GET _nasm_dl 1 _nasm_msg)
            message(WARNING "openh264: nasm download failed (${_nasm_msg})")
        endif()
    endif()
    if(EXISTS "${_nasm_root}/nasm-${_nasm_ver}/nasm.exe")
        set(NASM_EXECUTABLE "${_nasm_root}/nasm-${_nasm_ver}/nasm.exe")
    endif()
endif()

if(NOT NASM_EXECUTABLE)
    message(WARNING "openh264: nasm not found (required on x86/x86_64) — video recording disabled")
    return()
endif()
message(STATUS "openh264: nasm=${NASM_EXECUTABLE}")
get_filename_component(_nasm_dir "${NASM_EXECUTABLE}" DIRECTORY)
if(WIN32)
    set(ENV{PATH} "${_nasm_dir};$ENV{PATH}")
else()
    set(ENV{PATH} "${_nasm_dir}:$ENV{PATH}")
endif()

if(OPENH264_DOWNLOAD_URL STREQUAL "")
    set(_oh_url "https://github.com/cisco/openh264/archive/refs/tags/${OPENH264_VERSION}.tar.gz")
else()
    set(_oh_url "${OPENH264_DOWNLOAD_URL}")
endif()

set(_oh_root "${CMAKE_BINARY_DIR}/_deps/openh264")
set(_oh_src "${_oh_root}/src")
set(_oh_build "${_oh_root}/build")
set(_oh_archive "${_oh_root}/openh264-${OPENH264_VERSION}.tar.gz")
set(_oh_stamp "${_oh_build}/.akhenaten_built")

file(MAKE_DIRECTORY "${_oh_root}")

# --- Download -----------------------------------------------------------------
if(NOT EXISTS "${_oh_src}/meson.build")
    message(STATUS "openh264: downloading ${_oh_url}")
    file(DOWNLOAD "${_oh_url}" "${_oh_archive}"
        SHOW_PROGRESS
        STATUS _oh_dl_status
        TLS_VERIFY ON
    )
    list(GET _oh_dl_status 0 _oh_dl_code)
    if(NOT _oh_dl_code EQUAL 0)
        list(GET _oh_dl_status 1 _oh_dl_msg)
        message(WARNING "openh264: download failed (${_oh_dl_msg}) — video recording disabled")
        return()
    endif()
    file(SIZE "${_oh_archive}" _oh_sz)
    if(NOT _oh_sz GREATER 0)
        message(WARNING "openh264: empty download — video recording disabled")
        file(REMOVE "${_oh_archive}")
        return()
    endif()

    set(_oh_extract "${_oh_root}/extract")
    file(REMOVE_RECURSE "${_oh_extract}")
    file(MAKE_DIRECTORY "${_oh_extract}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf "${_oh_archive}"
        WORKING_DIRECTORY "${_oh_extract}"
        RESULT_VARIABLE _oh_tar_rc
        OUTPUT_QUIET
        ERROR_QUIET
    )
    if(NOT _oh_tar_rc EQUAL 0)
        message(WARNING "openh264: extract failed — video recording disabled")
        return()
    endif()
    file(GLOB _oh_dirs "${_oh_extract}/*")
    list(LENGTH _oh_dirs _oh_dir_count)
    if(NOT _oh_dir_count EQUAL 1)
        message(WARNING "openh264: unexpected archive layout — video recording disabled")
        return()
    endif()
    list(GET _oh_dirs 0 _oh_extracted)
    file(REMOVE_RECURSE "${_oh_src}")
    file(RENAME "${_oh_extracted}" "${_oh_src}")
    file(REMOVE_RECURSE "${_oh_extract}")
endif()

if(NOT EXISTS "${_oh_src}/meson.build")
    message(WARNING "openh264: meson.build missing after extract — video recording disabled")
    return()
endif()

set(OPENH264_INCLUDE_DIR "${_oh_src}/codec/api")

# --- Configure + build (configure-time so IMPORTED lib exists for link) -------
if(NOT EXISTS "${_oh_stamp}")
    file(REMOVE_RECURSE "${_oh_build}")
    file(MAKE_DIRECTORY "${_oh_build}")

    # MSVC + winget ninja has been observed to crash; use the VS backend there.
    if(MSVC)
        set(_oh_backend vs)
    else()
        set(_oh_backend ninja)
    endif()

    set(_oh_meson_args
        setup "${_oh_build}" "${_oh_src}"
        --default-library=static
        -Dtests=disabled
        --buildtype=release
        --backend=${_oh_backend}
    )

    message(STATUS "openh264: meson setup (backend=${_oh_backend})…")
    execute_process(
        COMMAND ${_OH_MESON_CMD} ${_oh_meson_args}
        WORKING_DIRECTORY "${_oh_src}"
        RESULT_VARIABLE _oh_meson_rc
        OUTPUT_VARIABLE _oh_meson_out
        ERROR_VARIABLE _oh_meson_err
    )
    if(NOT _oh_meson_rc EQUAL 0)
        message(WARNING "openh264: meson setup failed — video recording disabled\n${_oh_meson_err}")
        return()
    endif()

    message(STATUS "openh264: meson compile…")
    execute_process(
        COMMAND ${_OH_MESON_CMD} compile -C "${_oh_build}"
        RESULT_VARIABLE _oh_ninja_rc
        OUTPUT_VARIABLE _oh_ninja_out
        ERROR_VARIABLE _oh_ninja_err
    )
    if(NOT _oh_ninja_rc EQUAL 0)
        message(WARNING "openh264: meson compile failed — video recording disabled\n${_oh_ninja_err}")
        return()
    endif()

    file(WRITE "${_oh_stamp}" "ok\n")
endif()

# --- Locate static library ----------------------------------------------------
set(_oh_lib "")
file(GLOB_RECURSE _oh_libs
    "${_oh_build}/libopenh264.a"
    "${_oh_build}/**/libopenh264.a"
    "${_oh_build}/openh264.lib"
    "${_oh_build}/**/openh264.lib"
    "${_oh_build}/libopenh264.lib"
    "${_oh_build}/**/libopenh264.lib"
)

if(_oh_libs)
    list(GET _oh_libs 0 _oh_lib)
endif()

if(NOT _oh_lib OR NOT EXISTS "${_oh_lib}")
    message(WARNING "openh264: static library not found after build — video recording disabled")
    return()
endif()

if(NOT EXISTS "${OPENH264_INCLUDE_DIR}/wels/codec_api.h")
    message(WARNING "openh264: codec_api.h missing — video recording disabled")
    return()
endif()

add_library(openh264 STATIC IMPORTED GLOBAL)
set_target_properties(openh264 PROPERTIES
    IMPORTED_LOCATION "${_oh_lib}"
    INTERFACE_INCLUDE_DIRECTORIES "${OPENH264_INCLUDE_DIR}"
)

if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(openh264 INTERFACE Threads::Threads m)
elseif(APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(openh264 INTERFACE Threads::Threads)
endif()

set(AKHENATEN_HAS_OPENH264 TRUE)
message(STATUS "openh264: ready (${_oh_lib})")
