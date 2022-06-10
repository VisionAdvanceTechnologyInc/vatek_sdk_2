#ifndef _AVL68XX_TYPE_DEFINE_
#define _AVL68XX_TYPE_DEFINE_

#include <stdint.h>


typedef enum _AVL_Demod_Xtal
{
	Xtal_Null = -1,
    Xtal_30M = 0,
    Xtal_16M,
    Xtal_24M,
    Xtal_27M
}AVL_Demod_Xtal;

#define is_current_xtal(t)     (t >= Xtal_30M && t <= Xtal_27M)

typedef enum _AVL_DemodMode
{
    AVL_DemodNull = -1,
    AVL_DVBC = 0,
    AVL_DVBSX = 1,
    AVL_DVBTX = 2,
    AVL_ISDBT = 3,
    AVL_DTMB = 4,
    AVL_ISDBS = 5,
    AVL_ABSS = 6,
    AVL_ATSC = 7,
    AVL_DVBC2 = 8,
}AVL_DemodMode;

#define is_current_demodmode(m)  (m >= AVL_DVBC && m <= AVL_DVBC2)

typedef enum _AVL_TSMode
{
    AVL_TS_PARALLEL = 0, 
    AVL_TS_SERIAL =   1
}AVL_TSMode; 

typedef enum _AVL_TSClockEdge
{
    AVL_MPCM_FALLING   =   0, 
    AVL_MPCM_RISING    =   1  
}AVL_TSClockEdge; 

typedef enum _AVL_TSClockMode
{
    AVL_TS_CONTINUOUS_ENABLE = 0,   
    AVL_TS_CONTINUOUS_DISABLE = 1      
}AVL_TSClockMode; 

typedef enum _AVL_TSSerialPin
{
    AVL_MPSP_DATA0  =   0, 
    AVL_MPSP_DATA7  =   1  
}AVL_TSSerialPin; 

typedef enum _AVL_TSSerialOrder
{
    AVL_MPBO_LSB = 0, 
    AVL_MPBO_MSB = 1 
}AVL_TSSerialOrder; 

typedef enum _AVL_TSSerialSyncPulse
{
    AVL_TS_SERIAL_SYNC_8_PULSE    =   0,         
    AVL_TS_SERIAL_SYNC_1_PULSE      =   1        
}AVL_TSSerialSyncPulse; 

typedef enum _AVL_TSErrorBit
{
    AVL_TS_ERROR_BIT_DISABLE  =   0,  
    AVL_TS_ERROR_BIT_ENABLE   =   1  
}AVL_TSErrorBit; 

typedef enum _AVL_TSErrorPolarity
{
    AVL_MPEP_Normal = 0,  
    AVL_MPEP_Invert = 1  
}AVL_TSErrorPolarity; 

typedef enum _AVL_TSValidPolarity
{
    AVL_MPVP_Normal     =   0, 
    AVL_MPVP_Invert     =   1   
}AVL_TSValidPolarity; 

typedef enum _AVL_TSPacketLen
{
    AVL_TS_188 = 0,
    AVL_TS_204 = 1
}AVL_TSPacketLen; 

typedef enum _AVL_TSParallelOrder
{
    AVL_TS_PARALLEL_ORDER_INVERT =   0,
    AVL_TS_PARALLEL_ORDER_NORMAL =   1
}AVL_TSParallelOrder; 

typedef enum _AVL_TSParallelPhase
{
    AVL_TS_PARALLEL_PHASE_0 = 0,
    AVL_TS_PARALLEL_PHASE_1 = 1,
    AVL_TSG_PARALLEL_PHASE_2 = 2,
    AVL_TS_PARALLEL_PHASE_3 = 3
}AVL_TSParallelPhase;

typedef struct _avl_ts_param
{
    AVL_TSMode outputmode;
    AVL_TSClockEdge clockedge;
    AVL_TSClockMode clockmode;
    AVL_TSSerialPin serialpin;
    AVL_TSSerialOrder serialorder;
    AVL_TSSerialSyncPulse serialsyncpulse;
    AVL_TSParallelOrder parallelorder;
    AVL_TSParallelPhase parallelphase;
    AVL_TSErrorBit errbit;
    AVL_TSErrorPolarity errpolarity;
    AVL_TSValidPolarity validpolarity;
    AVL_TSPacketLen packagelen;
}avl_ts_param,*Pavl_ts_param;

typedef enum _AVL_GPIOPinNumber
{
    AVL_Pin37 = 0,
    AVL_Pin38 = 1,
    AVL_Pin15 = 2  //CS_0 PIN   
}AVL_GPIOPinNumber;

typedef enum _AVL_GPIOPinValue
{
    AVL_LOGIC_0 = 0,
    AVL_LOGIC_1 = 1,
    AVL_HIGH_Z = 2
}AVL_GPIOPinValue;

typedef enum _AVL_AGCPola
{
    AVL_AGC_NORMAL  =   0,        //  normal AGC polarity. Used for a tuner whose gain increases with increased AGC voltage.
    AVL_AGC_INVERTED=   1         //  inverted AGC polarity. Used for tuner whose gain decreases with increased AGC voltage.
}AVL_AGCPola;

typedef enum _AVL_InputPath
{
    AVL_IF_I,
    AVL_IF_Q
}AVL_InputPath;

typedef enum _AVL_DVBC_Standard
{
    AVL_DVBC_J83A    =   0,           //the J83A standard
    AVL_DVBC_J83B    =   1,           //the J83B standard
    AVL_DVBC_UNKNOWN =   2
}AVL_DVBC_Standard;

//Input_select enumeration definitions
typedef enum _AVL_DDC_Input
{
    AVL_DIG_IN       =   0,
    AVL_ADC_IN       =   1,
    AVL_VEC_IN       =   2,
    AVL_VEC1x_IN     =   3,
    AVL_DIG1x_IN     =   4
}AVL_DDC_Input;

typedef enum _AVL_ADC_Output_format
{
    AVL_2COMP    =   0,
    AVL_OFFBIN   =   1
}AVL_ADC_Output_format;

// Defines the AVL device spectrum inversion mode
typedef enum _AVL_SpectrumInversion
{
    AVL_SPECTRUM_NORMAL = 0,                      // Signal spectrum in normal.
    AVL_SPECTRUM_INVERTED = 1,                      // Signal spectrum in inverted.
    AVL_SPECTRUM_AUTO = 2                       // Signal spectrum isn't known.
}AVL_SpectrumInversion;

typedef enum _AVL_LockStatus
{
    AVL_STATUS_UNLOCK   =   0,                          // channel isn't locked
    AVL_STATUS_LOCK     =   1                           // channel is in locked state.
}AVL_LockStatus;

typedef struct _AVL_PLL_Conf0
{
        uint32_t m_RefFrequency_Hz;                   // The reference clock frequency in units of Hz.
        uint8_t m_PLL_CoreClock_DivR;                 // PLL reference clock divider value
        uint8_t m_PLL_CoreClock_DivF;                 // PLL feedback clock divider value
        uint8_t m_PLL_CoreClock_DivQ;                 // PLL feedback clock divider value
        uint8_t m_PLL_MPEGClock_DivR;                 // PLL reference clock divider value
        uint8_t m_PLL_MPEGClock_DivF;                 // PLL feedback clock divider value
        uint8_t m_PLL_MPEGClock_DivQ;                 // PLL feedback clock divider value
        uint8_t m_PLL_ADCClock_DivR;                  // PLL reference clock divider value
        uint8_t m_PLL_ADCClock_DivF;                  // PLL feedback clock divider value
        uint8_t m_PLL_ADCClock_DivQ;                  // PLL feedback clock divider value
        uint32_t m_CoreFrequency_Hz;                  // The internal core clock frequency in units of Hz.
        uint32_t m_MPEGFrequency_Hz;                  // The MPEG clock frequency in units of Hz.
        uint32_t m_ADCFrequency_Hz;                   // The ADC clock frequency in units of Hz.
        uint8_t m_PLL_DDCClock_sel;                   // ddc_clk source select (Emerald2 only)
        uint8_t m_PLL_DDCClock_DivQ;                  // PLL feedback clock divider value(Emerald2 only)
        uint8_t m_PLL_SDRAMClock_sel;                 // sdram_clk source select (Emerald2 only)
        uint8_t m_PLL_SDRAMClock_DivQ;                // PLL feedback clock divider value(Emerald2 only)
        uint32_t m_DDCFrequency_Hz;                   // The DDC clock frequency in units of Hz (Emerald2 only).
        uint32_t m_SDRAMFrequency_Hz;                 // The SDRAM clock frequency in units of Hz (Emerald2 only).
}AVL_PLL_Conf0;

typedef AVL_PLL_Conf0* PAVL_PLL_Conf0;

#endif
