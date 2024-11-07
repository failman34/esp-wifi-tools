#pragma once
#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include <WiFi.h>
#include <scan.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "attack.h"

class Display
{
private:

    
    int menu_index = 0;
    

    struct menu
    {
        const char* items[10];
        uint8_t size;
    };


    
    menu main_menu = {{"scan wifi", "select", "deauth", "eapol sniff", "settings", "SS", "AAAA", "lox", "suka", "eblan"}, 10};
    menu* menu[5] = {&main_menu};
	char moreInfo[6][18];
    uint selected_AP;
	

    
public:
    uint8_t menu_level = 0;
	void Scan_APs();
	void Select_APs();
	void Deauth();

	char** APs;

    void drawMenu();
	void drawAnotherMenu();
    void draw_WiFi_menu();
    void drawDeauthInfo(uint APs_count, float speed);

    void Down();
    void Up();
    void Select();
	void Hold();
};

extern Display disp;
extern Adafruit_SSD1306 display;

#endif