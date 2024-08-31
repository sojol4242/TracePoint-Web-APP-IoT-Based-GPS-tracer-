
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266Firebase.h>
#define REFERENCE_URL "https://tracepoint-a2ae7-default-rtdb.firebaseio.com/"  
TinyGPSPlus gps;          // The TinyGPS++ object
SoftwareSerial ss(4, 5);  // The serial connection to the GPS device

const char* ssid = "WiFI";
const char* password = "1234567568";
Firebase firebase(REFERENCE_URL);
float latitude, longitude;
int year, month, date, hour, minute, second;
String date_str, time_str, lat_str, lng_str;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  ss.begin(9600);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");


  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      if (gps.location.isValid()) {
        latitude = gps.location.lat();
        lat_str = String(latitude, 6);
        longitude = gps.location.lng();
        lng_str = String(longitude, 6);
      }

      if (gps.date.isValid()) {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        // Format the date string
        if (date < 10) date_str = '0';
        date_str += String(date);
        date_str += " / ";

        if (month < 10) date_str += '0';
        date_str += String(month);
        date_str += " / ";

        date_str += String(year);
      }

      if (gps.time.isValid()) {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();

        // Adjust to Bangladesh Standard Time (BST) - UTC+6
        minute = (minute + 0);  // No need to adjust minutes
        hour = (hour + 6);
        if (hour >= 24) {  // Handle overflow of hours
          hour = hour - 24;

        
          date += 1;
    
          if ((month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10) && date > 31) {
            date = 1;
            month += 1;
          } else if ((month == 4 || month == 6 || month == 9 || month == 11) && date > 30) {
            date = 1;
            month += 1;
          } else if (month == 2 && date > 28) {  
            date = 1;
            month += 1;
          } else if (month == 12 && date > 31) {
            date = 1;
            month = 1;
            year += 1;
          }
        }

        // Format the time string
        if (hour < 10) time_str = '0';
        time_str += String(hour);
        time_str += " : ";

        if (minute < 10) time_str += '0';
        time_str += String(minute);
        time_str += " : ";

        if (second < 10) time_str += '0';
        time_str += String(second);
      }

        firebase.setString("/lat", lat_str);
        firebase.setString("/lon", lng_str);
        firebase.setString("/date", date_str);
        firebase.setString("/time", time_str);
        
    }
  }

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  client.print(s);
  delay(100);
}
