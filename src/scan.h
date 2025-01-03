#pragma once

#ifndef Scan_h
#define Scan_h

#include <Arduino.h>
#include <vector>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <WiFi.h>
#include "display.h"

class Scan
{
private:
    struct AP
    {
        uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        char *ssid;
        uint8_t channel;
        int16_t rssi;
        bool selected = false;
    };

    std::vector<AP> Aps_vector;

public:
    uint8_t APs_count = 0;

    AP *AP_list;

    void start_APs_scan();

    static void beacon_packet_handler(void *buf, wifi_promiscuous_pkt_type_t type);

    bool is_network_stored(uint8_t *mac);

    bool compareArrays(const uint8_t *arr1, const uint8_t *arr2, size_t size);
    uint get_num(uint8_t *mac);

    static int compare_rssi(const void *a, const void *b);
};

extern Scan scan;

#endif
