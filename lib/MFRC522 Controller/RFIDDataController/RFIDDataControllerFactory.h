//
// Created by qingzhixing on 2025/7/25.
//

#ifndef MFRCDATAREADERFACTORY_H
#define MFRCDATAREADERFACTORY_H
#include "EmptyDataController.h"
#include "MFRC522.h"
#include "MIFARE_1K_DataController.h"
#include "RFIDDataController.h"

class RFIDDataControllerFactory
{
public:
	static RFIDDataController*
	GenerateDataController(const MFRC522& mfrc522,
						   const MFRC522::MIFARE_Key& key)
	{
		switch (MFRC522::PICC_GetType(mfrc522.uid.sak))
		{
		case MFRC522::PICC_TYPE_MIFARE_1K:
			return new MIFARE_1K_DataController(mfrc522, key);
			break;
		default:
			return new EmptyDataController(mfrc522, key);
			break;
		}
	};
};


#endif // MFRCDATAREADERFACTORY_H
