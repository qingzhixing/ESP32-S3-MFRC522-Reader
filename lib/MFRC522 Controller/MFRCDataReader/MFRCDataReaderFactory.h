//
// Created by qingzhixing on 2025/7/25.
//

#ifndef MFRCDATAREADERFACTORY_H
#define MFRCDATAREADERFACTORY_H
#include "EmptyDataReader.h"
#include "MFRC522.h"
#include "MFRCDataReader.h"
#include "MIFARE_1K_DataReader.h"

class MFRCDataReaderFactory
{
public:
	static MFRCDataReader* GenerateReader(const MFRC522& mfrc522)
	{
		switch (mfrc522.PICC_GetType(mfrc522.uid.sak))
		{
		case MFRC522::PICC_TYPE_MIFARE_1K:
			return new MIFARE_1K_DataReader(mfrc522);
		default:
			return new EmptyDataReader(mfrc522);
		}
	};
};


#endif // MFRCDATAREADERFACTORY_H
