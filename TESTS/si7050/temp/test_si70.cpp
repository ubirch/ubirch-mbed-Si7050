/*
 * SI7050 Sensor library tests.
 *
 * @author Waldemar Grünwald
 * @date 2017-06-02
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
#include "SI7050.h"

#include "utest/utest.h"
#include "unity/unity.h"
#include "greentea-client/test_env.h"

#if defined(TARGET_NRF52_DK)
#define SI7050_SDA I2C_SDA0
#define SI7050_SCL I2C_SCL0
#else
#define SI7050_SDA I2C_SDA
#define SI7050_SCL I2C_SCL
#endif

using namespace utest::v1;

SI7050 sensor(SI7050_SDA, SI7050_SCL);

void TestSi_reset() {
    int ret;
    ret = sensor.reset();
    TEST_ASSERT_UNLESS_MESSAGE(ret != 0, "failed to reset the sensor");
}

void TestSi_initialize() {
    int ret;
    ret = sensor.initialize();
    TEST_ASSERT_UNLESS_MESSAGE(ret != 0, "failed to initialize the sensor");
}

void TestSi_getTemperature() {
    int ret;
    ret = sensor.getTemperature();
    TEST_ASSERT_UNLESS_MESSAGE(ret == (-1), "failed to measure the temperature");
}

void TestSi_calcTemperature() {
    int ret;
    char data[2];
    data[0] = 100;
    data[1] = static_cast<char>(255);
    ret = sensor.calcTemperature(data);
    TEST_ASSERT_UNLESS_MESSAGE(ret != 2247, "failed to calculate the temperature");
}

// test if the measurement fails if the data argument is not 2 byte
void TestSi_measureFailWrongArgument() {
    int ret;
    char *data0 = NULL;
    char data2[2];
    char data3[3];
    char data5[5];

    // first measurement
    ret = sensor.measureTemperature(data0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, ret, "wrongly measured with zero byte storage");
    ret = sensor.measureTemperature(data2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "two byte storage should not fail");
    ret = sensor.measureTemperature(data3);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "larger storage (3) should not fail");
    ret = sensor.measureTemperature(data5);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "larger storage (5) should not fail");
}

// test if two consequent measurements are inside a range of +-2°C
void TestSi_measureTemperature() {
    int ret;
    char data[2];
    uint16_t temp_raw1, temp_raw2;
    uint16_t delta_2 = 746; // this is a raw temperature delta of 2°C
    uint16_t delta_real;

    // first measurement
    ret = sensor.measureTemperature(data);
    TEST_ASSERT_UNLESS_MESSAGE(ret != 0, "failed to measure the temperature 1");
    temp_raw1 = (((uint16_t) (data[0]) << 8) + (uint16_t) (data[1]));

    //wait_ms(5);

    // second measurement
    ret = sensor.measureTemperature(data);
    TEST_ASSERT_UNLESS_MESSAGE(ret != 0, "failed to measure the temperature 2");
    temp_raw2 = (((uint16_t) (data[0]) << 8) + (uint16_t) (data[1]));

    // calculate the difference between the two measurements
    if (temp_raw1 >= temp_raw2) {
        delta_real = temp_raw1 - temp_raw2;
    } else {
        delta_real = temp_raw2 - temp_raw1;
    }
    TEST_ASSERT_UNLESS_MESSAGE(delta_real > delta_2, "temperature difference bigger then +-2°C");
}

void TestSi_getFirmwareVersion() {
    int ret;
    ret = sensor.getFirmwareVersion();
    TEST_ASSERT_UNLESS_MESSAGE(ret == (-1), "failed to get the Firmware version");
}

void TestSi_getID() {
    int ret;
    ret = sensor.getID();
    TEST_ASSERT_UNLESS_MESSAGE(ret == (-1), "failed to get the ID");
}

void TestSi_calculationRange() {
    int ret;
    char data[2];
    int delta = 8786;
    int expect = 4100;

    // go through the complete range of values
    for (int i = 0; i < 256; i++) {
        data[0] = static_cast<char>(i);
        for (int j = 0; j < 256; j++) {
            data[1] = static_cast<char>(j);
            ret = sensor.calcTemperature(data);  // from -4685 to 12886 centre = 4100 delta = 8786
            TEST_ASSERT_INT_WITHIN(delta, expect, ret);
        }
    }
}


utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;
}

Case cases[] = {
Case("SI7050 fail with wrong storage arguments-0", TestSi_measureFailWrongArgument, greentea_failure_handler),
Case("SI7050 reset-0", TestSi_reset, greentea_failure_handler),
Case("SI7050 initialize-0", TestSi_initialize, greentea_failure_handler),
Case("SI7050 get temperature-0", TestSi_getTemperature, greentea_failure_handler),
Case("SI7050 calculate temperature-0", TestSi_calcTemperature, greentea_failure_handler),
Case("SI7050 measure temperature-0", TestSi_measureTemperature, greentea_failure_handler),
Case("SI7050 get firmware version-0", TestSi_getFirmwareVersion, greentea_failure_handler),
Case("SI7050 get ID-0", TestSi_getID, greentea_failure_handler),
Case("SI7050 check calculation range min to max-0", TestSi_calculationRange, greentea_failure_handler),
};

utest::v1::status_t greentea_test_setup(const size_t number_of_cases) {
    GREENTEA_SETUP(150, "default_auto");
    return greentea_test_setup_handler(number_of_cases);
}


int main() {
    wait_ms(100);

    Specification specification(greentea_test_setup, cases, greentea_test_teardown_handler);
    Harness::run(specification);
} 