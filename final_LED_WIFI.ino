#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <aREST.h>
#include <EEPROM.h>
#define PRINTDEBUG(STR) \
{  \
  Serial.println(STR); \
}

WiFiServer server(80);
aREST rest = aREST();
 char WifiSSID[10] = "";
 char wifiPass[10] = "";

// Fucntion to connect WiFi

void connectWifi(const char* ssid, const char* password) {
  int WiFiCounter = 0;
  // We start by connecting to a WiFi network
  PRINTDEBUG("Connecting to ");
  PRINTDEBUG(ssid);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED && WiFiCounter < 30) {
    delay(1000);
    WiFiCounter++;
    PRINTDEBUG(".");
  }
  saveCredentials();
  PRINTDEBUG("");
  PRINTDEBUG("WiFi connected");
  PRINTDEBUG("IP address: ");
  PRINTDEBUG(WiFi.localIP());
}

int setSSID(String command) {
  string2char(command, WifiSSID);
  PRINTDEBUG(WifiSSID);
  return 1;
}

int setPassword(String command) {
  string2char(command, wifiPass);
  PRINTDEBUG(wifiPass);
  return 1;
}

void string2char(String command, char *p){
    if(command.length()!=0){
          command.toCharArray(p, command.length() + 1); 
    }
}

void setupAP() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  rest.function("ssid",setSSID);
  rest.function("password",setPassword);
  rest.set_id("1");

  rest.set_name("esp8266");

  boolean result = WiFi.softAP("ESP8266", "123456789");
  if(result == true)
  {
    PRINTDEBUG("Ready");
  }
  else
  {
    PRINTDEBUG("Failed!");
  }
  server.begin();

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  PRINTDEBUG("HotSpt IP:");
  PRINTDEBUG(myIP);
  getWifiData();
 
}

void getWifiData() {
  WiFiClient client = server.available();
  while (!client) {
    client = server.available();
  }
  PRINTDEBUG("client Found!");
  while(!client.available()){
    delay(1);
  }
  PRINTDEBUG("client Available!");
  rest.handle(client);
PRINTDEBUG("-----------comp");
  PRINTDEBUG(strcmp(WifiSSID, ""));
  PRINTDEBUG(strcmp(wifiPass, ""));
  if ((strcmp(wifiPass, "") == 0) || (strcmp(WifiSSID, "") == 0)) {
      getWifiData();
      return;
  }
  PRINTDEBUG("Wifi SSID ");
  PRINTDEBUG(WifiSSID);
  PRINTDEBUG("Wifi PASS");
  PRINTDEBUG(wifiPass);
}

void loadCredentials() {
  Serial.println("Load credentials:");
  EEPROM.begin(512);
  EEPROM.get(0, WifiSSID);
  EEPROM.get(0+sizeof(WifiSSID), wifiPass);
  char ok[2+1];
  EEPROM.get(0+sizeof(WifiSSID)+sizeof(wifiPass), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    WifiSSID[0] = 0;
    wifiPass[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(WifiSSID);
  Serial.println(strlen(wifiPass)>0?"********":"<no password>");
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, WifiSSID);
  EEPROM.put(0+sizeof(WifiSSID), wifiPass);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(WifiSSID)+sizeof(wifiPass), ok);
  EEPROM.commit();
  EEPROM.end();
}

void setup() {
  // put your setup code here, to run once:
  // loadCredentials();
  Serial.begin(115200);  //Start Serial
 
  if (!isConnectedToWifi()) {
    PRINTDEBUG("Setup AP");
    setupAP();
  }
//  int temperature = 24;
//  int humidity = 40;
//  rest.variable("temperature",&temperature);
//  rest.variable("humidity",&humidity);

  
//  delay(10);
  Serial.print("Connecting");
  connectWifi(WifiSSID, wifiPass); // Start WiFi
    Serial.print("Connecting");
  pinMode(2, OUTPUT);
  server.begin();  // Start Server
//  dude.begin(DUDEPIN); // Start class
  delay(10);

  Firebase.begin("arduino-64ea7.firebaseio.com", "k1CwePMTgBBuP5n2JcWPrJELHA8m8PW6FN0KXils");
  Firebase.set("led", 0);
  
}

boolean isWifiInfoFetched() {
  return (wifiPass != "" && WifiSSID != "");
}

boolean isConnectedToWifi() {
  PRINTDEBUG("Wifi Connected ? -");
  PRINTDEBUG(WiFi.status() == WL_CONNECTED);
  return (WiFi.status() == WL_CONNECTED);
}

void loop() {
  // put your main code here, to run repeatedly:
// getWifiData();

if (digitalRead(2) != Firebase.getInt("led")) {
  digitalWrite(2, Firebase.getInt("led"));  
}
}
