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
	static MFRCDataReader* GenerateReader(const MFRC522& mfrc522,
										  const MFRC522::MIFARE_Key& key)
	{
		Serial.println("Factory Generating Reader...");
		switch (MFRC522::PICC_GetType(mfrc522.uid.sak))
		{
		case MFRC522::PICC_TYPE_MIFARE_1K:
			return new MIFARE_1K_DataReader(mfrc522, key);
			break;
		default:
			return new EmptyDataReader(mfrc522, key);
			break;
		}
	};
};


#endif // MFRCDATAREADERFACTORY_H
