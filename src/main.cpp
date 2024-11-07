#include <Wire.h>
#include <OneButton.h>
#include "display.h"
#include "attack.h"

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3){
    if (arg == 31337)
      return 1;
    else
      return 0;
}

OneButton btn1(13);
OneButton btn2(14);
OneButton btn3(33);


void setup()
{
	Serial.begin(115200);
	Wire1.begin(25, 26, 100000);								//инициализация дисплея

	WiFi.mode(WIFI_AP_STA);
	esp_wifi_set_max_tx_power(WIFI_POWER_19_5dBm);


	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		Serial.println(F("SSD1306 allocation failed"));
		for (;;);
	}

	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE);
	
	display.setRotation(2);
	display.display();

	//кнопочки
	btn1.setClickMs(0);
	btn1.setLongPressIntervalMs(100);
	btn1.setPressMs(500);
	btn1.attachClick([]
					{disp.Down(); });
	btn1.attachDuringLongPress([]
					{disp.Down(); });

	btn2.setClickMs(0);
	btn2.setLongPressIntervalMs(100);
	btn2.setPressMs(500);
	btn2.attachClick([]
					{disp.Up(); });
	btn2.attachDuringLongPress([]
					{disp.Up(); });

	btn3.setClickMs(0);
	btn3.setLongPressIntervalMs(50);
	btn3.setPressMs(500);
	btn3.attachClick([]
					{disp.Select(); });
	btn3.attachLongPressStart([]
					{disp.Hold(); });

	disp.drawMenu();

	disp.APs = new char*[1];
	disp.APs[0] = new char[5];
	
    strcpy(disp.APs[0], "Back");
}

void loop()
{
	btn1.tick();						//ещё кнопочки
	btn2.tick();
	btn3.tick();
	attack.deauthUpdate();
}
