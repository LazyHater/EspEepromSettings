// EspEepromSettings.h

#ifndef ESP_EPROM_SETTINGS_H
#define ESP_EPROM_SETTINGS_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <EEPROM.h>

#define ESP_EEPROM_VER "ESP_V01"
#define ESP_EEPROM_OK 0
#define ESP_EEPROM_VERSION_DO_NOT_MATCH -1
#define ESP_EEPROM_CRC_DO_NOT_MATCH -2
#define ESP_EEPROM_SETTINGS_NOT_INITED -3

class EspEepromSettings
{
  public:
	bool inited = false;
	int read();
	int write();
	int print();
	void setDefaults();
	void format(bool set_defaults = true);

	int setWifiSsid(char *ssid);
	int setWifiPassword(char *password);
	int setMqttHostname(char *hostname);
	int setMqttPort(uint16_t port);
	int setMqttUsername(char *username);
	int setMqttPassword(char *password);

	char *getWifiSsid();
	char *getWifiPassword();
	char *getMqttHostname();
	uint16_t getMqttPort();
	char *getMqttUsername();
	char *getMqttPassword();

  private:
	int setStringHelper(uint8_t *dst, char *src, int dst_size);
	int checkVersion();
	int checkCrc();
	struct Settings
	{
		uint8_t version[8];
		uint8_t wifi_ssid[32];
		uint8_t wifi_password[64];
		uint8_t mqtt_host[32];
		uint16_t mqtt_port;
		uint8_t mqtt_username[32];
		uint8_t mqtt_password[32];
		uint32_t crc32;
	} settings;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ESP_EEPROM_SETTINGS)
extern EspEepromSettings espEepromSettings;
#endif

#endif // ESP_EPROM_SETTINGS_H