cmake_minimum_required(VERSION 3.10)
project(AF_Lib)

# Set the C standard
set(CMAKE_C_STANDARD 99)



# Add common source files
file(GLOB_RECURSE COMMON_SOURCE_FILES 
    src/common/*.c
)

# Add main.c file
set(MAIN_SOURCE src/main.c)



# Platform-specific source files
if (APPLE)
    message(STATUS "Configuring for macOS")
    enable_language(OBJC)
    set(CMAKE_OBJC_COMPILER "/usr/bin/clang")
    #file(GLOB_RECURSE PLATFORM_SOURCE_FILES src/osx/*.c src/osx/*.m)
    file(GLOB_RECURSE PLATFORM_SOURCE_FILES src/osx/*.m)

    # Specific files to be treated as Objective-C
    set_source_files_properties(${PLATFORM_SOURCE_FILES} PROPERTIES LANGUAGE OBJC)
    
elseif (WIN32)
    message(STATUS "Configuring for windows")
    file(GLOB_RECURSE PLATFORM_SOURCE_FILES src/windows/*.c)
else()
    message(STATUS "Configuring for linux")
    file(GLOB_RECURSE PLATFORM_SOURCE_FILES src/linux/*.c)
endif()

# Combine all source files
set(SOURCE_FILES ${COMMON_SOURCE_FILES} ${PLATFORM_SOURCE_FILES} ${MAIN_SOURCE})


# Explicitly list ImGui source files
list(APPEND SOURCE_FILES
    # ${CMAKE_SOURCE_DIR}/../../SDK/imgui/imgui.cpp
   
)

# Add include directories
if(WIN32)
    include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/../SDK/stb

        # Add other include directories as needed
    )
elseif(APPLE)
    include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/../AF_Math/include
        ${CMAKE_SOURCE_DIR}/../../SDK/glfw/include
        ${CMAKE_SOURCE_DIR}/../SDK/glew-2.1.0/include
	${CMAKE_SOURCE_DIR}/../SDK/stb
        /usr/local/include
        
    )
endif()


# Add library directories
if(WIN32)
    link_directories(
       
    )
elseif(APPLE)
    link_directories(
        # Add other library directories as needed
        ${CMAKE_SOURCE_DIR}/../../SDK/glfw/lib-x86_64
        /usr/local/lib
        /usr/local/Cellar/glew/2.2.0_1/lib
        #${CMAKE_SOURCE_DIR}/../../SDK/vulkan/macOS/lib
    )
endif()



# Define the executable or library target
add_executable(${PROJECT_NAME} ${SOURCE_FILES})




# Link against libraries
if(WIN32)
    target_link_libraries(AF_Lib
    PRIVATE
        
        # Add other libraries as needed
    )
elseif(APPLE)
    # Link against frameworks
    #find_package(glfw3 3.3 REQUIRED)
    target_link_libraries(AF_Lib
    PRIVATE
        #"-framework Cocoa"
        "-framework Foundation"
        "-framework Cocoa"
        #"-framework OpenGL"
        #SDL2main
        #SDL2
        #SDL2_ttf
        #SDL2_image
        #opengl32
        #glu32
        #glew32
        GLEW
        #freetype
        #ssp
        #libMoltenVK.dylib
        #libvulkan.1.3.275.dylib
        #libvulkan.1.dylib
        
        glfw
        # Add other libraries as needed
    )
    find_package(glfw3 3.3 REQUIRED)
endif()

# Set environment variables
if (APPLE)
    #set(ENV{VK_ICD_FILENAMES} "vulkansdk/macOS/share/vulkan/icd.d/MoltenVK_icd.json")
    #set(ENV{VK_LAYER_PATH} "vulkansdk/macOS/share/vulkan/explicit_layer.d")
endif()


# Set compiler flags
target_compile_options(AF_Lib PRIVATE
    #-mwindows
    #-time
    
    -Wall
    -Wextra
    -Werror
    -pedantic
    -Wformat-security
    -Wnull-dereference
    -Wshadow
    -fstack-protector-strong
    -fno-omit-frame-pointer
    -g
    
    #-D_FORTIFY_SOURCE=2
    #-fsanitize=address
    #-O3
    -v
    #-shared
    #-o
)

target_link_options(${PROJECT_NAME} PRIVATE
    #-fsanitize=address
)


# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    OUTPUT_NAME ${PROJECT_NAME}
    PREFIX ""
)


# Copy the DLL to the "bin" directory after building
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/bin
#)

#
