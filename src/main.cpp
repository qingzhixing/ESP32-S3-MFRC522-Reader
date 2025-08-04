
#include <Arduino.h>
#include <MFRC522.h>

#include "MFRC522Controller.h"
#include "RFIDDataController/MIFARE_1K_DataController.h"

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10
#define SCK_PIN 36
#define MISO_PIN 37
#define MOSI_PIN 35

MFRC522Controller* controller;

byte blockData[MIFARE_1K_DataController::TOTAL_BLOCKS]
			  [MIFARE_1K_DataController::BYTES_PER_BLOCK];
bool blockDataPrepared[MIFARE_1K_DataController::TOTAL_BLOCKS];

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
		Serial.println("\nRetry R/W Card UID: " + cardUIDString);
	}

	// 使用默认密钥
	MFRC522::MIFARE_Key key;
	for (unsigned char& byte : key.keyByte)
	{
		byte = 0xFF;
	}

	RFIDDataController* dataController =
		_controller.GenerateDataController(key);

	// 暂时支支持解析1K卡
	if (dataController == nullptr)
	{
		Serial.println("No data reader available");
		return;
	}

	if (_controller.ReadPICCType() != MFRC522::PICC_TYPE_MIFARE_1K)
	{
		Serial.println("Not a MIFARE 1K card");
		delete dataController;
		return;
	}

	// 根据输入来进行读取或者写入数据
	bool rwMode = false; // true: 写入模式, false: 读取模式
	Serial.println("Enter R/W Mode (1: Write, 0: Read): ");

	while (true)
	{
		if (Serial.available())
		{
			String input = Serial.readStringUntil('\n');
			input.trim();
			Serial.println("Input: " + input);
			rwMode = input == "1";
			break;
		}
	}

	Serial.println(rwMode ? "Write Mode" : "Read Mode");

	if (false == rwMode)
	{
		Serial.println("Reading Card Data...");

		for (auto sectorIndex = 0;
			 sectorIndex < MIFARE_1K_DataController::SECTORS_PER_CARD;
			 sectorIndex++)
		{
			for (auto blockIndex = 0;
				 blockIndex < MIFARE_1K_DataController::BLOCKS_PER_SECTOR;
				 blockIndex++)
			{
				const auto blockNumber =
					sectorIndex * MIFARE_1K_DataController::BLOCKS_PER_SECTOR +
					blockIndex;

				if (blockDataPrepared[blockNumber])
					continue;

				Serial.println("Reading Block " + String(blockNumber) + "...");

				auto result = dataController->ReadBlock(blockNumber);

				// auto retry
				int retryCountDown = 10;
				while (!result.first && retryCountDown > 0)
				{
					retryCountDown--;
					result = dataController->ReadBlock(blockNumber);
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
					 byteIndex < MIFARE_1K_DataController::BYTES_PER_BLOCK;
					 byteIndex++)
				{
					blockData[blockNumber][byteIndex] =
						result.second[byteIndex];
					blockDataPrepared[blockNumber] = true;
				}
			}
		}

		for (const auto& prepared : blockDataPrepared)
		{
			if (!prepared)
			{
				delete dataController;
				return;
			}
		}

		// 打印全部
		for (auto blockNumber = 0;
			 blockNumber < MIFARE_1K_DataController::TOTAL_BLOCKS;
			 blockNumber++)
		{
			Serial.print("Block " + String(blockNumber) + ":");
			for (auto byteIndex = 0;
				 byteIndex < MIFARE_1K_DataController::BYTES_PER_BLOCK;
				 byteIndex++)
			{
				Serial.print(" " + MFRC522Controller::ByteToHexString(
									   blockData[blockNumber][byteIndex]));
			}
			Serial.println("");
		}

		delete dataController;
	}
	else
	{
		Serial.println("Writing Card Data...");

		// 获取用户要写入的UID
		Serial.println("Please input UID to write:(Like: FF FF FF FF)");

		while (true)
		{
			if (Serial.available())
			{
				String inputStr = Serial.readStringUntil('\n');
				inputStr.trim();
				Serial.println("Input UID: " + inputStr);
				break;
			}
		}

		// 解析字符串
		byte uid[6];

		// TODO: Coding Here,解析用户输入的UID

		delete dataController;
	}
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
