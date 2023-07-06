#include "interfaces/serial.h"

//-----------------------------------------------------------------------------------------
bool SerialCommunication::init(uint8_t* buffer, size_t bufferLength) {
	_buffer = buffer;
	_bufferLength = bufferLength;

	Serial.begin(SERIAL_BAUDRATE);

	return true;
}

size_t SerialCommunication::sendBytes(const uint8_t* bytes, size_t length) {
	return Serial.write(bytes, length);
}

template<typename T> size_t SerialCommunication::send(const T& v) {
	return Serial.print(v);
}

size_t SerialCommunication::read(size_t offset) {
	if (offset >= _bufferLength)
		return 0;
	return Serial.read(_buffer + offset, _bufferLength - offset);
}


//-----------------------------------------------------------------------------------------
template<typename INTERFACE> constexpr uint8_t CommunicationProtocol<INTERFACE>::_magicCommunicationInitialize[32];
template<typename INTERFACE> constexpr uint8_t CommunicationProtocol<INTERFACE>::_magicCommunicationFinish[32];
template<typename INTERFACE> constexpr uint8_t CommunicationProtocol<INTERFACE>::_magicMessageStart[8];
template<typename INTERFACE> constexpr uint8_t CommunicationProtocol<INTERFACE>::_magicMessageEnd[8];

template <typename INTERFACE>
bool CommunicationProtocol<INTERFACE>::init() {
	return _communication.init(_buffer, getBufferLength());
}

template <typename INTERFACE>
bool CommunicationProtocol<INTERFACE>::communicationBegin() {
	if (!_rawSendMagic(_magicCommunicationInitialize))
		return false;
	
	uint8_t infoChunk[8] = { COMMUNICATIONPROTOCOL_VERSION, 0, 0, 0, 0, 0, 0, 0 }; // version + reserved
	if (_rawSendBytes(infoChunk, 8) != 8)
		return false;

	return true;
}

template <typename INTERFACE>
bool CommunicationProtocol<INTERFACE>::communicationEnd() {
	if (!_rawSendMagic(_magicCommunicationFinish))
		return false;
	return true;
}

template <typename INTERFACE>
template<typename T> size_t CommunicationProtocol<INTERFACE>::_sendMessage(const T& v) {
	if (!_rawSendMagic(_magicMessageStart))
		return false;
	_communication.send(v);
	if (!_rawSendMagic(_magicMessageEnd))
		return false;
	return true;
}

template <typename INTERFACE>
size_t CommunicationProtocol<INTERFACE>::_rawSendMagic(const uint8_t* magic) {
	constexpr size_t magicSize = sizeof(magic) / sizeof(magic[0]);
	size_t sentCnt = _rawSendBytes(magic, magicSize);
	if (sentCnt != magicSize)
		return 0;
	return sentCnt;
}

template <typename INTERFACE>
size_t CommunicationProtocol<INTERFACE>::_rawSendBytes(const uint8_t* bytes, size_t length) {
	return _communication.sendBytes(bytes, length);
}

template <typename INTERFACE>
bool CommunicationProtocol<INTERFACE>::_readMagic(const uint8_t* magic, size_t& bufferCursor) {
	constexpr size_t magicSize = sizeof(magic) / sizeof(magic[0]);
	for (size_t cnt = _communication.read(); cnt;) {
		uint8_t crsMag = 0;
		for (size_t crsBuf = 0; crsBuf < getBufferLength(); ++crsBuf) {
			if (_magicMessageStart[crsMag] != _buffer[crsBuf])
				continue;
			crsMag++;
			if (crsMag == magicSize) {
				memmove(_buffer, _buffer + crsBuf, getBufferLength() - crsBuf);
				bufferCursor = _communication.read(crsBuf) + crsBuf;
				return true;
			}
		}
	}
	return false;
}

template <typename INTERFACE>
bool CommunicationProtocol<INTERFACE>::tick() {
	size_t bufferCursor = 0;
	if (!_readMagic(_magicMessageStart, bufferCursor))
		return false;
	
	uint8_t curBuff[128];
	memcpy(curBuff, _buffer, bufferCursor);
	size_t curBuffCursor = bufferCursor;
	
	for (size_t cnt = _communication.read(); cnt;) {
		memcpy(curBuff + curBuffCursor, _buffer, cnt);
		curBuffCursor += cnt;
	}

	Serial.write(curBuff, curBuffCursor);
	return true;
}

template <typename INTERFACE>
bool CommunicationProtocol<INTERFACE>::send(const String& v) {
	return true;
}

template <typename INTERFACE>
void CommunicationProtocol<INTERFACE>::receive(const CommunicationPacket& packet) {
	_listeningList.push_back(packet);
}