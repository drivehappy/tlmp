#pragma once

// Template member function definitions
// this file should _only_ be used in the header

template<typename T>
void TLMP::Network::Server::SendMessage(::google::protobuf::Message *message)
{
  // Write message data to array
  u32 size = sizeof(u8) * message->ByteSize();
  u8 *dump = new u8[size];
  message->SerializeToArray(dump, size);

  // Write data and size to raknet
  m_pBitStream->Write((u8)(ID_USER_PACKET_ENUM+1));
  m_pBitStream->Write(size);
  m_pBitStream->Write((const char *)dump, size);

  m_pServer->Send(m_pBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 1, UNASSIGNED_SYSTEM_ADDRESS, true);

  delete dump;
}

template<typename T>
T* TLMP::Network::Server::ParseMessage(u8 *packetData, u32 length)
{
  T* message = new T();

  m_pBitStream->Reset();
  m_pBitStream->Write((const char *)packetData, length);
	m_pBitStream->IgnoreBits(8);

  u32 size = 0;
  u8 *data;
  m_pBitStream->ReadPtr<u32>(&size);
  data = new u8[size];
  m_pBitStream->ReadAlignedBytes(data, size);
  log("Received serialized of size: %i", size);

  message->ParseFromArray(data, size);

  delete data;

  return message;
}
