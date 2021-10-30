#include <ESP8266WiFi.h>

#include "WiFiFunctions.h"

/* This functions tries to connect to any of the given accessPoints wlan in the order they are given. */
WiFiInfo WiFiConnect(WiFiInfo *accessPoints, int size)
{
  while (WiFi.status() != WL_CONNECTED)
  {
    for (int i = 0; i < size; i++)
    {
      WiFi.disconnect();

      // Connect to WiFi network
      Serial.println();
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(accessPoints[i].ssid);

      WiFi.begin(accessPoints[i].ssid, accessPoints[i].wpa);

      int wait = 0;
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
        wait++;
        if (wait > 20)
        {
          Serial.println();
          Serial.println("Couldn't connect to WiFi...");
          break;
        }
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println();
        Serial.println("Succesfully connected!");

        // Print the IP address
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        Serial.println();

        return accessPoints[i];
      }
    }
  }

  return WiFiInfo{0};
}
