#define NOMINAL_INPUT_VOLTAGE 124
#define NOMINAL_INPUT_CURRENT 0.94
#define INPUT_FREQUENCY 60

#include <ADE9000RegMap.h>
#include <ADE9000.h>
#include "Arduino.h"

/*Basic initializations*/
ADE9000 ade9000;
#define SPI_SPEED 4000000UL //SPI Speed
#define CS_PIN 5          //8-->Arduino Zero. 16-->ESP8266
// #define ADE9000_RESET_PIN 10 //Reset Pin on HW
#define PM_1 13 //PM1 Pin: 4-->Arduino Zero. 15-->ESP8266

/*Structure decleration */
struct ActivePowerRegs powerRegs;      // Declare powerRegs of type ActivePowerRegs to store Active Power Register data
struct CurrentRMSRegs curntRMSRegs;    // Current RMS
struct VoltageRMSRegs vltgRMSRegs;     // Voltage RMS
struct ResampledWfbData resampledData; // Resampled Data

/*Function Decleration*/
void readRegisterData(void);
// void resetADE9000(void);

void setup()
{
  Serial.begin(115200);
  pinMode(PM_1, OUTPUT);   //Set PM1 pin as output
  digitalWrite(PM_1, LOW); //Set PM1 select pin low for PSM0 mode
  // pinMode(ADE9000_RESET_PIN, OUTPUT);
  // digitalWrite(ADE9000_RESET_PIN, HIGH);
  // void resetADE9000();
  delay(1000);
  ade9000.SPI_Init(SPI_SPEED, CS_PIN); //Initialize SPI
  ade9000.setupADE9000();              //Initialize ADE9000 registers according to values in ADE9000API.h
  // ade9000.SPI_Write_16(ADDR_RUN,0x1); //Set RUN=1 to turn on DSP. Uncomment if SetupADE9000 function is not used
  Serial.print("RUN Register: ");
  Serial.println(ade9000.SPI_Read_16(ADDR_RUN), HEX);
}

void loop()
{
  readRegisterData();
  // readResampledData();
  delay(1200);
}

void readRegisterData()
{
  /*Read and Print Specific Register using ADE9000 SPI Library */
  Serial.print("AIRMS: ");
  Serial.print((0.7071 * 3000 / 10.2) * ade9000.SPI_Read_32(ADDR_AIRMS) / 52702092); // AIRMS
  Serial.println(" Arms");

  /*Read and Print RMS & WATT Register using ADE9000 Read Library*/
  ade9000.ReadVoltageRMSRegs(&vltgRMSRegs); //Template to read Power registers from ADE9000 and store data in Arduino MCU
  ade9000.ReadActivePowerRegs(&powerRegs);
  Serial.print("AVRMS: ");
  Serial.print(ade9000.convertCodeToVolts(vltgRMSRegs.VoltageRMSReg_A)); //Print AVRMS register
  Serial.println(" Vrms");
  Serial.print("AWATT: ");
  Serial.print(((0.7071 * 3000 / 10.2) * (0.7071 * ((800000 + 1000) / 1000))) * powerRegs.ActivePowerReg_A / 20694066); //Print AWATT register
  Serial.println(" WATTS");
}

// void resetADE9000(void)
// {
//   digitalWrite(ADE9000_RESET_PIN, LOW);
//   delay(50);
//   digitalWrite(ADE9000_RESET_PIN, HIGH);
//   delay(1000);
//   Serial.println("Reset Done");
// }