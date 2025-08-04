//
// Created by qingzhixing on 2025/7/26.
//

#ifndef EMPTYDATAREADER_H
#define EMPTYDATAREADER_H
#include "RFIDDataController.h"


class EmptyDataController final : public RFIDDataController
{
public:
	explicit EmptyDataController(const MFRC522& mfrc522,
								 const MFRC522::MIFARE_Key& key) :
		RFIDDataController(mfrc522, key)
	{
	}
	std::pair<bool, std::vector<byte>> ReadBlock(int blockNumber) override
	{
		return std::make_pair<bool, std::vector<byte>>(false, {});
	}
	MFRC522::StatusCode WriteBlock(int blockNumber,
								   const std::vector<byte>& data) override
	{
		return MFRC522::STATUS_INTERNAL_ERROR;
	}
};


#endif // EMPTYDATAREADER_H
