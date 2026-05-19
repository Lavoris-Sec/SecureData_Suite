include("D:/Education/Coursework/SecureData_Suite/build/Desktop_Qt_6_10_2_MSVC2022_64bit-u041eu0442u043bu0430u0434u043au0430/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/SecureData_Suite-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "D:/Education/Coursework/SecureData_Suite/build/Desktop_Qt_6_10_2_MSVC2022_64bit-u041eu0442u043bu0430u0434u043au0430/SecureData_Suite.exe"
    GENERATE_QT_CONF
)
