/*
  ADE9000API.h - Library based in ADE9000 - Energy and PQ monitoring AFE
  This library have with base the library of nchandra development by Analog Devices
  for compatibility at ESP32
  Author: Ariel Rios
  Date: 12-07-2020
*/
#ifndef ADE9000_H
#define ADE9000_H

/****************************************************************************************************************
 Includes
***************************************************************************************************************/

#include "Arduino.h"
#include "ADE9000RegMap.h"
#include <SPI.h>

/****************************************************************************************************************
									User Inputs
******************************************************************************************************************/

#ifndef NOMINAL_INPUT_VOLTAGE
/* prevent compiler error by supplying a default */
#warning "NOMINAL_INPUT_VOLTAGE not defined for <ADE9000.h>"
/** \ingroup ade9000
    \def NOMINAL_INPUT_VOLTAGE
    \brief Voltage nominal RMS for Input in Volts
 */
#define NOMINAL_INPUT_VOLTAGE 120
#endif

#ifndef NOMINAL_INPUT_CURRENT
/* prevent compiler error by supplying a default */
#warning "NOMINAL_INPUT_CURRENT not defined for <ADE9000.h>"
/** \ingroup ade9000
    \def NOMINAL_INPUT_VOLTAGE
    \brief Current nominal RMS for Input in Amperes
 */
#define NOMINAL_INPUT_CURRENT 1
#endif

#ifndef INPUT_FREQUENCY
/* prevent compiler error by supplying a default */
#warning "INPUT_FREQUENCY not defined for <ADE9000.h>"
/** \ingroup ade9000
    \def INPUT_FREQUENCY
    \brief Frequency in Hertz
 */
#define INPUT_FREQUENCY 60
#endif

#ifndef CALIBRATION_ANGLE_DEGREES
/* prevent compiler error by supplying a default */
#warning "CALIBRATION_ANGLE_DEGREES not defined for <ADE9000.h>"
/** \ingroup ade9000
    \def CALIBRATION_ANGLE_DEGREES
    \brief Used for phase calibration in Degrees
 */
#define CALIBRATION_ANGLE_DEGREES 60
#endif

#ifndef BURDEN_RESISTOR
/* prevent compiler error by supplying a default */
#warning "BURDEN_RESISTOR not defined for <ADE9000.h>"
/** \ingroup ade9000
    \def BURDEN_RESISTOR
    \brief Used for funtion transform current in Ohm
 */
#define BURDEN_RESISTOR 10.2f
#endif

#ifndef TURNS_RATIO_TRANSFORMER
/* prevent compiler error by supplying a default */
#warning "TURNS_RATIO_TRANSFORMER not defined for <ADE9000.h>"
/** \ingroup ade9000
    \def TURNS_RATIO_TRANSFORMER
    \brief Used for funtion transform current
 */
#define TURNS_RATIO_TRANSFORMER 3000
#endif

#ifndef ATTEUNATION_FACTOR
/* prevent compiler error by supplying a default */
#warning "ATTEUNATION_FACTOR not defined for <ADE9000.h>"
/** \ingroup ade9000
    \def TURNS_RATIO_TRANSFORMER
    \brief The defaul atteunation factor on board used funtion transform in Ohm/Ohm ((R1 + R2)/ R2)
 */
#define ATTEUNATION_FACTOR 801
#endif

#ifndef ACCUMULATION_TIME
/* prevent compiler error by supplying a default */
#warning "ACCUMULATION_TIME not defined for <ADE9000.h>"
/** \ingroup ade9000
    \def ACCUMULATION_TIME
    \brief Accumulation time in seconds when EGY_TIME=7999, accumulation mode= sample based
 */
#define ACCUMULATION_TIME 1
#endif        

// #ifndef F_CPU
// /* prevent compiler error by supplying a default */
// # warning "F_CPU not defined for <util/delay.h>"
// /** \ingroup util_delay
//     \def F_CPU
//     \brief CPU frequency in Hz

//     The macro F_CPU specifies the CPU frequency to be considered by
//     the delay macros.  This macro is normally supplied by the
//     environment (e.g. from within a project header, or the project's
//     Makefile).  The value 1 MHz here is only provided as a "vanilla"
//     fallback if no such user-provided definition could be found.

//     In terms of the delay functions, the CPU frequency can be given as
//     a floating-point constant (e.g. 3.6864E6 for 3.6864 MHz).
//     However, the macros in <util/setbaud.h> require it to be an
//     integer value.
//  */
// # define F_CPU 1000000UL
// #endif

/****************************************************************************************************************
 Definitions
****************************************************************************************************************/
/*Configuration registers*/
#define ADE9000_PGA_GAIN1 0x0000 /* PGA1@0x0000. Gain of all channels=1 */
#define ADE9000_PGA_GAIN2 0x1555 /* PGA2@0x1555. Gain of all channels=2 */
#define ADE9000_PGA_GAIN4 0x3FFF /* PGA4@0x3FFF. Gain of all channels=4 */

#define ADE9000_CONFIG0 0x00000000    /* Integrator disabled */
#define ADE9000_CONFIG1 0x0002        /* CF3/ZX pin outputs Zero crossing */
#define ADE9000_CONFIG2 0x0C00        /* Default High pass corner frequency of 1.25Hz */
#define ADE9000_CONFIG3 0x0000        /* Peak and overcurrent detection disabled */
#define ADE9000_ACCMODE 0x0000        /* 60Hz operation, 3P4W Wye configuration, signed accumulation Clear bit 8 i.e. ACCMODE=0x00xx for 50Hz operation ACCMODE=0x0x9x for 3Wire delta when phase B is used as reference */
#define ADE9000_TEMP_CFG 0x000C       /* Temperature sensor enabled */
#define ADE9000_ZX_LP_SEL 0x001E      /* Line period and zero crossing obtained from combined signals VA,VB and VC */
#define ADE9000_MASK0 0x00000001      /* Enable EGYRDY interrupt */
#define ADE9000_MASK1 0x00000000      /* MASK1 interrupts disabled*/
#define ADE9000_EVENT_MASK 0x00000000 /* Events disabled */
#define ADE9000_VLEVEL 0x0022EA28     /* Assuming Vnom=1/2 of full scale. Refer Technical reference manual for detailed calculations.*/
#define ADE9000_DICOEFF 0x00000000    /* Set DICOEFF= 0xFFFFE000 when integrator is enabled */

/*Constant Definitions***/
#define ADE90xx_FDSP 8000     /* ADE9000 FDSP: 8000sps, ADE9078 FDSP: 4000sps */
#define ADE9000_RUN_ON 0x0001 /* DSP ON */

/*Energy Accumulation Settings*/
#define ADE9000_EP_CFG 0x0011   /* Enable energy accumulation, accumulate samples at 8ksps */
                                /* latch energy accumulation after EGYRDY */
                                /* If accumulation is changed to half line cycle mode, change EGY_TIME */
#define ADE9000_EGY_TIME 0x1F3F /* Accumulate 8000 samples */

/*Waveform buffer Settings*/
#define ADE9000_WFB_CFG 0x1000     /* Neutral current samples enabled, Resampled data enabled */
                                   /* Burst all channels */
#define WFB_ELEMENT_ARRAY_SIZE 512 /*size of buffer to read. 512 Max.Each element IA,VA...IN has max 512 points */
                                   /*[Size of waveform buffer/number of sample sets = 2048/4 = 512] */
                                   /*(Refer ADE9000 technical reference manual for more details) */

/*Full scale Codes referred from Datasheet.Respective digital codes are produced when ADC inputs are at full scale. Donot Change. */
#define ADE9000_RMS_FULL_SCALE_CODES 52702092
#define ADE9000_WATT_FULL_SCALE_CODES 20694066
#define ADE9000_RESAMPLED_FULL_SCALE_CODES 18196
#define ADE9000_PCF_FULL_SCALE_CODES 74532013

enum CAL_STATE
{
  CAL_START,
  CAL_VI_CALIBRATE,
  CAL_PHASE_CALIBRATE,
  CAL_PGAIN_CALIBRATE,
  CAL_STORE,
  CAL_STOP,
  CAL_RESTART,
  CAL_COMPLETE
};

/*Transfer function*/
/****************************************************************************************************************
									Current Transfer Function
*****************************************************************************************************************
It is the voltage at the ADC input pins per input current(A) (Volts/Amp)
E.g: For a current output current transformer with Turns Ratio of 2500:1. Burden resistor on board = 5.1*2=10.2 Ohm.
Current transfer function= 1/2500*Total Burden = 1/2500*10.2= 0.00408
****************************************************************************************************************/
/****************************************************************************************************************
									Voltage Transfer Function
*****************************************************************************************************************
It is the voltage at the ADC input pins per input Voltage(V)(Volts/Volts)
E.g. The defaul atteunation factor on board is 801. 
Voltage transfer function = 1/801= 0.001248 ~=0.00125
****************************************************************************************************************/
#define CURRENT_TRANSFER_FUNCTION 1.0 / (TURNS_RATIO_TRANSFORMER * BURDEN_RESISTOR) //The RMS voltage at the ADC input pins per input RMS current  (V/A).(2500:1-->0.00408 with default burden resistors)
#define VOLTAGE_TRANSFER_FUNCTION 1.0 / ATTEUNATION_FACTOR                          //The RMS voltage at the ADC input pins per input RMS voltage (V/V)

/****************************************************************************************************************
									Constants: Do not change 
*****************************************************************************************************************/
#define F_DSP 8000                 //Signal update Rate
#define CALIBRATION_EGY_CFG 0xF011 //Latch after EGYRDY. Sample based accumulation. Read with reset disabled. Accumulation enabled
#define EGYACCTIME 0x1F3F          //Accumulate for a total of 8000 (EGY_TIME+1) samples.
#define CALIBRATION_ACC_TIME 1     //if EGYACCTIME= 0x1F3F, Accumulation time is 1sec. Change this if EGYACCTIME is changed.
#define EGY_INTERRUPT_MASK0 0x00000001      //Enable EGYRDY interrupt

#define IGAIN_CAL_REG_SIZE 4
#define VGAIN_CAL_REG_SIZE 3  
#define PHCAL_CAL_REG_SIZE 3
#define PGAIN_CAL_REG_SIZE 3
#define EGY_REG_SIZE 3

#define CAL_ANGLE_RADIANS(x) (x * 3.14159 / 180)
#define ONE_MILLION 1000000UL
#define ONE_THOUSAND 1000
#define SQRT_TO_2 1.41421

/****************************************************************************************************************
									Conversion Constants
*****************************************************************************************************************
If calibration is done with the the calibration sketch, the following conversion constants apply:
If CURRENT_TRANSFER_FUNCTION or VOLTAGE_TRANSFER_FUNCTION are changed, update the conversion constants

CAL_IRMS_CC in uA/code 				= 10^6/(CURRENT_TRANSFER_FUNCTION*ADE9000_RMS_FULL_SCALE_CODES*SQRT(2)) 
CAL_VRMS_CC in uV/code 				= 10^6/(VOLTAGE_TRANSFER_FUNCTION*ADE9000_RMS_FULL_SCALE_CODES*SQRT(2))
CAL_POWER_CC in mW/code 			= 10^3/(CURRENT_TRANSFER_FUNCTION*VOLTAGE_TRANSFER_FUNCTION*ADE9000_WATT_FULL_SCALE_CODES*2)
CAL_ENERGY_CC in uWhr/xTHR_HI code 	= 10^6/(CURRENT_TRANSFER_FUNCTION*VOLTAGE_TRANSFER_FUNCTION*ADE9000_WATT_FULL_SCALE_CODES*2*8000*3600*2^-13)

If conversion constants are set correctly, the register codes are converted to physical parameters as:
e.g Channel A Vrms = (AVRMS(register)*CAL_VRMS_CC/10^6) Channel A Active Power = (AWATT(register)*CAL_POWER_CC/10^3)	
*****************************************************************************************************************/
#define CAL_IRMS_CC (double)ONE_MILLION / (CURRENT_TRANSFER_FUNCTION * ADE9000_RMS_FULL_SCALE_CODES * SQRT_TO_2)                                                       // Conversion constants (uA/code)
#define CAL_VRMS_CC (double)ONE_MILLION / (VOLTAGE_TRANSFER_FUNCTION * ADE9000_RMS_FULL_SCALE_CODES * SQRT_TO_2)                                                       // Conversion constants (uV/code)
#define CAL_POWER_CC (double)ONE_THOUSAND / (VOLTAGE_TRANSFER_FUNCTION * CURRENT_TRANSFER_FUNCTION * ADE9000_WATT_FULL_SCALE_CODES * 2)                                // Conversion constants (mW/code) Applicable for Active, reactive and apparent power
#define CAL_ENERGY_CC (double)ONE_MILLION / (VOLTAGE_TRANSFER_FUNCTION * CURRENT_TRANSFER_FUNCTION * ADE9000_WATT_FULL_SCALE_CODES * 2 * 8000 * 3600 * (1.0 / 8192.0)) //  Conversion constants (uWhr/xTHR_HI code)Applicable for Active, reactive and apparent energy

/****************************************************************************************************************
 Structures and Global Variables
****************************************************************************************************************/

struct ResampledWfbData
{
  int16_t VA_Resampled[WFB_ELEMENT_ARRAY_SIZE];
  int16_t IA_Resampled[WFB_ELEMENT_ARRAY_SIZE];
  int16_t VB_Resampled[WFB_ELEMENT_ARRAY_SIZE];
  int16_t IB_Resampled[WFB_ELEMENT_ARRAY_SIZE];
  int16_t VC_Resampled[WFB_ELEMENT_ARRAY_SIZE];
  int16_t IC_Resampled[WFB_ELEMENT_ARRAY_SIZE];
  int16_t IN_Resampled[WFB_ELEMENT_ARRAY_SIZE];
};

struct ActivePowerRegs
{
  int32_t ActivePowerReg_A;
  int32_t ActivePowerReg_B;
  int32_t ActivePowerReg_C;
};

struct ReactivePowerRegs
{
  int32_t ReactivePowerReg_A;
  int32_t ReactivePowerReg_B;
  int32_t ReactivePowerReg_C;
};

struct ApparentPowerRegs
{
  int32_t ApparentPowerReg_A;
  int32_t ApparentPowerReg_B;
  int32_t ApparentPowerReg_C;
};

struct VoltageRMSRegs
{
  int32_t VoltageRMSReg_A;
  int32_t VoltageRMSReg_B;
  int32_t VoltageRMSReg_C;
};

struct CurrentRMSRegs
{
  int32_t CurrentRMSReg_A;
  int32_t CurrentRMSReg_B;
  int32_t CurrentRMSReg_C;
  int32_t CurrentRMSReg_N;
};

struct PGAGainRegs
{
  int8_t VoltagePGA_gain;
  int8_t CurrentPGA_gain;
};

// struct FundActivePowerRegs
// {
//   int32_t FundActivePowerReg_A;
//   int32_t FundActivePowerReg_B;
//   int32_t FundActivePowerReg_C;
// };

// struct FundReactivePowerRegs
// {
//   int32_t FundReactivePowerReg_A;
//   int32_t FundReactivePowerReg_B;
//   int32_t FundReactivePowerReg_C;
// };

// struct FundApparentPowerRegs
// {
//   int32_t FundApparentPowerReg_A;
//   int32_t FundApparentPowerReg_B;
//   int32_t FundApparentPowerReg_C;
// };

// struct FundVoltageRMSRegs
// {
//   int32_t FundVoltageRMSReg_A;
//   int32_t FundVoltageRMSReg_B;
//   int32_t FundVoltageRMSReg_C;
// };

// struct FundCurrentRMSRegs
// {
//   int32_t FundCurrentRMSReg_A;
//   int32_t FundCurrentRMSReg_B;
//   int32_t FundCurrentRMSReg_C;
//   //Fundamental neutral RMS is not calculated
// };

// struct HalfVoltageRMSRegs
// {
//   int32_t HalfVoltageRMSReg_A;
//   int32_t HalfVoltageRMSReg_B;
//   int32_t HalfVoltageRMSReg_C;
// };

// struct HalfCurrentRMSRegs
// {
//   int32_t HalfCurrentRMSReg_A;
//   int32_t HalfCurrentRMSReg_B;
//   int32_t HalfCurrentRMSReg_C;
//   int32_t HalfCurrentRMSReg_N;
// };

// struct Ten12VoltageRMSRegs
// {
//   int32_t Ten12VoltageRMSReg_A;
//   int32_t Ten12VoltageRMSReg_B;
//   int32_t Ten12VoltageRMSReg_C;
// };

// struct Ten12CurrentRMSRegs
// {
//   int32_t Ten12CurrentRMSReg_A;
//   int32_t Ten12CurrentRMSReg_B;
//   int32_t Ten12CurrentRMSReg_C;
//   int32_t Ten12CurrentRMSReg_N;
// };

// struct VoltageTHDRegs
// {
//   int32_t VoltageTHDReg_A;
//   int32_t VoltageTHDReg_B;
//   int32_t VoltageTHDReg_C;
//   float VoltageTHDValue_A;
//   float VoltageTHDValue_B;
//   float VoltageTHDValue_C;
// };

// struct CurrentTHDRegs
// {
//   int32_t CurrentTHDReg_A;
//   int32_t CurrentTHDReg_B;
//   int32_t CurrentTHDReg_C;
//   float CurrentTHDValue_A;
//   float CurrentTHDValue_B;
//   float CurrentTHDValue_C;
// };

struct PowerFactorRegs
{
  int32_t PowerFactorReg_A;
  int32_t PowerFactorReg_B;
  int32_t PowerFactorReg_C;
  float PowerFactorValue_A;
  float PowerFactorValue_B;
  float PowerFactorValue_C;
};

struct PeriodRegs
{
  int32_t PeriodReg_A;
  int32_t PeriodReg_B;
  int32_t PeriodReg_C;
  float FrequencyValue_A;
  float FrequencyValue_B;
  float FrequencyValue_C;
};

struct AngleRegs
{
  int16_t AngleReg_VA_VB;
  int16_t AngleReg_VB_VC;
  int16_t AngleReg_VA_VC;
  int16_t AngleReg_VA_IA;
  int16_t AngleReg_VB_IB;
  int16_t AngleReg_VC_IC;
  int16_t AngleReg_IA_IB;
  int16_t AngleReg_IB_IC;
  int16_t AngleReg_IA_IC;
  float AngleValue_VA_VB;
  float AngleValue_VB_VC;
  float AngleValue_VA_VC;
  float AngleValue_VA_IA;
  float AngleValue_VB_IB;
  float AngleValue_VC_IC;
  float AngleValue_IA_IB;
  float AngleValue_IB_IC;
  float AngleValue_IA_IC;
};

struct TemperatureRegnValue
{
  int16_t Temperature_Reg;
  float Temperature;
};

class ADE9000
{
public:
  ADE9000();

  /*
  Initializes the ADE9000. The initial settings for registers are defined in ADE9000API.h header file
  Input: Register settings in header files
  Output: 
  */
  void setupADE9000(void);

  /*----- SPI Functions -----*/

  /*
  Initializes the arduino SPI port using SPI.h library
  Input: SPI speed, chip select pin
  Output:-
  */
  void SPI_Init(uint32_t SPI_speed, uint8_t chipSelect_Pin);

  /* 
  Writes 16bit data to a 16 bit register. 
  Input: Register address, data
  Output:-
  */
  void SPI_Write_16(uint16_t Address, uint16_t Data);

  /* 
  Writes 32bit data to a 32 bit register. 
  Input: Register address, data
  Output:-
  */
  void SPI_Write_32(uint16_t Address, uint32_t Data);

  /* 
  Reads 16bit data from register. 
  Input: Register address
  Output: 16 bit data
  */
  uint16_t SPI_Read_16(uint16_t Address);

  /* 
  Reads 32bit data from register. 
  Input: Register address
  Output: 32 bit data
  */
  uint32_t SPI_Read_32(uint16_t Address);
  // void SPI_Burst_Read_Resampled_Wfb(uint16_t Address, uint16_t Read_Element_Length, ResampledWfbData *ResampledData);

  /*----- ADE9000 Calculated Parameter Read Functions -----*/

  /* 
  Reads the Active power registers AWATT,BWATT and CWATT
  Input: Structure name
  Output: Active power codes stored in respective structure
  */
  void ReadActivePowerRegs(ActivePowerRegs *Data);

  /* 
  Reads the Reactive power registers AVAR,BVAR and CVAR
  Input: Structure name
  Output: Reactive power codes stored in respective structure
  */
  void ReadReactivePowerRegs(ReactivePowerRegs *Data);
  void ReadApparentPowerRegs(ApparentPowerRegs *Data);
  void ReadVoltageRMSRegs(VoltageRMSRegs *Data);
  void ReadCurrentRMSRegs(CurrentRMSRegs *Data);
  // void ReadFundActivePowerRegs(FundActivePowerRegs *Data);
  // void ReadFundReactivePowerRegs(FundReactivePowerRegs *Data);
  // void ReadFundApparentPowerRegs(FundApparentPowerRegs *Data);
  // void ReadFundVoltageRMSRegs(FundVoltageRMSRegs *Data);
  // void ReadFundCurrentRMSRegs(FundCurrentRMSRegs *Data);
  // void ReadHalfVoltageRMSRegs(HalfVoltageRMSRegs *Data);
  // void ReadHalfCurrentRMSRegs(HalfCurrentRMSRegs *Data);
  // void ReadTen12VoltageRMSRegs(Ten12VoltageRMSRegs *Data);
  // void ReadTen12CurrentRMSRegs(Ten12CurrentRMSRegs *Data);
  // void ReadVoltageTHDRegsnValues(VoltageTHDRegs *Data);
  // void ReadCurrentTHDRegsnValues(CurrentTHDRegs *Data);
  void ReadPowerFactorRegsnValues(PowerFactorRegs *Data);
  void ReadPeriodRegsnValues(PeriodRegs *Data);
  void ReadAngleRegsnValues(AngleRegs *Data);
  void ReadTempRegnValue(TemperatureRegnValue *Data);

  /*----- ADE9000 Calculated Physical parameters -----*/

  /* 
  Convert the codes of registers AVRMS, BVRMS and CVRMS and converted to physical parameters
  Input: code value
  Output: Volts RMS physical parameter (Vrms)
  */
  double convertCodeToVolts(int32_t value);

  /* 
  Convert the codes of registers AIRMS, BIRMS and CIRMS and converted to physical parameters
  Input: code value
  Output: Volts RMS physical parameter (Arms)
  */
  double convertCodeToAmperes(int32_t value);

  /* 
  Convert the codes of registers to power
  Input: code value
  Output: Power physical parameter (Watt, VA, VAR)
  */
  double convertCodeToPower(int32_t value);

  /*----- ADE9000 Calibration functions -----*/

  /* 
  Get gain calibration of PGA of register PGA_GAIN 
  Input:-
  Output:- PGA_GAIN (1, 2 or 4)
  */
  void getPGA_gain(PGAGainRegs *);

  /* 
  Current gain calibration function
  Input: Stored in respective structure
  Output:-
  */
  void iGain_calibrate(int32_t *, int32_t *, int, uint8_t);

  /* 
  Voltage gain calibration function
  Input: Stored in respective structure
  Output:-
  */
  void vGain_calibrate(int32_t *, int32_t *, int, uint8_t);

  /* 
  Phase gain calibration function
  Input: Stored in respective structure
  Output:-
  */
  void phase_calibrate(int32_t *, int32_t *, int32_t *, int);

  /* 
  Power gain calibration function
  Input: Stored in respective structure
  Output:-
  */
  void pGain_calibrate(int32_t *, int32_t *, int, uint8_t, uint8_t, float);

  void updateEnergyRegisterFromInterrupt(uint32_t (&)[EGY_REG_SIZE], uint32_t (&)[EGY_REG_SIZE]);

private:
  uint8_t _chipSelect_Pin;
};

#endif