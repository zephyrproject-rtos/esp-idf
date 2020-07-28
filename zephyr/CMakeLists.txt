if(CONFIG_HAS_ESP_IDF)
  zephyr_include_directories(
    ../components/esp_rom/include/${CONFIG_SOC}
    ../components/${CONFIG_SOC}/include
    ../components/esp_common/include
    ../components/soc/soc/include
    ../components/soc/soc/${CONFIG_SOC}/include
    ../components/xtensa/include
    ../components/xtensa/${CONFIG_SOC}/include
    )

  zephyr_link_libraries(
    hal
    -L${CMAKE_CURRENT_SOURCE_DIR}/../components/xtensa/${CONFIG_SOC}
    )
endif()
