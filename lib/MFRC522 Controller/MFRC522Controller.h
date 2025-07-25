//
// Created by qingzhixing on 2025/7/25.
//

#ifndef MFRC522CONTROLLER_H
#define MFRC522CONTROLLER_H

#include <Arduino.h>
#include <ArxContainer.h>
#include <MFRC522.h>

class MFRC522Controller;

typedef void (*Handler)(MFRC522Controller& controller);

class MFRC522Controller final
{
public:
	MFRC522Controller(const int rstPin, const int ssPin, const Handler onNewCardDetected);
	~MFRC522Controller() = default;
	static void Begin();
	std::vector<byte> ReadUID() const;
	MFRC522::PICC_Type ReadPICCType() const;
	String ReadPICCTypeString() const;

	static String DumpByteArrayToString(const byte* buffer, const byte bufferSize);

	std::vector<byte> ReadData(MFRC522::MIFARE_Key& key) const;

	void SetOnNewCardDetected(Handler onNewCardDetected);

	void DetectCard();

private:
	MFRC522 mfrc522;
	Handler onNewCardDetected;
};


#endif // MFRC522CONTROLLER_H
