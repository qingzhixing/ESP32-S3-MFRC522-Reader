//
// Created by qingzhixing on 2025/7/25.
//

#ifndef MFRCDATAREADER_H
#define MFRCDATAREADER_H

#include <ArxContainer.h>

#include "MFRC522.h"

class RFIDDataController
{
public:
	/**
	 * @brief 读取数据
	 * @return 返回两个值:是否成功和读取到的数据
	 */
	virtual std::pair<bool, std::vector<byte>> ReadBlock(int blockNumber) = 0;

	/**
	 *
	 * @brief 写入数据
	 * @param blockNumber 写入的块编号
	 * @param data 写入的数据, 长度为 BYTES_PER_BLOCK, 多出的会被截断
	 * @return 操作状态码
	 */
	virtual MFRC522::StatusCode WriteBlock(int blockNumber,
										   const std::vector<byte>& data) = 0;

	explicit RFIDDataController(const MFRC522& mfrc522,
								const MFRC522::MIFARE_Key& key) :
		mfrc522(mfrc522), key(key)
	{
	}

	virtual ~RFIDDataController() = default;

protected:
	MFRC522 mfrc522;
	MFRC522::MIFARE_Key key;
};


#endif // MFRCDATAREADER_H
