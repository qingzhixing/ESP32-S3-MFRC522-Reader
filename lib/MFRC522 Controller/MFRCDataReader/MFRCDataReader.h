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
	 * @return 读取成功返回true，失败返回false
	 */
	virtual bool ReadData() = 0;

	/**
	 * @brief 获取之前读取过的数据
	 * @return 数据
	 * @warning 必须在ReadData()成功后调用，否则返回空数组
	 */
	virtual std::vector<byte> GetData() const = 0;

	explicit MFRCDataReader(const MFRC522& mfrc522, const MFRC522::MIFARE_Key& key) : mfrc522(mfrc522), key(key) {}

	virtual ~MFRCDataReader() = default;

protected:
	MFRC522 mfrc522;
	MFRC522::MIFARE_Key key;
};


#endif // MFRCDATAREADER_H
