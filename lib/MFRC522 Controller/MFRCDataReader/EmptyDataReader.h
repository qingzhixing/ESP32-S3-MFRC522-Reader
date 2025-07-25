//
// Created by qingzhixing on 2025/7/26.
//

#ifndef EMPTYDATAREADER_H
#define EMPTYDATAREADER_H
#include "MFRCDataReader.h"


class EmptyDataReader final : public MFRCDataReader
{
public:
	explicit EmptyDataReader(const MFRC522& mfrc522, const MFRC522::MIFARE_Key& key) : MFRCDataReader(mfrc522, key) {}
	bool ReadData() override { return false; }
	std::vector<byte> GetData() const override { return std::vector<byte>(); }
};


#endif // EMPTYDATAREADER_H
