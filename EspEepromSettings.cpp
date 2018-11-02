// EspEepromSettings.cpp
#include <FastCRC.h>
#include "EspEepromSettings.h"
#include "EspEepromSettingsDefaults.h"
// #include "Logger.h"

FastCRC32 CRC32;
int EspEepromSettings::checkVersion()
{
	for (int i = 0; i < sizeof(ESP_EEPROM_VER); ++i)
	{
		if (EEPROM.read(i) != ESP_EEPROM_VER[i])
		{
			Serial.println("Versions do not match, format needed.");
			return ESP_EEPROM_VERSION_DO_NOT_MATCH;
		}
	}
	return ESP_EEPROM_OK;
}

int EspEepromSettings::checkCrc()
{
	uint32_t crc32 = CRC32.crc32((uint8_t *)&settings, sizeof(Settings) - sizeof(Settings::crc32));
	if (crc32 != settings.crc32)
	{
		Serial.println("Crc32 not valid!");
		return ESP_EEPROM_CRC_DO_NOT_MATCH;
	}
	return ESP_EEPROM_OK;
}

int EspEepromSettings::read()
{
	EEPROM.begin(sizeof(Settings));
	uint8_t *curr_pos = (uint8_t *)&settings;
	for (int i = 0; i < sizeof(Settings); ++i)
	{
		*(curr_pos) = EEPROM.read(i);
		curr_pos++;
	}

	int version_result = checkVersion();
	if (version_result != ESP_EEPROM_OK)
	{
		this->inited = false;
		EEPROM.end();
		return version_result;
	}

	int crc_result = checkCrc();
	if (crc_result != ESP_EEPROM_OK)
	{
		this->inited = false;
		EEPROM.end();
		return crc_result;
	}

	this->inited = true;
	EEPROM.end();
	return ESP_EEPROM_OK;
}

void EspEepromSettings::setDefaults()
{
	this->setWifiSsid(default_wifi_ssid);
	this->setWifiPassword(default_wifi_password);
	this->setMqttHostname(default_mqtt_hostname);
	this->setMqttPort(default_mqtt_port);
	this->setMqttUsername(default_mqtt_username);
	this->setMqttPassword(default_mqtt_password);
}

int EspEepromSettings::print()
{
	Serial.print("\n\nEspEepromSettings:\nversion: ");
	Serial.write(settings.version, sizeof(Settings::version));
	Serial.print("\nwifi_ssid: ");
	Serial.write(settings.wifi_ssid, sizeof(Settings::wifi_ssid));
	Serial.print("\nwifi_password: ");
	Serial.write(settings.wifi_password, sizeof(Settings::wifi_password));
	Serial.print("\nmqtt_host: ");
	Serial.write(settings.mqtt_host, sizeof(Settings::mqtt_host));
	Serial.print("\nmqtt_port: ");
	Serial.println(settings.mqtt_port);
	Serial.print("mqtt_username: ");
	Serial.write(settings.mqtt_username, sizeof(Settings::mqtt_username));
	Serial.print("\nmqtt_password: ");
	Serial.write(settings.mqtt_password, sizeof(Settings::mqtt_password));
	Serial.print("\ncrc32: ");
	Serial.println(settings.crc32, HEX);
}

void EspEepromSettings::format(bool set_defaults)
{
	this->inited = true;
	this->setStringHelper(this->settings.version, ESP_EEPROM_VER, sizeof(Settings::version));
	memset(((uint8_t *)&settings) + sizeof(Settings::version), 0, sizeof(Settings) - sizeof(Settings::version));
	if (set_defaults)
	{
		this->setDefaults();
	}
	this->write();
}

int EspEepromSettings::write()
{
	if (!this->inited)
	{
		Serial.println("Write settings error: settings are not inited!");
		return ESP_EEPROM_SETTINGS_NOT_INITED;
	}

	settings.crc32 = CRC32.crc32((uint8_t *)&settings, sizeof(Settings) - sizeof(Settings::crc32));

	EEPROM.begin(sizeof(Settings));
	uint8_t *curr_pos = (uint8_t *)&settings;
	for (int i = 0; i < sizeof(Settings); ++i)
	{
		EEPROM.write(i, *(curr_pos + i));
	}
	EEPROM.end();
}

int EspEepromSettings::setWifiSsid(const char *ssid)
{
	return this->setStringHelper(this->settings.wifi_ssid, ssid, sizeof(Settings::wifi_ssid));
}
int EspEepromSettings::setWifiPassword(const char *password)
{
	return this->setStringHelper(this->settings.wifi_password, password, sizeof(Settings::wifi_password));
}
int EspEepromSettings::setMqttHostname(const char *hostname)
{
	return this->setStringHelper(this->settings.mqtt_host, hostname, sizeof(Settings::mqtt_host));
}
int EspEepromSettings::setMqttPort(uint16_t port)
{
	this->settings.mqtt_port = port;
	return ESP_EEPROM_OK;
}
int EspEepromSettings::setMqttUsername(const char *username)
{
	return this->setStringHelper(this->settings.mqtt_username, username, sizeof(Settings::mqtt_username));
}
int EspEepromSettings::setMqttPassword(const char *password)
{
	return this->setStringHelper(this->settings.mqtt_password, password, sizeof(Settings::mqtt_password));
}

int EspEepromSettings::setStringHelper(uint8_t *dst, const char *src, int dst_size)
{
	if (!this->inited)
	{
		Serial.println("Write settings error: settings are not inited!");
		return ESP_EEPROM_SETTINGS_NOT_INITED;
	}

	int size = strlen(src);
	if (size > dst_size - 1)
	{
		Serial.println("Write settings error: source string too long!");
		return ESP_EEPROM_SOURCE_STRING_TOO_LONG;
	}
	memset(dst + size, 0, dst_size - size);
	memcpy(dst, src, size);
	return ESP_EEPROM_OK;
}

const char *EspEepromSettings::getWifiSsid()
{
	return (char *)this->settings.wifi_ssid;
}
const char *EspEepromSettings::getWifiPassword()
{
	return (char *)this->settings.wifi_password;
}
const char *EspEepromSettings::getMqttHostname()
{
	return (char *)this->settings.mqtt_host;
}
uint16_t EspEepromSettings::getMqttPort()
{
	return this->settings.mqtt_port;
}
const char *EspEepromSettings::getMqttUsername()
{
	return (char *)this->settings.mqtt_username;
}
const char *EspEepromSettings::getMqttPassword()
{
	return (char *)this->settings.mqtt_password;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ESP_EEPROM_SETTINGS)
EspEepromSettings espEepromSettings;
#endif
