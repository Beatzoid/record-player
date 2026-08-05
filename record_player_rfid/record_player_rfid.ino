#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

#include <AccelStepper.h>

#include <WiFi.h>

#include <WebServer.h>

#include "Arduino.h"
#include "Audio.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"

// RST - WHITE - D9
// MISO - YELLOW - D12
// MOSI - ORANGE - D11
// SCK - GRAY - D13
// SDA - PURPLE - D10

// ---------------------- PINS ----------------------

// Stepper motor

#define STEPPER_IN1 D6
#define STEPPER_IN2 D5
#define STEPPER_IN3 D4
#define STEPPER_IN4 D3

#define HALL_SENSOR_PIN A0

// SD card reader

#define SD_CS    A1
#define SPI_MOSI A4
#define SPI_MISO A2
#define SPI_SCK  A3

// Audio output

#define I2S_DOUT      5
#define I2S_BCLK      17
#define I2S_LRC       18

// ---------------------- DEFINITIONS ----------------------

AccelStepper motor(AccelStepper::FULL4WIRE, STEPPER_IN1, STEPPER_IN3, STEPPER_IN2, STEPPER_IN4);

// Set web server port number to 80
WebServer server(80);

// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(D10);

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
// MFRC522DriverI2C driver{};     // Create I2C driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

SPIClass sdSPI(HSPI);

// ---------------------- GLOBAL VARIABLES ----------------------

// Controls whether the record player is actively playing music
bool playing = false;

// Variable to store the HTTP request
String header;

Audio audio;

const char* uploadPage = R"rawliteral(
<!DOCTYPE html>
<html>
<body>
  <h2>Upload file to SD card</h2>
  <form method="POST" action="/upload" enctype="multipart/form-data">
    <input type="file" name="upload">
    <input type="submit" value="Upload">
  </form>
</body>
</html>
)rawliteral";

File uploadFile;

void my_audio_info(Audio::msg_t m) {
    Serial.printf("%s: %s\n", m.s, m.msg);
}

void handleRoot() {
  server.send(200, "text/html", uploadPage);
}

void handleUpload() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.printf("Upload start: %s\n", filename.c_str());
    uploadFile = SD.open(filename, FILE_WRITE);
  } 
  else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
  } 
  else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
      Serial.printf("Upload complete: %u bytes\n", upload.totalSize);
    }
  }
}

void handleUploadDone() {
  server.send(200, "text/plain", "Upload successful");
}


void setup() {
  Serial.begin(115200);  // Initialize serial communication
  // while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).

  // WiFi.mode(WIFI_MODE_AP);

  // IPAddress local_ip(192, 168, 4, 1);
  // IPAddress gateway(192, 168, 4, 1);
  // IPAddress subnet(255, 255, 255, 0);

  // WiFi.softAPConfig(local_ip, gateway, subnet);
  // WiFi.softAP("RecordPlayer", "Mercury5280");
  
  // IPAddress IP = WiFi.softAPIP();
  // Serial.print("AP IP address: ");
  // Serial.println(IP);

  // server.on("/", HTTP_GET, handleRoot);
  // server.on("/upload", HTTP_POST, handleUploadDone, handleUpload);

  // server.begin();

  // Pin Mode

  pinMode(STEPPER_IN1, OUTPUT);
  pinMode(STEPPER_IN2, OUTPUT);
  pinMode(STEPPER_IN3, OUTPUT);
  pinMode(STEPPER_IN4, OUTPUT);

  pinMode(HALL_SENSOR_PIN, INPUT);

  // // Stepper

  motor.setMaxSpeed(500);
  motor.setAcceleration(1000);

  // RFID
  
  mfrc522.PCD_Init();    // Init MFRC522 board. 
  delay(4);
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.

  Audio::audio_info_callback = my_audio_info;

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  sdSPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
  sdSPI.setFrequency(4000000);
  SD.begin(SD_CS, sdSPI);

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  audio.setVolume(21); // 0...21
}

void loop() {
  // vTaskDelay(1);
  audio.loop();
  // server.handleClient();

  bool previousPlaying = playing;

  int hallValue = digitalRead(HALL_SENSOR_PIN);

  // If the hallValue is 0, it means the needle
  // is positioned over the record
  bool needlePositioned = hallValue == 0;

  // Returns true or false depending on whether
  // there is a record present on the turntable
  bool recordPresent = isRecordPresent();

  // If the record has already started playing (previousPlaying is true), 
  // then only stop playing (set playing to false) when the needle arm is moved,
  // as the record will rotate, and the nfc tag will pass in and out of the reader range
  if (previousPlaying) {
    playing = needlePositioned;
  } else {
    playing = recordPresent && needlePositioned;
  }

  // Save the UID on a String variable
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0"; 
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }

  // Only triggers once, at the beginning of playing the record
  if (playing && !previousPlaying) {
    Serial.println("Playing audio...");
    Serial.println(uidString);

    String fileName = uidString + ".mp3";
    Serial.println(fileName);
    audio.connecttoFS(SD, fileName.c_str());
  }

  // Only triggers once, at the end of playing the record 
  if (!playing && previousPlaying) {
    Serial.println("Stopping audio...");

    audio.stopSong();
  }

  if (playing) {
    // TODO: Identify song based on NFC tag
    // TODO: Play audio stored on SD card

    // Start the stepper motor spinning
    motor.setSpeed(500);
    motor.runSpeed();
  } else {
    // If we are not currently playing,
    // we set the stepper motor pins to LOW
    // to de-energize them and allow the motor
    // to cool down between usages

    digitalWrite(STEPPER_IN1, LOW);
    digitalWrite(STEPPER_IN2, LOW);
    digitalWrite(STEPPER_IN3, LOW);
    digitalWrite(STEPPER_IN4, LOW);
  }
}

bool isRecordPresent() {
  if (!mfrc522.PICC_IsNewCardPresent()) return false;
  if (!mfrc522.PICC_ReadCardSerial()) return false;
  return true;
}