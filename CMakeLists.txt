cmake_minimum_required(VERSION 3.16)

project(Test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)


qt_add_executable(appTest
    main.cpp
)

qt_add_qml_module(appTest
    URI Test
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES VideoFrameProvider.h VideoFrameProvider.cpp
        SOURCES FfmpegVideoReader.h FfmpegVideoReader.cpp
)

set(FFMPEG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/FFmpeg)

target_include_directories(appTest PUBLIC ${FFMPEG_DIR}/include)
target_link_directories(appTest PUBLIC ${FFMPEG_DIR}/lib)

set(FFMPEG_LIBS
    ${FFMPEG_DIR}/lib/libavformat.a
    ${FFMPEG_DIR}/lib/libavcodec.a
    ${FFMPEG_DIR}/lib/libavutil.a
    ${FFMPEG_DIR}/lib/libswscale.a
    ${FFMPEG_DIR}/lib/libswresample.a
)

target_link_libraries(appTest
    PRIVATE Qt6::Quick
    ${FFMPEG_LIBS}
)
