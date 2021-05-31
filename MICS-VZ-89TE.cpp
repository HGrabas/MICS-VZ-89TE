/***************************************************************************
 
 Hervé Grabas
 
 This is a library for the MICS_VZ_89TE Indoor Air Quality Sensor.
 
 No warranty is given
 
***************************************************************************/

#include "MICS-VZ-89TE.h"

#include <math.h>
#include <Wire.h>

MICS_VZ_89TE::MICS_VZ_89TE(void) {
    _i2caddr = MICS_VZ_89TE_ADDR;
    co2 = 0.0;
    voc = 0.0;
    _pwm = 0;
}

float MICS_VZ_89TE::getCO2(void) {
    return co2;
}

float MICS_VZ_89TE::getVOC(void) {
    return voc;
}

float MICS_VZ_89TE::getYear(void) {
    return year;
}

float MICS_VZ_89TE::getMonth(void) {
    return month;
}

float MICS_VZ_89TE::getDay(void) {
    return day;
}

float MICS_VZ_89TE::getRev(void) {
    return rev;
}

float MICS_VZ_89TE::getCrc(void) {
    return crc;
}

uint8_t MICS_VZ_89TE::getStatus(void) {
    return status;
}

void MICS_VZ_89TE::readSensor(void) {
    static uint8_t data[7];
    readData(MICS_VZ_89TE_ADDR_CMD_GETSTATUS, data);
    
    status = data[5];
    
    co2 = (data[1] - 13) * (1600.0 / 229) + 400; // ppm: 400 .. 2000
    voc = (data[0] - 13) * (1000.0/229);         // ppb: 0 .. 1000

}

void MICS_VZ_89TE::getVersion(void) {
    static uint8_t data[7];
    readData(MICS_VZ_89TE_DATE_CODE, data);
    
    year = data[0];
    month = data[1];
    day = data[2];
    rev = data[3];
    crc = data[4];
    // Comprobación de CRC:
    uint8_t _crc = getCrc(data, 4);

    // Tenemos comunicacion serie USB
    if (data[4] == _crc) {
        if(Serial)
            Serial.println("Rx: CRC ok");
        status = 0;
    } else {
        if(Serial)
            Serial.println("Rx: CRC not OK");
        status = 1;
    }
}

void MICS_VZ_89TE::begin() {
    Wire.begin();
}

/*
 After Power-on self-test (2 seconds) , the device will provide either a single “Failed Diagnostic
 Level” in case of sensor failure of the sensor or PWM multiplexed output indicating
 “CO_ equivalent_ Level” and “VOC _isobutylene_equivalent Level” referred to the isobutylene
 sensitivity unit.
 
 Out of this initial period, the device will have the I2C data CO2 equivalent [ppm] and tVOC equivalent
 referred to the isobutylene sensitivity unit [ppb].
 
 D1 : Data_byte_1: VOC_short [13…242]
 D2 : Data_byte_2: CO2_equ: [13…242] -> CO2_equ [ppm] = (D2 -13) * (1600/229) + 400
 D3 : Data_byte_3: Raw sensor first byte (MSB)
 D4 : Data_byte_4: Raw sensor second byte
 D5 : Data_byte_5: Raw sensor third byte (MSB) -> Resistor value [W] = 10*(D5+ (256*D4) + (65536*D3))
 D6 : Status
 D7 : CRC
 
 return CO2 equivalent [ppm] and tVOC equivalent referred to the isobutylene sensitivity unit [ppb].
 *
 */
void MICS_VZ_89TE::readData(byte reg, uint8_t data[]) {
    // Calculate CRC:
    uint8_t _buf[5] = {0, 0, 0, 0, 0};
    _buf[0] = (uint8_t)reg;
    crc = getCrc(_buf, 5);

    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg); //This send the command to get data
    Wire.write(0x0);
    Wire.write(0x0);
    Wire.write(0x0);
    Wire.write(0x0);
    Wire.write(crc); // CRC
    Wire.endTransmission();
    // Delay recomendado para la medida ~100 ms (datasheet)
    delay(110);
    
    Wire.requestFrom((uint8_t)_i2caddr, (byte)7);
    for (uint8_t i = 0; i < 7; i++) {
        data[i] = Wire.read();
    } 
    // Comprobación de CRC:
    crc = getCrc(data, 6);

    // Tenemos comunicacion serie USB
    if (data[6] == crc) {
        if(Serial)
            Serial.println("Rx: CRC ok");
        status = 0;
    } else {
        if(Serial)
            Serial.println("Rx: CRC not OK");
        status = 1;
    }
}

uint8_t MICS_VZ_89TE::getCrc(uint8_t *buf, uint8_t len){
    // Taken from datasheet.
    uint16_t sum = 0;
    uint8_t _crc;
    for (uint8_t i = 0; i < len; i++) { sum += buf[i]; }
    _crc = (uint8_t)sum;
    _crc += (sum / 0x0100);
    _crc = 0xFF - _crc;
    return _crc;
}

// -----------------
// PWM Handling:
// -----------------
void MICS_VZ_89TE::beginPWM(uint8_t pwm)
{
    _pwm = pwm;
    pinMode(_pwm, INPUT);
}

void MICS_VZ_89TE::readPWMData(uint32_t buff[])
{
    // Dejamos pasar el primer pulso por si se lee incompleto
    buff[0] = pulseIn(_pwm, HIGH, T_PWM_PULSES);
    delayMicroseconds(3);
    buff[1] = pulseIn(_pwm, HIGH, T_PWM_PULSES);
}

void MICS_VZ_89TE::readSensorPWM()
{
    uint32_t buff[2];
    readPWMData(buff);
    
    pwm_f[PWM_ERROR_FLAG_TVOC] = true;
    pwm_f[PWM_ERROR_FLAG_CO2]  = true;

    if(buff[0] > MIN_TVOC && buff[0] < MAX_TVOC){ // Primera lectura: tVOC
        voc_pwm = buff[0];
    } else if(buff[1] > MIN_TVOC && buff[1] < MAX_TVOC){
        voc_pwm = buff[1];
    } else {
        pwm_f[PWM_ERROR_FLAG_TVOC] = false;
    }

    if(buff[0] > MIN_CO2 && buff[0] < MAX_CO2){ // Primera lectura: CO2
        co2_pwm = buff[0];
    } else if(buff[1] > MIN_CO2 && buff[1] < MAX_CO2){
        co2_pwm = buff[1];
    } else {
        pwm_f[PWM_ERROR_FLAG_CO2]  = false;
    }
    
}

float MICS_VZ_89TE::getCO2PWM(){
    float co2 = 400.0 + ((float)co2_pwm * (1600.0 / (MAX_CO2 - MIN_CO2)));
    return co2;
}

float MICS_VZ_89TE::getVOCPWM(){
    float voc = (float)voc_pwm * (1000.0 / (MAX_TVOC - MIN_TVOC));
    return voc;
}

uint8_t MICS_VZ_89TE::errorPWM()
{
    uint8_t ec = 0;
    if(pwm_f[PWM_ERROR_FLAG_CO2])
        ec += PWM_ERROR_FLAG_CO2;
    if(pwm_f[PWM_ERROR_FLAG_TVOC])
        ec += PWM_ERROR_FLAG_TVOC;
    return ec;
}

// Medidas con ambos
float MICS_VZ_89TE::getAveragedCO2()
{
    float i2c = getCO2();
    float pwm = getCO2PWM();
    return ((i2c + pwm) / 2);
}

float MICS_VZ_89TE::getAveragedVOC()
{
    float i2c = getVOC();
    float pwm = getVOCPWM();
    return ((i2c + pwm) / 2);
}


