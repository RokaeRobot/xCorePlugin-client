# Add shared/static libraries
add_library(${PROJECT_NAME}_lib SHARED IMPORTED GLOBAL)
add_library(${PROJECT_NAME}_static STATIC IMPORTED GLOBAL)
add_library(xmatemodel_lib STATIC IMPORTED)

if(WIN32)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8) #64bit
        set(_ARCH_NAME "64bit")
    elseif(${CMAKE_SIZEOF_VOID_P} EQUAL 4) #32bit
        set(_ARCH_NAME "32bit")
    endif()

    set_target_properties(xmatemodel_lib PROPERTIES
        IMPORTED_LOCATION_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Windows/Release/64bit/xMateModel.lib
        IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Windows/Debug/64bit/xMateModeld.lib)
    set_target_properties(${PROJECT_NAME}_lib PROPERTIES
        IMPORTED_IMPLIB_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Windows/Release/${_ARCH_NAME}/${PROJECT_NAME}.lib
        IMPORTED_LOCATION_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Windows/Release/${_ARCH_NAME}/${PROJECT_NAME}.dll
        IMPORTED_IMPLIB_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Windows/Debug/${_ARCH_NAME}/${PROJECT_NAME}.lib
        IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Windows/Debug/${_ARCH_NAME}/${PROJECT_NAME}.dll
        )
    set_target_properties(${PROJECT_NAME}_static PROPERTIES
        IMPORTED_LOCATION_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Windows/Release/${_ARCH_NAME}/${PROJECT_NAME}_static.lib
        IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Windows/Debug/${_ARCH_NAME}/${PROJECT_NAME}_static.lib
        INTERFACE_LINK_LIBRARIES xmatemodel_lib
        )
elseif(UNIX)
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set_target_properties(${PROJECT_NAME}_lib PROPERTIES
                INTERFACE_LINK_LIBRARIES xmatemodel_lib)
    elseif((NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64") OR ${XCORE_USE_XMATE_MODEL})
        message(FATAL_ERROR "Unsupported: target ${CMAKE_SYSTEM_PROCESSOR} OR XCORE_USE_XMATE_MODEL option")
    endif()

    # set path to model library
    set_target_properties(xmatemodel_lib PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/Linux/x86_64/libxMateModel.a)

    set(XCORESDK_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Linux/${CMAKE_SYSTEM_PROCESSOR})
    set_target_properties(${PROJECT_NAME}_lib PROPERTIES
        IMPORTED_LOCATION ${XCORESDK_LIBRARY_DIR}/lib${PROJECT_NAME}.so.${CMAKE_PROJECT_VERSION}
        )
    set_target_properties(${PROJECT_NAME}_static PROPERTIES
        IMPORTED_LOCATION ${XCORESDK_LIBRARY_DIR}/lib${PROJECT_NAME}.a
        INTERFACE_LINK_LIBRARIES xmatemodel_lib
        )
endif()

if(XCORE_LINK_SHARED_LIBS)
    add_library(Rokae::Rokae ALIAS ${CMAKE_PROJECT_NAME}_lib)
else()
    add_library(Rokae::Rokae ALIAS ${CMAKE_PROJECT_NAME}_static)
endif()

# install runtime artifacts
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
    install(IMPORTED_RUNTIME_ARTIFACTS ${PROJECT_NAME}_lib
        RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR})
else()
    string(TOUPPER ${CMAKE_BUILD_TYPE} build_type_upper)
    get_target_property(runtime_dll_location ${PROJECT_NAME}_lib IMPORTED_LOCATION_${build_type_upper})
    if(${runtime_dll_location} MATCHES "NOTFOUND$")
        get_target_property(runtime_dll_location ${PROJECT_NAME}_lib IMPORTED_LOCATION)
    endif()
    if(NOT ${runtime_dll_location} MATCHES "NOTFOUND$")
        install(FILES ${runtime_dll_location} DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endif()
