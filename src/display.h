#pragma once
#ifndef Display_h
#define Display_h

#define y_offset 7
#define main_menu_size 1

#define WIFI_MENU 0
#define SNIFFER_MENU 1
#define AP_LIST_MENU 2
#define AP_INFO_MENU 3
#define DEAUTH_MENU 4
#define SNIFFING 5


#include <Arduino.h>
#include <WiFi.h>
#include <scan.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "attack.h"
#include "sniffer.h"

class Display
{
private:
    uint last_menu_level;
    uint last_menu_index;

    

    struct menu
    {
        const char *items[10];
        uint8_t size;
    };

    menu main_menu = {{"scan wifi", "select", "deauth", "sniffer", "settings"}, 5};
    menu sniffer_menu = {{"back", "channel: %d", "sniff with deauth"/*only one selected AP(if there are several, then it will be taken first from the list)*/ ,"start"}, 4};
    menu *menu[5] = {&main_menu, &sniffer_menu};
    uint selected_AP;

public:
    uint menu_level = 0;
    uint menu_index = 0;
    void Scan_APs();
    void Select_APs();
    void Deauth();

    char **APs;

    void tick();
    void updateMenu();
    void drawMenu();
    void draw_AP_info();
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