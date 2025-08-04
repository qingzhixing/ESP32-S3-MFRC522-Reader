//
// Created by qingzhixing on 2025/7/25.
//

#ifndef MFRC522CONTROLLER_H
#define MFRC522CONTROLLER_H

#include <Arduino.h>
#include <ArxContainer.h>
#include <MFRC522.h>

#include "RFIDDataController/RFIDDataController.h"

class MFRC522Controller;

typedef void (*ControllerHandler)(const MFRC522Controller& controller);

class MFRC522Controller final
{
public:
	MFRC522Controller(int rstPin, int ssPin,
					  ControllerHandler onNewCardDetected = nullptr);

	~MFRC522Controller() = default;

	static String ByteToHexString(byte value);

	static String DumpByteArrayToHexString(const byte* buffer, byte bufferSize);

	static String DumpByteArrayToHexString(const std::vector<byte>& bytes);

	static void Begin(int8_t sck, int8_t miso, int8_t mosi, int8_t ss);

	void PCD_DumpVersionToSerial();

	std::vector<byte> ReadUID() const;

	String ReadUIDString() const;

	MFRC522::PICC_Type ReadPICCType() const;

	String ReadPICCTypeString() const;

	RFIDDataController*
	GenerateDataController(const MFRC522::MIFARE_Key& key) const;

	void SetOnNewCardDetected(ControllerHandler onNewCardDetected);

	void DetectCard();

private:
	MFRC522 mfrc522;
	ControllerHandler onNewCardDetected;
};


#endif // MFRC522CONTROLLER_H
