#ifndef SERIAL_H__
#define SERIAL_H__

#include "Arduino.h"

#include "constants.h"

//-----------------------------------------------------------------------------------------
/// @brief The interface class for the lower application level of communication interfaces (e.g. Serial, WiFi, Bluetooth, etc)
class CommunicationInterface {
public:
	virtual bool init(uint8_t* buffer, size_t bufferLength) { return false; }

	virtual size_t sendBytes(const uint8_t* bytes, size_t length) { return 0; }
	template<typename T> size_t send(const T& v) { return 0; }
	
	virtual size_t read() { return 0; }
};

//-----------------------------------------------------------------------------------------
// TODO: may be a necessity for design improvement if more than 1 Serial interface is required
class SerialCommunication : public CommunicationInterface {
	uint8_t* _buffer = nullptr;
	size_t _bufferLength = 0;
public:
	SerialCommunication() {}

	bool init(uint8_t* buffer, size_t bufferLength) override;

	size_t sendBytes(const uint8_t* bytes, size_t length) override;
	template<typename T> size_t send(const T& v);

	size_t read(size_t offset = 0);
};

class CommunicationPacket {
	uint8_t _type = 0;
public:
	CommunicationPacket(uint8_t type) : _type(type) { }

	inline uint8_t getType() const { return _type; }
};

//-----------------------------------------------------------------------------------------
/// @brief The communication protocol that wraps the message
/// @tparam INTERFACE 
template <typename INTERFACE>
class CommunicationProtocol {
	constexpr static uint8_t _magicCommunicationInitialize[32] = { 0xfc, 0x3c, 0x23, 0x7b, 0x10, 0xf3, 0x32, 0xd0, 0xca, 0xdf, 0x7, 0x64, 0x0f, 0x03, 0xa6, 0x4d,
																   0x88, 0x4e, 0x8b, 0x54, 0x98, 0xa2, 0x27, 0x3b, 0xfb, 0x7c, 0xa, 0x67, 0x78, 0xdb, 0xc1, 0x56 };
	constexpr static uint8_t _magicCommunicationFinish[32] = { 0x3a, 0xcd, 0xac, 0x8f, 0xb3, 0xe8, 0xb5, 0xff, 0x1e, 0xe7, 0x71, 0x6f, 0xc3, 0x24, 0xe8, 0xe9, 
															   0xc0, 0x24, 0x46, 0xc4, 0x67, 0x6d, 0x2a, 0x66, 0x30, 0xf7, 0x60, 0x49, 0x21, 0x66, 0x1f, 0xbf };
	constexpr static uint8_t _magicMessageStart[8] = { 0xd3, 0xdc, 0x3f, 0x99, 0xb5, 0xa6, 0x73, 0xd3 };
	constexpr static uint8_t _magicMessageEnd[8] = { 0x78, 0x8a, 0x69, 0x48, 0xaa, 0x36, 0x19, 0x7b };

	INTERFACE _communication;
	uint8_t _buffer[COMMUNICATIONPROTOCOL_BUFFERSIZE];
	std::vector<CommunicationPacket> _listeningList;

public:
	CommunicationProtocol() = default;

	bool init();
	bool tick();

	constexpr inline size_t getBufferLength() const { return COMMUNICATIONPROTOCOL_BUFFERSIZE; }
	
	bool communicationBegin();
	bool communicationEnd();

	bool send(const String& v);
	/// @brief Registers protocol to listen for the desired packet type
	void receive(const CommunicationPacket& packet); // TODO: naming?

	// TODO: Chess specific stuff should be abstracted away! Keep it like this until more clarification on interfaces

private:
	template<typename T> size_t _sendMessage(const T& v);
	size_t _rawSendMagic(const uint8_t* magic);
	size_t _rawSendBytes(const uint8_t* bytes, size_t length);

	bool _readMagic(const uint8_t* magic, size_t& bufferCursor);
};

template class CommunicationProtocol<SerialCommunication>;

// class WebGUISerial : public SerialCommunicationInterface {

// };

#endif // SERIAL_H__