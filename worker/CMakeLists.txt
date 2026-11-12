
cmake_minimum_required(VERSION 3.20)

project(TPSWorker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(PQXX REQUIRED IMPORTED_TARGET libpqxx)
pkg_check_modules(HIREDIS REQUIRED IMPORTED_TARGET hiredis)

find_library(HPDF_LIBRARY hpdf REQUIRED)

set(WORKER_SOURCES
    src/main.cpp
    src/job.cpp
    src/job-parser.cpp
    src/processor.cpp
    src/threadpool.cpp
    src/redis-client.cpp
    src/postgres-client.cpp
    src/hash-utils.cpp
    src/pdf-generator.cpp
    src/image-resizer.cpp
)

add_executable(worker ${WORKER_SOURCES})

target_include_directories(worker PRIVATE
    include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(worker PRIVATE
    PkgConfig::PQXX
    PkgConfig::HIREDIS
    OpenSSL::Crypto
    ${OpenCV_LIBS}
    ${HPDF_LIBRARY}
    Threads::Threads
)
