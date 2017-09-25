/**
 ******************************************************************************
 * @file    SI7050.h
 * @author  Waldemar Gruenwald
 * @version V1.0.0
 * @date    31 May 2017
 * @brief   This file contains the class of a SI7050 temperature sensor library with I2C interface
 ******************************************************************************
 *
 * Copyright 2017 ubirch GmbH (https://ubirch.com)
 *
 * ```
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ```
 */
 
/**
 *  For more information about the SI7050:
 *    https://www.silabs.com/documents/public/data-sheets/Si7050-1-3-4-5-A20.pdf
 */
 
#ifndef MBED_SI7050_H
#define MBED_SI7050_H

#include "mbed.h"

// sensor commands
#define SI70_ADDRESS    (0x40 << 1)
#define SI70_MEASURE    0xF3    // WG was E3 before// measure temperature, hold master mode
#define SI70_RESET      0xFE    // reset
#define SI70_WRITE_UR   0xE6    // write user register 1
#define SI70_READ_UR    0xE7    // read user register 1
#define SI70_READ_FW_1  0x84    // read the firmware version (first command)
#define SI70_READ_FW_2  0xB8    // read the firmware version (second command)
#define SI70_READ_ID_11 0xFA    // read the electronic ID (first Byte, first command)
#define SI70_READ_ID_12 0x0F    // read the electronic ID (first Byte, second command)
#define SI70_READ_ID_21 0xFC    // read the electronic ID (second Byte, first command)
#define SI70_READ_ID_22 0xC9    // read the electronic ID (second Byte, second command)


// resolution settings
#define SI70_RES_MASK   0x81
#define SI70_RESOLUTION 0x00    // 0x00 = resolution is 14 bit
                                // 0x80 = resolution is 13 bit
                                // 0x01 = resolution is 12 bit
                                // 0x81 = resolution is 11 bit

// error markers
#define ERROR_RESET             (0x0001 << 0) //(1)error during reset
#define ERROR_INIT_WRITE        (0x0001 << 1) //(2)error during init set cmd
#define ERROR_INIT_READ         (0x0001 << 2) //(4)error during init read user register
#define ERROR_INIT_WRITE_BACK   (0x0001 << 3) //(8)error during init write back user register
#define ERROR_MEAS_START        (0x0001 << 4) //(16,10h)error during measurement start 
#define ERROR_MEAS_READ         (0x0001 << 5) //(32,20h)error during measurement read


/**  Interface for controlling SI7050 Sensor
 *
 * @code
 * #include "mbed.h"
 * #include "SI7050.h"
 * 
 * 
 * SI7050 sensor(I2C_SDA, I2C_SCL);
 * 
 * 
 * int main() {
 *     sensor.reset();
 *     sensor.initialize();
 *
 *     while(1) {
 *         printf("Temperature in 0.01°C = %d \r\n", sensor.getTemperature());
 *         wait(1);
 *     }
 * }
 * 
 * @endcode
 */

/** SI7050 class
 *
 *  SI7050: A library to control, measure and calculate the SI7050 temperature sensor device
 *
 */ 
class SI7050
{
public:

    /** Create a SI7050 instance
     *  which is connected to specified I2C pins with specified address
     *
     * @param sda I2C-bus SDA pin
     * @param scl I2C-bus SCL pin
     * @param slave_adr (option) I2C-bus address (default: 0x40)
     */
    SI7050(PinName sda, PinName sck, char slave_adr = (char) SI70_ADDRESS);


    /** Create a SI7050 instance
     *  which is connected to specified I2C pins with specified address
     *
     * @param i2c_obj I2C object (instance)
     * @param slave_adr (option) I2C-bus address (default: 0x40)
     */
    explicit SI7050(I2C &i2c_obj, char slave_adr = (char) SI70_ADDRESS);


    /** Destructor of SI7050
     */
    virtual ~SI7050();


    /** Reset SI7050 sensor
     *
     *  @return         (0) if no error, none (0) if error
     * 
     *  @note       after resetting the sensor, a minimum time of 15 ms has to
     *              be waited, before the sensor will communicate again
     */
    int reset();


    /** Initialize SI7050 sensor
     *
     *  Initialization with 14 bit resolution for temperature measurement
     *
     *  @return         (0) if no error, none (0) if error
     */
    int initialize();


    /** Get the current temperature value from SI7050 sensor
     *
     *  read the measured temperature value from the sensor and 
     *  calculate the temperature value in 0.01°C
     *  
     *  @return         temperature value in 0.01°C resolution, or
     *                  (-32768) if error 
     */
    int getTemperature();


    /** Calculate the Temperature value from the raw sensor data
     *
     *  take the raw sensor data and calculate the temperature
     *  
     *  @param  data    raw sensor temperature data
     *  @return         temperature value in 0.01°C resolution 
     */
    int calcTemperature(const char *data);


    /** Measure and read the current temperature value from SI7050 sensor
     *
     *  read the measured temperature value from the sensor and 
     *  write the raw data into the data array
     *  
     *  @return         (0) if measurement works, or
     *                  (-1) if error 
     */
    int measureTemperature(char *data);


    /** Get the Firmware version of the Sensor
     *
     *
     *  @return         Firmware version:   0xFF = version 1.0
     *                                      0x20 = version 2.0
     *                  (-1) if error
     */
    int getFirmwareVersion();


    /** Get the Electronic ID of the sensor
     *
     *  Read the complete ID of the sensor, which consists of 8 Bytes 
     *  plus CRC Bytes in between. 
     *  Inside the ID, the code for different sensor devices is included
     *  which can be used to determine the sensor type.
     *
     *  @return         Sensor type:    50 = 0x32 = Si7050
     *                                  51 = 0x33 = Si7051
     *                                  52 = 0x34 = Si7052
     *                                  53 = 0x35 = Si7053
     *                                  54 = 0x36 = Si7054
     *                                  55 = 0x37 = Si7055
     *                  (-1) if error
     */
    int getID();


    
private:

    I2C         *i2c_p;
    I2C         &i2c;
    char        address;
    int         ret;
};

#endif // MBED_SI7050_H
