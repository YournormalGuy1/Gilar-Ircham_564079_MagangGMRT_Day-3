// Gilar Ircham Wibisana
// 25/564079/TK/63622

// Masukkin semua library nya
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

// menentukan bagian bagian yg akan di pakai
Adafruit_MPU6050 mpu;
Servo servo1, servo2, servo3, servo4, servo5;

// memasukkan variabel nya
const int PIR_PIN = 23;
const int SERVO1_PIN = 14;
const int SERVO2_PIN = 27;
const int SERVO3_PIN = 18;
const int SERVO4_PIN = 5; 
const int SERVO5_PIN = 26;

// menentukan tipe 
float roll_input, pitch_input, yaw_input;

bool pirActive = false;
unsigned long pirTimer = 0;
bool yawActive = false;
unsigned long yawTimer = 0;

const float ROTATION_THRESHOLD = 2.0;

const int POS_AWAL = 90;
const int POS_BEBAS = 45;
const int POS_KANAN = 180;
const int POS_KIRI = 0;

// menentukan fungsi yg akan dipakai 
void resetAllServos();
void readMPU();
void handleAxisLogic();


void setup(){
  Serial.begin(115200);

  Serial.println("Searching for MPU.......");

  if (!mpu.begin()){
    Serial.println("MPU NOT FOUND");
    while (1);

  }
  Serial.println("MPU FOUND!!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  pinMode(PIR_PIN, INPUT);

  // nge attach untuk setiap servo ke pin fisik
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);
  servo5.attach(SERVO5_PIN);

  resetAllServos();
  Serial.println("Sistem siap.");
  
}


// inti dari programnya
void loop(){
  bool pirDetected = digitalRead(PIR_PIN);

  if (pirDetected) {
    if (!pirActive) {
      Serial.println("Detected Motion!");
      pirActive = true;
      pirTimer = millis();

      servo1.write(POS_BEBAS);
      servo2.write(POS_BEBAS);
      servo3.write(POS_BEBAS);
      servo4.write(POS_BEBAS);
      servo5.write(POS_BEBAS);
    
    }
    return;
  }

  if (!pirDetected && pirActive){
    if(millis() - pirTimer >= 500) {
      Serial.println("Motion Lost, Resetting...");
      resetAllServos();
      pirActive = false;
      yawActive = false;

    }
    return;

    if (!pirActive){
      readMPU();
      handleAxisLogic();

    }
  }

  delay(50);

}

void resetAllServos() {
  servo1.write(POS_AWAL);
  servo2.write(POS_AWAL);
  servo3.write(POS_AWAL);
  servo4.write(POS_AWAL);
  servo5.write(POS_AWAL);
}

void readMPU() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  roll_input = a.acceleration.y;
  pitch_input = a.acceleration.x;
  yaw_input = g.gyro.z;
}

// bagian untuk nge handle si Axisnya atau menentukan gerak dari masing2 servo
void handleAxisLogic() {

  if (yawActive) {
    if(millis() - yawTimer >= 1000) {
      Serial.println("Yaw returning to 0");
      servo5.write(POS_AWAL);
      yawActive = false;
    }
    return;
  }

  if (roll_input > ROTATION_THRESHOLD) {
    Serial.println("Roll positive");
    servo1.write(POS_KIRI);
    servo2.write(POS_KIRI);
  }

  else if (roll_input < -ROTATION_THRESHOLD) {
    Serial.println("Roll Negative");
    servo1.write(POS_KANAN);
    servo2.write(POS_KANAN);
  }
  
  //ketika si roll tidak menerima sinyal
  else {
    servo1.write(POS_AWAL);
    servo2.write(POS_AWAL);
  }

  if (pitch_input > ROTATION_THRESHOLD) {
    Serial.println("Yaw Positive");
    servo1.write(POS_KANAN);
    servo2.write(POS_KANAN);
  }

  else if (pitch_input < -ROTATION_THRESHOLD) {
    Serial.println("Pitch Negative");
    servo3.write(POS_KIRI);
    servo4.write(POS_KIRI);
  }

  // ketika si pitch tidak menerima sinyal
  else {
    servo3.write(POS_AWAL);
    servo4.write(POS_AWAL);
  }

  if (yaw_input > ROTATION_THRESHOLD){
    Serial.println("Yaw Positive");
    servo5.write(POS_KANAN);
    yawActive = true;
    yawTimer = millis();

  }

  // ketika si yaw tidak menerima sinyal
  else if (yaw_input < -ROTATION_THRESHOLD){
    Serial.println("Yaw Negative");
    servo5.write(POS_KIRI);
    yawActive = true;
    yawTimer = millis();

  }
} 


