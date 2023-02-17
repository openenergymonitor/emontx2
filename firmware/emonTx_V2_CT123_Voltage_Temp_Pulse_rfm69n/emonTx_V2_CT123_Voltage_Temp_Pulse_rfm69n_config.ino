/*
Configuration functions for emonTx_CT123_Voltage_Temp_Pulse_rfm69n.ino
*/


#include <Arduino.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <emonEProm.h>

 // Available Serial Commands
const PROGMEM char helpText1[] =                                
"\n"
"Available commands:\n"
"l\t\t- list the settings\n"
"r\t\t- restore sketch defaults\n"
"s\t\t- save settings to EEPROM\n"
"v\t\t- show firmware version\n"
"z\t\t- zero energy values\n"
"x\t\t- exit, lock and continue\n"
"?\t\t- show this text again\n"
"\n"
"w<x>\t\t- turn RFM Wireless data off: x = 0 or on: x = 1\n"
"b<n>\t\t- set r.f. band n = a single numeral: 4 = 433MHz, 8 = 868MHz, 9 = 915MHz (may require hardware change)\n"
"p<nn>\t\t- set the r.f. power. nn - an integer 0 - 31 representing -18 dBm to +13 dBm. Default: 25 (+7 dBm)\n"
"g<nnn>\t\t- set Network Group  nnn - an integer (OEM default = 210)\n"
"n<nn>\t\t- set node ID n= an integer (standard node ids are 1..60)\n"
"\n"
"d<xx.x>\t\t- xx.x = a floating point number for the datalogging period\n" 
"c<n>\t\t- n = 0 for OFF, n = 1 for ON, enable voltage, current & power factor values to serial output for calibration. \n"
"f<xx>\t\t- xx = the line frequency in Hz: normally either 50 or 60\n"
"k<x> <yy.y> <zz.z>\n"
"\t\t- Calibrate an analogue input channel:\n"
"\t\t- x = a single numeral: 0 = voltage calibration, 1 = ct1 calibration, 2 = ct2 calibration, etc\n"
"\t\t- yy.y = a floating point number for the voltage/current calibration constant\n"
"\t\t- zz.z = a floating point number for the phase calibration for this c.t. (z is not needed, or ignored if supplied, when x = 0)\n"
"\t\t-  e.g. k0 256.8\n"
"\t\t-       k1 90.9 2.00\n"
"a<xx.x>\t\t- xx.x = a floating point number for the assumed voltage if no a.c. is detected\n"
"m<x> <yy>\t- meter pulse counting:\n"
"\t\t\tx = 0 for OFF, x = 1 for ON, <yy> = an integer for the pulse minimum period in ms. (y is not needed, or ignored when x = 0)\n"
"t0 <y>\t\t- turn temperature measurement on or off:\n"
"\t\t\ty = 0 for OFF, y = 1 for ON\n"
"t<x> <yy> <yy> <yy> <yy> <yy> <yy> <yy> <yy>\n"
"\t\t- change a temperature sensor's address or position:\n"
"\t\t- x = a single numeral: the position of the sensor in the list (1-based)\n"
"\t\t- yy = 8 hexadecimal bytes representing the sensor's address\n"
"\t\t\te.g.  28 81 43 31 07 00 00 D9\n"
"\t\t\tN.B. Sensors CANNOT be added.\n"
;

extern DeviceAddress *temperatureSensors;

static void load_config(bool verbose)
{
  if ((eepromRead(eepromSig, (byte *)&EEProm)) )
  {  
    if (verbose)
      Serial.println(F("Loaded EEPROM config"));
  }
  else
  {
    if (verbose)
      Serial.println(F("No EEPROM config"));
  }
  if (verbose)
    list_calibration();
}

static void list_calibration(void)
{
  Serial.println(F("Settings:"));
  Serial.print(F("Band ")); 
  Serial.print(EEProm.RF_freq == RF69_433MHZ ? 433 : 
               EEProm.RF_freq == RF69_868MHZ ? 868 :
               EEProm.RF_freq == RF69_915MHZ ? 915 : 0);
  Serial.print(F(" MHz, Group ")); Serial.print(EEProm.networkGroup);
  Serial.print(F(", Node ")); Serial.print(EEProm.nodeID & 0x3F);
  Serial.print(F(", "));Serial.print(EEProm.rfPower - 18);Serial.println(F(" dBm"));
  Serial.println(F("Calibration:"));
  Serial.print(F("vCal = ")); Serial.println(EEProm.vCal);
  Serial.print(F("assumedV = ")); Serial.println(EEProm.assumedVrms);
  Serial.print(F("i1Cal = ")); Serial.println(EEProm.i1Cal);
  Serial.print(F("i1Lead = ")); Serial.println(EEProm.i1Lead);
  Serial.print(F("i2Cal = ")); Serial.println(EEProm.i2Cal);
  Serial.print(F("i2Lead = ")); Serial.println(EEProm.i2Lead);
  Serial.print(F("i3Cal = ")); Serial.println(EEProm.i3Cal);
  Serial.print(F("i3Lead = ")); Serial.println(EEProm.i3Lead);
  Serial.print(F("datalog = ")); Serial.println(EEProm.period);
  Serial.print(F("pulses = ")); Serial.println(EEProm.pulse_enable);
  Serial.print(F("pulse period = ")); Serial.println(EEProm.pulse_period);
  Serial.print(F("temp_enable = ")); Serial.println(EEProm.temp_enable);
  printTemperatureSensorAddresses();
  Serial.println(EEProm.rf_on ? F("RF on"):F("RF off"));
}

static void save_config()
{
  Serial.println(F("Saving..."));
  eepromWrite(eepromSig, (byte *)&EEProm, sizeof(EEProm));
  eepromPrint();
  Serial.println();
  Serial.println(F("Done. New config saved to EEPROM"));
}

static void wipe_eeprom(void)
{
  Serial.println(F("Resetting..."));
  
  eepromHide(eepromSig);   
  Serial.println(F("Sketch will now restart using default config."));
  delay(200);
}

void softReset(void)
{
  asm volatile ("  jmp 0");
}


int getPass(void)
{
/*
 * Get and verify the user's access code (replaces 'readString()' saves 1300 bytes)
 */

  char buf[5];
  if (Serial.readBytes(buf, 5) != 5)
    return 0;
  // Verbose
  if  (buf[0] == '+' 
    && buf[1] == '+'
    && buf[3] == '\r'
    && buf[4] == '\n')
  {
    if (buf[2] == '+')      // Verbose
    {
      calibration_enable = true;
      return 2;
    }
    else if (buf[2] == 's') //Silent
    {
      calibration_enable = true;
      return 1;
    }
  }
  return 0;
}


void getSettings(void)
{
/*
 * Reads settings information (if available and permitted) from the serial port
 *  see the user instruction above, the comments below or the separate documentation for details
 *
 * Data is expected generally in the format
 * 
 *  [l] [x] [y] [z]
 * 
 * where:
 *  [l] = a single letter denoting the variable to adjust
 *  [x] [y] [z] etc are values to be set.
 * 
 */
 
  if (Serial.available())
  {
    if (!calibration_enable) 
    {
      char pass_result;
      if ((pass_result = getPass()))
      {
        Serial.println(F("Entering Settings mode..."));
        if (pass_result == 2) 
          showString(helpText1);
      }
    }
    else
    {
      char c = Serial.read();
      int k1;
      double k2, k3; 
      switch (c) {
        case 'a':
          EEProm.assumedVrms = Serial.parseFloat();
          Serial.print(F("Assumed V: "));Serial.println(EEProm.assumedVrms);
          EmonLibCM_setAssumedVrms(EEProm.assumedVrms);
          break;
          
        case 'b':  // set band: 4 = 433, 8 = 868, 9 = 915
          EEProm.RF_freq = bandToFreq(Serial.parseInt());
          Serial.print(EEProm.RF_freq == RF69_433MHZ ? 433 : 
                       EEProm.RF_freq == RF69_868MHZ ? 868 :
                       EEProm.RF_freq == RF69_915MHZ ? 915 : 0);
          Serial.println(F(" MHz"));
          break;

        case 'c':
          /*
          * Format expected: c0 | c1
          */
          k1 = Serial.parseInt();
          switch (k1) {
            case 1 : EEProm.showCurrents = true;
              break;
            default: EEProm.showCurrents = false;
          }
          break;
        
        case 'd':
          /*  Format expected: d[x]
           * 
           * where:
           *  [x] = a floating point number for the datalogging period in s
           */
          k2 = Serial.parseFloat(); 
          EmonLibCM_datalog_period(k2); 
          EEProm.period = k2;
          Serial.print(F("datalog period: ")); Serial.print(k2);Serial.println(F(" s"));
          break;

        case 'f':
          /*
          *  Format expected: f50 | f60
          */
          k1 = Serial.parseFloat();
          EmonLibCM_cycles_per_second(k1);
          Serial.print(F("Freq: "));Serial.println(k1);
          break;
        
        case 'g':  // set network group
          EEProm.networkGroup = Serial.parseInt();
          Serial.print(F("Group ")); Serial.println(EEProm.networkGroup);
          break;
          
        /* case 'i' below */
          
        case 'k':
          /*  Format expected: k[x] [y] [z]
          * 
          * where:
          *  [x] = a single numeral: 0 = voltage calibration, 1 = ct1 calibration, 2 = ct2 calibration, etc
          *  [y] = a floating point number for the voltage/current calibration constant
          *  [z] = a floating point number for the phase calibration for this c.t. (z is not needed, or ignored if supplied, when x = 0)
          * 
          * e.g. k0 256.8
          *      k1 90.9 1.7 
          * 
          * If power factor is not displayed, it is impossible to calibrate for phase errors,
          *  and the standard value of phase calibration MUST BE SENT when a current calibration is changed.
          */
          k1 = Serial.parseInt(); 
          k2 = Serial.parseFloat(); 
          k3 = Serial.parseFloat(); 
                
          // Re-calculate intermediate values, write the values back.
          switch (k1) {
            case 0 : EmonLibCM_ReCalibrate_VChannel(k2);
              if (USA)
                EEProm.vCal_USA = k2;
              else
                EEProm.vCal = k2;
              break;
                
            case 1 : 
              if (!boardType25)
                EmonLibCM_ReCalibrate_IChannel(3, k2, k3);
              else
                EmonLibCM_ReCalibrate_IChannel(1, k2, k3);
                
              EEProm.i1Cal = k2;
              EEProm.i1Lead = k3;
              break;

            case 2 : 
              if (!boardType25)
                EmonLibCM_ReCalibrate_IChannel(0, k2, k3);
              else
                EmonLibCM_ReCalibrate_IChannel(2, k2, k3);
              EEProm.i2Cal = k2;
              EEProm.i2Lead = k3;
              break;

            case 3 : 
              if (!boardType25)
                EmonLibCM_ReCalibrate_IChannel(1, k2, k3);
              else
                EmonLibCM_ReCalibrate_IChannel(3, k2, k3);
              EEProm.i3Cal = k2;
              EEProm.i3Lead = k3;
              break;

            default : ;
          }
          Serial.print(F("Cal: k"));Serial.print(k1);Serial.print(F(" "));Serial.print(k2);Serial.print(F(" "));Serial.println(k3);        
          break;
            
        case 'l':
          list_calibration(); // print the settings & calibration values
          break;
            
        case 'm' :
          /*  Format expected: m[x] [y]
           * 
           * where:
           *  [x] = a single numeral: 0 = pulses OFF, 1 = pulses ON,
           *  [y] = an integer for the pulse min period in ms - ignored when x=0
           */
          k1 = Serial.parseInt(); 
          k2 = Serial.parseInt(); 
          while (Serial.available())
            Serial.read(); 

          switch (k1) {
            case 0 : EmonLibCM_setPulseEnable(false);
              EEProm.pulse_enable = false;
              break;
            
            case 1 : EmonLibCM_setPulseMinPeriod(k2);
              EmonLibCM_setPulseEnable(true);
              EEProm.pulse_enable = true;
              EEProm.pulse_period = k2;
              break;
          }
          Serial.print(F("Pulses: "));
          if (k1)
            {Serial.print(k2);Serial.println(F(" ms"));}
          else
            Serial.println("off");        
          break;        
    
        case 'i':  
        case 'n':  //  Set NodeID - range expected: 1 - 60
          EEProm.nodeID = Serial.parseInt();
          EEProm.nodeID = constrain(EEProm.nodeID, 1, 63);
          Serial.print(F("Node ")); Serial.println(EEProm.nodeID);
          break;
          
        case 'p': // set RF power level
          EEProm.rfPower = (Serial.parseInt() & 0x1F);
          Serial.print(F("p = "));Serial.print(EEProm.rfPower - 18);Serial.println(F(" dBm"));
          break;
          
        case 'r': // restore sketch defaults
          while (Serial.available())
            Serial.read(); 
          wipe_eeprom();
          softReset();
          break;

        case 's': // Save to EEPROM. ATMega328p has 1kB  EEPROM
          save_config();
          break;

        case 't' : // Temperatures
          /*  Format expected: t[x] [y] [y] ...
           */
          set_temperatures();
          break;

        case 'v': // print firmware version
          Serial.print(F("emonTxV2CM RFM69n Continuous Monitoring V")); Serial.write(firmware_version);
          break;
        
        case 'w' :  // Wireless - RF Off / On
          /* Format expected: w[x]
           */
          EEProm.rf_on = Serial.parseInt(); 
          Serial.println(EEProm.rf_on ? "RF on":"RF off");
          break;
          
        case 'x':  // exit and continue
          Serial.println(F("Continuing..."));
          while (Serial.available())
            Serial.read(); 
          calibration_enable = false;
          return;

        case 'z':  // zero energy variables in EEPROM
          Serial.println(F("Energy values set to zero"));
          zeroEValues(); 
          for (byte n=0; n<3; n++)
            EmonLibCM_setWattHour(n, 0);
          EmonLibCM_setPulseCount(0);
          break;

        case '?':  // show Help text        
          showString(helpText1);
          Serial.println(F(" "));
          break;
        
        default:
          ;
      } //end switch
      while (Serial.available())
        Serial.read(); 
    }
  }
}


static byte bandToFreq (byte band) {
  return band == 4 ? RF69_433MHZ : band == 8 ? RF69_868MHZ : band == 9 ? RF69_915MHZ : 0;
}


static void showString (PGM_P s) {
  for (;;) {
    char c = pgm_read_byte(s++);
    if (c == 0)
      break;
    if (c == '\n')
      Serial.print('\r');
    Serial.print(c);
  }
}

void set_temperatures(void)
{
  /*  Format expected: t[x] [y] [y] ...
  * 
  * where:
  *  [x] = 0  [y] = single numeral: 0 = temperature measurement OFF, 1 = temperature measurement ON
  *  [x] = a single numeral > 0: the position of the sensor in the list (1-based)
  *  [y] = 8 hexadecimal bytes representing the sensor's address
  *          e.g. t2 28 81 43 31 07 00 00 D9
  */
    
  DeviceAddress sensorAddress;
         
  unsigned int k1 = Serial.parseInt();

  if (k1 == 0)
  { 
    // write to EEPROM
    EEProm.temp_enable = Serial.parseInt();
    EmonLibCM_TemperatureEnable(EEProm.temp_enable);
  }
  else if (k1 > sizeof(EEProm.allAddresses) / sizeof(DeviceAddress))
    return;
  else
  {
    byte i = 0, a = 0, b;
    Serial.readBytes(&b,1);     // expect a leading space
    while (Serial.readBytes(&b,1) && i < 8)
    {            
      if (b == ' ' || b == '\r' || b == '\n')
      {
        sensorAddress[i++] = a;
        a = 0;
      }                
      else
      {
        a *= 16;
        a += c2h(b);
      }          
    }     
    // set address
    for (byte i=0; i<8; i++)
      EEProm.allAddresses[k1-1][i] = sensorAddress[i];
  }
}

byte c2h(byte b)
{
  if (b > 47 && b < 58) 
    return b - 48;
  else if (b > 64 && b < 71) 
    return b - 55;
  else if (b > 96 && b < 103) 
    return b - 87;
  return 0;
}
