// import required libraries
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <Adafruit_Si7021.h>

int i = 1; // used to control temp sensor print outputs
unsigned long time = 0; // used to reset marks on accel
// may be used to toggle on/off the alert for a particular sensor
bool accelAlert = true;
bool pulseAlert = true;
bool tempAlert = true;
bool humAlert = true;

// used to read serial input
String input;

// READ PINS
int axPin = A0; int ayPin = A1; int azPin = A2;
float zero_G = 512; float scale = 102.3; // accel
int pulsePin = A3; // pulse

// READ VALS
int pulseVal; int pulseThreshold = 550; int bpm; int bpmLimit = 90; // pulse
int ax, ay, az; float x,y,z; int motionLimit = 500; int avg; int marks = 0; int marksLimit = 6; // accel
float temperature; float humidity; int tempLimit = 39; int humLimit = 70; // temp+hum

// WRITE PINS
int vibePin = 6; // vibe board
int red = 10; int green = 8; int blue = 9; // rgb led

Adafruit_Si7021 tempSensor = Adafruit_Si7021();
PulseSensorPlayground pulseSensor;

void setup() {
  // Serial.begin(115200);  // Pulse sensor wanted 115200 - will have to test
  Serial.begin(9600);       // BT module runs at 9600
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
  if (Serial.available() > 0) {
    input = Serial.read();
    if (input.equals("accel")) {
      accelAlert = !accelAlert;
    }
    else if (input.equals("pulse")) {
      pulseAlert = !pulseAlert;
    }
    else if (input.equals("temp")) {
      tempAlert = !tempAlert;
    }
    else if (input.equals("hum")) {
      humAlert = !humAlert;
    }
  }

  accel(); // accelerometer
  //  pulse(); // pulse sensor
  //  temp(); // temperature and humidity sensor

  delay(500);
}

void accel() {
  ax = analogRead(axPin); delay(1);
  ay = analogRead(ayPin); delay(1);
  az = analogRead(azPin);
  
  x = ((float)x - zero_G) / scale;
  y = ((float)y - zero_G) / scale;
  z = ((float)z - zero_G) / scale;

  avg = (ax / 3) + (ay / 3) + (az / 3); // might weigh certain axes differently in future

  if (avg > motionLimit) { // every time motion is too much, it adds a mark
    marks++;
    if (marks == 1) { // on the first mark, it starts a timer
      time = millis();
    }
  }

  if ((millis() - time) > 30000 && marks > 0) { // if 30 seconds pass, reset marks to 0
    marks = 0;
  }

  //  if (marks > marksLimit) { // if marks reaches the limit, start breathing exercise
  //    if (accelAlert) {
  //      startExercise();
  //    }
  //  }

  Serial.print(((float)x - zero_G) / scale);
  Serial.print("\t");
  Serial.print(((float)y - zero_G) / scale);
  Serial.print("\t");
  Serial.print(((float)z - zero_G) / scale);
  Serial.print("\n");
  // Serial.print("avg: "); Serial.print(avg);
  // Serial.print("    marks: "); Serial.println(marks);
}

void pulse() {

  pulseVal = analogRead(pulsePin);
  Serial.print("BPM: "); Serial.println(pulseVal);

  //  if (pulseVal > bpmLimit) { // if BPM is too high, start exercise
  //    if (pulseAlert) {
  //      startExercise();
  //    }
  //  }
}

void temp() {
  temperature = tempSensor.readTemperature();
  humidity = tempSensor.readHumidity();
  // if temp or humidity is too high, start exercise
  //  if (temperature > tempLimit) {
  //    if (tempAlert) {
  //      startExercise();
  //    }
  //  }
  //  if (humidity > humLimit) {
  //    if (humAlert) {
  //      startExercise();
  //    }
  //  }

  if (i == 1) {
    // control the outputs of the temperature sensor to only print once per 1000 ms
    Serial.print("Humidity:    ");
    Serial.print(humidity, 2);
    Serial.print("\tTemperature: ");
    Serial.println(temperature, 2);
  }
  i++; if (i == 5) {
    i = 1;
  }
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

  digitalWrite(red, HIGH); // off
  digitalWrite(green, HIGH);
  digitalWrite(blue, HIGH);
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
