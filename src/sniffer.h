#pragma once

#ifndef Sniffer_h
#define Sniffer_h


#include "Arduino.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <WiFi.h>
#include "SPIFFS.h"
#include "PCAP.h"
#include <TimeLib.h>
#include "scan.h"

class Sniffer
{
private:
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
    bool check_filters = 0;
    uint8_t filters_count = 2;
public:
    
    filter *filters[2] = {&beacon, &eapol};

    

    bool capture_start = false;
    
    uint8_t sniffer_channel = 1;
    void StartEapolCapture();
    void StartCapture();
    void StopCapture();
};

extern Sniffer sniff;

#endif 