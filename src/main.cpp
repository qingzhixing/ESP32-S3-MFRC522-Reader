
#include <Arduino.h>
#include <MFRC522.h>

#include "MFRC522Controller.h"
#include "MFRCDataReader/MIFARE_1K_DataReader.h"

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN                                                                 \
	10 // Configurable, take an unused pin, only HIGH/LOW required, must be
	   // different to SS 2

MFRC522Controller* controller;

void OnNewCardDetected(const MFRC522Controller& _controller)
{
	Serial.print(("New card detected with UID: ") +
				 _controller.ReadUIDString());
	Serial.println("");
	Serial.print("PICC type: " + _controller.ReadPICCTypeString());
	Serial.println("");

	Serial.println("Generating Default Key: FF FF FF FF FF FF");

	// 使用默认密钥
	MFRC522::MIFARE_Key key;
	for (unsigned char& byte : key.keyByte)
	{
		byte = 0xFF;
	}

	Serial.println("Key Generated.");

	Serial.println("Generating Data Reader...");

	MFRCDataReader* reader = _controller.GenerateDataReader(key);

	Serial.println("Data Reader Generated.");

	// 暂时支支持解析1K卡
	if (reader == nullptr)
	{
		Serial.println("No data reader available");
		delete reader;
		return;
	}

	if (_controller.ReadPICCType() != MFRC522::PICC_TYPE_MIFARE_1K)
	{
		Serial.println("Not a MIFARE 1K card");
		delete reader;
		return;
	}

	Serial.println("Reading Card Data...");

	for (auto sectorIndex = 0;
		 sectorIndex < MIFARE_1K_DataReader::SECTORS_PER_CARD; sectorIndex++)
	{
		for (auto blockIndex = 0;
			 blockIndex < MIFARE_1K_DataReader::BLOCKS_PER_SECTOR; blockIndex++)
		{
			const auto blockNumber =
				sectorIndex * MIFARE_1K_DataReader::BLOCKS_PER_SECTOR +
				blockIndex;

			const auto result = reader->ReadBlock(blockNumber);

			// print
			Serial.print("Sector " + String(sectorIndex) + " , Block " +
						 String(blockIndex) + ": ");
			if (!result.first)
			{
				Serial.println("Failed to read block");
				return;
			}
			else
			{
				Serial.println(
					MFRC522Controller::DumpByteArrayToHexString(result.second));
			}
		}
	}

	delete reader;
}

void setup()
{

	Serial.begin(9600); // Initialize serial communications with the PC

	for (int i = 0; i < 20; i++)
	{
		Serial.println("");
	}
	Serial.println("Serial initialized.");

	MFRC522Controller::Begin();

	controller = new MFRC522Controller(RST_PIN, SS_PIN);

	Serial.println("Reader initialized.");

	controller->PCD_DumpVersionToSerial();

	controller->SetOnNewCardDetected(OnNewCardDetected);
}

/**
 * Main loop.
 */
void loop()
{
	// Look for new cards
	// Serial.println("Detecting cards...");
	controller->DetectCard();
}
