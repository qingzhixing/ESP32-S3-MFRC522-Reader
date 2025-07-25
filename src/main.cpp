
#include <Arduino.h>
#include <MFRC522.h>

#include "MFRC522Controller.h"
#include "MFRCDataReader/MIFARE_1K_DataReader.h"

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10 // Configurable, take an unused pin, only HIGH/LOW required, must be different to SS 2

MFRC522Controller* controller;

void OnNewCardDetected(const MFRC522Controller& _controller)
{
	Serial.print(("New card detected with UID: ") + _controller.ReadUIDString());
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

	const auto reader = _controller.GenerateDataReader(key);

	Serial.println("Data Reader Generated.");

	// 暂时支支持解析1K卡
	if (reader == nullptr)
	{
		Serial.println("No data reader available");
		goto done;
	}

	if (_controller.ReadPICCType() != MFRC522::PICC_TYPE_MIFARE_1K)
	{
		Serial.println("Not a MIFARE 1K card");
		goto done;
	}

	auto* extendReader = (MIFARE_1K_DataReader*)(reader);

	Serial.println("Reading Card Data...");

	const bool readState = extendReader->ReadData();

	Serial.println("Read Card Data State: " + String(readState));

	if (readState == false)
	{
		Serial.println("Read data failed");
		goto done;
	}
	Serial.println("Read data success");
	Serial.println("Dump data:");
	const auto cardData = extendReader->GetOrganizedData();

	for (int sectorIndex = 0; sectorIndex < MIFARE_1K_DataReader::SECTORS_PER_CARD; sectorIndex++)
	{
		for (int blockIndex = 0; blockIndex < MIFARE_1K_DataReader::BLOCKS_PER_SECTOR; blockIndex++)
		{
			const auto blockData = cardData[sectorIndex][blockIndex];
			String blockDataString =
				MFRC522Controller::DumpByteArrayToHexString(blockData, MIFARE_1K_DataReader::BYTES_PER_BLOCK);
			Serial.println("Sector " + String(sectorIndex) + " Block " + String(blockIndex) + ": " + blockDataString);
		}
	}

done:
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
