//
// Created by qingzhixing on 2025/7/25.
//

#ifndef MFRC522CONTROLLER_H
#define MFRC522CONTROLLER_H

#include <Arduino.h>
#include <ArxContainer.h>
#include <MFRC522.h>

#include "MFRCDataReader/MFRCDataReader.h"

class MFRC522Controller;

typedef void (*ControllerHandler)(const MFRC522Controller& controller);

class MFRC522Controller final
{
public:
	MFRC522Controller(const int rstPin, const int ssPin, const ControllerHandler onNewCardDetected = nullptr);

	~MFRC522Controller() = default;

	static void Begin();

	std::vector<byte> ReadUID() const;

	String ReadUIDString() const;

	MFRC522::PICC_Type ReadPICCType() const;

	String ReadPICCTypeString() const;

	static String ByteToHexString(const byte value);

	static String DumpByteArrayToHexString(const byte* buffer, const byte bufferSize);

	MFRCDataReader* GenerateDataReader(const MFRC522::MIFARE_Key& key) const;

	void SetOnNewCardDetected(ControllerHandler onNewCardDetected);

	void DetectCard();

private:
	MFRC522 mfrc522;
	ControllerHandler onNewCardDetected;
};


#endif // MFRC522CONTROLLER_H
