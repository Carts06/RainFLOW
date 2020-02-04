// Definitions
#define MICRO_BAUD_RATE 115200  // Microcontroller Baud Rate
#define TINY_GSM_MODEM_SIM800   // GSM/GPRS Module Model
#define GSM_RX  17              // GSM/GPRS Module RX Pin
#define GSM_TX  16              // GSM/GPRS Module TX Pin
#define RGPIN 2                 // Rain Guage Pin

// Libraries
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <TinyGsmClient.h>
#include <HardwareSerial.h>

// Network Details
const char apn[]      = "smartlte";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Server Details
const char* serverAddr            = "test.mosquitto.org";
const short unsigned serverPort   = 1883;


// Node Details
const char* APIKey  = "86ffc18d-8377-4653-bddc-df61fe88c448";   // Change to API-Key
// Variables
double rainfall   = 0;          // Total Amount of Rainfall
double tipAmount  = 1;          // Calibrated Tipping Amount

// Initialisation
HardwareSerial SerialAT(1); // RX, TX
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient rainflow(client);

void setup() {
  Serial.begin(MICRO_BAUD_RATE);
  Serial.println("Initialising System...");
  attachRainGauge(RGPIN);
  connectGSM();
}

void connectGSM() {
  SerialAT.begin(9600, SERIAL_8N1, GSM_TX, GSM_RX);
  if (!modem.restart()) {
    Serial.println("Failed to restart modem. Trying in 10s.");
    delay(3000);
    SerialAT.begin(9600, SERIAL_8N1, GSM_TX, GSM_RX);
    delay(10000);
    return;
  }
  Serial.println("Modem Name: " + modem.getModemName());
  Serial.println("Modem Info: " + modem.getModemInfo());
  Serial.println("");
  //Connect to GPRS
  Serial.println("Connecting to " + String(apn));
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    delay(10000);
    return;
  }
  if (modem.isGprsConnected()) {
    Serial.println("Connected");
  } else {
    Serial.println("Disconnected!");
  }
  Serial.println("CCID:           " + String(modem.getSimCCID()));
  Serial.println("IMEI:           " + String(modem.getIMEI()));
  Serial.println("Operator:       " + String(modem.getOperator()));
  Serial.println("IP Addr:        " + String(modem.localIP()));
  Serial.println("Signal Quality: " + String(modem.getSignalQuality()));
#ifndef TINY_GSM_MODEM_SIM800
  Serial.println("GSM Loc:        " + String(modem.getGsmLocation()));
  Serial.println("GSM Loc:        " + String(modem.getGSMDateTime(DATE_TIME)));
  Serial.println("GSM Loc:        " + String(modem.getGSMDateTime(DATE_DATE)));
#endif
  getBatteryStatus();
}

void connectServer() {
  rainflow.setServer(serverAddr, serverPort);
  rainflow.setCallback(rainflowCallback);
  Serial.println("Connecting to RainFLOW Server at " + String(serverAddr));
  while (connectMqtt() == false) continue;
  Serial.println();
}

void getBatteryStatus() {
  uint8_t chargeState = -99;
  int8_t percent = -99;
  uint16_t milliVolts = -9999;
  modem.getBattStats(chargeState, percent, milliVolts);
  Serial.println("Battery Charge State:   " + String(chargeState));
  Serial.println("Battery Charge Percent: " + String(percent));
  Serial.println("Battery Voltage:        " + String(milliVolts / 1000.0F));
}

bool connectMqtt() {
  if (!rainflow.connect("APIKey")) {
    Serial.print(".");
    return false;
  }
  Serial.println("Connected to server!");
  char topic[45];
  strcpy(topic, "status/");
  strcat(topic, APIKey);
  rainflow.subscribe(topic);
  return rainflow.connected();
}

void rainflowCallback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Received Message: ");
  Serial.write(payload, len);
  Serial.println();
}

void attachRainGauge(int rainGaugePin) {
  pinMode(rainGaugePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(rainGaugePin), tippingBucket, FALLING);
}

void tippingBucket() {
  unsigned long lastDetectedTipMillis;
  int tipInterval = 50; // 50ms
  if (millis() - lastDetectedTipMillis > tipInterval) {
    rainfall += tipAmount;
    Serial.print("Bucket tipped! Tipped ");
    Serial.print(rainfall);
    Serial.println(" times.");
    lastDetectedTipMillis = millis();
  }
}

void loop() {
}
