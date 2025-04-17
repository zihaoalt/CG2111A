// sensor_side.ino

#include <Arduino.h>
#include "packet.h"       // Defines TPacket and PACKET_SIZE, MAX_STR_LEN, etc.
#include "constants.h"    // Defines PACKET_TYPE_RESPONSE, RESP_COLOUR, etc.

#define S0 39
#define S1 38
#define S2 37
#define S3 36
#define sensorOut 35

// Structure to hold sensor readings and computed colour index.
struct ColorResult {
  long red;
  long green;
  long blue;
  int colorIndex; // 0 for Red, 1 for Green
};

const int numColors = 3;  // Only two colours are considered: Red and Green

// Calibrated reference values for Red and Green.
// (These values might be obtained via a calibration routine.)
float calibratedColors[numColors][3] = {
  {189, 270, 246},   // Reference for Red (index 0)
  {240, 225, 231},   // Reference for Green (index 1)
  {299, 294, 253}    // Reference for Others (index 2)
};

// Reads the sensor frequency for a selected filter configuration.
long readFrequency(int s2State, int s3State) {
  digitalWrite(S2, s2State);
  digitalWrite(S3, s3State);
  delay(50); // Allow the sensor to settle.
  return pulseIn(sensorOut, LOW);
}

const float THRESHOLD = 200; 

void setupColour() {
  // Set sensor control pins as outputs; sensorOut as input.
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Start Serial for debugging and communication.
  Serial.begin(9600);
  delay(2000); // Wait for Serial Monitor
  
  // Set frequency scaling mode.
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  
  //Serial.println("Sensor ready for colour detection.");
}

void evaluateColour(int r, int g, int b, TPacket *colour) {
  float rgbArr[3][3] = {{0.70, 0.7682927, 1.0875610}, {1.0666667, 1.0389610, 0.9740260}, {1.017007, 1.1818182, 1.162075}};
  float rgNew = (float)r / (float)g;
  float rbNew = (float)r / (float)b;
  float gbNew = (float)g / (float)b;
  float rgbCol[3] = {rgNew, rbNew, gbNew};

  float errCol[3] = {0, 0, 0};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      errCol[i] += (rgbCol[j] - rgbArr[i][j]) * (rgbCol[j] - rgbArr[i][j]);
    }
  }

  float minimum = errCol[0];
  int min_index = 0;
  for (int k = 0; k < 3; k++) {
    if (errCol[k] < minimum) {
      minimum = errCol[k];
      min_index = k;
    }
  }
  colour->params[3] = min_index;



/*
  // Use least squared error to determine best match.
  float minError = 1e9;
  int bestIndex = -1;
  for (int i = 0; i < numColors; i++) {
    float error = 0;
    error += abs(r   - calibratedColors[i][0]);
    error += abs(g - calibratedColors[i][1]);
    error += abs(b  - calibratedColors[i][2]);
    if(error>THRESHOLD)
    {
      bestIndex = -1;
 
    }
    else if (error < minError) {
      minError = error;
      bestIndex = i;
    }
  }
  colour->params[3] = bestIndex;
  */
}

// Reads all three channels and computes the closest match.
void readColour() {
  TPacket colour;

  colour.packetType = PACKET_TYPE_RESPONSE;
  colour.command = RESP_COLOUR;
  
  // Read Red (S2 = LOW, S3 = LOW)
  colour.params[0] = readFrequency(LOW, LOW);
  // Read Green (S2 = HIGH, S3 = HIGH)
  colour.params[1] = readFrequency(HIGH, HIGH);
  // Read Blue (S2 = LOW, S3 = HIGH)
  colour.params[2] = readFrequency(LOW, HIGH);
  

  evaluateColour(colour.params[0], colour.params[1], colour.params[2], &colour);
  sendResponse(&colour);
}

//// Sends a TPacket over Serial (simulate sending to server).
//void sendResponse(TPacket *packet) {
//  char buffer[PACKET_SIZE];
//  // For simplicity, store packetType in first byte and then the params array.
//  buffer[0] = packet->packetType;
//  memcpy(&buffer[1], packet->params, sizeof(packet->params));
//  Serial.write(buffer, PACKET_SIZE);
//}

//// Creates a TPacket with colour sensor data and sends it.
//void sendColourPacket() {
//  ColorResult currentColor = readColor();
//  
//  TPacket colourPacket;
//  colourPacket.packetType = PACKET_TYPE_RESPONSE;  // Generic response packet.
//  colourPacket.command = RESP_COLOUR;                // Indicates colour sensor data.
//  
//  // Fill parameters: params[0]=red, params[1]=green, params[2]=blue, params[3]=colour index.
//  colourPacket.params[0] = currentColor.red;
//  colourPacket.params[1] = currentColor.green;
//  colourPacket.params[2] = currentColor.blue;
//  colourPacket.params[3] = currentColor.colorIndex;
//  
//  sendResponse(&colourPacket);
//}
