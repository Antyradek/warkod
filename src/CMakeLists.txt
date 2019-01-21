# Z jakiegoś powodu opencv4 instaluje się do własnego katalogu
include_directories(/usr/include/opencv4)

# Skompiluj wszystkie pliki .cpp
file(GLOB SOURCES *.cpp)
add_executable(${PROJECT_NAME} ${SOURCES}) 
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
target_link_libraries(${PROJECT_NAME} opencv_core opencv_imgcodecs pthread)


# Po co komu install(), nikt nie będzie tego instalował
