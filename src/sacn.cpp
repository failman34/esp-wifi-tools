#include "scan.h"

Scan scan;


void Scan::start_APs_scan(){
    Serial.println("started");
    WiFi.disconnect();
    esp_wifi_set_promiscuous(true);
	APs_count = 0;

    esp_wifi_set_promiscuous_rx_cb(&beacon_packet_handler);
	
    for (uint8_t i = 1; i <= 13; i++)
    {
		display.clearDisplay();
		display.setCursor(30, 18);
		display.print("Scaning Wifi");
		display.setCursor(30, 28);
		display.printf("APs found: %d", APs_count);
		display.fillRect(12, 38, 8*i, 10, SSD1306_WHITE);
		display.display();
    	esp_wifi_set_channel(i, WIFI_SECOND_CHAN_NONE);
		delay(500);
    }
	display.clearDisplay();
	display.setCursor(30, 18);
	display.print("Scane Done");
	display.setCursor(30, 28);
	display.printf("APs found: %d", APs_count);
	display.fillRect(12, 38, 8*13, 10, SSD1306_WHITE);
	display.display();
	delay(1000);
	disp.drawMenu();

	esp_wifi_set_promiscuous(false);

	delete[] AP_list;
	AP_list = new AP[APs_count];
	std::copy(Aps_vector.begin(), Aps_vector.end(), AP_list);
	qsort(AP_list, APs_count, sizeof(AP), compare_rssi);

    Serial.println("Stop");

	Serial.println(Aps_vector.size());

	Aps_vector.clear();
	Aps_vector.shrink_to_fit();

	for (size_t i = 0; i < APs_count; i++)
	{
		Serial.println(AP_list[i].ssid);
	}
	

}

void Scan::beacon_packet_handler(void* buf, wifi_promiscuous_pkt_type_t type){

    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t*)buf;
    

    if (type == WIFI_PKT_MGMT)
    {
        if (pkt->payload[0] == 0x80)
        {

            uint8_t addr[8];



            memcpy(addr, pkt->payload+10, 6);

            if (!scan.is_network_stored(addr))
            {	
				AP newap;

                memcpy(newap.mac, pkt->payload+10, 6);
				newap.rssi = pkt->rx_ctrl.rssi;
				newap.channel = pkt->rx_ctrl.channel;
				if (pkt->payload[37]>0)
				{

					newap.ssid = new char[pkt->payload[37]+1];
                	for (int i = 0; i < pkt->payload[37]; i++)
                	{	
						
                		newap.ssid[i] = pkt->payload[38+i];
						
                	}
					newap.ssid[pkt->payload[37]] = '\0';
				}
				else{
					newap.ssid = new char[12];
					strcpy(newap.ssid, "Hiden SSID");
					newap.ssid[12] = '\0';
				}

				scan.Aps_vector.push_back(newap);
                scan.APs_count++;
                
            }else{
				uint n = scan.get_num(addr);
				if (scan.Aps_vector[n].rssi < pkt->rx_ctrl.rssi)
				{
					scan.Aps_vector[n].rssi = pkt->rx_ctrl.rssi;
					scan.Aps_vector[n].channel = pkt->rx_ctrl.channel;
				}
				
			}
            Serial.println(scan.APs_count);
        }
        
    }
    
}


bool Scan::is_network_stored(uint8_t* mac) {
	for (int i = 0; i < APs_count; i++) {
		if (compareArrays(Aps_vector[i].mac, mac, 6) == 1) {
      	return true;
    }
  }
  return false;
}


uint Scan::get_num(uint8_t* mac){
	for (int i = 0; i < APs_count; i++) {
    	if (compareArrays(Aps_vector[i].mac, mac, 6)) {
      	return i;
    }
  }
  return 0;
}


int Scan::compare_rssi(const void *a, const void *b) {
    AP *APa = (AP*)a;
    AP *APb = (AP*)b;
    return APb->rssi - APa->rssi; // По убыванию
}

bool Scan::compareArrays(const uint8_t* arr1, const uint8_t* arr2, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (arr1[i] != arr2[i]) {
            return false;  // Найдено различие
        }
    }
    return true;  // Все элементы совпадают
}