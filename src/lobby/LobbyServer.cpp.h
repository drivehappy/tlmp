#pragma once

// Template member function definitions
// this file should _only_ be used in the header

template<typename T>
void TLMP::Network::Lobby::LobbyServer::BroadcastMessage(Message msg, ::google::protobuf::Message *message)
{
  if (m_pServer) {
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
}

template<typename T>
void TLMP::Network::Lobby::LobbyServer::BroadcastMessage(const AddressOrGUID systemIdentifier, Message msg, ::google::protobuf::Message *message)
{
  if (m_pServer) {
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
}

template<typename T>
void TLMP::Network::Lobby::LobbyServer::SendMessage(const AddressOrGUID systemIdentifier, Message msg, ::google::protobuf::Message *message)
{
  if (m_pServer) {
    // Write message data to array
    unsigned int size = sizeof(unsigned char) * message->ByteSize();
    unsigned char *dump = new unsigned char[size];
    message->SerializeToArray(dump, size);

    // Write data and size to raknet
    m_pBitStream->Reset();
    m_pBitStream->Write((unsigned char)(ID_USER_PACKET_ENUM+1));
    m_pBitStream->Write((unsigned int)msg);
    m_pBitStream->Write(size);
    m_pBitStream->Write((const char *)dump, size);

    m_pServer->Send(m_pBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 1, systemIdentifier, false);

    delete dump;
  }
}

template<typename T>
T* TLMP::Network::Lobby::LobbyServer::ParseMessage(RakNet::BitStream *bitStream)
{
  T* message = new T();

  unsigned int size = 0;
  unsigned char *data;
  bitStream->ReadPtr<unsigned int>(&size);
  data = new unsigned char[size];
  bitStream->ReadAlignedBytes(data, size);

  message->ParseFromArray(data, size);

  delete data;

  return message;
}
