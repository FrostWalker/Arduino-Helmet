#include <SPI.h>            // To talk to the SD card and MP3 chip
#include <SdFat.h>          // SD card file system
#include <SFEMP3Shield.h>   // MP3 decoder chip

SFEMP3Shield MP3player;
SdFat sd;

char filename[5][13];

int inputPin = A0;

const int numReadings = 50;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int timer = 0;

void setup()
{
  int x, index;
  SdFile file;
  byte result;
  char tempfilename[13];
  
  memset(readings, 0, sizeof(readings));
  
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.print("Initialising SD Card...");
  if (!sd.begin(9, SPI_HALF_SPEED)) {
    Serial.println("Card failed, or not present");
    return;
  } Serial.println("Card initialised.");

  Serial.print("Initialising MP3 Player...");
  if(!MP3player.begin()) {
    Serial.println("Player failed");
    return;
  } Serial.println("Player initialised.");

  Serial.print("Reading SD Card...");
  sd.chdir("/",true);
  while (file.openNext(sd.vwd(),O_READ)) {
    file.getFilename(tempfilename);
    if (tempfilename[0] >= '1' && tempfilename[0] <= '5')
    {
      index = tempfilename[0] - '1';
      strcpy(filename[index],tempfilename);

      Serial.print(F("Found a file with a leading "));
      Serial.print(index+1);
      Serial.print(F(": "));
      Serial.println(filename[index]);
    } else {
      Serial.print(F("Found a file w/o a leading number: "));
      Serial.println(tempfilename);
    }
      
    file.close();
  }
  Serial.println(F("done reading root directory"));
  
  MP3player.setVolume(0);
  digitalWrite(A2,HIGH);
  delay(2);
}

void loop() {
  total -= readings[readIndex %= numReadings];
  average = (total += (readings[readIndex++] = analogRead(inputPin))) / numReadings;
  
  if(average > 300) {
      timer = 1;
      if(!MP3player.isPlaying()) {
        Serial.println("PLAY");
        MP3player.playMP3(filename[0], random(10000, 60000));
      }
  } else if(average < 300 && timer <= 0 && MP3player.isPlaying()) {
    Serial.print(timer);
    Serial.print("/0.5k - ");
    Serial.println(average);
    Serial.print("HALT - ");
    Serial.println(MP3player.isPlaying());
      MP3player.stopTrack();
  }

  if(timer % 100 == 0) Serial.println(average);

  ++timer %= 500;
  
  delay(1);
}


