#include "thingProperties.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>
#include <Wire.h>
#define TdsSensorPin A0
#define VREF 3.3  // analog reference voltage(Volt) of the ADC
#define SCOUNT 30
#define oneWireBus D4



Servo myservo;

unsigned long servoTimer = 0;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

int analogBuffer[SCOUNT];  // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;
float temperature = 23; 

// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0) {
    bTemp = bTab[(iFilterLen - 1) / 2];
  } else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

void setup() {
  Serial.begin(115200);
  myservo.attach(D5);
  myservo.write(0);
  
  sensors.begin();
  initProperties();
  pinMode(TdsSensorPin, INPUT);
 

  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();

  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  tempreturenew=tempC;
  
   if (tempC != DEVICE_DISCONNECTED_C) {
     Serial.print("Temperature: ");
     Serial.print(tempC);
     Serial.println(" °C");
   } else {
     Serial.println("Error: Could not read temperature data");
   }

 

  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) {  //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);  //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) {
      analogBufferIndex = 0;
    }
  }

  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];

      // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0;

      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
      //temperature compensation
      float compensationVoltage = averageVoltage / compensationCoefficient;

      //convert voltage value to tds value
      tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;
      ph = tdsValue;
      Serial.print("voltage:");
      Serial.print(averageVoltage,2);
      Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue, 0);
      Serial.println("ppm");
    }
  }
  
  if (sch.isActive()){
  if(millis() - servoTimer >= 10000) { // hour
    servoTimer = millis();
    toggleServo();
  }
  delay(10000);
  }
  
}

void toggleServo(){
  myservo.write(90);
  unsigned long firstTime = millis();
  while(millis() - firstTime < 4000){
  }
  myservo.write(0);
}

if (sch.isActive()){
   myservo.write(90);
   unsigned long firstTime = millis();
   while(millis() - firstTime < 4000){
   }
   myservo.write(0);
  
 }
 }
2



void onPhChange() {
}

void onTempreturenewChange() {
}

void onServoChange() {
   int servoValue = servo;     // Get the new servo value from IoT Cloud
   myservo.write(servoValue);  // Set the servo to the new position

}
/*
  Since Sch is READ_WRITE variable, onSchChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onSchChange()  {
  // Add your code here to act upon Sch change
}