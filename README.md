# Si7050 Temperature Sensor

These driver functions can be used for I2C communication with the
[Temperature Sensor Si7050](https://www.silabs.com/documents/public/data-sheets/Si7050-1-3-4-5-A20.pdf)
group. The functions include setting up the sensor, making and reading
the measurement and calculating the temperature values with a 0.01°C resolution

## Library Usage

Use `mbed add https://github.com/ubirch/ubirch-mbed-Si7050` to add this
library to your own application. 

```C++
SI7050 sensor(I2C_SDA, I2C_SCL);

void main() {
  int temp = sensor.getTemperature();
  printf("temp = %d\r\n", temp);
}
``` 

## Testing

Run `mbed test -n tests-si7050*` to run all local Si7050 tests.

## License

Author: Waldemar Grünwald ([@gruenwaldi](http://github.com/gruenwaldi))

```
Copyright 2017 ubirch GmbH (http://www.ubirch.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```