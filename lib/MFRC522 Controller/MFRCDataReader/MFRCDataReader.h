//
// Created by qingzhixing on 2025/7/25.
//

#ifndef MFRCDATAREADER_H
#define MFRCDATAREADER_H

#include <ArxContainer.h>

#include "MFRC522.h"

class MFRCDataReader
{
public:
	/**
	 * @brief 读取数据
	 * @return 返回两个值:是否成功和读取到的数据
	 */
	virtual std::pair<bool, std::vector<byte>> ReadBlock(int blockNumber) = 0;

	explicit MFRCDataReader(const MFRC522& mfrc522,
							const MFRC522::MIFARE_Key& key) :
		mfrc522(mfrc522), key(key)
	{
	}

	virtual ~MFRCDataReader() = default;

protected:
	MFRC522 mfrc522;
	MFRC522::MIFARE_Key key;
};


#endif // MFRCDATAREADER_H
