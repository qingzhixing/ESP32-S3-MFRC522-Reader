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
	static constexpr int BLOCKS_PER_SECTOR = 4;
	static constexpr int BYTES_PER_BLOCK = 16;
	static constexpr int SECTORS_PER_CARD = 16;

	static constexpr int TOTAL_BLOCKS = BLOCKS_PER_SECTOR * SECTORS_PER_CARD;

public:
public:
	explicit MIFARE_1K_DataReader(const MFRC522& mfrc522,
								  const MFRC522::MIFARE_Key& key) :
		MFRCDataReader(mfrc522, key)
	{
	}

	std::pair<bool, std::vector<byte>> ReadBlock(const int blockNumber) override
	{
		std::vector<byte> blockData{};

		const auto trailerBlock = (blockNumber / 4) * 4 + 3;

		// 认证当前扇区
		MFRC522::StatusCode status =
			mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
									 trailerBlock, &key, &(mfrc522.uid));

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
};


#endif // MIFARE_1K_DATAREADER_H
