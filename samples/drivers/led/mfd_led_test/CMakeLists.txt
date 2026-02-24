cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(mfd_led_test)

# Use wifi_test.c if WiFi is enabled, otherwise use main.c
if(CONFIG_WIFI)
  target_sources(app PRIVATE src/wifi_test.c)
else()
  target_sources(app PRIVATE src/main.c)
endif()
