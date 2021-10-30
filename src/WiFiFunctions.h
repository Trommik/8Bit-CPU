/*
 * Holds functions for WiFi use.
 */

#ifndef WIFI_FUNCTIONS
#define WIFI_FUNCTIONS

/* Holds information about a wireless network. */
struct WiFiInfo
{
  const char *ssid;
  const char *wpa;
};

/* This functions tries to connect to any of the given accessPoints wlan in the order they are given. */
WiFiInfo WiFiConnect(WiFiInfo *accessPoints, int size);

#endif