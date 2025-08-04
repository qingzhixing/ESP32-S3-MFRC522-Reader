//
// Created by qingzhixing on 2025/7/25.
//

#ifndef MIFARE_1K_DATA_CONTROLLER_H
#define MIFARE_1K_DATA_CONTROLLER_H
#include "RFIDDataController.h"


class MIFARE_1K_DataController final : public RFIDDataController
{
public:
	// 准备缓冲区存储所有数据
	static constexpr int BLOCKS_PER_SECTOR = 4;
	static constexpr int BYTES_PER_BLOCK = 16;
	static constexpr int SECTORS_PER_CARD = 16;

	static constexpr int TOTAL_BLOCKS = BLOCKS_PER_SECTOR * SECTORS_PER_CARD;

public:
	explicit MIFARE_1K_DataController(const MFRC522& mfrc522,
									  const MFRC522::MIFARE_Key& key) :
		RFIDDataController(mfrc522, key)
	{
	}

	std::pair<bool, std::vector<byte>> ReadBlock(const int blockNumber) override
	{
		std::vector<byte> blockData{};

		// 检查块号是否有效
		if (blockNumber < 0 || blockNumber >= TOTAL_BLOCKS)
		{
			return std::make_pair<bool, std::vector<byte>>(false, {});
		}

		// 认证当前扇区
		MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
			MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNumber, &key, &(mfrc522.uid));

		if (status != MFRC522::STATUS_OK)
		{
			return std::make_pair<bool, std::vector<byte>>(false, {});
		}

		// 准备读取缓冲区
		byte buffer[18];
		byte size = sizeof(buffer);

		// 读取块数据
		status = mfrc522.MIFARE_Read(blockNumber, buffer, &size);

		if (status == MFRC522::STATUS_OK && size == 18)
		{
			for (byte i = 0; i < 16; i++)
			{
				blockData.push_back(buffer[i]);
			}
		}
		else
		{
			return std::make_pair<bool, std::vector<byte>>(false, {});
		}

		return std::make_pair(true, blockData);
	}
	MFRC522::StatusCode WriteBlock(int blockNumber,
								   const std::vector<byte>& data) override
	{
		// 检查块号是否有效
		if (blockNumber < 0 || blockNumber >= TOTAL_BLOCKS)
		{
			return MFRC522::STATUS_INVALID;
		}

		// 认证当前扇区
		MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
			MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNumber, &key, &(mfrc522.uid));

		if (status != MFRC522::STATUS_OK)
		{
			return status;
		}

		// 写入数据
		byte buffer[BYTES_PER_BLOCK + 2];
		for (byte i = 0; i < BYTES_PER_BLOCK; i++)
		{
			buffer[i] = data[i];
		}

		status = mfrc522.MIFARE_Write(blockNumber, buffer, BYTES_PER_BLOCK);

		return status;
	}
};


#endif // MIFARE_1K_DATA_CONTROLLER_H
