
#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10 // Configurable, take an unused pin, only HIGH/LOW required, must be different to SS 2

MFRC522 mfrc522; // Create MFRC522 instance.

void dump_byte_array(const byte* buffer, byte bufferSize);
void readCardData(MFRC522& mfrc522);

/**
 * Initialize.
 */
void setup()
{

	Serial.begin(9600); // Initialize serial communications with the PC

	SPIClass::begin(); // Init SPI bus

	mfrc522.PCD_Init(SS_PIN, RST_PIN); // Init each MFRC522 card
	Serial.print(F("MFRC522 Reader Version: "));
	mfrc522.PCD_DumpVersionToSerial();
}

/**
 * Main loop.
 */
void loop()
{

	// Look for new cards
	if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
	{
		// Show some details of the PICC (that is: the tag/card)
		Serial.print(F("Card UID:"));
		dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
		Serial.println();
		Serial.print(F("PICC type: "));
		const MFRC522::PICC_Type piccType = MFRC522::PICC_GetType(mfrc522.uid.sak);
		Serial.println(MFRC522::PICC_GetTypeName(piccType));

		// 读取卡片数据
		readCardData(mfrc522);

		// Halt PICC
		mfrc522.PICC_HaltA();
		// Stop encryption on PCD
		mfrc522.PCD_StopCrypto1();
	} // if (mfrc522.PICC_IsNewC
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(const byte* buffer, const byte bufferSize)
{
	for (byte i = 0; i < bufferSize; i++)
	{
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], HEX);
	}
}

// 读取卡片1KB数据
void readCardData(MFRC522& mfrc522)
{
	MFRC522::MIFARE_Key key;
	// 使用默认密钥FFFFFFFFFFFF
	for (unsigned char& i : key.keyByte)
	{
		i = 0xFF;
	}

	// 准备缓冲区存储所有数据
	constexpr int totalBlocks = 64; // MIFARE 1K有64个块
	byte cardData[totalBlocks][16]; // 存储所有块的数据
	bool readStatus[totalBlocks]; // 记录每个块的读取状态

	// 初始化状态数组
	for (bool& readStatu : readStatus)
	{
		readStatu = false;
	}

	// 遍历所有扇区（0-15）
	for (byte sector = 0; sector < 16; sector++)
	{
		// 计算扇区的第一个块（每个扇区4个块）
		const byte firstBlock = sector * 4;
		const byte trailerBlock = firstBlock + 3; // 控制块

		// 认证当前扇区
		MFRC522::StatusCode status =
			mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

		if (status != MFRC522::STATUS_OK)
		{
			// Serial.print(F("Sector "));
			// Serial.print(sector);
			// Serial.println(F(" authentication failed!"));
			continue;
		}

		// 读取当前扇区的4个块（包括控制块）
		for (byte blockOffset = 0; blockOffset < 4; blockOffset++)
		{
			const byte block = firstBlock + blockOffset;

			// 准备读取缓冲区
			byte buffer[18];
			byte size = sizeof(buffer);

			// 读取块数据
			status = mfrc522.MIFARE_Read(block, buffer, &size);

			if (status == MFRC522::STATUS_OK && size == 18)
			{
				// 复制数据到存储数组
				memcpy(cardData[block], buffer, 16);
				readStatus[block] = true;
			}
			else
			{
				readStatus[block] = false;
			}
		}
	}

	for (auto readState : readStatus)
	{
		if (!readState)
		{
			Serial.println(F("Read failed!"));
			return;
		}
	}

	// 一次性输出所有块的数据
	Serial.println(F("\n====== Card Data Dump ======"));
	Serial.println(F("Block | Data (Hex)"));
	Serial.println(F("------+-------------------------------"));

	for (byte block = 0; block < totalBlocks; block++)
	{

		if (readStatus[block])
		{
			Serial.print(block < 10 ? "  " : " "); // 对齐块号
			Serial.print(block);
			Serial.print("  | ");
			// 打印数据块内容
			dump_byte_array(cardData[block], 16);

			// 特殊标记控制块
			if ((block + 1) % 4 == 0)
			{
				Serial.print("  [Sector Trailer]");
			}
		}
		Serial.println();
	}

	Serial.println(F("====== End of Dump ======\n"));
}
