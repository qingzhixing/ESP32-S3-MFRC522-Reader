//
// Created by qingzhixing on 2025/7/25.
//

#ifndef MIFARE_1K_DATAREADER_H
#define MIFARE_1K_DATAREADER_H
#include "MFRCDataReader.h"


class MIFARE_1K_DataReader final : public MFRCDataReader
{
public:
	// 准备缓冲区存储所有数据
	static constexpr int BLOCKS_PER_SECTOR = 16;
	static constexpr int BYTES_PER_BLOCK = 4;
	static constexpr int SECTORS_PER_CARD = 16;

	static constexpr int TOTAL_BLOCKS = BLOCKS_PER_SECTOR * SECTORS_PER_CARD;

private:
	byte cardData[SECTORS_PER_CARD][BLOCKS_PER_SECTOR][BYTES_PER_BLOCK]; // 存储所有块的数据
	bool readStatus[TOTAL_BLOCKS]; // 记录每个块的读取状态

public:
	const byte (*GetOrganizedData() const)[BLOCKS_PER_SECTOR][BYTES_PER_BLOCK] { return cardData; }

public:
	explicit MIFARE_1K_DataReader(const MFRC522& mfrc522, const MFRC522::MIFARE_Key& key) : MFRCDataReader(mfrc522, key)
	{
		memset(cardData, 0, sizeof(cardData));
		memset(readStatus, 0, sizeof(readStatus));
	}

	bool ReadData() override
	{
		// 清空数据
		memset(cardData, 0, sizeof(cardData));
		memset(readStatus, 0, sizeof(readStatus));

		// 遍历所有扇区（0 - 15）
		for (byte sectorIndex = 0; sectorIndex < SECTORS_PER_CARD; sectorIndex++)
		{
			// 计算扇区的第一个块（每个扇区4个块）
			const byte firstBlockNumber = sectorIndex * BLOCKS_PER_SECTOR;
			const byte trailerBlock = firstBlockNumber + 3; // 控制块

			// 认证当前扇区
			MFRC522::StatusCode status =
				mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

			if (status != MFRC522::STATUS_OK)
			{
				return false;
			}

			// 读取当前扇区的4个块（包括控制块）
			for (byte blockIndex = 0; blockIndex < BLOCKS_PER_SECTOR; blockIndex++)
			{
				// 真实的块号
				const byte blockNumber = firstBlockNumber + blockIndex;

				// 准备读取缓冲区
				byte buffer[18];
				byte size = sizeof(buffer);

				// 读取块数据
				status = mfrc522.MIFARE_Read(blockNumber, buffer, &size);

				if (status == MFRC522::STATUS_OK && size == 18)
				{
					// 复制数据到存储数组
					memcpy(cardData[sectorIndex][blockIndex], buffer, BYTES_PER_BLOCK);
					readStatus[blockNumber] = true;
				}
				else
				{
					readStatus[blockNumber] = false;
				}
			}
		}

		for (const auto readState : readStatus)
		{
			if (!readState)
			{
				return false;
			}
		}

		return true;
	}
	std::vector<byte> GetData() const override
	{
		// 检查数据是否完整
		for (const auto readState : readStatus)
		{
			if (!readState)
			{
				return std::vector<byte>();
			}
		}

		std::vector<byte> data;

		// 将数组里的数据提取出来
		for (byte sectorIndex = 0; sectorIndex < SECTORS_PER_CARD; sectorIndex++)
		{
			for (int blockIndex = 0; blockIndex < BLOCKS_PER_SECTOR; blockIndex++)
			{
				for (int byteIndex = 0; byteIndex < BYTES_PER_BLOCK; byteIndex++)
				{
					data.push_back(cardData[sectorIndex][blockIndex][byteIndex]);
				}
			}
		}
		return data;
	}
};


#endif // MIFARE_1K_DATAREADER_H
