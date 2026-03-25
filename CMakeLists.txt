cmake_minimum_required(VERSION 3.19)
project(SecureData_Suite LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(SecureData_Suite
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources.qrc
)

qt_add_translations(
    TARGETS SecureData_Suite
    TS_FILES SecureData_Suite_ru_BY.ts
)

target_link_libraries(SecureData_Suite
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS SecureData_Suite
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SecureData_Suite
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
