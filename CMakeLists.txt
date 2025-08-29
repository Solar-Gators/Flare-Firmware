# Set this one as the cmakelists in vscode for intellisense in all subdirectories at the same time (hopefully)

cmake_minimum_required(VERSION 3.15)
project(SolarGatorsSTM32Projects)

add_subdirectory(firmware/NUCLEOL476RG-FreeRTOS-ExampleProject)
add_subdirectory(firmware/CAN-DevBoard)

# Add more as needed
