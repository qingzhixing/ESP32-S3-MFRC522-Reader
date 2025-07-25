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
	virtual std::vector<byte> ReadData(MFRC522::MIFARE_Key& key) = 0;
	explicit MFRCDataReader(const MFRC522& mfrc522) : mfrc522(mfrc522) {};
	virtual ~MFRCDataReader() = default;

protected:
	MFRC522 mfrc522;
};


#endif // MFRCDATAREADER_H
