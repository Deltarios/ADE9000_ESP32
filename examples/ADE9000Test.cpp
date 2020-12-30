#include "Arduino.h"
#include <ADE9000RegMap.h>
#include <ADE9000.h>

const uint8_t NOMINAL_INPUT_VOLTAGE = 220;
/* Current nominal RMS for Input in Amperes */
const float NOMINAL_INPUT_CURRENT = 1;

/* Frequency in Hertz */
const uint8_t INPUT_FREQUENCY = 60;

/* Used for funtion transform current in Ohm */
const uint8_t CALIBRATION_ANGLE_DEGREES = 60;

/* Current nominal RMS for Input in Amperes */
const float BURDEN_RESISTOR = 10.20;

/* Used for funtion transform current */
const uint16_t TURNS_RATIO_TRANSFORMER = 5000;

/* The defaul atteunation factor on board used funtion transform in Ohm/Ohm ((R1 + R2)/ R2) */
const float ATTEUNATION_FACTOR = 1200;

/* Accumulation time in seconds when EGY_TIME=7999, accumulation mode= sample based */
const uint8_t ACCUMULATION_TIME = 5;

/*Basic initializations*/
ADE9000 ade9000;
#define SPI_SPEED 4000000UL // SPI Speed
#define CS_PIN 5           
// #define ADE9000_RESET_PIN 10 //Reset Pin on HW
#define PM_1 13 // PM1 Pin

/*Structure decleration */
struct ActivePowerRegs powerRegs;      // Declare powerRegs of type ActivePowerRegs to store Active Power Register data
struct CurrentRMSRegs currentRMSRegs;  // Current RMS
struct VoltageRMSRegs vltgRMSRegs;     // Voltage RMS
struct ResampledWfbData resampledData; // Resampled Data

/*Function Decleration*/
void readRegisterData(void);

void setup()
{
  Serial.begin(115200);
  pinMode(PM_1, OUTPUT);   // Set PM1 pin as output
  digitalWrite(PM_1, LOW); // Set PM1 select pin low for PSM0 mode
  delay(1000);
  ade9000.SPI_Init(SPI_SPEED, CS_PIN); // Initialize SPI
  ade9000.setupADE9000();              // Initialize ADE9000 registers according to values in ADE9000API.h
  Serial.print("RUN Register: ");
  Serial.println(ade9000.SPI_Read_16(ADDR_RUN), HEX);
}

void loop()
{
  readRegisterData();
  delay(1200);
}

void readRegisterData()
{
  /*Read and Print RMS & WATT Register using ADE9000 Read Library*/
  ade9000.readCurrentRMSRegs(&currentRMSRegs); // Template to current rms registers from ADE9000 and store data in ESP32 MCU
  ade9000.readVoltageRMSRegs(&vltgRMSRegs);
  ade9000.readActivePowerRegs(&powerRegs); // Template to read Power registers from ADE9000 and store data in ESP32 MCU
  Serial.print("AIRMS: ");
  Serial.print(ade9000.convertCodeToAmperes(currentRMSRegs.CurrentRMSReg_A)); // Print AIRMS register to Irms (A)
  Serial.println(" Arms");
  Serial.print("AVRMS: ");
  Serial.print(ade9000.convertCodeToVolts(vltgRMSRegs.VoltageRMSReg_A)); //Print AVRMS register to Vrms (V)
  Serial.println(" Vrms");
  Serial.print("AWATT: ");
  Serial.print(ade9000.convertCodeToPower(powerRegs.ActivePowerReg_A)); //Print AWATT register to Watts (W)
  Serial.println(" WATTS");
}