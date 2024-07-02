<h2> LoRaWAN-based Weather Data Collection System <h2>

Refer to the Notion [Link](https://periwinkle-traffic-2ca.notion.site/LoRaWAN-based-Weather-Data-Collection-System-0f2b02b3bb934b1d8de3828fd8ccf65a?pvs=4) for detailed documentation of project.

## Introduction

The project involves developing a sensor system to measure environmental temperature, humidity, and sunlight data, uploading this data to The Things Network using a LoRaWAN gateway. Devices will join the network via over-the-air activation (OTAA) and be uniquely identified. To ensure minimal energy consumption, the system will implement low-power strategies, periodically sleeping and waking to transmit data. Operating continuously 24/7, the system will include a watchdog to enhance reliability.

This also includes an open source code (../src/Board/Src/lorawan.c, and ../src/Board/Src/sx1262-board.c )


The [Porting Guide](https://stackforce.github.io/LoRaMac-doc/LoRaMac-doc-v4.7.0/_p_o_r_t_i_n_g__g_u_i_d_e.html) used in the implementation of the lora node stack.


## Technical Specification
The technical specifications are below:
    - MCU - stm32f401ccu6
    - Temperature and Humidity: DHT 11
    - LoRa module: SX1262 LoRa Node Module
    - Photo Sensor: TEMT6000

Refer to (../src/Board/Inc/board-config.h) for pin to sensor connection.

##  Firmware Architecture and Design
![file](images/design.png)

##  Get Started
- Clone the public repository

```bash
$ git clone https://github.com/mrdean05/LoRaWAN-based-Data-Collector.git
```

- Initialize submodules contained in the project
```bash
$ cd LoRaWAN-based-Data-Collector
$ git submodule update --init
```

- Follow the [link](https://www.waveshare.com/wiki/SX1302_LoRaWAN_Gateway_HAT) on how to set up a lorawan gateway and set up your credentials on the things network. The necessary credentials for OTAA (Over-The-Air-Activation) include Device EUI, APP EUI  and APP Key. 

- Include the credentials in (`../src/Core/Inc/config.h`)
- Refer to (`../src/Board/Inc/board-config.h`) for pin to sensor connection.

- Build the project to generate the executables.
```bash
$ cmake .
$ make
```
- Run the executable file
```bash 
$ cd build/
```

## Documents
- SX1262 module datasheet: [Link](https://www.mouser.com/datasheet/2/761/DS_SX1261-2_V1.1-1307803.pdf)
