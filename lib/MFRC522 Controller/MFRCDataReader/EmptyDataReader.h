//
// Created by qingzhixing on 2025/7/26.
//

#ifndef EMPTYDATAREADER_H
#define EMPTYDATAREADER_H
#include "MFRCDataReader.h"


class EmptyDataReader final : public MFRCDataReader
{
public:
	explicit EmptyDataReader(const MFRC522& mfrc522) : MFRCDataReader(mfrc522) {}
	std::vector<byte> ReadData(MFRC522::MIFARE_Key& key) override { return {}; }
};


#endif // EMPTYDATAREADER_H
