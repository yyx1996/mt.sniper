### INCLUDE GUARD BEGIN ###
if(__offline_EDM_macro)
    return()
endif()
set(__offline_EDM_macro true)
### INCLUDE GUARD END ###

include(CMakeParseArguments)
include(XOD)
include(RootDict)
#############################################################################
# Build Event Data Model 
# + HDRDIR: Header
# + FILES
#       Generate Dict
# + SRCS
#       .cc files (not generated by XOD, assume in the src directories)
# + HDRS
#       .h files only
# Options
# - BASE: this is BaseEvent
# - NOXOD: don't use XOD to generate files
#############################################################################
function(EDM _package)

    ##########################################################################
    # = Prelude =
    ##########################################################################

    ##########################################################################
    # == the targets will be installed ==
    ##########################################################################
    # 
    # How it works?
    # - decalre several lists for shlib targets 
    #   and include directories
    # - add the target/directory into one of the list
    # - create global property
    #   - ${_package}InstallShlibTargets
    #   - ${_package}InstallIncludeDirs
    #     - the interface library should be also belong to this.
    # 
    set(_package_install_shlib_targets)
    set(_package_install_include_dirs)

    ##########################################################################
    # == cache package name ==
    ##########################################################################
    set_property(GLOBAL
        APPEND
        PROPERTY OFFLINE_PACKAGES "${_package}"
    )
    set_property(GLOBAL
        PROPERTY "${_package}_root" "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    ##########################################################################
    # == parse arguments ==
    ##########################################################################
    CMAKE_PARSE_ARGUMENTS(ARG "BASE;ISEDMUTILS;NOXOD" 
                              "HDRDIR" 
                              "FILES;HDRS;SRCS;DEPENDS" 
                              "" ${ARGN})

    set(_other_srcs "${ARG_UNPARSED_ARGUMENTS}") # will not be used to generate dict
    if (_other_srcs)
        message(STATUS "${_package} will use addition files: ${_other_srcs}")
    endif()
    
    set(_files "${ARG_FILES}")
    set(_srcs "${ARG_SRCS}")
    set(_hdr_dir "Event")
    
    if (ARG_HDRDIR)
        set(_hdr_dir "${ARG_HDRDIR}")
    endif(ARG_HDRDIR)
    
    FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Event 
                        ${CMAKE_CURRENT_BINARY_DIR}/src
                        ${CMAKE_CURRENT_BINARY_DIR}/xml
    )
    
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    ##########################################################################
    # Declare the dependents
    ##########################################################################
    set(${_package}_depends_orig)
    set(${_package}_depends)
    set(${_package}_depends_virtual)
    if (NOT ARG_BASE)
        set(${_package}_depends_orig EDMUtil BaseEvent ${ARG_DEPENDS})
    else()
        set(${_package}_depends_orig ${ARG_DEPENDS})
    endif()


    foreach(_f ${${_package}_depends_orig})
      if (TARGET ${_f})
        list(APPEND ${_package}_depends ${_f})
      elseif(TARGET offline::${_f}) # the namespace offline::
        list(APPEND ${_package}_depends offline::${_f})
        
        # This is used by the RootDict
        get_target_property(hdrs offline::${_f} INTERFACE_INCLUDE_DIRECTORIES)
        foreach(d ${hdrs})
          include_directories(${d})
        endforeach()
      else()
        list(APPEND ${_package}_depends ${_f})
      endif()
    endforeach()

    ##########################################################################
    # Here, we assume the XOD and NOXOD are mutual exclusion
    ##########################################################################
    if (ARG_NOXOD)
        message("EDM ${_package} will not use XOD")
        # if don't use XOD, append _files to SRCS
    
        foreach (_f ${_files})
            set(_index -1)
            if (_srcs)
                list (FIND _srcs "${_f}" _index)
            endif(_srcs)
            if (${_index} GREATER -1)
    
            else()
                list (APPEND _srcs "${_f}")
            endif()
    
        endforeach()
    else()
        # generate source code
        xod_gen_lists(${_package}
            FILES
            ${_files}
            SRCS
            ${_srcs}
        )
    
    endif()
    
    ##########################################################################
    # COPY HEADERS (Event or ${package})
    ##########################################################################
    
    foreach (_f ${_files} ${ARG_HDRS})

        set(_header)
        set(_header_dir)

        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_hdr_dir}/${_f}.h")
            set(_header "${CMAKE_CURRENT_SOURCE_DIR}/${_hdr_dir}/${_f}.h")
            set(_header_dir "${_hdr_dir}")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_package}/${_f}.h")
            set(_header "${CMAKE_CURRENT_SOURCE_DIR}/${_package}/${_f}.h")
            set(_header_dir "${_package}")
        elseif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${_hdr_dir}/${_f}.h")
            set(_header "${CMAKE_CURRENT_BINARY_DIR}/${_hdr_dir}/${_f}.h")
            set(_header_dir "${_hdr_dir}")
        else()
            continue()
        endif()

        if (${_header} IS_NEWER_THAN ${CMAKE_BINARY_DIR}/include/${_header_dir}/${_f}.h)
            file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/${_header_dir})
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            ${_header}
                            ${CMAKE_BINARY_DIR}/include/${_header_dir}/${_f}.h
                COMMAND_ECHO STDERR
            )
        endif()

        # because the header dir is not always Event, we handle the install here
        install(FILES ${_header}
                DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${_header_dir}")
    endforeach()

    ##########################################################################
    # = Generate Dictionaries (from FILES) =
    ##########################################################################
    #message("${list_xod_src_${_package}}")


    foreach (_f ${_files})
        set (_linkdef)
        set (_header)
        if (ARG_NOXOD)
            # LinkDef.h
            if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${_f}LinkDef.h")
                set(_linkdef "${CMAKE_CURRENT_SOURCE_DIR}/src/${_f}LinkDef.h")
            endif()

            # header (Event, EDMUtil or include)
            if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_hdr_dir}/${_f}.h")
                set(_header "${CMAKE_CURRENT_SOURCE_DIR}/${_hdr_dir}/${_f}.h")
            elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_package}/${_f}.h")
                set(_header "${CMAKE_CURRENT_SOURCE_DIR}/${_package}/${_f}.h")
            elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/${_f}.h")
                set(_header "${CMAKE_CURRENT_SOURCE_DIR}/include/${_f}.h")
            elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${_f}.h")
                set(_header "${CMAKE_CURRENT_SOURCE_DIR}/src/${_f}.h")
            else()
                message(FATAL_ERROR "Can't find the path of header file ${_f}.h")
            endif()
        else()
            set(_linkdef "${CMAKE_CURRENT_BINARY_DIR}/src/${_f}LinkDef.h")
            set(_header "${CMAKE_CURRENT_BINARY_DIR}/${_hdr_dir}/${_f}.h")
        endif()

        message("------> _header: ${_header}")
        message("------> _package: ${_package}")
        message("------> _linkdef: ${_linkdef}")


        ROOT_GENERATE_DICTIONARY(${CMAKE_CURRENT_BINARY_DIR}/src/${_f}Dict
            ${_header}
            LIBRARY ${_package}
            LINKDEF ${_linkdef}
            DEPENDS ${_header} ${_linkdef}
        )
    endforeach()
    
    ##########################################################################
    # == Handle the src/rootmap/pcm from the Generation of Dictionaries ==
    ##########################################################################
    set (src_${_package}_list)
    set (rootmap_${_package}_list) # store the individual rootmaps
    set (pcm_${_package}_list)     # store the individual pcm
    foreach (_f ${_files})
        ######################################################################
        # === src ===
        ######################################################################
        set(_index -1)
        if (_srcs)
            list (FIND _srcs "${_f}" _index)
        endif(_srcs)
        #message("index: " "${_index}")
        if (${_index} GREATER -1)
            if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${_f}.cc")
                list(APPEND src_${_package}_list "${CMAKE_CURRENT_SOURCE_DIR}/src/${_f}.cc")
            endif()
        elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${_f}.cc")
            list(APPEND src_${_package}_list "${CMAKE_CURRENT_SOURCE_DIR}/src/${_f}.cc")
        else()
            if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/src/${_f}.cc")
                list(APPEND src_${_package}_list "${CMAKE_CURRENT_BINARY_DIR}/src/${_f}.cc")
            endif()
        endif()
        list(APPEND src_${_package}_list "${CMAKE_CURRENT_BINARY_DIR}/src/${_f}Dict.cxx")
        ######################################################################
        # === rootmap ===
        ######################################################################
        list(APPEND rootmap_${package}_list "${CMAKE_CURRENT_BINARY_DIR}/src/${_f}Dict.rootmap")
    
        ######################################################################
        # === PCM ===
        ######################################################################
        list(APPEND pcm_${package}_list "${CMAKE_CURRENT_BINARY_DIR}/src/${_f}Dict_rdict.pcm")
    endforeach(_f)
    
    
    #message("${_package} SRC: " "${src_${_package}_list}")
    
    ##########################################################################
    # = Create the library and set the dependencies =
    ##########################################################################
    add_library(${_package} SHARED 
        ${src_${_package}_list}
        ${_other_srcs}
    )
    
    # ============================================================================
    # ---- copy a merged rootmap/pcm under lib (BEGIN) ---------------------------
    # ============================================================================
    
    add_custom_command(TARGET ${_package}
                       POST_BUILD
                       COMMAND cat ${rootmap_${package}_list} > ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${_package}.rootmap
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${pcm_${package}_list} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                       VERBATIM
    )
    
    # ============================================================================
    # ---- copy a merged rootmap/pcm under lib (END) -----------------------------
    # ============================================================================
    
    
    ##########################################################################
    # Force other EDMs depends on BaseEvent and EDMUtil
    ##########################################################################

    foreach (_f ${${_package}_depends})
        if (TARGET ${_f})
            add_dependencies(${_package} ${_f})
        endif()
    endforeach()
    target_link_libraries(${_package}
        ${${_package}_depends}
        ${ROOT_LIBRARIES}
        # ${ARG_DEPENDS}
    )
    target_include_directories(${_package}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PUBLIC
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_FULL_INCLUDEDIR}>
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src>
    )

    ##########################################################################
    # == at final, export these install targets/dirs ==
    ##########################################################################

    # === targets ===
    install(TARGETS ${_package}
            EXPORT ${PROJECT_NAME}Targets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib)
    # === headers ===

    # === rootmap, dict etc. ===
    # if (EXISTS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${_package}.rootmap)
    # endif()
    if (pcm_${package}_list)
        install(FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${_package}.rootmap
                DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib)
        install(FILES ${pcm_${package}_list}
                DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib)
    endif()


endfunction(EDM)
