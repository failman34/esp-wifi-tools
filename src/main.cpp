#include <Wire.h>
#include <OneButton.h>
#include "display.h"
#include "attack.h"
#include "SPIFFS.h"
#include "WebServer.h"
#include "sniffer.h"

#include "Adafruit_PN532.h"
#include "Font5x7Fixed.h"

// Adafruit_PN532 nfc(-1, -1, &Wire1);

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3)
{
	if (arg == 31337)
		return 1;
	else
		return 0;
}

OneButton btn1(32, 1, 1);
OneButton btn2(19, 1, 1);
OneButton btn3(33, 1, 1);

void setup()
{
	Serial.begin(115200);
	Wire1.begin(25, 26, 100000); // инициализация дисплея

	WiFi.mode(WIFI_AP_STA);
	WiFi.softAP("lol", "12345678", 13, 0, 1);
	esp_wifi_set_max_tx_power(WIFI_POWER_19_5dBm);

	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		Serial.println(F("SSD1306 allocation failed"));
		for (;;);
	}


	SPI.begin(14, 27, 13, 12);
	
	if(SD.begin(12)){
    	Serial.println("Card Mounted");
  	}

	display.setFont(&Font5x7Fixed);
	display.setTextWrap(0);
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE);

	display.setRotation(2);
	display.display();

	// кнопочки
	btn1.setClickMs(0);
	btn1.setLongPressIntervalMs(100);
	btn1.setPressMs(500);
	btn1.attachClick([]
					 { disp.Down(); });
	btn1.attachDuringLongPress([]
							   { disp.Down(); });

	btn2.setClickMs(0);
	btn2.setLongPressIntervalMs(100);
	btn2.setPressMs(500);
	btn2.attachClick([]
					 { disp.Up(); });
	btn2.attachDuringLongPress([]
							   { disp.Up(); });

	btn3.setClickMs(0);
	btn3.setLongPressIntervalMs(50);
	btn3.setPressMs(500);
	btn3.attachClick([]
					 { disp.Select(); });
	btn3.attachLongPressStart([]
							  { disp.Hold(); });
	disp.drawMenu();

}

void loop()
{
	btn1.tick(); // ещё кнопочки
	btn2.tick();
	btn3.tick();
	disp.tick();
	attack.deauthUpdate();
	sniff.tick();
}
