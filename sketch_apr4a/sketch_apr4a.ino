#include <AM2302-Sensor.h>
#include <SerialCommands.h>

#define ONE_HOUR 3600000UL

/*GNU Lesser General Public License.
See <http://www.gnu.org/licenses/> for details.
All above must be included in any redistribution
****************************************************/
/***********Notice and Trouble shooting***************
1.Connection and Diagram can be found here
2.This code is tested on Arduino Uno.
****************************************************/
constexpr unsigned int TEMP_HUMI_SENSOR_PIN {2};
constexpr unsigned int NUM_SENSOR{4};
constexpr unsigned int CAP_MOISTURE_SENSOR_PINS[]{A0, A1, A2, A3};
constexpr unsigned int MOTOR_FET_PINS[]{6, 5, 4, 3};

AM2302::AM2302_Sensor am2302{TEMP_HUMI_SENSOR_PIN};

int AirValues[]{605, 605, 605, 605}; //you need to change this value that you had recorded in the air, should be length number in NUM_SENSOR
int WaterValues[]{300, 300, 300, 300}; //you need to change this value that you had recorded in the water, should be length number in NUM_SENSOR

bool Watered[]{false, false, false, false}; //initialize a hydration state for allowing drying cycles.

void cmd_calibrate_air(SerialCommands* sender){// SHOULD BE CALLED ONLY WHEN CALIBRATING, SENSORS IN AIR (*NOT SOIL*) 
  for (int i=0; i<NUM_SENSOR; i++ ){
    AirValues[i] = analogRead(CAP_MOISTURE_SENSOR_PINS[i]);
    sender->GetSerial()->println(AirValues[i]);
  }
  sender->GetSerial()->println("Air calibration complete.");
}
SerialCommand cmd_calibrate_air_("cal_air", cmd_calibrate_air); //Binds the above calibration to the string, calls when recieved on serial

void cmd_calibrate_water(SerialCommands* sender){// SHOULD BE CALLED ONLY WHEN CALIBRATING, SENSORS IN WATER (*NOT SOIL*)
  for (int i=0; i<NUM_SENSOR; i++ ){
    WaterValues[i] = analogRead(CAP_MOISTURE_SENSOR_PINS[i]);
    sender->GetSerial()->println(WaterValues[i]);
  }
  sender->GetSerial()->println("Water calibration complete.");
}
SerialCommand cmd_calibrate_water_("cal_water", cmd_calibrate_water);

void cmd_show_cal(SerialCommands* sender){
  sender->GetSerial()->println("Showing calibration values...");
  for (int i=0; i<NUM_SENSOR; i++ ){
    sender->GetSerial()->print("w:\t");
    sender->GetSerial()->print(WaterValues[i]);
    sender->GetSerial()->print("\ta:\t");
    sender->GetSerial()->println(AirValues[i]);
  }
  sender->GetSerial()->println("Done.");
}
SerialCommand cmd_show_cal_("show_cal", cmd_show_cal);


//Create a 32 bytes static buffer to be used exclusive by SerialCommands object.
//The size should accomodate command token, arguments, termination sequence and string delimeter \0 char.
char serial_command_buffer_[32];
//Creates SerialCommands object attached to Serial
//working buffer = serial_command_buffer_
//command delimeter: Cr & Lf
//argument delimeter: SPACE
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
	sender->GetSerial()->print("ERROR: Unrecognized command [");
	sender->GetSerial()->print(cmd);
	sender->GetSerial()->println("]");
}

int soilMoistureValue = 0;
void setup() {
  Serial.begin(115200); // open serial port, set the baud rate to 9600 bps
  serial_commands_.AddCommand(&cmd_calibrate_air_);
  serial_commands_.AddCommand(&cmd_calibrate_water_);
  serial_commands_.AddCommand(&cmd_show_cal_);
  serial_commands_.SetDefaultHandler(&cmd_unrecognized);
  pinMode(TEMP_HUMI_SENSOR_PIN, INPUT_PULLUP);
  for (int i=0; i<NUM_SENSOR; i++){
    pinMode(MOTOR_FET_PINS[i], OUTPUT);
  }
  while (!Serial) {
    yield();
  }
  if (!am2302.begin()) {
    Serial.println("Error: sensor check. => Please check AM2302 Tempurature and Humidity sensor connection!");
    delay(3000);
  }
}


void soilMoistureSampleReaction(unsigned int i){
  soilMoistureValue = analogRead(CAP_MOISTURE_SENSOR_PINS[i]); //put Sensor insert into soil
  int normalizedSMValue =  (100 * (AirValues[i] - soilMoistureValue)) / (AirValues[i]-WaterValues[i]);
  Serial.print("Soil Moisture(%), pin ");
  Serial.print(i);
  Serial.print(": ");
  Serial.print(normalizedSMValue);
  Serial.print(" (raw ");
  Serial.print(soilMoistureValue);
  Serial.println(")");
  if (normalizedSMValue > 60){
    Watered[i] = true;
  }

  if (normalizedSMValue < 25){
    Watered[i] = false;
  }

  if (Watered[i]) {
    digitalWrite(MOTOR_FET_PINS[i], LOW);
  }
  else {
    digitalWrite(MOTOR_FET_PINS[i], HIGH);
  }
  // if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
  // {
  // Serial.println("Very Wet");
  // }
  // else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
  // {
  // Serial.println("Wet");
  // }
  // else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
  // {
  // Serial.println("Dry");
  // }
}
void loop() {
  serial_commands_.ReadSerial();
  if (!am2302.begin()) {
    Serial.println("Error: sensor check. => Please check AM2302 Tempurature and Humidity sensor connection!");
  }
  else {
    auto status = am2302.read();
    Serial.print("\n\nstatus: ");
    Serial.println(AM2302::AM2302_Sensor::get_sensorState(status));

    Serial.print("T(C): ");
    Serial.println(am2302.get_Temperature());

    Serial.print("H(%): ");
    Serial.println(am2302.get_Humidity());
  }

  for (int i=0; i < NUM_SENSOR; i++ ){
    soilMoistureSampleReaction(i);
  }

  delay(5000);
}