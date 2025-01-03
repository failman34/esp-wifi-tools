#include "display.h"

Adafruit_SSD1306 display(128, 64, &Wire1);
Display disp;

void Display::updateMenu()
{
	switch (menu_level)
	{
	case AP_LIST_MENU:
		draw_WiFi_menu();
		break;

	case AP_INFO_MENU:
		draw_AP_info();
		break;

	default:
		drawMenu();
		break;
	}
}

void Display::drawMenu()
{
	display.clearDisplay();
	display.setTextSize(main_menu_size);

	int index = 0;
	int max_len = 8;
	uint menu_size = menu[menu_level]->size;
	Serial.println(menu_size);
	Serial.println(menu_index);

	if (menu_index + 1 > max_len) // расчеты для хер знает чего(забыл)
	{
		index = menu_index + 1 - max_len;
	}

	for (int i = 0; i < max_len; i++) // рисование
	{
		if (i + index < menu_size)
		{
			if (i + index == menu_index)
			{
				display.setCursor(0, (i * 8) + y_offset);
				display.print(">"); // символ выбора
			}
			display.setCursor(10, (i * 8) + y_offset);
			if(menu_level == SNIFFER_MENU && i+index == 1){
				display.printf(menu[menu_level]->items[index + i], sniff.sniffer_channel);
			}else{
				display.print(menu[menu_level]->items[index + i]);
				Serial.println(menu[menu_level]->items[index + i]);
			}
		}
		else
		{
			break;
		}
	}
	display.display(); // Обновление экрана
	display.setTextSize(1);
}

void Display::draw_WiFi_menu()
{
	Serial.println(menu_index);
	display.clearDisplay();
	int index = 0;
	int max_len = 8;

	if (menu_index + 1 > max_len) // расчеты для хер знает чего(забыл)
	{
		index = menu_index + 1 - max_len;
	}

	display.setCursor(0, 0 + y_offset);

	for (int i = 0; i < max_len; i++) // рисование
	{
		if (i + index < scan.APs_count + 1)
		{
			if (i + index == menu_index)
			{
				display.setCursor(0, (i * 8) + y_offset);
				display.print(">"); // символ выбора
			}
			if (i + index != 0 && scan.AP_list[(i + index - 1)].selected == true)
			{
				display.setCursor(8, (i * 8) + y_offset);
				display.print("*"); // символ выбора
			}
			display.setCursor(16, (i * 8) + y_offset);
			if (index == 0 && i == 0)
			{
				display.print("Back");
			}
			else
			{
				display.printf("%s", scan.AP_list[(index + i) - 1].ssid);
			}
		}
		else
		{
			break;
		}
	}
	display.display(); // Обновление экрана
}

void Display::draw_AP_info()
{
	display.clearDisplay();
	display.setTextWrap(1);
	display.setCursor(0, 0 + y_offset);
	display.printf("SSID: %s", scan.AP_list[selected_AP].ssid);

	uint8_t y = display.getCursorY() - 7;

	display.setCursor(0, y + 8 + y_offset);
	display.printf("MAC %02X:%02X:%02X:%02X:%02X:%02X\n",
				   scan.AP_list[selected_AP].mac[0], scan.AP_list[selected_AP].mac[1],
				   scan.AP_list[selected_AP].mac[2], scan.AP_list[selected_AP].mac[3],
				   scan.AP_list[selected_AP].mac[4], scan.AP_list[selected_AP].mac[5]);

	display.setCursor(0, y + 16 + y_offset);

	display.printf("Channel: %u\n", scan.AP_list[selected_AP].channel);

	display.setCursor(0, y + 24 + y_offset);

	display.printf("RSSI: %d dBm\n", scan.AP_list[selected_AP].rssi);

	display.display(); // Обновление экрана
	display.setTextWrap(0);
}

void Display::drawDeauthInfo(uint APs_count, float speed)
{
	display.clearDisplay();
	display.setCursor(37, 17 + y_offset);
	display.println("Deauthing");
	display.setCursor(7, 27 + y_offset);
	display.printf("Deauth APs Count: %u", APs_count);
	display.setCursor(7, 37 + y_offset);
	display.printf("Speed: %.2fpkt/s", speed);
	display.display();
}

void Display::Down()
{
	switch (menu_level)
	{
	case AP_LIST_MENU:
		if (menu_index < scan.APs_count)
		{
			menu_index++;
			if (menu_index != 0){selected_AP = menu_index-1;}
			
			updateMenu();
		}
		break;

	default:
		if (menu_index < menu[menu_level]->size - 1)
		{
			menu_index++;
			updateMenu();
		}
		break;
	}
}

void Display::Up()
{

	if (menu_index > 0)
	{
		menu_index--;
		if (menu_level == AP_LIST_MENU && menu_index != 0){selected_AP = menu_index-1;}
		updateMenu();
	}

}

void Display::Select()
{
	switch (menu_level)
	{
	case WIFI_MENU:
		switch (menu_index)
		{
		case 0:
			Scan_APs();
			break;
		case 1:
			Select_APs();
			break;
		case 2:
			Deauth();
			break;
		case 3:
			menu_level = SNIFFER_MENU;
			menu_index = 0;
			updateMenu();
			//sniff.StartEapolCapture();
			break;
		default:
			break;
		}
		break;

	case SNIFFER_MENU:
		switch (menu_index)
		{
		case 0:
			menu_level = WIFI_MENU;
			updateMenu();
			break;
		case 1:
			if(sniff.sniffer_channel<13){
				sniff.sniffer_channel++;
			}else{
				sniff.sniffer_channel = 1;
			}
			updateMenu();
			break;
		case 3:
			if (!sniff.capture_start)
			{
				sniff.StartEapolCapture();
			}else{
				sniff.StopCapture();
			}
			break;
		case 4:
			if (!sniff.capture_start)
			{
				sniff.StartCapture();
			}else{
				sniff.StopCapture();
			}
			break;
		default:
			break;
		}
		break;

	case AP_LIST_MENU:
		switch (menu_index)
		{
		case 0:
			menu_level = WIFI_MENU;
			menu_index = 0;
			updateMenu();
			break;

		default:
			scan.AP_list[selected_AP].selected = !scan.AP_list[selected_AP].selected;
			updateMenu();
			break;
		}
		break;

	case AP_INFO_MENU:
		menu_level = AP_LIST_MENU;
		updateMenu();
		break;

	case DEAUTH_MENU:
		attack.deauth_attack = 0;
		menu_level = WIFI_MENU;
		updateMenu();
		break;

	default:
		break;
	}
}

void Display::Hold()
{
	if (menu_level == AP_LIST_MENU)
	{
		menu_level = AP_INFO_MENU;
		updateMenu();
	}
}

void Display::Scan_APs()
{
	scan.start_APs_scan();
}

void Display::Select_APs()
{
	menu_level = AP_LIST_MENU;
	menu_index = 0;
	updateMenu();
}

void Display::Deauth()
{
	menu_level = DEAUTH_MENU;
	attack.deauth_attack = !attack.deauth_attack;
}