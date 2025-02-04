#include "sniffer.h"

Sniffer sniff;

void Sniffer::tick()
{
	if (capture_state != last_capture_state)
	{
		last_capture_state = capture_state;
		if (capture_state)
		{
			if(eapol_capturing){
				StartEapolCapture();
			}else{
				StartCapture();
			}
		}
		else
		{
			StopCapture();
		}
	}
	if(packet_captured > 0){
		if (millis() - last_time_save > FLASH_INTERVAL)
		{
			
			if(!SD.exists(filename)){
				display.clearDisplay();
				display.setCursor(0, 7);
				display.print("File flashing failed");
				display.display();
				delay(1500);
				StopCapture();
				return;
			}
			pcap.flushFile();
			last_time_save = millis();
			display.setCursor(0, 14);
			display.fillRect(0, 7, 128, 7, 0);
			display.printf("packed captured: %d", packet_captured);
			display.display();
		}
		
	}
}

void Sniffer::StartCapture()
{
	if (SD.cardType() == CARD_NONE || SD.cardType() == CARD_UNKNOWN)
	{
		display.clearDisplay();
		display.setCursor(0, 7);
		display.print("Card not mount, mounting");
		display.display();
		delay(1000);
		if (SD.begin(12))
		{
			Serial.println("Card Mounted");
		}
		else
		{
			display.clearDisplay();
			display.setCursor(0, 7);
			display.print("Card mount Failed");
			display.display();
			delay(2000);
			StopCapture();
			return;
		}
	}


	int c = 0;
	sprintf(filename, "%s%d%s", file_basename, c, file_extension);
	Serial.println(filename);
	while (SD.exists(filename))
	{
		c++;
		sprintf(filename, "%s%d%s", file_basename, c, file_extension);
	}
	c = 0;
	Serial.println(filename);
	File file = SD.open(filename, FILE_WRITE);
	if (!file)
	{
		display.clearDisplay();
		display.setCursor(0, 7);
		display.print("File openning failed");
		display.display();
		delay(1500);
		StopCapture();
		return;
	}
	pcap.openFile(file);

	display.clearDisplay();
	display.setCursor(0, 7);
	display.printf("Started at %s", filename);
	display.display();

	esp_wifi_set_promiscuous(true);
	esp_wifi_set_promiscuous_rx_cb(&SnifferCallback);
	esp_wifi_set_channel(sniffer_channel, WIFI_SECOND_CHAN_NONE);

	capture_state = true;
}

void Sniffer::StartEapolCapture()
{

	if (SD.cardType() == CARD_NONE || SD.cardType() == CARD_UNKNOWN)
	{
		display.clearDisplay();
		display.setCursor(0, 7);
		display.print("Card not mount, mounting");
		display.display();
		delay(1000);
		if (SD.begin(12))
		{
			Serial.println("Card Mounted");
		}
		else
		{
			display.clearDisplay();
			display.setCursor(0, 7);
			display.print("Card mount Failed");
			display.display();
			delay(2000);
			StopCapture();
			return;
		}
	}


	int c = 0;
	sprintf(filename, "%s%d%s", file_basename, c, file_extension);
	Serial.println(filename);
	while (SD.exists(filename))
	{
		c++;
		sprintf(filename, "%s%d%s", file_basename, c, file_extension);
	}
	c = 0;
	Serial.println(filename);
	File file = SD.open(filename, FILE_WRITE);
	if (!file)
	{
		display.clearDisplay();
		display.setCursor(0, 7);
		display.print("File openning failed");
		display.display();
		delay(1500);
		StopCapture();
		return;
	}
	pcap.openFile(file);

	

	esp_wifi_set_promiscuous(true);
	esp_wifi_set_promiscuous_rx_cb(&SnifferCallback);
	esp_wifi_set_channel(sniffer_channel, WIFI_SECOND_CHAN_NONE);

	


	bool is_selected = false;
	for (size_t i = 0; i < scan.APs_count; i++)
	{
		if (scan.AP_list[i].selected == true)
		{
			display.clearDisplay();
			display.setCursor(0, 7);
			display.print("Deauthing");
			display.display();
			packet_captured = 0;
			for (size_t b = 0; b < 200; b++)
			{
				attack.deauthDevice(i);
				delay(10);
			}
			Serial.println(scan.AP_list[i].ssid);
			sniffer_channel = scan.AP_list[i].channel;
			is_selected = true;
			break;
		}
	}
	if (!is_selected)
	{
		display.clearDisplay();
		display.setCursor(0, 7);
		display.print("no selected networks");
		display.display();
		delay(1500);
		StopCapture();
		return;
	}

	display.clearDisplay();
	display.setCursor(0, 7);
	display.printf("Started at %s", filename);
	display.display();

	capture_state = true;

	// int c = 0;
	// strcpy(filename, "/esp0.pcap");
	// Serial.println(filename);
	// while(SD.exists(filename)){
	// 	c++;
	// 	sprintf(filename, "%s%d%s", file_basename, c, file_extension);
	// }
	// c = 0;
	// Serial.println(filename);
	// File file = SD.open(filename, FILE_WRITE);
	// if (!file)
	// {
	// 	Serial.println("Error");
	// }

	// display.setCursor(0, 7);
	// display.print("Sniffing EAPOL");
	// display.display();
	// pcap.openFile(file);

	// esp_wifi_set_promiscuous(true);
	// esp_wifi_set_promiscuous_rx_cb(&SnifferCallback);
	// esp_wifi_set_channel(sniffer_channel, WIFI_SECOND_CHAN_NONE);

	// capture_state = true;
}

void Sniffer::StopCapture()
{
	
	
	esp_wifi_set_promiscuous(false);
	delay(500);
	eapol_capturing = false;
	capture_state = false;
	if(packet_captured>0){
		pcap.flushFile();
		pcap.closeFile();
		packet_captured = 0;
	}
	disp.menu_level = SNIFFER_MENU;
};

void Sniffer::SnifferCallback(void *buf, wifi_promiscuous_pkt_type_t type)
{

	wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
	wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;
	int len = pkt->rx_ctrl.sig_len - 4;

	pcap.newPacketSD(ctrl.timestamp / 1000000U, ctrl.timestamp % 1000000U, len, pkt->payload);
	Serial.println(sniff.packet_captured++);

	// if (sniff.eapol_capturing)
	// {
	// 	// Beacon Frame (тип кадра 0x80)
	// 	//   if (memcmp(snifferPacket->payload+sniff.beacon.pos, sniff.beacon.value, sniff.beacon.size) == 0){
	// 	// 	pcap.newPacketSD(now(), (unsigned int)(micros() - millis() * 1000), len, snifferPacket->payload);
	// 	//   }
	// 	//   else if (memcmp(snifferPacket->payload+sniff.eapol.pos, sniff.eapol.value, sniff.eapol.size) == 0){
	// 	// 	pcap.newPacketSD(now(), (unsigned int)(micros() - millis() * 1000), len, snifferPacket->payload);
	// 	// 	display.clearDisplay();
	// 	// 	display.setCursor(0, 7);
	// 	// 	display.printf("EAPOL captured: %d", sniff.eapol_captured++);
	// 	// 	display.display();
	// 	// 	}
	// 	if (pkt->payload[0] == 0x80)
	// 	{
	// 		pcap.newPacketSD(ctrl.timestamp / 1000000U, ctrl.timestamp % 1000000U, len, pkt->payload);
	// 	}
	// 	else if (pkt->payload[32] == 0x88 && pkt->payload[33] == 0x8e)
	// 	{
	// 		pcap.newPacketSD(ctrl.timestamp / 1000000U, ctrl.timestamp % 1000000U, len, pkt->payload);
	// 		display.setCursor(0, 14);
	// 		display.fillRect(0, 7, 128, 7, 0);
	// 		display.printf("EAPOL captured: %d", sniff.packet_captured++);
	// 		display.display();
	// 	}
	// }
	// else
	// {
		
	// }

}
