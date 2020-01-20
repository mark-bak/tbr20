

//calibration and sample rate
float cal = 16.93;  //calibration factor mm/ADC_out
int sampleRate = 5;  //hz
unsigned long sampleTime = 1000/sampleRate; //ms
unsigned long delStart = 0;
bool del = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(0,INPUT);
  Serial.begin(9600);
  delStart = millis();
  del = true;
}

void loop() {
  
  //get start time of reading (useful for accurate rate datalogging)
  unsigned long t = millis();
  if (del && (t-delStart)>=sampleTime) {
    
    delStart+=sampleTime;
    
    //read and print    
    int reading = analogRead(0);
    float ext = reading/cal;
    Serial.println(String(ext) + "        " + String(t));
  }

}
