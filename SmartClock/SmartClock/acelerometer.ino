//Accelerometer Module for Smart Alarm Clock
#include <Wire.h>         // biblioteca de comunicação I2C

 
// Map some registers for MPU6050
const int MPU_ADDR =      0x68; // define sensor adress MPU6050 (0x68)
const int WHO_AM_I =      0x75; // device identification
const int PWR_MGMT_1 =    0x6B; // energy configuration
const int GYRO_CONFIG =   0x1B; // gyro configuration
const int ACCEL_CONFIG =  0x1C; // accelerometer configuration
const int ACCEL_XOUT =    0x3B; // X axis of accelerometer
 
const int sda_pin = D5; // I2C SDA pin
const int scl_pin = D6; // I2C SCL pin
 
bool led_state = false;
 
// Variables to store raw data
int AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; 
float mid = 0;
int count = 0;
int last[30];  
int sum = 0;
int walk = 0;
/*
 * Configure I2C binding to the pins
 * sda_pin -> D5
 * scl_pin -> D6
 */
void initI2C() 
{
  Wire.begin(sda_pin, scl_pin);
}
 
/*
 * Func to write value to MPU register
 */
void writeRegMPU(int reg, int val)
{
  Wire.beginTransmission(MPU_ADDR);     // Start communication with MPU6050
  Wire.write(reg);                      // send register to work with
  Wire.write(val);                      // write value to register
  Wire.endTransmission(true);           // ends transmition
}
 
/*
 * Func to read data from register
 */
uint8_t readRegMPU(uint8_t reg)     
{
  uint8_t data;
  Wire.beginTransmission(MPU_ADDR);     
  Wire.write(reg);                      
  Wire.endTransmission(false);          
  Wire.requestFrom(MPU_ADDR, 1);        
  data = Wire.read();                   
  return data;                          
}
 
/*
 * Function to find sensor at 0x68
 */
void findMPU(int mpu_addr)
{
  Wire.beginTransmission(MPU_ADDR);
  int data = Wire.endTransmission(true);
 
  if(data == 0)
  {
    Serial.print("Device found at: 0x");
    Serial.println(MPU_ADDR, HEX);
  }
  else
  {
    Serial.println("ERROR: Device not found!");
  }
}
 
/*
 * Check if sensor is active and responding
 */
void checkMPU(int mpu_addr)
{
  findMPU(MPU_ADDR);
     
  int data = readRegMPU(WHO_AM_I); // Register 117 – Who Am I - 0x75
   
  if(data == 104) 
  {
    Serial.println("MPU6050 device returns OK! (104)");
 
    data = readRegMPU(PWR_MGMT_1); // Register 107 – Power Management 1-0x6B
 
    if(data == 64) Serial.println("MPU6050 in SLEEP mode! (64)");
    else Serial.println("MPU6050 in ACTIVE mode!"); 
  }
  else Serial.println("MPU6050 not available!");
}
 
/*
 * Start MPU sensor
 */
void initMPU()
{
  setSleepOff();
  setGyroScale();
  setAccelScale();
}
 
/* 
 *  Configure sleep bit 
 */
void setSleepOff()
{
  writeRegMPU(PWR_MGMT_1, 0); // writes 0 to energy management registry (0x68), settin sensor to ACTIVE mode
}
 
/* Set gyro scales
   scale register: 0x1B[4:3]
   0 == 250°/s
 
    FS_SEL  Full Scale Range
      0        ± 250 °/s      0b00000000
      1        ± 500 °/s      0b00001000
      2        ± 1000 °/s     0b00010000
      3        ± 2000 °/s     0b00011000
*/
void setGyroScale()
{
  writeRegMPU(GYRO_CONFIG, 0);
}
 
/* Set accelerometer scale
   Acc scale register: 0x1C[4:3]
   0 == 2g
 
    AFS_SEL   Full Scale Range
      0           ± 2g            0b00000000
      1           ± 4g            0b00001000
      2           ± 8g            0b00010000
      3           ± 16g           0b00011000
*/
void setAccelScale()
{
  writeRegMPU(ACCEL_CONFIG, 0);
}
 
/* Reads sensors raw data
   14 bytes, 2 bytes for each axis and 2 bytes for temperature:
 
  0x3B 59 ACCEL_XOUT[15:8]
  0x3C 60 ACCEL_XOUT[7:0]
  0x3D 61 ACCEL_YOUT[15:8]
  0x3E 62 ACCEL_YOUT[7:0]
  0x3F 63 ACCEL_ZOUT[15:8]
  0x40 64 ACCEL_ZOUT[7:0]
 
  0x41 65 TEMP_OUT[15:8]
  0x42 66 TEMP_OUT[7:0]
 
  0x43 67 GYRO_XOUT[15:8]
  0x44 68 GYRO_XOUT[7:0]
  0x45 69 GYRO_YOUT[15:8]
  0x46 70 GYRO_YOUT[7:0]
  0x47 71 GYRO_ZOUT[15:8]
  0x48 72 GYRO_ZOUT[7:0]
    
*/
void readRawMPU()
{  
  Wire.beginTransmission(MPU_ADDR);       
  Wire.write(ACCEL_XOUT);                 
  Wire.endTransmission(false);            
  Wire.requestFrom(MPU_ADDR, 14, true);   
 
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  int AcXMod = (AcX > 0) ? AcX : -AcX;
  int AcYMod = (AcY > 0) ? AcY : -AcY;
  int AcZMod = (AcZ > 0) ? AcZ : -AcZ;
  
  int Sum = AcXMod + AcYMod + AcZMod;

  if(count < 30 ){
    last[count] = Sum;
    sum += Sum;
    count++;
    mid = sum/count;
  } else {
    sum  = sum + Sum - last[0];
    for(int i=0; i<29; i++){
      last[i] = last[i+1];
    }
    last[29] = Sum;
    mid = sum/30;
    int cur = ((Sum - mid) > 0) ? 1 : -1;
    if(cur == 1 && walk == -1 ||
       cur == -1 && walk == 1) {
      Serial.print("PASSO");    
    }
    walk = cur;
  }

//  Serial.print("AcXMod: ");
//  Serial.print(AcXMod);
//  Serial.print(" . AcYMod: ");
//  Serial.print(AcYMod);
//  Serial.print(" . AcZMod: ");
//  Serial.print(AcZMod);
//  Serial.print("\n");
//  Serial.print(mid);
//  Serial.print("\n");
//  Serial.print("\n");
  //Tmp = Wire.read() << 8 | Wire.read();
 
  //GyX = Wire.read() << 8 | Wire.read();
  //GyY = Wire.read() << 8 | Wire.read();
  //GyZ = Wire.read() << 8 | Wire.read(); 
 
  //Serial.print("AcX = "); Serial.print(AcX/16384.0);
  //Serial.print(" | AcY = "); Serial.print(AcY/16384.0);
  //Serial.print(" | AcZ = "); Serial.print(AcZ/16384.0);
  //Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);
  //Serial.print(" | GyX = "); Serial.print(GyX/131.0);
  //Serial.print(" | GyY = "); Serial.print(GyY/131.0);
  //Serial.print(" | GyZ = "); Serial.println(GyZ/131.0);
 
  led_state = !led_state;
  digitalWrite(LED_BUILTIN, led_state);        
  delay(50);                                        
}
 
void setupMpu9060() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Starting MPU6050 configuration");
  initI2C();
  initMPU();
  checkMPU(MPU_ADDR);
  Serial.println("End of MPU6050 configuration, beggin");  
}
// 
void loopMpu9060() {
  readRawMPU();   
}
