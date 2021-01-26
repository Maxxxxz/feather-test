#include <stdio.h>
//#include <HX711.h>

// to install
//https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Progmem strings

// TODO: optimize strings into progmem

//const char* DBURL     PROGMEM = "https://mygunwasmoved.firebaseio.com";
//const char* DBKEY     PROGMEM = "AIzaSyAVIcrbM3zrIjGvaRh5DgcAEN_IOSrgQUw"
//const char* LOGINURL  PROGMEM = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=AIzaSyAVIcrbM3zrIjGvaRh5DgcAEN_IOSrgQUw";
//
//const char* USERS     PROGMEM = "/users/";
//const char* DEVICES   PROGMEM = "/Devices.json";
//const char* AUTH      PROGMEM = "?auth=";

const char* _TESTPOSTURL = "https://mygunwasmoved.firebaseio.com/users/michaelcooper99@hotmail<dot>com/Devices.json?auth=";

// Temporary String vars for what will be progmem strings

const String DBURL = "https://mygunwasmoved.firebaseio.com";
const String DBKEY = "AIzaSyAVIcrbM3zrIjGvaRh5DgcAEN_IOSrgQUw";
const String LOGINURL = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=";

const String USERS = "/users/";
const String DEVICES = "/Devices.json";
const String AUTH = "?auth=";

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
 *     stores authKey.
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
        bool load();
        void save();
    private:
        BearSSL::WiFiClientSecure client;
        String authKey = "";
        String email = "";
        String emailClean = "";
        String password = "";
        String name = "";
        String ID = "";
};

FireBaseApp::FireBaseApp()
{
    this->client.setInsecure(); // This could lead to our URL being hijacked by a rogue dns; unlikely but something to think about
}

FireBaseApp::~FireBaseApp()
{
//    Serial.println("Destructed!");
//    delete[] this->authKey;
//    delete[] this->email;
//    delete[] this->password;
//    delete[] this->name;
}

// Save data to flash
void FireBaseApp::save()
{
  Serial.println("Called Save.");
}

// Load data from flash; also check if device ID exists in database under user
bool FireBaseApp::load()
{
    Serial.println("Called Load.");

    /****
     * Load data
     * if data exists:
     *  if device ID not still in db:
     *    return false
     *  else:
     *    return true
     * else:
     *  return false
    ****/
    
    return false;
}

bool FireBaseApp::FirebaseAuthAttempt(const char *email, const char *password)
{
    this->email = strdup(email);
    this->emailClean = strdup(email);
    this->emailClean.replace(".", "<dot>");
    this->password = strdup(password);
//    Serial.println(this->email);
//    Serial.println(this->password);

    HTTPClient https;
    
    // Load URL and other bits to build URL from progmem
    https.begin(this->client, (LOGINURL + DBKEY));
//    http.addHeader("Content-Type", "application/json");

    String data = "{"
                    "\"email\": \"" + this->email + "\","
                    "\"password\": \"" + this->password + "\","
                    "\"returnSecureToken\": \"true\","
                  "}";
    
    int rescode = https.POST(data);
//    Serial.println(rescode);
    String res = https.getString();
//    Serial.println(res);

    if(rescode == 200)
    {
//      this->authKey = parse token here
      int loc = res.indexOf("idToken");

      // failed to find idToken; should not happen if rescode == 200
      if(loc == -1) return false;

      
      {
        String toPass = "idToken\" \"";
        loc += toPass.length();
      }// put toPass out of scope to remove it

      String key = "";
      
      res = res.substring(loc);
      int cur = 1;
      // could optimize this with do-while with a curChar var
      while(res[cur] != '\"')
      {
        key += res[cur];
        cur++;
        yield();  // Stop the watchdog timer from killing the program
      }
//      Serial.println(key);
      this->authKey = key;
      
      return true;
    }
    else
    {
      return false;
    }
}

bool FireBaseApp::FirebaseRegisterDeviceAttempt(const char *name)
{
    this->name = strdup(name);

    HTTPClient https;

//"https://mygunwasmoved.firebaseio.com/users/michaelcooper99@hotmail<dot>com/Devices.json?auth="

    https.begin(this->client, (DBURL + USERS + this->emailClean + DEVICES + AUTH + this->authKey));

    String data = "{"
                    "\"name\": \"" + this->name + "\","
                    "\"status\": \"DISABLED\""
                  "}";
    int rescode = https.POST(data);
//    Serial.println(rescode);
//    Serial.println(https.getString());

    return (rescode == 200);
}

// Default start function; will be used in startup and in loop for when device it not found in db or on first time startup.
void defaultStart(FireBaseApp app)
{
  
}

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("Hello, World!");

  // Testing; always disconnect from wireless
  wfm.disconnect();

  // Testing; always start in station mode
  WiFi.mode(WIFI_STA);

  // Load data here
  FireBaseApp a;

  // Figure out some way to do check against changed wifi password;
  //    Maybe check if device ID exists in memory on failed wireless login
  if(a.load())
  {
    // 
  }
  else
  {
    // Could not load data, must be new
    // Add parameters here

    wfm.addParameter(&ownerEmail);
    wfm.addParameter(&ownerPassword);
    wfm.addParameter(&deviceName);
  
    wfm.setDarkMode(true);

  

//    wfm.autoConnect("feather", "PASSWORD");
    
    /*************************************/
    //  The config portal defaults to blocking behavior
    //  we can use this to handle failed wireless login
    //  attempts, as this is done interally by the library.
    //  After a successful connection, it closes the portal.
    //  This means that we can grab the information and
    //  attempt to connect to firebase with it, and fail the loop
    //  if these credentials are incorrect.
    bool firebaseSuccess = false; // store firebase connection success in bool
    
    do
    {
      wfm.startConfigPortal("feather");
  
      // Grab firebase information from wfm object and store in FireBaseApp object
      firebaseSuccess = a.FirebaseAuthAttempt(ownerEmail.getValue(), ownerPassword.getValue());
  
      // remove this
      firebaseSuccess = a.FirebaseRegisterDeviceAttempt(deviceName.getValue());
      
    }while(!firebaseSuccess);
    /*************************************/
  }
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
