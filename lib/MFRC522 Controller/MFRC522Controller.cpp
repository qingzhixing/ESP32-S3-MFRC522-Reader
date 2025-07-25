//
// Created by qingzhixing on 2025/7/25.
//
#include "MFRC522Controller.h"

#include <Arduino.h>
#include <ArxContainer.h>
#include <MFRC522.h>
#include <SPI.h>

#include "MFRCDataReader/MFRCDataReaderFactory.h"

String MFRC522Controller::ByteToHexString(const byte value)
{
	char buffer[5];
	sprintf(buffer, "%02x", value);
	return buffer;
}

MFRC522Controller::MFRC522Controller(const int rstPin, const int ssPin, const ControllerHandler onNewCardDetected) :
	onNewCardDetected(onNewCardDetected)
{
	mfrc522.PCD_Init(ssPin, rstPin);
}

void MFRC522Controller::Begin() { SPIClass::begin(); }

void MFRC522Controller::PCD_DumpVersionToSerial() { mfrc522.PCD_DumpVersionToSerial(); }

std::vector<byte> MFRC522Controller::ReadUID() const
{
	std::vector<byte> uid;
	for (byte i = 0; i < mfrc522.uid.size; i++)
	{
		uid.push_back(mfrc522.uid.uidByte[i]);
	}
	return uid;
}

String MFRC522Controller::ReadUIDString() const
{
	String uid;
	for (byte i = 0; i < mfrc522.uid.size; i++)
	{
		uid += " " + ByteToHexString(mfrc522.uid.uidByte[i]);
	}
	return uid;
}

MFRC522::PICC_Type MFRC522Controller::ReadPICCType() const { return MFRC522::PICC_GetType(mfrc522.uid.sak); }

String MFRC522Controller::ReadPICCTypeString() const { return MFRC522::PICC_GetTypeName(ReadPICCType()); }

String MFRC522Controller::DumpByteArrayToHexString(const byte* buffer, const byte bufferSize)
{
	String result;

	for (byte i = 0; i < bufferSize; i++)
	{
		result += " " + ByteToHexString(buffer[i]);
	}
	return result;
}

MFRCDataReader* MFRC522Controller::GenerateDataReader(const MFRC522::MIFARE_Key& key) const
{
	return MFRCDataReaderFactory::GenerateReader(mfrc522, key);
}

void MFRC522Controller::SetOnNewCardDetected(const ControllerHandler onNewCardDetected)
{
	this->onNewCardDetected = onNewCardDetected;
}

void MFRC522Controller::DetectCard()
{
	if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
	{
		onNewCardDetected(*this);

		// Halt PICC
		mfrc522.PICC_HaltA();
		// Stop encryption on PCD
		mfrc522.PCD_StopCrypto1();
	}
}
