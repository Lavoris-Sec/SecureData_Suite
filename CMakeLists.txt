add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
cmake_minimum_required(VERSION 3.19)
project(SecureData_Suite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Gui LinguistTools)

qt_standard_project_setup()

qt_add_executable(SecureData_Suite
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/ui/MainWindow.cpp
    include/ui/MainWindow.h
    src/ui/MainWindow.ui
    src/crypto/cipher_base.cpp
    src/crypto/cipher_manager.cpp
    src/crypto/caesar_cipher.cpp
    src/crypto/vigenere_cipher.cpp
    src/crypto/atbash_cipher.cpp
    src/crypto/xor_cipher.cpp
    src/crypto/permutation_cipher.cpp
    src/crypto/simple_aes_cipher.cpp
    src/crypto/huffman_codec.cpp
    src/crypto/rsa_cipher.cpp
    src/stego/lsb_steganography.cpp
    src/utils/file_cache.cpp
    src/utils/translation_manager.cpp
    include/utils/translation_manager.h
    resources/resources.qrc
)


qt_add_translations(
    TARGETS SecureData_Suite
    TS_FILES resources/translations/SecureData_Suite_ru_BY.ts
)

target_include_directories(SecureData_Suite
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SecureData_Suite
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Gui
)

if (MSVC)
    target_compile_options(SecureData_Suite PRIVATE /utf-8)
endif()

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
