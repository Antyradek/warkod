# Z jakiegoś powodu opencv4 instaluje się do własnego katalogu
include_directories(/usr/include/opencv4)

# Skompiluj wszystkie pliki .cpp
file(GLOB SOURCES *.cpp)
add_executable(${PROJECT_NAME} ${SOURCES}) 
target_link_libraries(${PROJECT_NAME} opencv_core opencv_imgcodecs)

# Po co komu install(), nikt nie będzie tego instalował
