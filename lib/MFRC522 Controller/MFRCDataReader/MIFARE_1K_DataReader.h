//
// Created by qingzhixing on 2025/7/25.
//

#ifndef MIFARE_1K_DATAREADER_H
#define MIFARE_1K_DATAREADER_H
#include "MFRCDataReader.h"


class MIFARE_1K_DataReader final : public MFRCDataReader
{
private:
	// 准备缓冲区存储所有数据
	static constexpr int TOTAL_BLOCKS = 64; // MIFARE 1K有64个块
	byte cardData[TOTAL_BLOCKS][16]; // 存储所有块的数据
	bool readStatus[TOTAL_BLOCKS]; // 记录每个块的读取状态

public:
	std::vector<byte> GetBlock(const int blockIndex) const
	{
		if (blockIndex < 0 || blockIndex >= TOTAL_BLOCKS)
		{
			return std::vector<byte>();
		}

		// 检查数据是否完整
		for (const auto readState : readStatus)
		{
			if (!readState)
			{
				return std::vector<byte>();
			}
		}

		std::vector<byte> blockData;
		for (int i = 0; i < 16; i++)
		{
			blockData.push_back(cardData[blockIndex][i]);
		}
		return blockData;
	}

public:
	explicit MIFARE_1K_DataReader(const MFRC522& mfrc522) : MFRCDataReader(mfrc522)
	{
		memset(cardData, 0, sizeof(cardData));
		memset(readStatus, 0, sizeof(readStatus));
	}

	std::vector<byte> ReadData(MFRC522::MIFARE_Key& key) override
	{

		// 遍历所有扇区（0 - 15）
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
				// 失败直接返回空数据
				return std::vector<byte>();
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

		for (const auto readState : readStatus)
		{
			if (!readState)
			{
				return std::vector<byte>();
			}
		}


		std::vector<byte> data;

		// 将数组里的数据提取出来
		for (int blockIndex = 0; blockIndex < TOTAL_BLOCKS; blockIndex++)
		{
			for (int byteIndex = 0; byteIndex < 16; byteIndex++)
			{
				data.push_back(cardData[blockIndex][byteIndex]);
			}
		}

		return data;
	}
};


#endif // MIFARE_1K_DATAREADER_H
