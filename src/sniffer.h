#pragma once

#ifndef Sniffer_h
#define Sniffer_h

#define FLASH_INTERVAL 5000 //ms

#include "Arduino.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <WiFi.h>
#include "PCAP.h"
#include "scan.h"
#include <SD.h>
#include "display.h"

class Sniffer
{
private:
    bool last_capture_state = false;
    static void SnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    struct filter
    {
        uint8_t pos;
        uint8_t value[10];
        uint8_t size;
        bool selected;
    };
    filter eapol{32, {0x88, 0x8e}, 2, 1};
    filter beacon{0, {0x80}, 1, 1};
   
    bool check_filters = false;
    uint8_t filters_count = 2;
    

    const char *file_basename = "/esp";
    const char *file_extension = ".pcap";
    char filename[20];

    uint32_t last_time_save = 0;

    uint16_t packet_captured = 0;

public:
    void tick();

    bool capture_state = false;
    bool eapol_capturing = false;


    uint8_t sniffer_channel = 1;
    void StartEapolCapture();
    void StartCapture();
    void StopCapture();
};

extern Sniffer sniff;

#endif 