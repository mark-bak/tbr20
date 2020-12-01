

//calibration and sample rate
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#define len(x) (sizeof(x) / sizeof((x)[0]))

//SD SHIZZLE
File root;
File logFile;
String prefix = "tst"; //this cant be longer than 3 letters for soome reason
String filename;
//bool customFileName = false;

//constants - THIS IS ALL YOU WOULD REALLY WANT TO CHANGE
float cal[] = {0.0435, 0.0435, 0.0435, 0.0435, 1,1.055}; //Measured_Range/ADC_out(0-1023)
float offset[] = {0,0,0,0,0,-420};
int sampleRate = 30;  //hz
unsigned long sampleTime = 1000 / sampleRate; //ms
int sensorPins[] = {0, 1, 2, 3, 4, 5}; //no of inputs to sense
float readingCalibrated[len(sensorPins)];

bool logging = true;

//delay stuff
unsigned long delStart = 0;
bool del = false;

//SD card pin location
const int chipSelect = 10;

//file_index
int index = 0;

void setup() {

  //Start serial & pin modes
  pinMode(0, INPUT);
  Serial.begin(9600);
  //power the SD module :)
  pinMode(SDA, OUTPUT);
  digitalWrite(SDA, HIGH);

  if (logging == true) {
    //SD Card init.
    Serial.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
      // don't do anything more:
      while (1);
    }
    Serial.println("card initialized.");

    index = EEPROM.read(0);
    index ++;
    EEPROM.write(0,index);

    filename = String(prefix + String(index));
    
    logFile = SD.open(filename,FILE_WRITE);
    if (logFile) {
      Serial.println("created" + filename);
    } else {
      Serial.println("whoops");
    }
    logFile.close();
    
  }
  //timer stuff
  delStart = millis();
  del = true;
}

void loop() {

  //get start time of reading (useful for accurate rate datalogging)
  unsigned long t = millis();

  //read data at sample interval
  if (del && (t - delStart) >= sampleTime) {
    delStart += sampleTime; //new delay start (doing it this way stops drift)

    logFile = SD.open(filename, FILE_WRITE);
    //read and print
    for (int i = 0; i < len(sensorPins); i++) {
      int reading = analogRead(i);
      readingCalibrated[i] = reading * cal[i] + offset[i];
      logFile.print(String(readingCalibrated[i]) + ",");
      //Serial.print(String(readingCalibrated[i]) + ",");
    }

    logFile.print(String(t) + "\n");
    //Serial.print(String(t) + "\n");
    logFile.close();//maybe make opeining and closing only happen every set number of cycles for speed
  }
}
