/**
 ******************************************************************************
 * @file    SI7050.cpp
 * @author  Waldemar Gruenwald
 * @version V1.0.0
 * @date    31 May 2017
 * @brief   SI7050 class implementation
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


#include "mbed.h"
#include "SI7050.h"

SI7050::SI7050(PinName sda, PinName scl, char slave_adr)
        :
        i2c_p(new I2C(sda, scl)),
        i2c(*i2c_p),
        address(slave_adr),
        ret(0) {
    /* nothing to do */
}


SI7050::SI7050(I2C &i2c_obj, char slave_adr)
        :
        i2c_p(NULL),
        i2c(i2c_obj),
        address(slave_adr),
        ret(0) {
    /* nothing to do */
}

SI7050::~SI7050() {
    delete i2c_p;
}

int SI7050::reset() {
    char cmd[1];

    cmd[0] = static_cast<char>(SI70_RESET); // RESET
    ret = i2c.write(address, cmd, 1, false);

    return ret;
}

int SI7050::initialize() {
    char cmd[2];
    char temp;

    cmd[0] = static_cast<char>(SI70_READ_UR); // read user register
    ret = i2c.write(address, cmd, 1, true);
    ret |= i2c.read(address, &cmd[1], 1, false);

    // set the new resolution, without changeing the other bits in the register 
    temp = (char) ((cmd[1] & ~SI70_RES_MASK) | SI70_RESOLUTION);

    cmd[0] = static_cast<char>(SI70_WRITE_UR); // write user register
    cmd[1] = temp;
    ret |= i2c.write(address, cmd, 2, false);

    return ret;
}

int SI7050::measureTemperature(char *data) {
    int ret_ = -1;
    char cmd[1];

    // check the length of the data buffer and if pointer is set correct  
    if (data == NULL) {
        return ret_;
    }

    cmd[0] = static_cast<char>(SI70_MEASURE); // measure temperature
    ret = i2c.write(address, cmd, 1, false); // WG last 0 was a 1
    // WG small delay
    wait_ms(11);

    ret |= i2c.read(address, data, 2, false);

    if (ret) {
        return (ret_);
    }

    return 0;
}

int SI7050::calcTemperature(const char *data) {
    uint32_t temp_raw = 0;
    int32_t temp_c;

    temp_raw = static_cast<uint32_t>((data[0] << 8) | (data[1]));
    temp_c = ((17572 * (temp_raw)) >> 16) - 4685;

    return (int) (temp_c);
}

int SI7050::getTemperature() {
    int ret_ = -32768;
    char data[2];

    ret = measureTemperature(data);
    if (ret) {
        return (ret_);
    }
    ret = calcTemperature(data);

    return (ret);
}

int SI7050::getFirmwareVersion() {
    char cmd[2];
    char data[1];
    int ret_ = -1;


    cmd[0] = static_cast<char>(SI70_READ_FW_1);
    cmd[1] = static_cast<char>(SI70_READ_FW_2);

    ret = i2c.write(address, cmd, 2, false);
    ret |= i2c.read(address, data, 1, false);
    if (ret) {
        return ret_;
    }

    ret_ = (int) (data[0]);

    return ret_;
}

int SI7050::getID() {
    unsigned char serial[16];
    int ret = getSerial(serial);
    if(!ret) return serial[4];
    return ret;
}

int SI7050::getSerial(unsigned char serial[8]) {
    char cmd[4];
    char data[16];

    // first access for the first 4 Bytes
    cmd[0] = static_cast<char>(SI70_READ_ID_11);
    cmd[1] = static_cast<char>(SI70_READ_ID_12);

    ret = i2c.write(address, cmd, 2, true);
    ret |= i2c.read(address, data, 8, false);
    if (ret) return -1;

    serial[0] = (unsigned char) data[0];
    serial[1] = (unsigned char) data[2];
    serial[2] = (unsigned char) data[4];
    serial[3] = (unsigned char) data[6];

    // second access for the last 4 Bytes
    cmd[2] = static_cast<char>(SI70_READ_ID_21);
    cmd[3] = static_cast<char>(SI70_READ_ID_22);

    ret = i2c.write(address, &cmd[2], 2, true);
    ret |= i2c.read(address, &data[8], 8, false);
    if (ret) return -1;

    serial[4] = (unsigned char) data[8];
    serial[5] = (unsigned char) data[9];
    serial[6] = (unsigned char) data[11];
    serial[7] = (unsigned char) data[12];

    return ret;
}





