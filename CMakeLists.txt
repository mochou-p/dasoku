cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# use `cmake ../ -G "MSYS Makefiles"`  inside of ./Build
# + You won;t need your .vscode/c_cpp_properties.json anymore.... for CMake.... (if using CMake-Tools) 
# [Do remember to Allow CMake-Tools to configure Intellisense]

# Directories in cmake doesn't have to be OS-Specific.... I just like using UNIX-like ones....
# You need double 'backslash' like below, thats why I always use UNIX-Style 'forward' slashes
set(CMAKE_MAKE_PROGRAM "C:/msys64/usr/bin/make.exe" CACHE STRING "Comment...." FORCE)
set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe" CACHE STRING "Comment...." FORCE)    #Equivalent of L20

#-----------------------------------------------------------------------------
# Initialize project.
project(DASOKU LANGUAGES CXX)

# Occationally stuffs like `std=c++17` [cmd-line arg of `g++`] will have equivalent/counter 'CMake thingy' like this....
#  'Internal CMake Variables', these are called....
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)







#-----------------------------------------------------------------------------
# What we actually came to 'MakeFile' or 'CMake' for....   

set(VULKAN_SDK_PATH $ENV{VK_SDK_PATH})

set(INC
    libs/GLFW/include
    libs/GLM
    ${VULKAN_SDK_PATH}/include
    libs/TINYOBJ
    libs/IMGUI
    libs/STB

    ${PROJECT_SOURCE_DIR}/source    # '-I.' at [CFLAGS]  (LINE 17)
)

set(LIB_DIR
    libs/GLFW/lib
    ${VULKAN_SDK_PATH}/lib
)

set(LIBS
    libglfw3.a     #I don't think .lib is needed or not.... as we just pass this to    target_link_libraries()  function....
    vulkan-1.lib   #   Will make sure and let you know 😅   for now I'm gonna just Hardcode the file names
)
if (WIN32)
    list(APPEND LIBS gdi32)
endif()

# 'SRC' is the Var that stores the names
file(GLOB SRC
    # This variable is 'Given' by CMake, it is set to whereever this 'CMakeLists.txt' file will be.... as it as a project() command at top
    ${PROJECT_SOURCE_DIR}/source/*.cpp
)
#message("\nAll the Source:- ")
#foreach(X ${SRC})
#    message("    " ${X})
#endforeach()

file(GLOB IMGUI_SRC
    libs/IMGUI/*.cpp            # 'IMGUI_CPP_FILES' at (LINE 22)
)
#message("\nAll IMGUI Source:- ")
#foreach(X ${IMGUI_SRC})
#    message("    " ${X})
#endforeach()







#-----------------------------------------------------------------------------
#This is how you make a Library.... a.k.a    imgui.lib    for e.g.
add_library(IMGUI ${IMGUI_SRC})
target_link_libraries(IMGUI PRIVATE ${LIBS})    
#
# [PRIVATE   PUBLIC   INTERFACE]       ....just like CPP Classes 😉
#
#   using   'PUBLIC'  we wouldn't need to add '${LIBS}' at line 100
# More on this: https://leimao.github.io/blog/CMake-Public-Private-Interface/
#   He explains in a really Cool way 😄

target_include_directories(IMGUI PRIVATE ${INC})    #IMGUI Needs GLFW & Vulkan too!
target_link_directories(IMGUI PRIVATE ${LIB_DIR})


#-----------------------------------------------------------------------------
# Finally Building our Executable
add_executable(Dasoku ${SRC})
target_include_directories(Dasoku PRIVATE ${INC})
target_link_directories(Dasoku PRIVATE ${LIB_DIR})
target_link_libraries(Dasoku PRIVATE IMGUI ${LIBS})


#-----------------------------------------------------------------------------
# When you run   `cmake --build ./ --target install`
install(TARGETS Dasoku
  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
#  smtimes you might not have a call to    `project()` command, PROJECT_SOURCE_DIR won't be defined (or possibly set to sm other directory....) 
#  in that case we use this new CMAKE_CURRENT_SOURCE_DIR var
