
#include <Arduino.h>
#include <MFRC522.h>

#include "MFRC522Controller.h"
#include "MFRCDataReader/MIFARE_1K_DataReader.h"

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10 // Configurable, take an unused pin, only HIGH/LOW required, must be different to SS 2

MFRC522Controller mfrc522Controller(SS_PIN, RST_PIN);

void OnNewCardDetected(const MFRC522Controller& controller)
{
	Serial.print(("New card detected with UID: ") + controller.ReadUIDString());
	Serial.println("");
	Serial.print("PICC type: " + controller.ReadPICCTypeString());
	Serial.println("");

	// 使用默认密钥
	MFRC522::MIFARE_Key key;
	for (auto& byte : key.keyByte)
	{
		byte = 0xFF;
	}

	auto* reader = controller.GenerateDataReader(key);

	// 暂时支支持解析1K卡
	if (reader == nullptr || controller.ReadPICCType() != MFRC522::PICC_TYPE_MIFARE_1K)
		return;

	auto* extendReader = static_cast<MIFARE_1K_DataReader*>(reader);

	const bool readState = extendReader->ReadData();
	if (readState == false)
	{
		Serial.println("Read data failed");
		return;
	}
	const auto cardData = extendReader->GetOrganizedData();
	Serial.println("Read data success");
	Serial.println("Dump data:");
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
}

void setup()
{

	Serial.begin(9600); // Initialize serial communications with the PC

	MFRC522Controller::Begin();

	mfrc522Controller.SetOnNewCardDetected(OnNewCardDetected);
}

/**
 * Main loop.
 */
void loop() { mfrc522Controller.DetectCard(); }
