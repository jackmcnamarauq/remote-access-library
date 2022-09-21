/*
 * Prototype Teleduino Client code for the ESP8266
 * Extracted from the Teleduino Arduino Library and Teleduino Example Programs
 * Thanks to Nathan Kennedy at teleduino.com for his assistance
 *
 * This code allows an LED connected to GPIO2 on the ESP8266 (ESP-01) to be controlled from anywhere in the world
 *
 * To turn LED on, paste this URL into your browser
 * https://us01.proxy.teleduino.org/api/1.0/328.php?k=<YOUR KEY GOES HERE>&r=setDigitalOutput&pin=2&output=1&expire_time=0&save=0
 *
 * To turn LED off, paste this URL into your browser
 * https://us01.proxy.teleduino.org/api/1.0/328.php?k=<YOUR KEY GOES HERE>&r=setDigitalOutput&pin=2&output=0&expire_time=0&save=0
 *
 * The keys in each of these URLs must match the key in the key array below
 *
 * Ported by: Craig A. Lindley
 * Last Update: 05/30/2015
 */
#include <Arduino.h> 
//#include <WiFi.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <Wire.h>
#include <Teleduino328.h>
#include <SPI.h>


// Define the single digital pin on the ESP-01
#define GPIO2 2

// Replace ssid and password with ones from your WiFi network
#ifndef STASSID
#define STASSID "iPhone"
#define STAPSK  "robotics"
#endif
const char* ssid     = STASSID;
const char* password = STAPSK;

// Teleduino attributes
const char serverName[]       = "us01.proxy.teleduino.org";
const unsigned int serverPort = 5353; // Can be set to either 53 or 5353
const char apiVersion[]       = "328-0.6.9";

// Use WiFiclient class to create TCP connections
WiFiClient client;

// Replace this key with one you download from teleduino.com
byte key[] = { 0x75, 0x97, 0x78, 0x32,
               0xB9, 0xFA, 0x59, 0x76,
               0x5C, 0x7C, 0x3E, 0xB3,
               0x3C, 0xCD, 0xB2, 0x9C };

byte data[257];
byte dataLength;
boolean hexStage;
byte stage;

#define WRITE_QUEUE_SIZE 50

byte writeQueue[WRITE_QUEUE_SIZE];
int writeQueueIndex;

void initWriteQueue() {
  memset(writeQueue, 0, sizeof(writeQueue));
  writeQueueIndex = 0;
}

void queueData(byte b) {
  writeQueue[writeQueueIndex++] = b;
}

void writeQueueData() {
  client.write((uint8_t *) writeQueue, writeQueueIndex);
  writeQueueIndex = 0;
}

byte hexDecode(byte c) {

  if (c >= '0' && c <= '9')
  {
    return c - '0';
  }
  else if (c >= 'a' && c <= 'f')
  {
    return c - 'a' + 10;
  }
  else if (c >= 'A' && c <= 'F')
  {
    return c - 'A' + 10;
  }
  else
  {
    return 0;
  }
}

byte hexEncode(byte n, boolean cap) {

  if (n >= 0 && n <= 9)
  {
    return n + '0';
  }
  else if (n >= 10 && n <= 15)
  {
    if (cap)
    {
      return n - 10 + 'A';
    }
    else
    {
      return n - 10 + 'a';
    }
  }
  else
  {
    return '0';
  }
}

byte hexEncode(byte n) {
  return hexEncode(n, true);
}

void executeInstruction(byte* data) {

  Serial.println();
  Serial.print("Instruction Code: 0x");
  Serial.println(data[0], HEX);

  switch (data[0])
  {
    case 0x10: // reset
      Serial.println("reset");
      if (data[1] == 0)
      {
        data[0] = 1;
        data[1] = 0;
      }
      else {
        data[0] = 0;
        data[1] = 0;
      }
      break;

    case 0x11: // getVersion
      Serial.println("getVersion");
      if (data[1] == 0)
      {
        data[0] = 1;
        byte index;
        for (index = 0; index < 16 && apiVersion[index] > 0; index++)
        {
          data[2 + index] = apiVersion[index];
        }
        data[1] = index;
      }
      else
      {
        data[0] = 0;
        data[1] = 0;
      }
      break;

    case 0x13: // setStatusLed
      Serial.print("LED blink count: ");
      Serial.println(data[2]);
      if (data[1] == 1)
      {
        data[0] = 1;
        data[1] = 0;
      }
      else
      {
        data[0] = 0;
        data[1] = 0;
      }
      break;

    case 0x15: // ping
      Serial.println("ping");
      if (data[1] == 0)
      {
        data[0] = 1;
        data[1] = 0;
      }
      else
      {
        data[0] = 0;
        data[1] = 0;
      }
      break;

    case 0x21: // setDigitalOutput
      {
        Serial.println("setDigitalOutput");
        byte pin = data[2];
        byte state = data[3];

        if ((pin == GPIO2) && ((state == 0) || (state == 1)))
        {
          digitalWrite(GPIO2, state);

          // Return success
          data[0] = 1;
          data[1] = 0;
        }
        else
        {
          // Return failure
          data[0] = 0;
          data[1] = 0;
        }
      }
      break;

    case 0x62: // getEeprom
      Serial.println("getEeprom");
      if (data[1] == 3 &&
          data[2] <= 3 &&
          data[4] >= 1)
      {
        int offset = (data[2] * 256) + data[3];
        int readLength = data[4];

        // Setup return status
        data[0] = 1;
        data[1] = readLength;

        if ((offset == 0) && (readLength == 3)) {
          // Return eeprom valid indication
          data[2] = '#';
          data[3] = 0;
          data[4] = '#';

        }  else if ((offset == 160) && (readLength == 18)) {
          // Return key
          data[2] = '#';
          for (byte i = 0; i < 16; i++)
          {
            data[3 + i] = key[i];
          }
          data[19] = '#';
        }
      }
      else
      {
        data[0] = 0;
        data[1] = 0;
      }
      break;

    default:
      data[0] = 0;
      data[1] = 0;
      break;
  }
}

/*******************************************************************/
/***                     Main Program Setup                     ***/
/*******************************************************************/

void setup() {

  // Initialize variables
  Serial.begin(115200);
  delay(10);

  // Set the GPIO2 pin to output
  pinMode(GPIO2, OUTPUT);

  // Start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected - Local IP address: ");
  Serial.println(WiFi.localIP());
}

/*******************************************************************/
/***                      Main Program Loop                      ***/
/*******************************************************************/

void loop() {

  // Is client connected to Teleduino server ?
  if (! client.connected()) {
    // Not connected so attempt connection

    delay(1000);

    // Opening connection to Teleduino server
    Serial.println();
    Serial.println();
    Serial.println("Attemping connection to Teleduino server");

    if (client.connect(serverName, serverPort)) {
      Serial.println("connection succeeded!");
      initWriteQueue();
      stage = 0;
    }  else  {
      Serial.println("connection failed");
    }
    Serial.println();

  }  else  {

    // What we need to do depends on which 'stage' we are at
    switch (stage)
    {
      case 0: // Wait for start byte
        if (client.available())
        {
          char c = client.read();
          if (c == '?')
          {
            stage++;
          }
        }
        break;

      case 1: // Reset variables
        dataLength = 0;
        hexStage = false;
        stage++;
        break;

      case 2: // Instruction byte
        if (client.available())
        {
          char c = client.read();
          if (c == '?')
          {
            stage = 1;
            break;
          }
          else if (c == '\r' || c == '\n' || c == '.')
          {
            stage = 0;
            break;
          }
          if (!hexStage)
          {
            data[0] = hexDecode(c) * 16;
          }
          else
          {
            data[0] += hexDecode(c);
          }
          hexStage = !hexStage;
          if (!hexStage)
          {
            stage++;
          }
        }
        break;

      case 3: // Data length byte
        if (client.available())
        {
          char c = client.read();
          if (c == '?')
          {
            stage = 1;
            break;
          }
          else if (c == '\r' || c == '\n' || c == '.')
          {
            stage = 0;
            break;
          }
          if (!hexStage)
          {
            data[1] = hexDecode(c) * 16;
          }
          else
          {
            data[1] += hexDecode(c);
          }
          hexStage = !hexStage;
          if (!hexStage)
          {
            stage++;
          }
        }
        break;

      case 4: // Data
        if (client.available())
        {
          char c = client.read();
          if (c == '?')
          {
            stage = 1;
            break;
          }
          else if (c == '\r' || c == '\n' || c == '.')
          {
            if (dataLength == data[1])
            {
              stage++;
              break;
            }
            else
            {
              stage = 0;
              break;
            }
          }
          if (!hexStage)
          {
            data[2 + dataLength] = hexDecode(c) * 16;
          }
          else
          {
            data[2 + dataLength] += hexDecode(c);
          }
          hexStage = !hexStage;
          if (!hexStage)
          {
            dataLength++;
          }
        }
        break;

      case 5: // Execute instruction and return result
        executeInstruction(data);

        queueData('!');
        for (int i = 0; i < data[1] + 2; i++)
        {
          queueData(hexEncode(data[i] / 16));
          queueData(hexEncode(data[i] % 16));
        }
        queueData('\n');
        writeQueueData();
        stage = 0;
        break;
    }
  }
}
