cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME lorawan-node)


project(${PROJECT_NAME})

add_library(${PROJECT_NAME} INTERFACE)

target_compile_definitions(${PROJECT_NAME} INTERFACE -DSOFT_SE)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DREGION_EU868)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DREGION_US915)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DREGION_CN779)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DREGION_EU433)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DREGION_AU915)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DREGION_AS923)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DREGION_CN470)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DREGION_KR920)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DREGION_IN865)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DREGION_RU864)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DACTIVE_REGION=LORAMAC_REGION_US915)

target_include_directories(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/LmHandler
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/LmHandler/packages
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/boards
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/peripherals/soft-se
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/radio
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/radio/sx126x
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/system
)

target_sources(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/CayenneLpp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/LmHandlerMsgDisplay.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/NvmDataMgmt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/LmHandler/LmHandler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/LmHandler/packages/FragDecoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/LmHandler/packages/LmhpClockSync.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/LmHandler/packages/LmhpCompliance.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/LmHandler/packages/LmhpFragmentation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/LmHandler/packages/LmhpRemoteMcastSetup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/NvmDataMgmt.c

    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/boards/mcu/utilities.c

    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/apps/LoRaMac/common/NvmDataMgmt.c

    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/Region.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionAS923.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionAU915.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionBaseUS.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionCN470.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionCN470A20.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionCN470A26.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionCN470B20.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionCN470B26.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionCN779.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionCommon.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionEU433.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionEU868.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionIN865.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionKR920.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionRU864.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/region/RegionUS915.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/LoRaMac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/LoRaMacAdr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/LoRaMacClassB.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/LoRaMacCommands.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/LoRaMacConfirmQueue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/LoRaMacCrypto.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/LoRaMacParser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/mac/LoRaMacSerializer.c

    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/peripherals/soft-se/aes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/peripherals/soft-se/cmac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/peripherals/soft-se/soft-se-hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/peripherals/soft-se/soft-se.c

    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/radio/sx126x/sx126x.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/radio/sx126x/radio.c

    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/system/delay.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/system/gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/system/nvmm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/system/systime.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LoRaMac-node/src/system/timer.c
)

target_link_directories(${PROJECT_NAME} INTERFACE
)

target_link_libraries(${PROJECT_NAME} INTERFACE
)