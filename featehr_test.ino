#include <stdio.h>
//#include <HX711.h>

// to install
//https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide
#include <WiFiManager.h>

//HX711 cell;
//const int CELL_D_PIN = 12;
//const int CELL_SCK_PIN = 14;

//const long LOADCELL_OFFSET = 50682624;
//const long LOADCELL_DIVIDER = 5895655;

// what? https://github.com/tzapu/WiFiManager/issues/379
WiFiManagerParameter ownerEmail("email", "E-Mail", "", 50);
WiFiManagerParameter ownerPassword("password", "Password", "", 50);

WiFiManager wfm;

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("Hello, World!");

  wfm.disconnect();
  
  // Testing; always start in station mode
  WiFi.mode(WIFI_STA);

  // Add parameters here

  wfm.addParameter(&ownerEmail);
  wfm.addParameter(&ownerPassword);

  wfm.setDarkMode(true);
//  wfm.autoConnect("feather", "PASSWORD");
  wfm.startConfigPortal("feather");

//  cell.begin(CELL_D_PIN, CELL_SCK_PIN);
//  cell.set_cell(LOADCELL_DIVIDER);
//  cell.set_offset(LOADCELL_OFFSET);
}

void loop() {
//  if (cell.is_ready()) {
//    long reading = cell.read();
//    Serial.print("HX711 reading: ");
//    Serial.println(reading);
//  } else {
//    Serial.println("HX711 not found.");
//  }

//  Serial.println(ownerEmail.getValue());
//  Serial.println(ownerPassword.getValue());

  Serial.println("Hello!");

  delay(1000);
}
