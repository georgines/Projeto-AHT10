add_library(Aht10 STATIC
    Aht10.cpp
)

target_include_directories(Aht10 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(Aht10 PUBLIC
    pico_stdlib
    hardware_i2c
    hardware_gpio
)
