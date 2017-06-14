/**
 ******************************************************************************
 * @file    SI7050.cpp
 * @author  Waldemar Gruenwald
 * @version V1.0.0
 * @date    31 May 2017
 * @brief   SI7050 class implementation
 ******************************************************************************
 * @attention
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
    ret(0)
{
    /* nothing to do */
}


SI7050::SI7050(I2C &i2c_obj, char slave_adr)
    :
    i2c_p(NULL), 
    i2c(i2c_obj),
    address(slave_adr),
    ret(0)
{
    /* nothing to do */
}

SI7050::~SI7050()
{
    if (NULL != i2c_p)
        delete  i2c_p;
}

int SI7050::reset()
{
    char cmd[1];

    cmd[0] = SI70_RESET; // RESET
    ret = i2c.write(address, cmd, 1, 0);

    return ret;
}

int SI7050::initialize()
{
    char cmd[2];
    char temp;

    cmd[0] = SI70_READ_UR; // read user register
    ret = i2c.write(address, cmd, 1, 1);
    ret |= i2c.read(address, &cmd[1], 1, 0);
    
    // set the new resolution, without changeing the other bits in the register 
    temp = (cmd[1] & ~SI70_RES_MASK) | SI70_RESOLUTION; 
 
    cmd[0] = SI70_WRITE_UR; // write user register
    cmd[1] = temp;
    ret |= i2c.write(address, cmd, 2, 0);

    return ret;
}
 
int SI7050::measureTemperature(char *data)
{
    int ret_ = -1;
    char cmd[1];
    
    // check the length of the data buffer and if pointer is set correct  
    if((sizeof(data)<2) || (data == 0)){
        return (ret_);
    }

    cmd[0] = SI70_MEASURE; // measure temperature
    ret = i2c.write(address, cmd, 1, 0); // WG last 0 was a 1
    // WG small delay
    wait_ms(11);

    ret |= i2c.read(address, data, 2, 0);

    if(ret){
        return (ret_);
    }

    return 0;
}

int SI7050::calcTemperature(const char *data)
{
    uint32_t temp_raw = 0;
    int32_t temp_c;

    temp_raw = (data[0] << 8) | (data[1]); 
    temp_c = ((17572 * (temp_raw))>>16) -4685;

    return (int)(temp_c);
}

int SI7050::getTemperature()
{
    int ret_ = -32768;
    char data[2];

    ret = measureTemperature(data);
    if(ret){
        return (ret_);
    }
    ret = calcTemperature(data);

    return (ret);    
}

int SI7050::getFirmwareVersion()
{
    char cmd[2];
    char data[1];
    int ret_ = -1;


    cmd[0] = SI70_READ_FW_1;
    cmd[1] = SI70_READ_FW_2;

    ret = i2c.write(address, cmd, 2, 0);
    ret |= i2c.read(address, data, 1, 0);
    if(ret){
        return ret_;
    }
    
    ret_ = (int)(data[0]);

    return ret_;
}

int SI7050::getID()
{
    char cmd[4];
    char data[16];
    int ret_ = -1;

    // first access for the first 4 Bytes
    cmd[0] = SI70_READ_ID_11;
    cmd[1] = SI70_READ_ID_12;

    ret = i2c.write(address, cmd, 2, 1);
    ret |= i2c.read(address, data, 8, 0);
    if(ret){
        return ret_;
    }

    // second access for the last 4 Bytes
    cmd[2] = SI70_READ_ID_21;
    cmd[3] = SI70_READ_ID_22;

    ret = i2c.write(address, &cmd[2], 2, 1);
    ret |= i2c.read(address, &data[8], 8, 0);
    if(ret){
        return ret_;
    }
    
    // get the sensor type out of the data
    ret_ = (int)(data[8]);

    return ret_;
}





