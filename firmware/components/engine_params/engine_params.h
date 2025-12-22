#ifndef _ENGINE_PARAMS_
#define _ENGINE_PARAMS_

#include <stdio.h>
#include <inttypes.h>


//structure based on rusefi can dbc

typedef struct {
    uint8_t RevLimAct : 1;
    uint8_t MainRelayAct : 1;
    uint8_t FuelPumpAct : 1;
    uint8_t CELAct: 1;
    uint8_t EGOHeatAct: 1;
    uint8_t LambdaProtectAct: 1;
    uint8_t Fan: 1;
    uint8_t Fan2: 1;
}flags_t;

typedef struct {

    uint16_t WarningCounter;
    uint16_t LastError;
    uint16_t DistanceTraveled; // factor 0.1
    uint16_t RPM;
    uint16_t IgnitionTiming; // 0.02 factor
    uint16_t PPS; // factor 0.01 %
    uint16_t TPS1; // factor 0.01 %
    uint16_t TPS2; // factor 0.01 %
    uint16_t Wastegate; // factor 0.01 %
    uint16_t MAP; // factor 0.03 kPa
    uint16_t OilPress; // factor 0.03 kPa
    uint16_t BattVolt; // factor 0.001 mV
    uint16_t CylAM; // factor 1 mg
    uint16_t EstMAF; // factor 0.01 kg/h
    uint16_t InjPW; // factor 0.003 ms
    uint16_t KnockCt; // factor 1
    uint16_t FuelUsed; // factor 1 g
    uint16_t FuelFlow; // factor 0.005 g/s
    uint16_t FuelTrim1; // factor 0.01 offset -50 %
    uint16_t FuelTrim2; // factor 0.01 offset -50 %
    uint16_t Lam1; // factor 0.0001 lambda
    uint16_t Lam2; // factor 0.0001 lambda
    uint16_t FpLow; // factor 0.03 Kpda
    uint16_t FpHigh; // factor 0.1 bar

    uint8_t CurrentGear;
    uint8_t InjDuty; // factor 0.5
    uint8_t IgnDuty; // factor 0.5
    uint8_t VehicleSpeed; // factor 1 km/h
    uint8_t FlexPct; // factor 1 %
    uint8_t CoolantTemp; // factor 1, offset -40
    uint8_t IntakeTemp; // factor 1, offset -40
    uint8_t AUX1Temp; // factor 1, offset -40
    uint8_t AUX2Temp; // factor 1, offset -40
    uint8_t MCUTemp; // factor 1, offset -40
    uint8_t FuelLevel; // factor 0.5 %
    uint8_t OilTemperature; // factor 1, offset -40
    uint8_t FuelTemperature; // factor 1, offset -40
    int8_t Cam1I;
    int8_t Cam1Itar; 
    int8_t Cam1E;
    int8_t Cam1Etar;
    int8_t Cam2I;
    int8_t Cam2Itar;
    int8_t Cam2E;
    int8_t Cam2Etar;
    int8_t Egt1; //factor 5 
    int8_t Egt2; //factor 5 
    int8_t Egt3; //factor 5 
    int8_t Egt4; //factor 5 
    int8_t Egt5; //factor 5 
    int8_t Egt6; //factor 5 
    int8_t Egt7; //factor 5 
    int8_t Egt8; //factor 5 
    int8_t knock0; // factor 1, -60 -> 10 db
    int8_t knock1; // factor 1, -60 -> 10 db
    int8_t knock2; // factor 1, -60 -> 10 db
    int8_t knock3; // factor 1, -60 -> 10 db
    int8_t knock4; // factor 1, -60 -> 10 db
    int8_t knock5; // factor 1, -60 -> 10 db
    int8_t knock6; // factor 1, -60 -> 10 db
    int8_t knock7; // factor 1, -60 -> 10 db

    flags_t flags0;

}engine_param_t;

#endif