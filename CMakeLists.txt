cmake_minimum_required(VERSION 3.16)

project(OpenMars)

set(CMAKE_CXX_STANDARD 17)

# ---------- BUILD TYPE ----------
set(PRODUCTION_BUILD OFF CACHE BOOL "Production build")

# ---------- OPTIMIZATIONS ----------
if(PRODUCTION_BUILD)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
endif()

# ---------- MSVC ----------
if(MSVC)
    add_compile_options(/arch:AVX2)

    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>"
    )

    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# ---------- SOURCES ----------
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${MY_SOURCES})

# ---------- INCLUDE ----------
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ---------- RAYLIB ----------
find_package(raylib REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# ---------- DEFINES ----------
target_compile_definitions(${PROJECT_NAME} PUBLIC
    RESOURCES_PATH="src/resources/"
)

if(PRODUCTION_BUILD)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        PRODUCTION_BUILD=1
        DEVELOPLEMT_BUILD=0
    )
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        PRODUCTION_BUILD=0
        DEVELOPLEMT_BUILD=1
    )
endif()

# ---------- COPY RESOURCES ----------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/src/resources"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/src/resources/"
)

# ---------- OPENMP (opcjonalne) ----------
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif()