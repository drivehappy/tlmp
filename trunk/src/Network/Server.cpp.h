#pragma once

// Template member function definitions
// this file should _only_ be used in the header

template<typename T>
void TLMP::Network::Server::BroadcastMessage(Message msg, ::google::protobuf::Message *message)
{
  // Write message data to array
  u32 size = sizeof(u8) * message->ByteSize();
  u8 *dump = new u8[size];
  message->SerializeToArray(dump, size);

  // Write data and size to raknet
  m_pBitStream->Reset();
  m_pBitStream->Write((u8)(ID_USER_PACKET_ENUM+1));
  m_pBitStream->Write((u32)msg);
  m_pBitStream->Write(size);
  m_pBitStream->Write((const char *)dump, size);

  m_pServer->Send(m_pBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 1, UNASSIGNED_SYSTEM_ADDRESS, true);

  delete dump;
}

template<typename T>
void TLMP::Network::Server::BroadcastMessage(const AddressOrGUID systemIdentifier, Message msg, ::google::protobuf::Message *message)
{
  // Write message data to array
  u32 size = sizeof(u8) * message->ByteSize();
  u8 *dump = new u8[size];
  message->SerializeToArray(dump, size);

  // Write data and size to raknet
  m_pBitStream->Reset();
  m_pBitStream->Write((u8)(ID_USER_PACKET_ENUM+1));
  m_pBitStream->Write((u32)msg);
  m_pBitStream->Write(size);
  m_pBitStream->Write((const char *)dump, size);

  m_pServer->Send(m_pBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 1, systemIdentifier, true);

  delete dump;
}

template<typename T>
void TLMP::Network::Server::SendMessage(const AddressOrGUID systemIdentifier, Message msg, ::google::protobuf::Message *message)
{
  // Write message data to array
  u32 size = sizeof(u8) * message->ByteSize();
  u8 *dump = new u8[size];
  message->SerializeToArray(dump, size);

  // Write data and size to raknet
  m_pBitStream->Reset();
  m_pBitStream->Write((u8)(ID_USER_PACKET_ENUM+1));
  m_pBitStream->Write((u32)msg);
  m_pBitStream->Write(size);
  m_pBitStream->Write((const char *)dump, size);

  m_pServer->Send(m_pBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 1, systemIdentifier, false);

  delete dump;
}

template<typename T>
T* TLMP::Network::Server::ParseMessage(RakNet::BitStream *bitStream)
{
  T* message = new T();

  u32 size = 0;
  u8 *data;
  bitStream->ReadPtr<u32>(&size);
  data = new u8[size];
  bitStream->ReadAlignedBytes(data, size);
  //log("Received serialized of size: %i", size);

  message->ParseFromArray(data, size);

  delete data;

  return message;
}
