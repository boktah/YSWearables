// import required libraries
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <Adafruit_Si7021.h>

int i = 1; // used to control temp sensor print outputs
unsigned long time = 0; // used to reset marks on accel
// may be used to toggle on/off the alert for a particular sensor
bool accelAlert, pulseAlert, tempAlert, humAlert;
accelAlert = false; pulseAlert = false; tempAlert = false; humAlert = false;

// READ PINS
int axPin = A0; int ayPin = A1; int azPin = A2; // accel
int pulsePin = A3; // pulse

// READ VALS
int pulseVal; int pulseThreshold = 550; int bpmLimit = 90; // pulse
int ax, ay, az; int motionLimit = 500; int avg; int marks = 0; int marksLimit = 6; // accel
float temperature; float humidity; int tempLimit = 39; int humLimit = 70; // temp+hum

// WRITE PINS
int vibePin = 6; // vibe board
int red = 10; int green = 8; int blue = 9; // rgb led

Adafruit_Si7021 tempSensor = Adafruit_Si7021();
PulseSensorPlayground pulseSensor;

void setup() {
  Serial.begin(115200);
  // configure pinmodes for each pin used
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

  avg = (ax/3) + (ay/3) + (az/3); // might weigh certain axes differently in future

  if (avg > motionLimit) { // every time motion is too much, it adds a mark
    marks++;
    if (marks == 1) { // on the first mark, it starts a timer
      time = millis();
    }
  }

  if ((millis() - time) > 30000 && marks > 0) { // if 30 seconds pass, reset marks to 0
    marks = 0;
  }

  if (marks > marksLimit) { // if marks reaches the limit, start breathing exercise
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
  if (pulseVal > bpmLimit) { // if BPM is too high, start exercise
    startExercise();
  }
}

void temp() {
  temperature = tempSensor.readTemperature();
  humidity = tempSensor.readHumidity();
  
  if (temperature > tempLimit || humidity > humLimit) {
    startExcercise(); // if temp or humidity is too high, start exercise
  }
  
  if (i == 1) {
    // control the outputs of the temperature sensor to only print once per 1000 ms
    Serial.print("Humidity:    ");
    Serial.print(humidity, 2);
    Serial.print("\tTemperature: ");
    Serial.println(temperature, 2);
  }
  i++; if (i == 50) {i = 1;}
}

void controlVibe(bool val) { // turn vibe board on/off
  val ? digitalWrite(vibePin, HIGH) : digitalWrite(vibePin, LOW);
}

void startExercise() { // gives a 3 second vibration before looping the exercise for 60 seconds
  controlVibe(true);
  delay(3000);
  controlVibe(false);
  
  for (int n = 0; n < 6; n++) { // n<6 means the cycle will run 6 times
    breathExerciseCycle();
  }
}

void breathExerciseCycle() { // one cycle is 10 seconds
  digitalWrite(red, HIGH); // blue
  digitalWrite(green, HIGH);
  digitalWrite(blue, LOW);
  delay(4000);

  controlVibe(true);
  digitalWrite(red, HIGH); // cyan
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
  delay(1000);
  controlVibe(false);

  digitalWrite(red, HIGH); // green
  digitalWrite(green, LOW);
  digitalWrite(blue, HIGH);
  delay(4000);

  controlVibe(true);
  digitalWrite(red, HIGH); // cyan
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
  delay(1000);
  controlVibe(false);
}
