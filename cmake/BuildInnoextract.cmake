# Download prebuilt innoextract (host tool) from GitHub Releases.
# Not linked into akhenaten — the game only looks for innoextract next to the
# binary / on PATH and spawns it. Override with:
#   -DINNOEXTRACT_VERSION=v1.0.0
#   -DINNOEXTRACT_DOWNLOAD_URL=<full archive url>
#   -DINNOEXTRACT_LOCAL_EXE=<path to existing binary>
# Sibling ../innoextract/build*/**/innoextract[.exe] is used when present (dev).

set(INNOEXTRACT_VERSION "v1.0.0" CACHE STRING
    "innoextract-nb release tag to download")
set(INNOEXTRACT_DOWNLOAD_BASE
    "https://github.com/dalerank/innoextract-nb/releases/download/${INNOEXTRACT_VERSION}"
    CACHE STRING "Base URL for innoextract release archives")
set(INNOEXTRACT_DOWNLOAD_URL "" CACHE STRING
    "Optional full URL to innoextract archive (overrides version/base)")
set(INNOEXTRACT_LOCAL_EXE "" CACHE FILEPATH
    "Optional path to an existing innoextract binary (skips download)")

set(INNOEXTRACT_PREFIX "${CMAKE_BINARY_DIR}/tools/innoextract")
set(INNOEXTRACT_BIN_DIR "${INNOEXTRACT_PREFIX}/bin")
set(INNOEXTRACT_EXE_NAME "innoextract${CMAKE_EXECUTABLE_SUFFIX}")
set(INNOEXTRACT_EXE "${INNOEXTRACT_BIN_DIR}/${INNOEXTRACT_EXE_NAME}")

set(AKHENATEN_HAS_INNOEXTRACT FALSE)

# --- Resolve host archive name ------------------------------------------------
set(_inno_archive "")
if(WIN32)
    set(_inno_archive "innoextract-windows-x64.zip")
elseif(APPLE)
    set(_inno_cpu "${CMAKE_HOST_SYSTEM_PROCESSOR}")
    if(_inno_cpu MATCHES "^(arm64|aarch64)$")
        set(_inno_archive "innoextract-macos-arm64.tar.gz")
    else()
        set(_inno_archive "innoextract-macos-x64.tar.gz")
    endif()
elseif(UNIX)
    set(_inno_archive "innoextract-linux-x64.tar.gz")
endif()

# --- Prefer explicit local binary / sibling build -----------------------------
set(_inno_source_exe "")
if(INNOEXTRACT_LOCAL_EXE AND EXISTS "${INNOEXTRACT_LOCAL_EXE}")
    set(_inno_source_exe "${INNOEXTRACT_LOCAL_EXE}")
    message(STATUS "innoextract: using INNOEXTRACT_LOCAL_EXE=${_inno_source_exe}")
else()
    get_filename_component(_akhenaten_parent "${CMAKE_SOURCE_DIR}" DIRECTORY)
    set(_sibling_root "${_akhenaten_parent}/innoextract")
    if(EXISTS "${_sibling_root}")
        file(GLOB_RECURSE _sibling_bins
            "${_sibling_root}/build*/${INNOEXTRACT_EXE_NAME}"
            "${_sibling_root}/build*/Release/${INNOEXTRACT_EXE_NAME}"
            "${_sibling_root}/build*/RelWithDebInfo/${INNOEXTRACT_EXE_NAME}"
        )
        if(_sibling_bins)
            list(GET _sibling_bins 0 _inno_source_exe)
            message(STATUS "innoextract: using sibling build ${_inno_source_exe}")
        endif()
    endif()
endif()

function(_akhenaten_stage_innoextract _src)
    file(MAKE_DIRECTORY "${INNOEXTRACT_BIN_DIR}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_src}" "${INNOEXTRACT_EXE}"
        RESULT_VARIABLE _copy_rc
    )
    if(NOT _copy_rc EQUAL 0)
        message(WARNING "innoextract: failed to stage ${_src} -> ${INNOEXTRACT_EXE}")
        return()
    endif()
    if(UNIX)
        execute_process(COMMAND chmod +x "${INNOEXTRACT_EXE}")
    endif()
endfunction()

if(_inno_source_exe)
    _akhenaten_stage_innoextract("${_inno_source_exe}")
    if(EXISTS "${INNOEXTRACT_EXE}")
        set(AKHENATEN_HAS_INNOEXTRACT TRUE)
    endif()
elseif(EXISTS "${INNOEXTRACT_EXE}")
    message(STATUS "innoextract: already present at ${INNOEXTRACT_EXE}")
    set(AKHENATEN_HAS_INNOEXTRACT TRUE)
elseif(_inno_archive STREQUAL "")
    message(STATUS "innoextract: no prebuilt archive for this host — helper disabled")
else()
    if(INNOEXTRACT_DOWNLOAD_URL STREQUAL "")
        set(_inno_url "${INNOEXTRACT_DOWNLOAD_BASE}/${_inno_archive}")
    else()
        set(_inno_url "${INNOEXTRACT_DOWNLOAD_URL}")
    endif()

    set(_inno_dl_dir "${INNOEXTRACT_PREFIX}/download")
    set(_inno_archive_path "${_inno_dl_dir}/${_inno_archive}")
    file(MAKE_DIRECTORY "${_inno_dl_dir}")
    file(MAKE_DIRECTORY "${INNOEXTRACT_BIN_DIR}")

    message(STATUS "innoextract: downloading ${_inno_url}")
    file(DOWNLOAD "${_inno_url}" "${_inno_archive_path}"
        STATUS _inno_dl_status
        SHOW_PROGRESS
        TLS_VERIFY ON
    )
    list(GET _inno_dl_status 0 _inno_dl_code)
    list(GET _inno_dl_status 1 _inno_dl_msg)

    if(NOT _inno_dl_code EQUAL 0)
        message(WARNING "innoextract: download failed (${_inno_dl_msg}) — helper disabled")
    else()
        # Extract archive into bin dir (archive contains only the executable).
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xf "${_inno_archive_path}"
            WORKING_DIRECTORY "${INNOEXTRACT_BIN_DIR}"
            RESULT_VARIABLE _inno_extract_rc
            OUTPUT_VARIABLE _inno_extract_out
            ERROR_VARIABLE _inno_extract_err
        )
        if(NOT _inno_extract_rc EQUAL 0)
            message(WARNING
                "innoextract: extract failed (rc=${_inno_extract_rc}): ${_inno_extract_err}")
        elseif(NOT EXISTS "${INNOEXTRACT_EXE}")
            # Some archives may nest one directory — search one level.
            file(GLOB _inno_found "${INNOEXTRACT_BIN_DIR}/*/${INNOEXTRACT_EXE_NAME}")
            if(_inno_found)
                list(GET _inno_found 0 _nested)
                _akhenaten_stage_innoextract("${_nested}")
            endif()
        endif()

        if(EXISTS "${INNOEXTRACT_EXE}")
            if(UNIX)
                execute_process(COMMAND chmod +x "${INNOEXTRACT_EXE}")
            endif()
            set(AKHENATEN_HAS_INNOEXTRACT TRUE)
            message(STATUS "innoextract: ready at ${INNOEXTRACT_EXE}")
        else()
            message(WARNING "innoextract: archive downloaded but ${INNOEXTRACT_EXE_NAME} not found")
        endif()
    endif()
endif()

if(AKHENATEN_HAS_INNOEXTRACT)
    add_custom_target(innoextract
        COMMAND ${CMAKE_COMMAND} -E echo "innoextract: ${INNOEXTRACT_EXE}"
        VERBATIM
    )
else()
    add_custom_target(innoextract
        COMMAND ${CMAKE_COMMAND} -E echo "innoextract helper is not available"
        VERBATIM
    )
endif()

# Copy tool next to the game binary after akhenaten builds (optional; never blocks link).
function(akhenaten_copy_innoextract)
    if(NOT TARGET ${GAME} OR NOT AKHENATEN_HAS_INNOEXTRACT)
        return()
    endif()

    add_custom_command(TARGET ${GAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${INNOEXTRACT_EXE}"
            "$<TARGET_FILE_DIR:${GAME}>/${INNOEXTRACT_EXE_NAME}"
        COMMENT "Copy innoextract next to ${GAME}"
        VERBATIM
    )

    set(_copy_dests "${CMAKE_BINARY_DIR}")
    if(DEFINED GAME_WORKING_DIRECTORY)
        list(APPEND _copy_dests "${GAME_WORKING_DIRECTORY}")
    endif()

    foreach(_dest IN LISTS _copy_dests)
        add_custom_command(TARGET ${GAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_dest}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${INNOEXTRACT_EXE}"
                "${_dest}/${INNOEXTRACT_EXE_NAME}"
            COMMENT "Copy innoextract to ${_dest}"
            VERBATIM
        )
    endforeach()
endfunction()
