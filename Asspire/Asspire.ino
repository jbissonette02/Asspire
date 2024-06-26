// Adapted from:
//IMU example code (GY-521 module): Michael Schoeffler 2017, http://www.mschoeffler.de
//Pressure Sensor (MPS20N0040D): FILE: test_modulo_MPS20N0040D.ino; VERSION: 1.0   Description: Program for testing; pressure sensor module type MPS20N0040D; Created on 10/14/2018 by Adriano Gandolfo; Website http://www.adrirobot.it   Blog http://it.emcelettronica.com/author/adrirobot; Facebook page https://www.facebook.com/Adrirobot-318949048122955; Instagram https://www.instagram.com/adrirobot/ 

#include "Wire.h" // This library allows you to communicate with I2C devices.

#include  "HX711.h"  // analog-to-digital converter management library

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data

char tmp_str[7]; // temporary variable used in convert function

HX711 scale;

double tareValue;

char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void setup() {
  Serial.begin(9600);
  
  // setup for IMU
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  // setup for pressure sensor
  scale.begin (A1 ,  A0);
  scale.set_scale(2280.f); // this value is obtained by calibrating the scale with known weights
  tareValue = scale.get_units();

}
void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
  
  // print out data from IMU
  Serial.print(convert_int16_to_str(accelerometer_x));
  Serial.print("/"); 
  Serial.print(convert_int16_to_str(accelerometer_y));
  Serial.print("/"); 
  Serial.print(convert_int16_to_str(accelerometer_z));
  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  Serial.print("/");
  Serial.print(temperature/340.00+36.53);
  Serial.print("/"); 
  Serial.print(convert_int16_to_str(gyro_x));
  Serial.print("/"); 
  Serial.print(convert_int16_to_str(gyro_y));
  Serial.print("/"); 
  Serial.print(convert_int16_to_str(gyro_z));

  // print out data from pressure sensor
  if (millis() < 1000){
    tareValue = scale.get_units();
  }
  Serial.print("/");
  Serial.print(scale.get_units() - tareValue, 1);
  Serial.println();


  scale.power_down(); // puts the ADC into sleep 
  // delay
  delay(5);
  scale.power_up();
}