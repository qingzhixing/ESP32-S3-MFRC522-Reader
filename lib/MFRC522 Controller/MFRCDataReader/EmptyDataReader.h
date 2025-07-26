//
// Created by qingzhixing on 2025/7/26.
//

#ifndef EMPTYDATAREADER_H
#define EMPTYDATAREADER_H
#include "MFRCDataReader.h"


class EmptyDataReader final : public MFRCDataReader
{
public:
	explicit EmptyDataReader(const MFRC522& mfrc522,
							 const MFRC522::MIFARE_Key& key) :
		MFRCDataReader(mfrc522, key)
	{
	}
	std::pair<bool, std::vector<byte>> ReadBlock(int blockNumber) override
	{
		return std::make_pair<bool, std::vector<byte>>(false, {});
	}
};


#endif // EMPTYDATAREADER_H
