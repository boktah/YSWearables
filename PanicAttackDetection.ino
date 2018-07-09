// import required libraries
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <Adafruit_Si7021.h>

int i = 1;
unsigned long time = 0;

// READ PINS
int axPin = A0; int ayPin = A1; int azPin = A2;
int pulsePin = A3;

// READ VALS
int pulseVal; int pulseThreshold = 550; int bpmLimit = 90;
int ax, ay, az; int motionLimit = 500; int avg; int marks = 0; int marksLimit = 6;

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

  avg = (ax/3) + (ay/3) + (az/3);

  if (avg > motionLimit) {
    marks++;
    if (marks == 1) {
      time = millis();
    }
  }

  if ((millis() - time) > 30000 && marks > 0) {
    marks = 0;
  }

  if (marks > marksLimit) {
    startExercise();
  }
  
  Serial.print("x: "); Serial.println(ax);
  Serial.print("y: "); Serial.println(ay);
  Serial.print("z: "); Serial.println(az);
  Serial.print("avg: "); Serial.println(avg);
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
  Serial.print("BPM: "); Serial.println(pulseVal);
  if (pulseVal > bpmLimit) {
    startExercise();
  }
}

void temp() {
  if (i == 1) {
    // control the outputs of the temperature sensor to only print once per 1000 ms
    Serial.print("Humidity:    ");
    Serial.print(tempSensor.readHumidity(), 2);
    Serial.print("\tTemperature: ");
    Serial.println(tempSensor.readTemperature(), 2);
  }
  i++; if (i == 50) {i = 1;}
}

void controlVibe(bool val) {
  val ? digitalWrite(vibePin, HIGH) : digitalWrite(vibePin, LOW);
}

void startExercise() {
  controlVibe(true);
  delay(3000);
  controlVibe(false);
  
  for (int n = 1; n < 6; n++) {
    breathExerciseCycle();
  }
}

void breathExerciseCycle() { // one cycle is 10 seconds
  digitalWrite(red, HIGH);
  digitalWrite(green, HIGH);
  digitalWrite(blue, LOW);
  delay(4000);

  controlVibe(true);
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
  delay(1000);
  controlVibe(false);

  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  digitalWrite(blue, HIGH);
  delay(4000);

  controlVibe(true);
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
  delay(1000);
  controlVibe(false);
}
