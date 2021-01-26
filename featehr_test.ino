#include <stdio.h>
//#include <HX711.h>

// to install
//https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Progmem strings

// TODO: optimize strings into progmem

const char* dbURL     PROGMEM = "https://mygunwasmoved.firebaseio.com";
const char* loginURL  PROGMEM = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword";

const char* USERS     PROGMEM = "/users/";
const char* DEVICES   PROGMEM = "/Devices.json";
const char* AUTH      PROGMEM = "?auth";

const char* _TESTPOSTURL = "https://mygunwasmoved.firebaseio.com/users/michaelcooper99@hotmail<dot>com/Devices.json?auth=";

//HX711 cell;
//const int CELL_D_PIN = 12;
//const int CELL_SCK_PIN = 14;

//const long LOADCELL_OFFSET = 50682624;
//const long LOADCELL_DIVIDER = 5895655;

// what? https://github.com/tzapu/WiFiManager/issues/379
WiFiManagerParameter ownerEmail("email", "E-Mail", "", 50);
WiFiManagerParameter ownerPassword("password", "Password", "", 50);
WiFiManagerParameter deviceName("devicename", "Device Name", "", 50);

WiFiManager wfm;

/************************************
 * Device Authentication and Registration Process
 * 1. Device calls FirebaseAuthAttempt on successful WiFi connection; stores email and password and device name.
 *  a. If FirebaseAuthAttempt fails, returns false; should cause device to return to station mode.
 *  b. If FirebaseAuthAttempt succeeds, returns true; device continues into client mode;
 *     stores authToken.
 * 2. Device (or FirebaseAuthAttempt?) will call FirebaseRegisterDeviceAttempt
 *  a. If FirebaseRegisterDeviceAttempt fails, returns false; something bad went wrong,
 *     return to station mode with error stating something went wrong on firebase's end.
 *  b. If FirebaseRegisterDeviceAttempt succeeds, returns true; device successfully connected.
************************************/
// This class will store important connection details to the Firebase Database.
class FireBaseApp{
    public:
        FireBaseApp();
        ~FireBaseApp();
        bool FirebaseAuthAttempt(const char *email, const char* password);
        bool FirebaseRegisterDeviceAttempt(const char *name);
        void getname();
    private:
        BearSSL::WiFiClientSecure client;
        char *authToken = "";
        char *email = "";
        char *password = "";
        char *name = "";
};

FireBaseApp::FireBaseApp()
{
    this->client.setInsecure(); // This could lead to our URL being hijacked by a rogue dns; unlikely but something to think about
}

FireBaseApp::~FireBaseApp()
{
//    Serial.println("Destructed!");
//    delete[] this->authToken;
//    delete[] this->email;
//    delete[] this->password;
//    delete[] this->name;
}

void FireBaseApp::getname()
{
    Serial.println("Get Name Called.");
}

bool FireBaseApp::FirebaseAuthAttempt(const char *email, const char *password)
{
    this->email = strdup(email);
    this->password = strdup(password);
//    Serial.println(this->email);
//    Serial.println(this->password);

    HTTPClient https;
    
    // Load URL and other bits to build URL from progmem
    https.begin(client, _TESTPOSTURL);
    String data = "";
    int rescode = https.POST(data);
    Serial.println(rescode);
    Serial.println(https.getString());

    return true;
}

bool FireBaseApp::FirebaseRegisterDeviceAttempt(const char *name)
{
    this->name = strdup(name);

    HTTPClient https;

    String data = "{"
                    "\"name\": " this->name ","
                    "\"status\": \"DISABLED\""
                  "}";
    int rescode = https.POST(data);
    Serial.println(rescode);
    Serial.println(https.getString());

    return true;
}

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
  wfm.addParameter(&deviceName);

  wfm.setDarkMode(true);
//  wfm.autoConnect("feather", "PASSWORD");
  
  /*************************************/
  //  The config portal defaults to blocking behavior
  //  we can use this to handle failed wireless login
  //  attempts, as this is done interally by the library.
  //  After a successful connection, it closes the portal.
  //  This means that we can grab the information and
  //  attempt to connect to firebase with it, and fail the loop
  //  if these credentials are incorrect.
  bool firebaseSuccess = false; // store firebase connection success in bool

  FireBaseApp a;
  
  
  do
  {
    wfm.startConfigPortal("feather");

    // Grab firebase information from wfm object and store in FireBaseApp object
    firebaseSuccess = a.FirebaseAuthAttempt(ownerEmail.getValue(), ownerPassword.getValue());

    // remove this
    firebaseSuccess ? (void)(firebaseSuccess = a.FirebaseRegisterDeviceAttempt(deviceName.getValue())) : ((void)0);
    
  }while(!firebaseSuccess);
  /*************************************/

  // Set up HX711 here

//  cell.begin(CELL_D_PIN, CELL_SCK_PIN);
//  cell.set_cell(LOADCELL_DIVIDER);
//  cell.set_offset(LOADCELL_OFFSET);
}

/************************************
 * Here in loop we will continously read
 * (and possibly convert) the value given to
 * us by the HX711 amplifier. We will compare this with the 
 * 
************************************/
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
