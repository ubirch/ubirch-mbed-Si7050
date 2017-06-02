
#include "SI7050.h"

#include "utest/utest.h"
#include "unity/unity.h"
#include "greentea-client/test_env.h"

#include "config.h"



#define STACK_SIZE 24000

using namespace utest::v1;

// the interrupt pin is needed to setup the gpio for triggering an interrupt
InterruptIn wakeup(PTA4);
DigitalOut modem_en(PTD7);
DigitalOut rf(PTA18);
DigitalOut ext_3v3(PTC8);


SI7050 sensor(I2C_SDA, I2C_SCL);

void TestSi_reset(){
    int ret;
    ret = sensor.reset();
    TEST_ASSERT_UNLESS_MESSAGE(ret != 0, "Failed to reset the sensor");
}

void TestSi_initialize(){
    int ret;
    ret = sensor.initialize();
    TEST_ASSERT_UNLESS_MESSAGE(ret != 0, "Failed to initialize the sensor");
}

void TestSi_getTemperature(){
    int ret;
    ret = sensor.getTemperature();
    TEST_ASSERT_UNLESS_MESSAGE(ret == (-1), "Failed to measure the temperature");
}

void TestSi_calcTemperature(){
    int ret;
    char data[2];
    data[0] = 100;
    data[1] = 255;
    ret = sensor.calcTemperature(data);
    TEST_ASSERT_UNLESS_MESSAGE(ret != 2247, "Failed to calculate the temperature");
}

// test if two consequent measurements are inside a range of +-2°C
void TestSi_measureTemperature(){
    int ret;
    char data[2];
    uint16_t temp_raw1,temp_raw2;
    uint16_t delta_2 = 746; // this is a raw temperature delta of 2°C
    uint16_t delta_real;

    // first measurement
    ret = sensor.measureTemperature(data);
    TEST_ASSERT_UNLESS_MESSAGE(ret != 0, "Failed to measure the temperature 1");
    temp_raw1 = ((uint16_t)(data[0])<<8 + (uint16_t)(data[1]));

    //wait_ms(5);

    // second measurement
    ret = sensor.measureTemperature(data);
    TEST_ASSERT_UNLESS_MESSAGE(ret != 0, "Failed to measure the temperature 2");
    temp_raw2 = ((uint16_t)(data[0])<<8 + (uint16_t)(data[1]));

    // calculate the difference between the two measurements
    if(temp_raw1 >= temp_raw2){
        delta_real = temp_raw1 - temp_raw2;
    }
    else{
        delta_real = temp_raw2 - temp_raw1;
    }
    TEST_ASSERT_UNLESS_MESSAGE(delta_real > delta_2, "temperature difference bigger then +-2°C");
}

void TestSi_getFirmwareVersion(){
    int ret;
    ret = sensor.getFirmwareVersion();
    TEST_ASSERT_UNLESS_MESSAGE(ret == (-1), "Failed to get the Firmware version");
}

void TestSi_getID(){
    int ret;
    ret = sensor.getID();
    TEST_ASSERT_UNLESS_MESSAGE(ret == (-1), "Failed to get the ID");
}

void TestSi_calculationRange(){
    int ret;
    char data[2];
    int delta = 8786;
    int expect = 4100;

    // go through the complete range of values
    for(int i = 0; i < 256; i++)
    {
        data[0] = i;
        for (int j = 0; j < 256; j++)
        {
            data[1] = j;
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
        Case("Sensor Reset-0", TestSi_reset, greentea_failure_handler),
        Case("Sensor Initialize-0", TestSi_initialize, greentea_failure_handler),
        Case("Sensor get Temperature-0", TestSi_getTemperature, greentea_failure_handler),
        Case("Sensor calculate Temperature-0", TestSi_calcTemperature, greentea_failure_handler),
        Case("Sensor measure Temperature-0", TestSi_measureTemperature, greentea_failure_handler),
        Case("Sensor get Frimware version-0", TestSi_getFirmwareVersion, greentea_failure_handler),
        Case("Sensor get ID-0", TestSi_getID, greentea_failure_handler),
        Case("Sensor check calculation range min to max-0", TestSi_calculationRange, greentea_failure_handler),
};

utest::v1::status_t greentea_test_setup(const size_t number_of_cases) {
    GREENTEA_SETUP(150, "default_auto");
    return greentea_test_setup_handler(number_of_cases);
}


int main() {

    ext_3v3 = 1;
    wait_ms(100);


    Specification specification(greentea_test_setup, cases, greentea_test_teardown_handler);
    Harness::run(specification);
} 