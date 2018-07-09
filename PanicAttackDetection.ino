// import required libraries
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <Adafruit_Si7021.h>

int i = 1;

// READ PINS
int axPin = A0; int ayPin = A1; int azPin = A2;
int pulsePin = A3;

// READ VALS
int pulseVal; int pulseThreshold = 550; int bpmLimit = 90;
int ax, ay, az;

// WRITE PINS
int vibePin = 6;
int red = 10; int green = 8; int blue = 9;

Adafruit_Si7021 tempSensor = Adafruit_Si7021();
PulseSensorPlayground pulseSensor;

void setup() {
  Serial.begin(115200);

  pinMode(pulsePin, INPUT);
  pinMode(axPin,    INPUT);
  pinMode(ayPin,    INPUT);
  pinMode(azPin,    INPUT);
  pinMode(vibePin, OUTPUT);
  pinMode(red,     OUTPUT);
  pinMode(green,   OUTPUT);
  pinMode(blue,    OUTPUT);
}

void loop() {
  accel(); // accelerometer
  pulse(); // pulse sensor
  temp(); // temperature and humidity sensor
  
  delay(20);
}

void accel() {
  ax = analogRead(axPin);
  ay = analogRead(ayPin);
  az = analogRead(azPin);

  Serial.print("x: "); Serial.println(ax);
  Serial.print("y: "); Serial.println(ay);
  Serial.print("z: "); Serial.println(az);
}

void pulse() {
  pulseSensor.analogInput(pulsePin);
  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(SERIAL_PLOTTER);
  pulseSensor.setThreshold(pulseThreshold);

  pulseSensor.outputSample();
  if (pulseSensor.sawStartOfBeat()) {
    pulseSensor.outputBeat();
  }

  pulseVal = pulseSensor.getBeatsPerMinute();
  (pulseVal > bpmLimit) ? controlVibe(true) : controlVibe(false);
}

void temp() {
  if (i == 1) { // control the outputs of the temperature sensor to only print once per 1000 ms
    Serial.print("Humidity:    "); Serial.print(tempSensor.readHumidity(), 2);
    Serial.print("\tTemperature: "); Serial.println(tempSensor.readTemperature(), 2);
  }
  i++; if (i == 50) {i = 1;}
}

void controlVibe(bool val) {
  val ? digitalWrite(vibePin, HIGH) : digitalWrite(vibePin, LOW);
}
