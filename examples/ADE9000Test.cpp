#define DEBUG_MODE_OFF

#define NOMINAL_INPUT_VOLTAGE 124
#define NOMINAL_INPUT_CURRENT 0.94
#define INPUT_FREQUENCY 60

#define ACCUMULATION_TIME 5

#include "Arduino.h"
#include <ADE9000RegMap.h>
#include <ADE9000.h>

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