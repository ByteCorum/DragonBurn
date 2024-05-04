#______                            ______                  
#|  _  \                           | ___ \                 
#| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
#| | | | '__/ _` |/ _` |/ _ \| '_ \| ___ \ | | | '__| '_ \ 
#| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
#|___/ |_|  \__,_|\__, |\___/|_| |_\____/ \__,_|_|  |_| |_|
#                  __/ |                                   
#                 |___/             
#                        
#https://github.com/ByteCorum/DragonBurn

# Minimum required CMake version
cmake_minimum_required(VERSION 3.0)

# Project name
project(DragonBurn)

# Add an executable from source files
add_executable(DragonBurn
    DragonBurn/main.cpp
    #DragonBurn/other_file.cpp
    # Add more source files here
)

# Specify C++ standard (optional)
set(CMAKE_CXX_STANDARD 17)

# Additional configuration (e.g., include directories, libraries, etc.)
# ...

# Install rules (optional)
install(TARGETS DragonBurn DESTINATION bin)

