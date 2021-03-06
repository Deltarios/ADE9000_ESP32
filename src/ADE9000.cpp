/*
  ADE9000.cpp - Library for ADE9000 with ESP32 - Energy and PQ monitoring AFE
  Date: 12-07-2020
*/
/****************************************************************************************************************
 Includes
***************************************************************************************************************/

#include "ADE9000.h"

ADE9000::ADE9000(uint32_t SPI_speed, uint8_t chipSelect_Pin)
{
    this->_SPI_speed = SPI_speed;
    this->_chipSelect_Pin = chipSelect_Pin;
}

void ADE9000::initADE9000(bool initSPI)
{
    if (!initSPI)
    this->SPI_Init();

    pinMode(_chipSelect_Pin, OUTPUT);                                   // Set Chip select pin as output
    digitalWrite(_chipSelect_Pin, HIGH);                                // Set Chip select pin high
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

void ADE9000::resetADE9000(uint8_t ADE9000_RESET_PIN)
{
    digitalWrite(ADE9000_RESET_PIN, LOW);
    delay(50);
    digitalWrite(ADE9000_RESET_PIN, HIGH);
    delay(1000);
    Serial.println("Reset Done");
}

void ADE9000::SPI_Init()
{
    SPI.begin();                                                       //Initiate SPI port
    SPI.beginTransaction(SPISettings(_SPI_speed, MSBFIRST, SPI_MODE0)); //Setup SPI parameters
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

void ADE9000::readActivePowerRegs(ActivePowerRegs *Data)
{
    Data->ActivePowerReg_A = int32_t(SPI_Read_32(ADDR_AWATT));
    Data->ActivePowerReg_B = int32_t(SPI_Read_32(ADDR_BWATT));
    Data->ActivePowerReg_C = int32_t(SPI_Read_32(ADDR_CWATT));
}

void ADE9000::readReactivePowerRegs(ReactivePowerRegs *Data)
{
    Data->ReactivePowerReg_A = int32_t(SPI_Read_32(ADDR_AVAR));
    Data->ReactivePowerReg_B = int32_t(SPI_Read_32(ADDR_BVAR));
    Data->ReactivePowerReg_C = int32_t(SPI_Read_32(ADDR_CVAR));
}

void ADE9000::readApparentPowerRegs(ApparentPowerRegs *Data)
{
    Data->ApparentPowerReg_A = int32_t(SPI_Read_32(ADDR_AVA));
    Data->ApparentPowerReg_B = int32_t(SPI_Read_32(ADDR_BVA));
    Data->ApparentPowerReg_C = int32_t(SPI_Read_32(ADDR_CVA));
}

void ADE9000::readVoltageRMSRegs(VoltageRMSRegs *Data)
{
    Data->VoltageRMSReg_A = int32_t(SPI_Read_32(ADDR_AVRMS));
    Data->VoltageRMSReg_B = int32_t(SPI_Read_32(ADDR_BVRMS));
    Data->VoltageRMSReg_C = int32_t(SPI_Read_32(ADDR_CVRMS));
}

void ADE9000::readCurrentRMSRegs(CurrentRMSRegs *Data)
{
    Data->CurrentRMSReg_A = int32_t(SPI_Read_32(ADDR_AIRMS));
    Data->CurrentRMSReg_B = int32_t(SPI_Read_32(ADDR_BIRMS));
    Data->CurrentRMSReg_C = int32_t(SPI_Read_32(ADDR_CIRMS));
    Data->CurrentRMSReg_N = int32_t(SPI_Read_32(ADDR_NIRMS));
}

void ADE9000::readPowerFactorRegsnValues(PowerFactorRegs *Data)
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

void ADE9000::readPeriodRegsnValues(PeriodRegs *Data)
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

void ADE9000::readAngleRegsnValues(AngleRegs *Data)
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

void ADE9000::readTempRegnValue(TemperatureRegnValue *Data)
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

double ADE9000::convertCodeToPower(int32_t value)
{
    double result = 0;
    result = (double)(CAL_POWER_CC * value) / ONE_THOUSAND;
    return result;
}

double ADE9000::convertCodeToEnergy(int32_t value)
{
    double result = 0;
    result = (double)(CAL_ENERGY_CC * value) / ONE_MILLION;
    return result;
}

void ADE9000::getPGA_gain(PGAGainRegs *Data)
{
    int16_t pgaGainRegister;
    int16_t temp;
    pgaGainRegister = SPI_Read_16(ADDR_PGA_GAIN); // Ensure PGA_GAIN is set correctly in SetupADE9000 function.
    temp = pgaGainRegister & (0x0003);            // Extract gain of current channel
#ifdef DEBUG_MODE
    Serial.print("PGA Gain Register is: ");
    Serial.println(temp, HEX);
#endif
    if (temp == 0) // 00-->Gain 1: 01-->Gain 2: 10/11-->Gain 4
    {
        Data->CurrentPGA_gain = 1;
    }
    else
    {
        if (temp == 1)
        {
            Data->CurrentPGA_gain = 2;
        }
        else
        {
            Data->CurrentPGA_gain = 4;
        }
    }
    temp = (pgaGainRegister >> 8) & (0x0003); //extract gain of voltage channel
    if (temp == 0)
    {
        Data->VoltagePGA_gain = 1;
    }
    else
    {
        if (temp == 1)
        {
            Data->VoltagePGA_gain = 2;
        }
        else
        {
            Data->VoltagePGA_gain = 4;
        }
    }
}

void ADE9000::iGain_calibrate(int32_t *igainReg, int32_t *iRmsRegAddress, int arraySize, uint8_t currentPGA_gain)
{
    float temp;
    int32_t actualCodes;
    int32_t expectedCodes;

    temp = ADE9000_RMS_FULL_SCALE_CODES * CURRENT_TRANSFER_FUNCTION * currentPGA_gain * NOMINAL_INPUT_CURRENT * sqrt(2);
    expectedCodes = (int32_t)temp; // Round off
#ifdef DEBUG_MODE
    Serial.print("Expected IRMS Code: ");
    Serial.println(expectedCodes, HEX);
    for (uint8_t i = 0; i < arraySize; i++)
#endif
    {
        actualCodes = SPI_Read_32(iRmsRegAddress[i]);
        temp = (((float)expectedCodes / (float)actualCodes) - 1) * 134217728; // Calculate the gain.
        igainReg[i] = (int32_t)temp;                                          // Round off
#ifdef DEBUG_MODE
        Serial.print("Channel ");
        Serial.print(i + 1);
        Serial.print(" actual IRMS Code: ");
        Serial.println(actualCodes, HEX);
        Serial.print("Current Gain Register: ");
        Serial.println(igainReg[i], HEX);
#endif
    }
}

void ADE9000::vGain_calibrate(int32_t *vgainReg, int32_t *vRmsRegAddress, int arraySize, uint8_t voltagePGA_gain)
{
    float temp;
    int32_t actualCodes;
    int32_t expectedCodes;

    temp = ADE9000_RMS_FULL_SCALE_CODES * (VOLTAGE_TRANSFER_FUNCTION * voltagePGA_gain * NOMINAL_INPUT_VOLTAGE * sqrt(2));
    expectedCodes = (int32_t)temp; // Round off
#ifdef DEBUG_MODE
    Serial.print("Expected VRMS Code: ");
    Serial.println(expectedCodes, HEX);
#endif
    for (uint8_t i = 0; i < arraySize; i++)
    {
        actualCodes = SPI_Read_32(vRmsRegAddress[i]);
        temp = (((float)expectedCodes / (float)actualCodes) - 1) * 134217728; // Calculate the gain.
        vgainReg[i] = (int32_t)temp;                                          // Round off
#ifdef DEBUG_MODE
        Serial.print("Channel ");
        Serial.print(i + 1);
        Serial.print(" actual VRMS Code: ");
        Serial.println(actualCodes, HEX);
        Serial.print("Voltage Gain Register: ");
        Serial.println(vgainReg[i], HEX);
#endif
    }
}

void ADE9000::phase_calibrate(int32_t *phcalReg, int32_t *accActiveEgyReg, int32_t *accReactiveEgyReg, int arraySize)
{
    Serial.println("Computing phase calibration registers...");
    delay((ACCUMULATION_TIME + 1) * 1000); // Delay to ensure the energy registers are accumulated for defined interval
    float errorAngle;
    float errorAngleDeg;
    float omega;
    double temp;
    int32_t actualActiveEnergyCode;
    int32_t actualReactiveEnergyCode;
    int i;
    omega = (float)2 * (float)3.14159 * (float)INPUT_FREQUENCY / (float)ADE9000_FDSP;

    for (i = 0; i < arraySize; i++)
    {
        actualActiveEnergyCode = accActiveEgyReg[i];
        actualReactiveEnergyCode = accReactiveEgyReg[i];
        errorAngle = (double)-1 * atan(((double)actualActiveEnergyCode * (double)sin(CAL_ANGLE_RADIANS(CALIBRATION_ANGLE_DEGREES)) - (double)actualReactiveEnergyCode * (double)cos(CAL_ANGLE_RADIANS(CALIBRATION_ANGLE_DEGREES))) / ((double)actualActiveEnergyCode * (double)cos(CAL_ANGLE_RADIANS(CALIBRATION_ANGLE_DEGREES)) + (double)actualReactiveEnergyCode * (double)sin(CAL_ANGLE_RADIANS(CALIBRATION_ANGLE_DEGREES))));
        temp = (((double)sin((double)errorAngle - (double)omega) + (double)sin((double)omega)) / ((double)sin(2 * (double)omega - (double)errorAngle))) * 134217728;
        phcalReg[i] = (int32_t)temp;
        errorAngleDeg = (float)errorAngle * 180 / 3.14159;
#ifdef DEBUG_MODE
        Serial.print("Channel ");
        Serial.print(i + 1);
        Serial.print(" actual Active Energy Register: ");
        Serial.println(actualActiveEnergyCode, HEX);
        Serial.print("Channel ");
        Serial.print(i + 1);
        Serial.print(" actual Reactive Energy Register: ");
        Serial.println(actualReactiveEnergyCode, HEX);
        Serial.print("Phase Correction (degrees): ");
        Serial.println(errorAngleDeg, 5);
        Serial.print("Phase Register: ");
        Serial.println(phcalReg[i], HEX);
#endif
    }
}

void ADE9000::pGain_calibrate(int32_t *pgainReg, int32_t *accActiveEgyReg, int arraySize, uint8_t currentPGA_gain, uint8_t voltagePGA_gain, float pGaincalPF)
{
    Serial.println("Computing power gain calibration registers...");
    delay((ACCUMULATION_TIME + 1) * 1000); // Delay to ensure the energy registers are accumulated for defined interval
    int32_t expectedActiveEnergyCode;
    int32_t actualActiveEnergyCode;
    int i;
    float temp;
    temp = ((float)ADE9000_FDSP * (float)NOMINAL_INPUT_VOLTAGE * (float)NOMINAL_INPUT_CURRENT * (float)CALIBRATION_ACC_TIME * (float)CURRENT_TRANSFER_FUNCTION * (float)currentPGA_gain * (float)VOLTAGE_TRANSFER_FUNCTION * (float)voltagePGA_gain * (float)ADE9000_WATT_FULL_SCALE_CODES * 2 * (float)(pGaincalPF)) / (float)(8192);
    expectedActiveEnergyCode = (int32_t)temp;
#ifdef DEBUG_MODE
    Serial.print("Expected Active Energy Code: ");
    Serial.println(expectedActiveEnergyCode, HEX);
#endif

    for (i = 0; i < arraySize; i++)
    {
        actualActiveEnergyCode = accActiveEgyReg[i];

        temp = (((float)expectedActiveEnergyCode / (float)actualActiveEnergyCode) - 1) * 134217728; // Calculate the gain.
        pgainReg[i] = (int32_t)temp;                                                                // Round off
#ifdef DEBUG_MODE
        Serial.print("Channel ");
        Serial.print(i + 1);
        Serial.print("Actual Active Energy Code: ");
        Serial.println(actualActiveEnergyCode, HEX);
        Serial.print("Power Gain Register: ");
        Serial.println(pgainReg[i], HEX);
#endif
    }
}

void ADE9000::updateEnergyRegisterFromInterrupt(int32_t *accumulatedActiveEnergy_registers, int32_t *accumulatedReactiveEnergy_registers, int32_t *accumulatedApparentEnergy_registers)
{
    const int32_t xWATTHRHI_registers_address[EGY_REG_SIZE] = {ADDR_AWATTHR_HI, ADDR_BWATTHR_HI, ADDR_CWATTHR_HI};
    const int32_t xVARHRHI_registers_address[EGY_REG_SIZE] = {ADDR_AVARHR_HI, ADDR_BVARHR_HI, ADDR_CVARHR_HI};
    const int32_t xVAHRHI_registers_address[EGY_REG_SIZE] = {ADDR_AVAHR_HI, ADDR_BVAHR_HI, ADDR_CVAHR_HI};

    static int8_t count = 0;
    static int32_t intermediateActiveEgy_Reg[EGY_REG_SIZE] = {0};
    static int32_t intermediateReactiveEgy_Reg[EGY_REG_SIZE] = {0};
    static int32_t intermediateApparentEgy_Reg[EGY_REG_SIZE] = {0};

    uint32_t temp;
    temp = SPI_Read_32(ADDR_STATUS0);
    temp &= EGY_INTERRUPT_MASK0;
    if (temp == EGY_INTERRUPT_MASK0)
    {
        SPI_Write_32(ADDR_STATUS0, 0xFFFFFFFF);
        for (int8_t i = 0; i < EGY_REG_SIZE; i++)
        {
            intermediateActiveEgy_Reg[i] += SPI_Read_32(xWATTHRHI_registers_address[i]);  // Accumulate the registers
            intermediateReactiveEgy_Reg[i] += SPI_Read_32(xVARHRHI_registers_address[i]); // Accumulate the registers
            intermediateApparentEgy_Reg[i] += SPI_Read_32(xVAHRHI_registers_address[i]);  // Accumulate the registers
        }

        if (count == (ACCUMULATION_TIME - 1)) //if the accumulation time is reached, update the final values to registers
        {
            for (uint8_t i = 0; i < EGY_REG_SIZE; i++)
            {
                accumulatedActiveEnergy_registers[i] = intermediateActiveEgy_Reg[i];
                accumulatedReactiveEnergy_registers[i] = intermediateReactiveEgy_Reg[i];
                accumulatedApparentEnergy_registers[i] = intermediateApparentEgy_Reg[i];
                intermediateActiveEgy_Reg[i] = 0;   // Reset the intermediate registers
                intermediateReactiveEgy_Reg[i] = 0; // Reset the intermediate registers
                intermediateApparentEgy_Reg[i] = 0; // Reset the intermediate registers
            }
            count = 0; //Reset counter
            return;    //exit function
        }
        count++;
        return;
    }
}
