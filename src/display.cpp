#include "display.h"

Adafruit_SSD1306 display(128, 64, &Wire1);
Display disp;

void Display::drawMenu()
{
	display.clearDisplay();
	int index = 0;
	int max_len = 6;
	uint menu_size = menu[menu_level]->size;
	Serial.println(menu_size);
	Serial.println(menu_index);

	if (menu_index + 1 > max_len)		// расчеты для хер знает чего(забыл)
	{
		index = menu_index + 1 - max_len;
	}

	for (int i = 0; i < max_len; i++)		// рисование
	{
		if (i + index < menu_size)
		{
			if (i + index == menu_index)
			{
				display.setCursor(0, (i * 10)+y_offset);
				display.print(">");  //символ выбора
			}
			display.setCursor(10, (i * 10)+y_offset);
			display.print(menu[menu_level]->items[index+i]);
			Serial.println(menu[menu_level]->items[index+i]);
		}
		else
		{
			break;
		}
	}
	display.display(); // Обновление экрана
}

void Display::draw_WiFi_menu()
{
	Serial.println(menu_index);
	display.clearDisplay();
	int index = 0;
	int max_len = 8;
	
	if (menu_index + 2 > max_len)   // расчеты для хер знает чего(забыл)
	{
		index = menu_index + 2 - max_len;
	}

	display.setCursor(0, 0+y_offset);
	
	for (int i = 0; i < max_len; i++)	// рисование
	{
		if (i + index < scan.APs_count+1)
		{
			if (i-1 + index == menu_index)
			{
				display.setCursor(0, (i * 8)+y_offset);
				display.print(">");  //символ выбора
			}
			if(i-1 + index != -1 && scan.AP_list[(i-1 + index)].selected == true){
				display.setCursor(8, (i * 8)+y_offset);
				display.print("*");  //символ выбора
			}
			display.setCursor(16, (i * 8)+y_offset);
			if(index == 0 && i-1 == -1){
				display.print("Back");
			}
			else{
				display.printf("%s", scan.AP_list[(index + i)-1].ssid);
			}
		}
		else
		{
			break;
		}
	}
	display.display(); // Обновление экрана
}

void Display::drawAnotherMenu(){
	display.clearDisplay();
	display.setTextWrap(1);
	display.setCursor(0, 0+y_offset);
	display.printf("SSID:%s", scan.AP_list[selected_AP].ssid);

	uint8_t y = display.getCursorY()-7;
	
	display.setCursor(0, y+8+y_offset);
	display.printf("MAC %02X:%02X:%02X:%02X:%02X:%02X\n", 
	               scan.AP_list[selected_AP].mac[0], scan.AP_list[selected_AP].mac[1], 
	               scan.AP_list[selected_AP].mac[2], scan.AP_list[selected_AP].mac[3], 
	               scan.AP_list[selected_AP].mac[4], scan.AP_list[selected_AP].mac[5]);

	display.setCursor(0, y+16+y_offset);
	
	display.printf("Channel:%u\n", scan.AP_list[selected_AP].channel);

	display.setCursor(0, y+24+y_offset);
	
	display.printf("RSSI:%d dBm\n", scan.AP_list[selected_AP].rssi);

	
	display.display(); // Обновление экрана
	display.setTextWrap(0);
}

void Display::drawDeauthInfo(uint APs_count, float speed){
	display.clearDisplay();
	display.setCursor(37, 17+y_offset);
	display.println("Deauthing");
	display.setCursor(7, 27+y_offset);
	display.printf("Deauth APs Count:%u", APs_count);
	display.setCursor(7, 37+y_offset);
	display.printf("Speed:%.2fpkt/s", speed);
	display.display();
}

void Display::Down(){
	switch (menu_level)
	{
	case 0:
		if (menu_index < menu[menu_level]->size-1){
			menu_index++;
			drawMenu();
		}
		break;

	case 1:
		if (menu_index < scan.APs_count-1){
			menu_index++;
			selected_AP = menu_index;
			draw_WiFi_menu();
		}
		break;

	default:
		break;
	}
}

void Display::Up(){

	switch (menu_level)
	{
	case 0:
		if (menu_index > 0){
			menu_index--;
			drawMenu();
		}
		break;

	case 1:
		if (menu_index > -1){
			menu_index--;
			selected_AP = menu_index;
			draw_WiFi_menu();
		}
		break;

	default:
		break;
	}
}

void Display::Select(){
	if (menu_level == 0)
	{
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
		default:
			break;
		}	
	}
	else if (menu_level == 1)
	{
		switch (menu_index)
		{
		case -1:
			menu_level = 0;
			menu_index = 0; 
			drawMenu();
			break;
		
		default:
			scan.AP_list[selected_AP].selected = !scan.AP_list[selected_AP].selected;
			draw_WiFi_menu();
			break;
		}
	}
	else if (menu_level == 2)
	{
		menu_level--;
		draw_WiFi_menu();
	}
	else if (menu_level == 3)
	{
		attack.deauth_attack = 0;
		menu_level = 0;
		drawMenu();
	}
}

void Display::Hold(){
	if (menu_level == 1)
	{
		menu_level = 2;
		drawAnotherMenu();
	}
	
}

void Display::Scan_APs(){
	scan.start_APs_scan();
}

void Display::Select_APs(){
	menu_level = 1;
	menu_index = -1;
	draw_WiFi_menu();
}

void Display::Deauth(){
	menu_level = 3;
	attack.deauth_attack = !attack.deauth_attack;
}