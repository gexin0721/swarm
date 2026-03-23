idf_component_register(SRCS "main.cpp" 
                            "task_attitude/task_attitude.cpp"
                            "../Hardware/WIFI/WIFI.cpp"
                            "../Hardware/I2Cdev/I2Cdev.cpp"
                            "../Hardware/MPU6050/MPU6050.cpp"
                            "../Hardware/BMP280/BMP280.cpp"
                            "../Hardware/QMC5883L/QMC5883L.cpp"
                            "../Hardware/TIME/TIME.cpp"
                            "../Hardware/ADC/ADC.cpp"
                            "../System/delay/delay.cpp"
                            "../System/sys/sys.cpp"
                       INCLUDE_DIRS "." "task_attitude" "../Hardware" "../System"
                       REQUIRES mqtt
                                driver
                                esp_driver_gpio
                                esp_driver_gptimer
                                esp_driver_i2c
                                esp_adc
                                nvs_flash
                                esp_wifi
                                esp_event)
