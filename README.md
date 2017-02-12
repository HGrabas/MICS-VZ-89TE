<snippet>
  <content>
# MICS-VZ-89TE
Provides a tested Arduino and ESP8266 library to read data from the MICS-VZ-89TE - Indoor Air Quality Sensor from SGX Sensortech Limited.


MICS-VZ-89TE
Indoor Air Quality Sensor from SGX Sensortech Limited
Integrated Dual Sensor for IAQ
 
Calibration-free 
Low power 
Wide VOCs detection range 
High sensitivity 
High resistance to shocks and vibrations
SGX Sensortech Limited (formerly e2v), Gas Sensors, MICS-VZ-89TE, Component Distributors Inc. (CDI)
The MiCS-VZ-89TE combines state-of-the-art MOS sensor technology with intelligent detection algorithms to monitor VOCs and CO2 equivalent variations in confined spaces, e.g. meeting rooms or vehicle cabins.
The dual signal output can be used to control ventilation on-demand, saving energy and reducing cost-of-ownership.

## Installation
Download the folder containing the files and place it with your other Arduino library folders.

## Usage
To use the library add:
\#include "MICS-VZ-89TE.h" at the top of your Arduino sketch.

Give the module few minutes of warmup to see sensible results.
Couple of tests to do (disclaimer: not responsible to any damage you maye cause to yourself of your equipment in the process - be carefull):
- Bring a small cup of alcool near the sensor.
- Empty a CO2 cartrige near the sensor.
- Burn a match or a lighter near the sensor.

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :D

## History
V1.0 Initial commit

## License
Under MIT licensing

></content>
  <tabTrigger>readme</tabTrigger>
</snippet>
