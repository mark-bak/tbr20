

//calibration and sample rate
#include <SPI.h>
#include <SD.h>
#define len(x) (sizeof(x) / sizeof((x)[0]))

//SD SHIZZLE
File root;
File logFile;
String prefix = "tst"; //this cant be longer than 3 letters for soome reason
String filename = "test";
//bool customFileName = false;

//constants
float cal[] = {-22.97, 22.97, 22.97, 22.97, 2.842}; //calibration factor mm/ADC_out
float offset[] = {51.224,0,0,0,0};
int sampleRate = 1;  //hz
unsigned long sampleTime = 1000 / sampleRate; //ms
int sensorPins[] = {0, 1, 2, 3, 4}; //no of inputs to sense
float readingCalibrated[len(sensorPins)];

bool logging = true;

//delay stuff
unsigned long delStart = 0;
bool del = false;

//SD card pin location
const int chipSelect = 10;


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
  }

  //Make a new filename if custom filename not created

  root = SD.open("/");
  int index = 0;
  while (true) {
    index ++;
    Serial.println(index);
    File entry =  root.openNextFile();
    if (! entry) {
      // no more files
      filename = String(prefix + String(index) + ".csv");
      Serial.println(filename);
      break;
    }
  }

  
  logFile = SD.open(filename,FILE_WRITE);
  if (logFile) {
    Serial.println("created" + filename);
  } else {
    Serial.println("fuck");
  }
  logFile.close();


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
      readingCalibrated[i] = reading / cal[i] + offset[i];
      logFile.print(String(readingCalibrated[i]) + ",");
      Serial.print(String(readingCalibrated[i]) + ",");
    }

    logFile.print(String(t) + "\n");
    Serial.print(String(t) + "\n");
    logFile.close();//maybe make opeining and closing only happen every set number of cycles for speed
  }
}
