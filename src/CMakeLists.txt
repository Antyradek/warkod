include_directories(/usr/include/opencv4)
add_executable(${PROJECT_NAME} main.cpp pixel.cpp image.cpp) 
target_link_libraries(${PROJECT_NAME} opencv_core opencv_imgcodecs)
