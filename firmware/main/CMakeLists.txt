idf_component_register(
    SRCS "main.c" 
         "src/tasks/task_contactors.c"
         "src/hardware/bms_hardware.c"
         "src/hardware/bms_spi.c"
         "src/tasks/task_simulator.c"
         "src/tasks/task_safety.c"
         "src/tasks/task_termal.c"
         "src/tasks/task_soc.c"
         "src/tasks/task_balancing.c"
         "src/tasks/task_can.c"
    INCLUDE_DIRS "." "src" "src/hardware" "src/tasks"
    REQUIRES driver
    PRIV_REQUIRES esp_driver_gpio
                  esp_driver_spi
                  esp_driver_ledc
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=cpp)