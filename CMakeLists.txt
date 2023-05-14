cmake_minimum_required(VERSION 3.5)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(1)#起用打包
    set(VERSION_MAJOR    "2")#主要版本
    set(VERSION_MINOR    "3")#次要版本
    set(VERSION_PATCH    "0")#补丁版本
    set(VERSION_REVISION "5")#修订版本
    set(PROJECT_WEBLINK "None")#项目网络链接
    set(PROJECT_DESCRIPTION "PERFControl")#项目说明

    #Linux操作系统生成UUID的方法 cat /proc/sys/kernel/random/uuid
    set(PROJECT_GUID "30464f44-7c5e-445e-b70f-34f182886c36")#APPIDS
    set(PROJECT_ProductName "PERFControl")#产品名称
    set(PROJECT_ProductVersion "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_REVISION}")#产品版本
    set(PROJECT_Copyright "Copyright (C) 2023 sixylc All rights reserved")#版权
    set(PROJECT_LANG_CHARSET "080404B0")  #
    set(PROJECT_LANG_ID "0x0804")  #
    set(PROJECT_CHARSET_ID "1200")  #
    set(PROJECT_LegalTrademarks "(R) sixylc")  # 合法商标
    set(PROJECT_OriginalFilename "PERFControl")  # 原始文件名
    set(PROJECT_ICON "${CMAKE_SOURCE_DIR}/res/project.svg")#图标

    project(${PROJECT_OriginalFilename} VERSION 0.1 LANGUAGES CXX)


    set(CMAKE_BUILD_RPATH "${PROJECT_SOURCE_DIR}/build")#编译文件目录
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BUILD_RPATH}/deploy)#打包目录

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(CMAKE_SYSTEM_PROCESSOR "amd64")
    endif()

    if(CMAKE_CXX_COMPILER_LOADED)#编译信息
        get_filename_component(COMPILER ${CMAKE_CXX_COMPILER} NAME_WLE)
        set(COMPILER_INFO "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}_${COMPILER}${CMAKE_CXX_COMPILER_VERSION}_${CMAKE_BUILD_TYPE}")
    elseif(CMAKE_C_COMPILER_LOADED)
        get_filename_component(COMPILER ${CMAKE_CXX_COMPILER} NAME_WLE)
        set(COMPILER_INFO "$${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}_{COMPILER}${CMAKE_C_COMPILER_VERSION}_${CMAKE_BUILD_TYPE}")
    endif()

    #部署&打包
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")

        set(CONFIG_PATH "${CMAKE_SOURCE_DIR}/config/Linux")
        set(Depends '\n')#依赖包如果没有依赖包则使用'\n'

        set(DEPLOY_TEMPLATE "${CONFIG_PATH}/Deploy.Template")
        set(DEPLOY_SHELL "${CMAKE_BUILD_RPATH}/Deploy.sh")
        configure_file("${DEPLOY_TEMPLATE}" "${DEPLOY_SHELL}")

        set(Pack_DIR "${CMAKE_SOURCE_DIR}/PackDIR")
        set(Pack_TEMP "${CMAKE_BUILD_RPATH}/Pack_TEMP")
        set(Pack_CLI "dpkg -b ${Pack_TEMP} ${Pack_DIR}/Install_${PROJECT_ProductName}_V${PROJECT_ProductVersion}_${COMPILER_INFO}.deb")

        set(PACK_TEMPLATE "${CONFIG_PATH}/Pack_Debian.Template")#选择打包模板
        set(PACK_SHELL "${CMAKE_BUILD_RPATH}/Pack.sh")
        configure_file("${PACK_TEMPLATE}" "${PACK_SHELL}")

        set(PROJECT_SOURCES
            ${PROJECT_SOURCES}
            ${DEPLOY_TEMPLATE}
            ${PACK_TEMPLATE}
        )

    endif()

    if(CMAKE_SYSTEM_NAME MATCHES "Windows")

        set(CONFIG_PATH "${CMAKE_SOURCE_DIR}/config/Windows")

        set(VersionInfo_TEMPLATE "${CONFIG_PATH}/VersionInfo.Template")
        set(VersionInfo_RC "${CMAKE_BUILD_RPATH}/VersionInfo.rc")
        configure_file("${VersionInfo_TEMPLATE}" "${VersionInfo_RC}")

        set(DEPLOY_TEMPLATE "${CONFIG_PATH}/Deploy.Template")
        set(DEPLOY_SHELL "${CMAKE_BUILD_RPATH}/Deploy.bat")
        configure_file("${DEPLOY_TEMPLATE}" "${DEPLOY_SHELL}")

        set(Pack_DIR "${CMAKE_SOURCE_DIR}/PackDIR")
        set(PACK_TEMPLATE "${CONFIG_PATH}/Pack.Template")
        set(PACK_SHELL "${CMAKE_BUILD_RPATH}/Pack.iss")
        configure_file("${PACK_TEMPLATE}" "${PACK_SHELL}")

        set(PROJECT_SOURCES
            ${PROJECT_SOURCES}
            ${VersionInfo_TEMPLATE}
            ${VersionInfo_RC}
            ${DEPLOY_TEMPLATE}
            ${PACK_TEMPLATE}
        )

    endif()


endif()


FILE(GLOB HEADER_LIST  "*.h*")
FILE(GLOB SOURCES_LIST "*.c" "*.cpp" "*.ui")
FILE(GLOB LIBRARY_LIST "lib/*.lib" "lib/*.dll" "lib/*.so")
FILE(GLOB QRC_LIST  "res/*.qrc")
#set(TS_FILES ${PROJECT_OriginalFilename}_zh_CN.ts)
set(PROJECT_SOURCES
        ${PROJECT_SOURCES}
        ${HEADER_LIST}
        ${SOURCES_LIST}
        ${LIBRARY_LIST}
        ${QRC_LIST}
        ${TS_FILES}
)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_OriginalFilename}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${PROJECT_OriginalFilename} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_OriginalFilename} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_OriginalFilename}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(${PROJECT_OriginalFilename} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(${PROJECT_OriginalFilename} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_OriginalFilename}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_OriginalFilename})
endif()
