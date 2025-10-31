# FP_Dash
Freely Programmable Dash firmware

after clone do not forget:
	git submodule update --init --recursive

How to prepare work environment:

https://github.com/espressif/vscode-esp-idf-extension/blob/master/README.md

2.4inch_ESP32-2432S024

	ESP32 Dev Module
	Flash Mode: DIO
	Flash Size: 4MB (32Mb)
	
https://www.waveshare.com/esp32-s3-touch-lcd-5.htm?sku=28117
https://www.waveshare.com/esp32-s3-touch-lcd-7.htm?sku=30406
https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-5#Schematic_diagram

idf.py build
idf.py flash monitor

ISO 15765-4 (CAN).
Standard CAN IDs (11-bit identifiers)
Tester to ECU (Request) - ID: 0x7DF (functional sent to all ECUs)
ECU to Tester (Response) - IDs: 0x7E8 to 0x7EF (physical)
		0x7E8: Response from the Engine ECU
		0x7E9: Response from the Transmission ECU

