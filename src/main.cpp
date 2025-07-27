
#include <Arduino.h>
#include <MFRC522.h>

#include "MFRC522Controller.h"
#include "MFRCDataReader/MIFARE_1K_DataReader.h"

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10
#define SCK_PIN 36
#define MISO_PIN 37
#define MOSI_PIN 35

MFRC522Controller* controller;

byte blockData[MIFARE_1K_DataReader::TOTAL_BLOCKS]
			  [MIFARE_1K_DataReader::BYTES_PER_BLOCK];
bool blockDataPrepared[MIFARE_1K_DataReader::TOTAL_BLOCKS];

String lastCardString;

void OnNewCardDetected(const MFRC522Controller& _controller)
{
	const String cardUIDString = _controller.ReadUIDString();
	if (cardUIDString != lastCardString)
	{
		lastCardString = cardUIDString;

		memset(blockData, 0, sizeof(blockData));
		memset(blockDataPrepared, 0, sizeof(blockDataPrepared));

		Serial.println(("\nNew card detected with UID: ") + cardUIDString);
		Serial.println("PICC type: " + _controller.ReadPICCTypeString());
	}
	else
	{
		Serial.println("\nRetry Reading Card UID: " + cardUIDString);
	}

	// 使用默认密钥
	MFRC522::MIFARE_Key key;
	for (unsigned char& byte : key.keyByte)
	{
		byte = 0xFF;
	}

	MFRCDataReader* reader = _controller.GenerateDataReader(key);

	// 暂时支支持解析1K卡
	if (reader == nullptr)
	{
		Serial.println("No data reader available");
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

			if (blockDataPrepared[blockNumber])
				continue;

			Serial.println("Reading Block " + String(blockNumber) + "...");

			auto result = reader->ReadBlock(blockNumber);

			// auto retry
			int retryCountDown = 10;
			while (!result.first && retryCountDown > 0)
			{
				retryCountDown--;
				result = reader->ReadBlock(blockNumber);
			}

			if (!result.first)
			{
				// print
				Serial.print("Sector " + String(sectorIndex) + " , Block " +
							 String(blockIndex) + ": ");
				Serial.println("Failed to read block");
				return;
			}

			for (auto byteIndex = 0;
				 byteIndex < MIFARE_1K_DataReader::BYTES_PER_BLOCK; byteIndex++)
			{
				blockData[blockNumber][byteIndex] = result.second[byteIndex];
				blockDataPrepared[blockNumber] = true;
			}
		}
	}

	for (const auto& prepared : blockDataPrepared)
	{
		if (!prepared)
		{
			delete reader;
			return;
		}
	}

	// 打印全部
	for (auto blockNumber = 0; blockNumber < MIFARE_1K_DataReader::TOTAL_BLOCKS;
		 blockNumber++)
	{
		Serial.print("Block " + String(blockNumber) + ":");
		for (auto byteIndex = 0;
			 byteIndex < MIFARE_1K_DataReader::BYTES_PER_BLOCK; byteIndex++)
		{
			Serial.print(" " + MFRC522Controller::ByteToHexString(
								   blockData[blockNumber][byteIndex]));
		}
		Serial.println("");
	}

	delete reader;
}

void setup()
{

	Serial.begin(9600);

	for (int i = 0; i < 20; i++)
	{
		Serial.println("");
	}
	Serial.println("Serial initialized.");

	MFRC522Controller::Begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

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
