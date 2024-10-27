project(rest_server)

cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# this is heuristically generated, and may not be correct
find_package(cpprestsdk CONFIG REQUIRED)
# this is heuristically generated, and may not be correct
find_package(jwt-cpp CONFIG REQUIRED)

set(SOURCE main.cpp)

add_executable(rest_server main.cpp)
target_link_libraries(rest_server PRIVATE cpprestsdk::cpprest cpprestsdk::cpprestsdk_zlib_internal cpprestsdk::cpprestsdk_boost_internal cpprestsdk::cpprestsdk_openssl_internal)
target_link_libraries(rest_server PRIVATE jwt-cpp::jwt-cpp)

#add_custom_command(TARGET auth_server POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        $<TARGET_FILE:prism_container>
#        $<TARGET_FILE:prism_qt_modular>
#        $<TARGET_FILE:prism_qt_core>
#        $<TARGET_FILE:prism_qt_ui>
#        $<TARGET_FILE:dv_common>
#        $<TARGET_FILE:dv_algorithm>
#        $<TARGET_FILE:dv_camera>
#        #${ZLOG_BINARIES}
#        $<TARGET_FILE_DIR:dv_app>
#        COMMENT "复制依赖项"
#)
## 声明自定义目标，将复制动态库的命令作为依赖
#add_custom_target(copy_dependencies DEPENDS dv_app)



# 设置 app 目标的输出路径
set_target_properties(rest_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${CMAKE_SYSTEM_PROCESSOR}
)


