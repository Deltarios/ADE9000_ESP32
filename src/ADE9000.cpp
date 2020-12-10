/*
  ADE9000.cpp - Library for ADE9000 with ESP32 - Energy and PQ monitoring AFE
  Date: 12-07-2020
*/
/****************************************************************************************************************
 Includes
***************************************************************************************************************/

#include "ADE9000.h"

ADE9000::ADE9000()
{
}

void ADE9000::setupADE9000(void)
{
    SPI_Write_16(ADDR_PGA_GAIN, ADE9000_PGA_GAIN1);
    SPI_Write_32(ADDR_CONFIG0, ADE9000_CONFIG0);
    SPI_Write_16(ADDR_CONFIG1, ADE9000_CONFIG1);
    SPI_Write_16(ADDR_CONFIG2, ADE9000_CONFIG2);
    SPI_Write_16(ADDR_CONFIG3, ADE9000_CONFIG3);
    SPI_Write_16(ADDR_ACCMODE, ADE9000_ACCMODE);
    SPI_Write_16(ADDR_TEMP_CFG, ADE9000_TEMP_CFG);
    SPI_Write_16(ADDR_ZX_LP_SEL, ADE9000_ZX_LP_SEL);
    SPI_Write_32(ADDR_MASK0, ADE9000_MASK0);
    SPI_Write_32(ADDR_MASK1, ADE9000_MASK1);
    SPI_Write_32(ADDR_EVENT_MASK, ADE9000_EVENT_MASK);
    SPI_Write_16(ADDR_WFB_CFG, ADE9000_WFB_CFG);
    SPI_Write_32(ADDR_VLEVEL, ADE9000_VLEVEL);
    SPI_Write_32(ADDR_DICOEFF, ADE9000_DICOEFF);
    SPI_Write_16(ADDR_EGY_TIME, ADE9000_EGY_TIME);
    SPI_Write_16(ADDR_EP_CFG, ADE9000_EP_CFG); //Energy accumulation ON
    SPI_Write_16(ADDR_RUN, ADE9000_RUN_ON);    //DSP ON
}

void ADE9000::SPI_Init(uint32_t SPI_speed, uint8_t chipSelect_Pin)
{
    SPI.begin();                                                       //Initiate SPI port
    SPI.beginTransaction(SPISettings(SPI_speed, MSBFIRST, SPI_MODE0)); //Setup SPI parameters
    pinMode(chipSelect_Pin, OUTPUT);                                   //Set Chip select pin as output
    digitalWrite(chipSelect_Pin, HIGH);                                //Set Chip select pin high

    _chipSelect_Pin = chipSelect_Pin;
}

void ADE9000::SPI_Write_16(uint16_t Address, uint16_t Data)
{
    uint16_t temp_address;

    digitalWrite(_chipSelect_Pin, LOW);
    temp_address = ((Address << 4) & 0xFFF0); // Shift address to align with cmd packet
    SPI.transfer16(temp_address);
    SPI.transfer16(Data);

    digitalWrite(_chipSelect_Pin, HIGH);
}

void ADE9000::SPI_Write_32(uint16_t Address, uint32_t Data)
{
    uint16_t temp_address;

    digitalWrite(_chipSelect_Pin, LOW);
    temp_address = ((Address << 4) & 0xFFF0); // Shift address  to align with cmd packet
    SPI.transfer16(temp_address);
    SPI.transfer32(Data);

    digitalWrite(_chipSelect_Pin, HIGH);
}

uint16_t ADE9000::SPI_Read_16(uint16_t Address)
{
    uint16_t temp_address;
    uint16_t returnData;

    digitalWrite(_chipSelect_Pin, LOW);

    temp_address = (((Address << 4) & 0xFFF0) + 8);
    SPI.transfer16(temp_address);
    returnData = SPI.transfer16(0);

    digitalWrite(_chipSelect_Pin, HIGH);
    return returnData;
}

uint32_t ADE9000::SPI_Read_32(uint16_t Address)
{
    uint16_t temp_address;
    uint32_t returnData;

    digitalWrite(_chipSelect_Pin, LOW);

    temp_address = (((Address << 4) & 0xFFF0) + 8);
    SPI.transfer16(temp_address);
    returnData = SPI.transfer32(0);

    digitalWrite(_chipSelect_Pin, HIGH);
    return returnData;
}

void ADE9000::ReadActivePowerRegs(ActivePowerRegs *Data)
{
    Data->ActivePowerReg_A = int32_t(SPI_Read_32(ADDR_AWATT));
    Data->ActivePowerReg_B = int32_t(SPI_Read_32(ADDR_BWATT));
    Data->ActivePowerReg_C = int32_t(SPI_Read_32(ADDR_CWATT));
}

void ADE9000::ReadReactivePowerRegs(ReactivePowerRegs *Data)
{
    Data->ReactivePowerReg_A = int32_t(SPI_Read_32(ADDR_AVAR));
    Data->ReactivePowerReg_B = int32_t(SPI_Read_32(ADDR_BVAR));
    Data->ReactivePowerReg_C = int32_t(SPI_Read_32(ADDR_CVAR));
}

void ADE9000::ReadApparentPowerRegs(ApparentPowerRegs *Data)
{
    Data->ApparentPowerReg_A = int32_t(SPI_Read_32(ADDR_AVA));
    Data->ApparentPowerReg_B = int32_t(SPI_Read_32(ADDR_BVA));
    Data->ApparentPowerReg_C = int32_t(SPI_Read_32(ADDR_CVA));
}

void ADE9000::ReadVoltageRMSRegs(VoltageRMSRegs *Data)
{
    Data->VoltageRMSReg_A = int32_t(SPI_Read_32(ADDR_AVRMS));
    Data->VoltageRMSReg_B = int32_t(SPI_Read_32(ADDR_BVRMS));
    Data->VoltageRMSReg_C = int32_t(SPI_Read_32(ADDR_CVRMS));
}

void ADE9000::ReadCurrentRMSRegs(CurrentRMSRegs *Data)
{
    Data->CurrentRMSReg_A = int32_t(SPI_Read_32(ADDR_AIRMS));
    Data->CurrentRMSReg_B = int32_t(SPI_Read_32(ADDR_BIRMS));
    Data->CurrentRMSReg_C = int32_t(SPI_Read_32(ADDR_CIRMS));
    Data->CurrentRMSReg_N = int32_t(SPI_Read_32(ADDR_NIRMS));
}

void ADE9000::ReadPowerFactorRegsnValues(PowerFactorRegs *Data)
{
    uint32_t tempReg;
    float tempValue;

    tempReg = int32_t(SPI_Read_32(ADDR_APF)); // Read PF register
    Data->PowerFactorReg_A = tempReg;
    tempValue = (float)tempReg / (float)134217728; // Calculate PF
    Data->PowerFactorValue_A = tempValue;
    tempReg = int32_t(SPI_Read_32(ADDR_BPF)); // Read PF register
    Data->PowerFactorReg_B = tempReg;
    tempValue = (float)tempReg / (float)134217728; // Calculate PF
    Data->PowerFactorValue_B = tempValue;
    tempReg = int32_t(SPI_Read_32(ADDR_CPF)); // Read PF register
    Data->PowerFactorReg_C = tempReg;
    tempValue = (float)tempReg / (float)134217728; // Calculate PF
    Data->PowerFactorValue_C = tempValue;
}

void ADE9000::ReadPeriodRegsnValues(PeriodRegs *Data)
{
    uint32_t tempReg;
    float tempValue;
    tempReg = int32_t(SPI_Read_32(ADDR_APERIOD)); // Read PERIOD register
    Data->PeriodReg_A = tempReg;
    tempValue = (float)(8000 * 65536) / (float)(tempReg + 1); // Calculate Frequency
    Data->FrequencyValue_A = tempValue;
    tempReg = int32_t(SPI_Read_32(ADDR_BPERIOD)); // Read PERIOD register
    Data->PeriodReg_B = tempReg;
    tempValue = (float)(8000 * 65536) / (float)(tempReg + 1); // Calculate Frequency
    Data->FrequencyValue_B = tempValue;
    tempReg = int32_t(SPI_Read_32(ADDR_CPERIOD)); // Read PERIOD register
    Data->PeriodReg_C = tempReg;
    tempValue = (float)(8000 * 65536) / (float)(tempReg + 1); // Calculate Frequency
    Data->FrequencyValue_C = tempValue;
}

void ADE9000::ReadAngleRegsnValues(AngleRegs *Data)
{

    uint32_t tempReg;
    uint16_t temp;
    float mulConstant;
    float tempValue;

    temp = SPI_Read_16(ADDR_ACCMODE); // Read frequency setting register
    if ((temp & 0x0100) >= 0)
    {
        mulConstant = 0.02109375; // Multiplier constant for 60Hz system
    }
    else
    {
        mulConstant = 0.017578125; // Multiplier constant for 50Hz system
    }

    tempReg = int16_t(SPI_Read_32(ADDR_ANGL_VA_VB)); // Read ANGLE register
    Data->AngleReg_VA_VB = tempReg;
    tempValue = tempReg * mulConstant; // Calculate Angle in degrees
    Data->AngleValue_VA_VB = tempValue;
    tempReg = int16_t(SPI_Read_32(ADDR_ANGL_VB_VC));
    Data->AngleReg_VB_VC = tempReg;
    tempValue = tempReg * mulConstant;
    Data->AngleValue_VB_VC = tempValue;
    tempReg = int16_t(SPI_Read_32(ADDR_ANGL_VA_VC));
    Data->AngleReg_VA_VC = tempReg;
    tempValue = tempReg * mulConstant;
    Data->AngleValue_VA_VC = tempValue;
    tempReg = int16_t(SPI_Read_32(ADDR_ANGL_VA_IA));
    Data->AngleReg_VA_IA = tempReg;
    tempValue = tempReg * mulConstant;
    Data->AngleValue_VA_IA = tempValue;
    tempReg = int16_t(SPI_Read_32(ADDR_ANGL_VB_IB));
    Data->AngleReg_VB_IB = tempReg;
    tempValue = tempReg * mulConstant;
    Data->AngleValue_VB_IB = tempValue;
    tempReg = int16_t(SPI_Read_32(ADDR_ANGL_VC_IC));
    Data->AngleReg_VC_IC = tempReg;
    tempValue = tempReg * mulConstant;
    Data->AngleValue_VC_IC = tempValue;
    tempReg = int16_t(SPI_Read_32(ADDR_ANGL_IA_IB));
    Data->AngleReg_IA_IB = tempReg;
    tempValue = tempReg * mulConstant;
    Data->AngleValue_IA_IB = tempValue;
    tempReg = int16_t(SPI_Read_32(ADDR_ANGL_IB_IC));
    Data->AngleReg_IB_IC = tempReg;
    tempValue = tempReg * mulConstant;
    Data->AngleValue_IB_IC = tempValue;
    tempReg = int16_t(SPI_Read_32(ADDR_ANGL_IA_IC));
    Data->AngleReg_IA_IC = tempReg;
    tempValue = tempReg * mulConstant;
    Data->AngleValue_IA_IC = tempValue;
}

void ADE9000::ReadTempRegnValue(TemperatureRegnValue *Data)
{
    uint32_t trim;
    uint16_t gain;
    uint16_t offset;
    uint16_t tempReg;
    float tempValue;

    SPI_Write_16(ADDR_TEMP_CFG, ADE9000_TEMP_CFG); // Start temperature acquisition cycle with settings in defined in ADE9000_TEMP_CFG
    delay(2);                                      // delay of 2ms. Increase delay if TEMP_TIME is changed

    trim = SPI_Read_32(ADDR_TEMP_TRIM);
    gain = (trim & 0xFFFF);                // Extract 16 LSB
    offset = ((trim >> 16) & 0xFFFF);      // Extract 16 MSB
    tempReg = SPI_Read_16(ADDR_TEMP_RSLT); // Read Temperature result register
    tempValue = (float)(offset >> 5) - ((float)tempReg * (float)gain / (float)65536);

    Data->Temperature_Reg = tempReg;
    Data->Temperature = tempValue;
}

double ADE9000::convertCodeToVolts(int32_t value)
{
    double result = 0;
    result = (double)(CAL_VRMS_CC * value) / ONE_MILLION;
    return result;
}

double ADE9000::convertCodeToAmperes(int32_t value)
{
    double result = 0;
    result = (double)(CAL_IRMS_CC * value) / ONE_MILLION;
    return result;
}

double ADE9000::convertCodeToWatts(int32_t value)
{
    double result = 0;
    result = (double)(CAL_POWER_CC * value) / ONE_THOUSAND;
    return result;
}
