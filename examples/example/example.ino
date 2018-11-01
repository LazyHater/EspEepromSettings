#include <EspEepromSettings.h>

void setup()
{
  Serial.begin(115200);

  if (espEepromSettings.read() != ESP_EEPROM_OK)
  {
    Serial.println("Formatting...");
    espEepromSettings.format();
  }
  espEepromSettings.print();
  Serial.print("Ssid: ");
  Serial.println(espEepromSettings.getWifiSsid());

  Serial.println("Setting ssid to: asdf");
  espEepromSettings.setWifiSsid("asdf");
  espEepromSettings.write();

  espEepromSettings.read();
  Serial.print("Ssid is now: ");
  Serial.println(espEepromSettings.getWifiSsid());
}

void loop()
{
  // put your main code here, to run repeatedly:
}
