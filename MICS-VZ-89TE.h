
/***************************************************************************
 
 Hervé Grabas
  
 This is a library for the MICS-VZ-89TE Indoor Air Quality Sensor from SGX Sensortech Limited.
 
 
 ***************************************************************************/

#ifndef __MICS_VZ_89TE_H__

#define __MICS_VZ_89TE_H__

#include "Arduino.h"


#define MICS_VZ_89TE_ADDR      0x70 //0x70 default I2C address
#define MICS_VZ_89TE_ADDR      0x70 //0x70 default I2C address
//registers
#define MICS_VZ_89TE_ADDR_CMD_GETSTATUS	0x0C	// This command is used to read the VZ89 status coded with 6 bytes:
#define MICS_VZ_89TE_DATE_CODE 0x0D

// Constantes datasheet:
#define T_PWM_PULSES 33300 // us
#define MIN_TVOC 1665 // 33300 * 0.05 us
#define MAX_TVOC 14985 // us
#define MIN_CO2 18315 // 33300 * 0.55 us
#define MAX_CO2 31635 // us

#define PWM_ERROR_FLAG_TVOC 1
#define PWM_ERROR_FLAG_CO2  2
#define PWM_ERROR_FLAG_ALL  3
/*=========================================================================

Main Class for the MICS_VZ_89TE library

=========================================================================*/


class MICS_VZ_89TE

{
    
public:
    MICS_VZ_89TE(void);
    // To get the data
    float getCO2(void);
    float getVOC(void);

    float getYear(void);
    float getMonth(void);
    float getDay(void);
    float getRev(void);
    float getCrc(void);

    uint8_t getStatus(void);
    
    void begin(void);
    void readSensor(void);
    void getVersion(void);
    
    void beginPWM(uint8_t pin);
    void readSensorPWM(void);
    float getCO2PWM(void);
    float getVOCPWM(void);
    uint8_t errorPWM();
    
    float getAveragedCO2();
    float getAveragedVOC();

private:
    uint8_t   _i2caddr;
    uint8_t   _pwm;
    void readData(byte reg, uint8_t data[]);
    void readPWMData(uint32_t data[]);

    uint8_t getCrc(uint8_t *buf, uint8_t len);

    uint8_t co2;
    uint8_t voc;
    uint8_t status;

    uint32_t co2_pwm, voc_pwm;
    
    float year;
    float month;
    float day;
    float rev;
    uint8_t crc;

    // Error flags()
    bool pwm_f[2];
};

#endif
