/*
  emonTxV2 Continuous Sampling
  using EmonLibCM https://github.com/openenergymonitor/EmonLibCM
  Authors: Robin Emley, Robert Wall, Trystan Lea
  
  -----------------------------------------
  Part of the openenergymonitor.org project
  Licence: GNU GPL V3



Change Log:
*/

const char *firmware_version = {"1.1.0\n\r"};
/*


v1.0.0: First release for Jeelib "RFM69 Native" format.
v1.1.0: LowPowerLabs support

emonhub.conf node decoder (nodeid is 15, unless changed)
See: https://github.com/openenergymonitor/emonhub/blob/emon-pi/configuration.md

P4 & E4 are retained for compatibility with emonTx V3 - will always report zero.

Copy the following into emonhub.conf:

[[15]]
  nodename = emontx2cm15
  [[[rx]]]
    names = MSG, Vrms, P1, P2, P3, P4, E1, E2, E3, E4, T1, T2, T3, pulse
    datacodes = L,h,h,h,h,h,l,l,l,l,h,h,h,L
    scales = 1,0.01,1,1,1,1,1,1,1,1,0.01,0.01,0.01,1
    units = n,V,W,W,W,W,Wh,Wh,Wh,Wh,C,C,C,p

*/
// Comment/Uncomment as applicable
//#define DEBUG                                              // Debug level print out

//#define EEWL_DEBUG

#define RFM69_JEELIB_CLASSIC 1
#define RFM69_JEELIB_NATIVE 2
#define RFM69_LOW_POWER_LABS 3
#define RadioFormat RFM69_LOW_POWER_LABS

#define EMONHUBTX3EINTERFACER                              // Wired serial connection format: Use EmonHubTx3eInterfacer format if an ESP8266 is used with emonhub. Default is space-separated values.
#define RFM12B                                             // Radio Module in use: RFM12B or RFM69CW
  
#define V22                                                // PCB version: V22 or V25 for V2.2 & V2.5 respectively


#define FACTORYTESTGROUP 1                                 // Transmit the Factory Test on Grp 1 
                                                           //   to avoid interference with recorded data at power-up.


#include <Arduino.h>
#include <avr/wdt.h>

#if RadioFormat == RFM69_LOW_POWER_LABS
  #include <RFM69.h>                             // RFM69 LowPowerLabs radio library
  RFM69 radio;
#elif RadioFormat == RFM69_JEELIB_CLASSIC
  #include <rfmTxLib.h>                          // RFM69 transmit-only library using "JeeLib RFM69 Native" message format
#elif RadioFormat == RFM69_JEELIB_NATIVE
  #include <rfm69nTxLib.h>                       // RFM69 transmit-only library using "JeeLib RFM69 Native" message format
#endif

#include <emonEProm.h>                                     // OEM EEPROM library
#include <emonLibCM.h>                                     // OEM Continuous Monitoring library
// Include emonTx_CT123_Voltage_Temp_Pulse_rfm69n_config.ino in the same directory for settings functions & data

// Radio - checks for traffic
const int busyThreshold = -97;                             // Signal level below which the radio channel is clear to transmit
const byte busyTimeout = 15;                               // Time in ms to wait for the channel to become clear, before transmitting anyway

struct {
    unsigned long Msg;
    int Vrms,P1,P2,P3,P4;                                  // P4 & E4 retained for compatibility - will always report zero
    long E1,E2,E3,E4; 
    int T1,T2,T3;
    unsigned long pulse;
} emontx;                                                  // create a data packet for the RFM
static void showString (PGM_P s);
 
#define MAX_TEMPS 3                                        // The maximum number of temperature sensors
//---------------------------- emonTx Settings - Stored in EEPROM and shared with config.ino ------------------------------------------------
struct {
  byte RF_freq = RF69_433MHZ;                               // Frequency of radio module can be RF69_433MHZ, RF69_868MHZ or RF69_915MHZ. 
  byte networkGroup = 210;                                 // wireless network group, must be the same as emonBase / emonPi and emonGLCD. OEM default is 210
  byte  nodeID = 15;                                       // node ID for this emonTx.
  byte  rf_on = 1;                                         // RF - 0 = no RF, 1 = RF on.
  byte  rfPower = 25;                                      // 7 = -10.5 dBm, 25 = +7 dBm for RFM12B; 0 = -18 dBm, 31 = +13 dBm for RFM69CW. Default = 25 (+7 dBm)
  float vCal  = 227.6;                                     // (240V x 11) / 11.6V = 227.6 Calibration for UK AC-AC adapter 77DB-06-09
  float vCal_USA = 110.0;                                  // (120V × 11) / 12.0V = 110.0  Calibration for US AC-AC adapter 77DA-10-09 
  float assumedVrms = 240.0;                               // Assumed Vrms when no a.c. is detected
  float lineFreq = 50;                                     // Line Frequency = 50 Hz
  float i1Cal = 111.1;                                     // (100 A / 50 mA / 18 Ohm burden) = 111.1
  float i1Lead = 1.0;                                      // 1.0° phase lead
  float i2Cal = 111.1;
  float i2Lead = 1.0;
  float i3Cal = 111.1;
  float i3Lead = 1.0;
  float period = 9.85;                                     // datalogging period - should be fractionally less than the PHPFINA database period in emonCMS
  bool  pulse_enable = true;                               // pulse counting
  int   pulse_period = 100;                                // pulse min period - 0 = no de-bounce
  bool  temp_enable = true;                                // enable temperature measurement
  DeviceAddress allAddresses[MAX_TEMPS];                   // sensor address data
  bool  showCurrents = false;                              // Print to serial voltage, current & p.f. values  
} EEProm;

uint16_t eepromSig = 0x0013;                               // oem EProm signature - see oemEProm Library documentation for details.
 

#ifdef EEWL_DEBUG
  extern EEWL EVmem;
#endif


DeviceAddress allAddresses[MAX_TEMPS];                     // Array to receive temperature sensor addresses
/*   Example - how to define temperature sensors, prevents an automatic search
DeviceAddress allAddresses[] = {       
    {0x28, 0x81, 0x43, 0x31, 0x7, 0x0, 0xFF, 0xD9}, 
    {0x28, 0x8D, 0xA5, 0xC7, 0x5, 0x0, 0x0, 0xD5},         // Use the actual addresses, as many as required
    {0x28, 0xC9, 0x58, 0x32, 0x7, 0x0, 0x0, 0x89}          // up to a maximum of 6    
};
*/

int allTemps[MAX_TEMPS];                                   // Array to receive temperature measurements

bool  USA=false;

bool calibration_enable = false;                           // Enable on-line calibration when running. 
                                                           //  For safety, thus MUST default to false. (Required due to faulty ESP8266 software.)

//----------------------------emonTx V2 hard-wired connections-----------------------------------
const byte LEDpin      = 9;  // emonTx LED
const byte DS18B20_PWR = 0;  // DS18B20 Power - emonTx V2.2: 0;  emonTx V2.5: 5

//---------------------------------CT availability status----------------------------------------
byte CT_count = 0;
bool CT1, CT2, CT3;     // Record if CT present during startup
bool boardType25 = false;

//----------------------------------------Setup--------------------------------------------------
void setup() 
{  
  wdt_enable(WDTO_8S);
  
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin,HIGH);
  
 
  // Serial---------------------------------------------------------------------------------
  Serial.begin(115200);
  // ---------------------------------------------------------------------------------------

//  #ifdef DEBUG
    Serial.print(F("emonTx V2 Continuous Monitoring V")); Serial.write(firmware_version);
    Serial.println(F("OpenEnergyMonitor.org"));
//  #else
    Serial.println(F("describe:"));
//  #endif
 
  load_config(true);                                                   // Load RF config from EEPROM (if any exists)
  if (EEProm.rf_on)
  {
    #ifdef DEBUG
      #ifdef RFM12B
      Serial.print(F("RFM12B "));
      #endif
      #ifdef RFM69CW
      Serial.print(F("RFM69CW "));
      #endif
      Serial.print(" Freq: ");
      if (EEProm.RF_freq == RF69_433MHZ) Serial.print(F("433MHz"));
      if (EEProm.RF_freq == RF69_868MHZ) Serial.print(F("868MHz"));
      if (EEProm.RF_freq == RF69_915MHZ) Serial.print(F("915MHz"));
      Serial.print(F(" Group: ")); Serial.print(EEProm.networkGroup);
      Serial.print(F(" Node: ")); Serial.print(EEProm.nodeID);
      Serial.println(" ");
    #endif
  }
 // Unable to detect presence of CT sensors - assume present ------------------------------------------------------------
  CT1 = 1; 
  CT2 = 1;
  CT3 = 1;
  CT_count = 3;

  // ---------------------------------------------------------------------------------------

  if (EEProm.rf_on)
  {
    Serial.println(F("Factory Test"));
    #if RadioFormat == RFM69_LOW_POWER_LABS
      radio.initialize(RF69_433MHZ,EEProm.nodeID,EEProm.networkGroup);  
      radio.encrypt("89txbe4p8aik5kt3");                                                      // initialize RFM
      radio.setPowerLevel(EEProm.rfPower);
    #else
      rfm_init();                                                        // initialize RFM
      for (int i=10; i>=0; i--)                                          // Send RF test sequence using test Group (for factory testing)
      {
        emontx.P1=i;
        rfm_send((byte *)&emontx, sizeof(emontx), FACTORYTESTGROUP, EEProm.nodeID, EEProm.RF_freq, EEProm.rfPower, busyThreshold, busyTimeout);
        delay(100);
      }
    #endif
    emontx.P1=0;
    delay(EEProm.nodeID * 20);                                         // try to avoid r.f. collisions at start-up
    
  }
  // ---------------------------------------------------------------------------------------
  
  digitalWrite(LEDpin,LOW);

  #ifdef DEBUG
    if (CT_count==0) {
      Serial.println(F("NO CT's detected"));
    } else {
      if (CT1) { Serial.print(F("CT1 detected, i1Cal:")); Serial.println(EEProm.i1Cal); }
      if (CT2) { Serial.print(F("CT2 detected, i2Cal:")); Serial.println(EEProm.i2Cal); }
      if (CT3) { Serial.print(F("CT3 detected, i3Cal:")); Serial.println(EEProm.i3Cal); }
    }
    delay(200);
  #endif
#ifdef EEWL_DEBUG
  Serial.print("End of mem=");Serial.print(E2END);
  Serial.print("  Avail mem=");Serial.print((E2END>>2) * 3);
  Serial.print("  Start addr=");Serial.print(E2END - (((E2END>>2) * 3) / (sizeof(mem)+1))*(sizeof(mem)+1));
  Serial.print("  Num blocks=");Serial.println(((E2END>>2) * 3) / 21);
//  EVmem.dump_buffer();
#endif
  // ----------------------------------------------------------------------------
  // EmonLibCM config
  // ----------------------------------------------------------------------------
  #ifdef V25
    boardType25 = true;
    EmonLibCM_SetADC_VChannel(0, EEProm.vCal);                         // ADC Input channel, voltage calibration - for Ideal UK Adapter = 268.97
    if (USA) EmonLibCM_SetADC_VChannel(0, EEProm.vCal_USA);            // ADC Input channel, voltage calibration - for US AC-AC adapter 77DA-10-09

    EmonLibCM_SetADC_IChannel(1, EEProm.i1Cal, EEProm.i1Lead);         // ADC Input channel, current calibration, phase calibration
    EmonLibCM_SetADC_IChannel(2, EEProm.i2Cal, EEProm.i2Lead);         // The current channels will be read in this order
    EmonLibCM_SetADC_IChannel(3, EEProm.i3Cal, EEProm.i3Lead);         // 111.1 for 100 A : 50 mA c.t. with 18Ω burden - v.t. leads c.t by ~1.0 degrees

  #else
 
    EmonLibCM_SetADC_VChannel(2, EEProm.vCal);                         // ADC Input channel, voltage calibration - for Ideal UK Adapter = 268.97
    if (USA) EmonLibCM_SetADC_VChannel(2, EEProm.vCal_USA);            // ADC Input channel, voltage calibration - for US AC-AC adapter 77DA-10-09

    EmonLibCM_SetADC_IChannel(3, EEProm.i1Cal, EEProm.i1Lead);         // ADC Input channel, current calibration, phase calibration
    EmonLibCM_SetADC_IChannel(0, EEProm.i2Cal, EEProm.i2Lead);         // The current channels will be read in this order
    EmonLibCM_SetADC_IChannel(1, EEProm.i3Cal, EEProm.i3Lead);         // 111.1 for 100 A : 50 mA c.t. with 18Ω burden - v.t. leads c.t by ~1.0 degrees
 #endif
  EmonLibCM_ADCCal(3.3);                                               // ADC Reference voltage, (3.3 V for emonTx,  5.0 V for Arduino)
  // mains frequency 50Hz
  if (USA) EmonLibCM_cycles_per_second(60);                            // mains frequency 60Hz
  EmonLibCM_datalog_period(EEProm.period);                             // period of readings in seconds - normal value for emoncms.org  
  EmonLibCM_setAssumedVrms(EEProm.assumedVrms);
  EmonLibCM_setPulseEnable(EEProm.pulse_enable);                       // Enable pulse counting
  EmonLibCM_setPulsePin(3);
  EmonLibCM_setPulseMinPeriod(EEProm.pulse_period);

  EmonLibCM_setTemperatureDataPin(5);                                  // OneWire data pin (emonTx V3.4)
  #ifdef V22
  EmonLibCM_setTemperaturePowerPin(-1);                                // Temperature sensor Power is permanent for V2.2
  #else
  EmonLibCM_setTemperaturePowerPin(A3);                                // Temperature sensor Power Pin - AIO 3  
  #endif
  EmonLibCM_setTemperatureResolution(11);                              // Resolution in bits, allowed values 9 - 12. 11-bit resolution, reads to 0.125 degC
  EmonLibCM_setTemperatureAddresses(EEProm.allAddresses);              // Name of array of temperature sensors
  EmonLibCM_setTemperatureArray(allTemps);                             // Name of array to receive temperature measurements
  EmonLibCM_setTemperatureMaxCount(3);                                 // Max number of sensors, limited by wiring and array size.
  {
    long e0=0, e1=0, e2=0, e3=0;
    unsigned long p=0;
    
    recoverEValues(&e0,&e1,&e2,&e3,&p);
    EmonLibCM_setWattHour(0, e0);
    EmonLibCM_setWattHour(1, e1);
    EmonLibCM_setWattHour(2, e2);
    EmonLibCM_setWattHour(3, e3);
    EmonLibCM_setPulseCount(p);
  }
#ifdef EEWL_DEBUG
  EVmem.dump_control();
  EVmem.dump_buffer();  
#endif
  EmonLibCM_TemperatureEnable(EEProm.temp_enable);  
  EmonLibCM_Init();                                                    // Start continuous monitoring.
  emontx.Msg = 0;
}

void loop()             
{
  getSettings();
  
  if (EmonLibCM_Ready())   
  {
    #ifdef DEBUG
    if (emontx.Msg==0) 
    {
      if (EmonLibCM_acPresent())
        Serial.println(F("AC present - Real Power calc enabled"));
      else
      {
        Serial.print(F("AC missing - Apparent Power calc enabled, assuming ")); Serial.print(EEProm.assumedVrms); Serial.println(F(" V"));
      }
    }
    delay(5);
    #endif

    emontx.Msg++;

    // Other options calculated by EmonLibCM
    // RMS Current:    EmonLibCM_getIrms(ch)
    // Apparent Power: EmonLibCM_getApparentPower(ch)
    // Power Factor:   EmonLibCM_getPF(ch)
    
    emontx.P1 = EmonLibCM_getRealPower(0); 
    emontx.E1 = EmonLibCM_getWattHour(0); 

    emontx.P2 = EmonLibCM_getRealPower(1); 
    emontx.E2 = EmonLibCM_getWattHour(1); 
    
    emontx.P3 = EmonLibCM_getRealPower(2); 
    emontx.E3 = EmonLibCM_getWattHour(2); 
  
    emontx.P4 = 0.0;                                                 // Retain for compatibility with emonTx V3's emonHub definition
    emontx.E4 = 0.0;

    emontx.Vrms = EmonLibCM_getVrms() * 100;
    
    emontx.T1 = allTemps[0];
    emontx.T2 = allTemps[1];
    emontx.T3 = allTemps[2];

    emontx.pulse = EmonLibCM_getPulseCount();
    
    if (EEProm.rf_on)
    {
      #if RadioFormat == RFM69_LOW_POWER_LABS
        radio.sendWithRetry(5, (const void*)(&emontx), sizeof(emontx));
        radio.sleep();
      #else
        rfm_send((byte *)&emontx, sizeof(emontx), EEProm.networkGroup, EEProm.nodeID, EEProm.RF_freq, EEProm.rfPower, busyThreshold, busyTimeout);     //send data
        delay(50);
      #endif
    }

#ifdef EMONHUBTX3EINTERFACER
      // ---------------------------------------------------------------------
      // Key:Value format, used by EmonESP & emonhub EmonHubTx3eInterfacer
      // ---------------------------------------------------------------------
      Serial.print(F("MSG:")); Serial.print(emontx.Msg);
      Serial.print(F(",Vrms:")); Serial.print(emontx.Vrms*0.01);
      
      if (CT1) { Serial.print(F(",P1:")); Serial.print(EmonLibCM_getRealPower(0)); }
      if (CT2) { Serial.print(F(",P2:")); Serial.print(EmonLibCM_getRealPower(1)); }
      if (CT3) { Serial.print(F(",P3:")); Serial.print(EmonLibCM_getRealPower(2)); }
      
      if (CT1) { Serial.print(F(",E1:")); Serial.print(EmonLibCM_getWattHour(0)); }
      if (CT2) { Serial.print(F(",E2:")); Serial.print(EmonLibCM_getWattHour(1)); }
      if (CT3) { Serial.print(F(",E3:")); Serial.print(EmonLibCM_getWattHour(2)); }
   
      if (emontx.T1!=30000) { Serial.print(F(",T1:")); Serial.print(emontx.T1*0.01); }
      if (emontx.T2!=30000) { Serial.print(F(",T2:")); Serial.print(emontx.T2*0.01); }
      if (emontx.T3!=30000) { Serial.print(F(",T3:")); Serial.print(emontx.T3*0.01); }

      Serial.print(F(",pulse:")); Serial.println(emontx.pulse);  
#else
      Serial.print(EEProm.nodeID); Serial.print(F(" "));
      Serial.print(emontx.Msg); Serial.print(F(" "));
      Serial.print(emontx.Vrms*0.01); Serial.print(F(" "));
         
      Serial.print(EmonLibCM_getRealPower(0)); Serial.print(F(" "));
      Serial.print(EmonLibCM_getRealPower(1)); Serial.print(F(" "));
      Serial.print(EmonLibCM_getRealPower(2)); Serial.print(F(" "));
      Serial.print(0); Serial.print(F(" "));
      
      Serial.print(EmonLibCM_getWattHour(0)); Serial.print(F(" "));
      Serial.print(EmonLibCM_getWattHour(1)); Serial.print(F(" "));
      Serial.print(EmonLibCM_getWattHour(2)); Serial.print(F(" "));
      Serial.print(0); Serial.print(F(" "));
      
      Serial.print(emontx.T1*0.01); Serial.print(F(" "));
      Serial.print(emontx.T2*0.01); Serial.print(F(" "));
      Serial.print(emontx.T3*0.01); Serial.print(F(" "));

      Serial.println(emontx.pulse);  
#endif
    delay(20);

    digitalWrite(LEDpin,HIGH); delay(50);digitalWrite(LEDpin,LOW);

    if (EEProm.showCurrents)
    {
      // to show voltage, current & power factor for calibration:
      Serial.print(F("Vrms:")); Serial.print(emontx.Vrms*0.01); 
      Serial.print(F(",I1:")); Serial.print(EmonLibCM_getIrms(0));
      Serial.print(F(",I2:")); Serial.print(EmonLibCM_getIrms(1));
      Serial.print(F(",I3:")); Serial.print(EmonLibCM_getIrms(2));

      Serial.print(F(",pf1:")); Serial.print(EmonLibCM_getPF(0),4);
      Serial.print(F(",pf2:")); Serial.print(EmonLibCM_getPF(1),4);
      Serial.print(F(",pf3:")); Serial.println(EmonLibCM_getPF(2),4);
    }
    // End of print out ----------------------------------------------------
     storeEValues(emontx.E1,emontx.E2,emontx.E3,0,emontx.pulse);
 }
  wdt_reset();
  delay(20);
}
