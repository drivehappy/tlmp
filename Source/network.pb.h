// Generated by the protocol buffer compiler.  DO NOT EDIT!

#ifndef PROTOBUF_network_2eproto__INCLUDED
#define PROTOBUF_network_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2002000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2002000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>

namespace TLMP {
namespace NetworkMessages {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_network_2eproto();
void protobuf_AssignDesc_network_2eproto();
void protobuf_ShutdownFile_network_2eproto();

class Position;
class Entity;
class Item;
class Player;
class Player_Pet;

enum Player_ClassType {
  Player_ClassType_ALCHEMIST = 0,
  Player_ClassType_VANQUISHER = 1,
  Player_ClassType_DESTROYER = 2
};
bool Player_ClassType_IsValid(int value);
const Player_ClassType Player_ClassType_ClassType_MIN = Player_ClassType_ALCHEMIST;
const Player_ClassType Player_ClassType_ClassType_MAX = Player_ClassType_DESTROYER;

const ::google::protobuf::EnumDescriptor* Player_ClassType_descriptor();
inline const ::std::string& Player_ClassType_Name(Player_ClassType value) {
  return ::google::protobuf::internal::NameOfEnum(
    Player_ClassType_descriptor(), value);
}
inline bool Player_ClassType_Parse(
    const ::std::string& name, Player_ClassType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Player_ClassType>(
    Player_ClassType_descriptor(), name, value);
}
enum Player_PetType {
  Player_PetType_DOG = 0,
  Player_PetType_CAT = 1
};
bool Player_PetType_IsValid(int value);
const Player_PetType Player_PetType_PetType_MIN = Player_PetType_DOG;
const Player_PetType Player_PetType_PetType_MAX = Player_PetType_CAT;

const ::google::protobuf::EnumDescriptor* Player_PetType_descriptor();
inline const ::std::string& Player_PetType_Name(Player_PetType value) {
  return ::google::protobuf::internal::NameOfEnum(
    Player_PetType_descriptor(), value);
}
inline bool Player_PetType_Parse(
    const ::std::string& name, Player_PetType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Player_PetType>(
    Player_PetType_descriptor(), name, value);
}
// ===================================================================

class Position : public ::google::protobuf::Message {
 public:
  Position();
  virtual ~Position();
  
  Position(const Position& from);
  
  inline Position& operator=(const Position& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Position& default_instance();
  void Swap(Position* other);
  
  // implements Message ----------------------------------------------
  
  Position* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Position& from);
  void MergeFrom(const Position& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const { _cached_size_ = size; }
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required float x = 1;
  inline bool has_x() const;
  inline void clear_x();
  static const int kXFieldNumber = 1;
  inline float x() const;
  inline void set_x(float value);
  
  // required float y = 2;
  inline bool has_y() const;
  inline void clear_y();
  static const int kYFieldNumber = 2;
  inline float y() const;
  inline void set_y(float value);
  
  // required float z = 3;
  inline bool has_z() const;
  inline void clear_z();
  static const int kZFieldNumber = 3;
  inline float z() const;
  inline void set_z(float value);
  
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  float x_;
  float y_;
  float z_;
  friend void  protobuf_AddDesc_network_2eproto();
  friend void protobuf_AssignDesc_network_2eproto();
  friend void protobuf_ShutdownFile_network_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Position* default_instance_;
};
// -------------------------------------------------------------------

class Entity : public ::google::protobuf::Message {
 public:
  Entity();
  virtual ~Entity();
  
  Entity(const Entity& from);
  
  inline Entity& operator=(const Entity& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Entity& default_instance();
  void Swap(Entity* other);
  
  // implements Message ----------------------------------------------
  
  Entity* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Entity& from);
  void MergeFrom(const Entity& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const { _cached_size_ = size; }
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required int32 level = 1;
  inline bool has_level() const;
  inline void clear_level();
  static const int kLevelFieldNumber = 1;
  inline ::google::protobuf::int32 level() const;
  inline void set_level(::google::protobuf::int32 value);
  
  // required int64 guid = 2;
  inline bool has_guid() const;
  inline void clear_guid();
  static const int kGuidFieldNumber = 2;
  inline ::google::protobuf::int64 guid() const;
  inline void set_guid(::google::protobuf::int64 value);
  
  // required bool noItems = 3;
  inline bool has_noitems() const;
  inline void clear_noitems();
  static const int kNoItemsFieldNumber = 3;
  inline bool noitems() const;
  inline void set_noitems(bool value);
  
  // optional .TLMP.NetworkMessages.Position destination = 4;
  inline bool has_destination() const;
  inline void clear_destination();
  static const int kDestinationFieldNumber = 4;
  inline const ::TLMP::NetworkMessages::Position& destination() const;
  inline ::TLMP::NetworkMessages::Position* mutable_destination();
  
  // repeated .TLMP.NetworkMessages.Position position = 5;
  inline int position_size() const;
  inline void clear_position();
  static const int kPositionFieldNumber = 5;
  inline const ::google::protobuf::RepeatedPtrField< ::TLMP::NetworkMessages::Position >& position() const;
  inline ::google::protobuf::RepeatedPtrField< ::TLMP::NetworkMessages::Position >* mutable_position();
  inline const ::TLMP::NetworkMessages::Position& position(int index) const;
  inline ::TLMP::NetworkMessages::Position* mutable_position(int index);
  inline ::TLMP::NetworkMessages::Position* add_position();
  
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int32 level_;
  ::google::protobuf::int64 guid_;
  bool noitems_;
  ::TLMP::NetworkMessages::Position* destination_;
  ::google::protobuf::RepeatedPtrField< ::TLMP::NetworkMessages::Position > position_;
  friend void  protobuf_AddDesc_network_2eproto();
  friend void protobuf_AssignDesc_network_2eproto();
  friend void protobuf_ShutdownFile_network_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Entity* default_instance_;
};
// -------------------------------------------------------------------

class Item : public ::google::protobuf::Message {
 public:
  Item();
  virtual ~Item();
  
  Item(const Item& from);
  
  inline Item& operator=(const Item& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Item& default_instance();
  void Swap(Item* other);
  
  // implements Message ----------------------------------------------
  
  Item* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Item& from);
  void MergeFrom(const Item& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const { _cached_size_ = size; }
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required int64 guid = 1;
  inline bool has_guid() const;
  inline void clear_guid();
  static const int kGuidFieldNumber = 1;
  inline ::google::protobuf::int64 guid() const;
  inline void set_guid(::google::protobuf::int64 value);
  
  // required int32 level = 2;
  inline bool has_level() const;
  inline void clear_level();
  static const int kLevelFieldNumber = 2;
  inline ::google::protobuf::int32 level() const;
  inline void set_level(::google::protobuf::int32 value);
  
  // required int32 unk0 = 3;
  inline bool has_unk0() const;
  inline void clear_unk0();
  static const int kUnk0FieldNumber = 3;
  inline ::google::protobuf::int32 unk0() const;
  inline void set_unk0(::google::protobuf::int32 value);
  
  // required int32 unk1 = 4;
  inline bool has_unk1() const;
  inline void clear_unk1();
  static const int kUnk1FieldNumber = 4;
  inline ::google::protobuf::int32 unk1() const;
  inline void set_unk1(::google::protobuf::int32 value);
  
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int64 guid_;
  ::google::protobuf::int32 level_;
  ::google::protobuf::int32 unk0_;
  ::google::protobuf::int32 unk1_;
  friend void  protobuf_AddDesc_network_2eproto();
  friend void protobuf_AssignDesc_network_2eproto();
  friend void protobuf_ShutdownFile_network_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Item* default_instance_;
};
// -------------------------------------------------------------------

class Player_Pet : public ::google::protobuf::Message {
 public:
  Player_Pet();
  virtual ~Player_Pet();
  
  Player_Pet(const Player_Pet& from);
  
  inline Player_Pet& operator=(const Player_Pet& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Player_Pet& default_instance();
  void Swap(Player_Pet* other);
  
  // implements Message ----------------------------------------------
  
  Player_Pet* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Player_Pet& from);
  void MergeFrom(const Player_Pet& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const { _cached_size_ = size; }
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required int64 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int64 id() const;
  inline void set_id(::google::protobuf::int64 value);
  
  // required .TLMP.NetworkMessages.Player.PetType type = 2;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 2;
  inline ::TLMP::NetworkMessages::Player_PetType type() const;
  inline void set_type(::TLMP::NetworkMessages::Player_PetType value);
  
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int64 id_;
  int type_;
  friend void  protobuf_AddDesc_network_2eproto();
  friend void protobuf_AssignDesc_network_2eproto();
  friend void protobuf_ShutdownFile_network_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Player_Pet* default_instance_;
};
// -------------------------------------------------------------------

class Player : public ::google::protobuf::Message {
 public:
  Player();
  virtual ~Player();
  
  Player(const Player& from);
  
  inline Player& operator=(const Player& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Player& default_instance();
  void Swap(Player* other);
  
  // implements Message ----------------------------------------------
  
  Player* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Player& from);
  void MergeFrom(const Player& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const { _cached_size_ = size; }
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  typedef Player_Pet Pet;
  
  typedef Player_ClassType ClassType;
  static const ClassType ALCHEMIST = Player_ClassType_ALCHEMIST;
  static const ClassType VANQUISHER = Player_ClassType_VANQUISHER;
  static const ClassType DESTROYER = Player_ClassType_DESTROYER;
  static inline bool ClassType_IsValid(int value) {
    return Player_ClassType_IsValid(value);
  }
  static const ClassType ClassType_MIN =
    Player_ClassType_ClassType_MIN;
  static const ClassType ClassType_MAX =
    Player_ClassType_ClassType_MAX;
  static inline const ::google::protobuf::EnumDescriptor*
  ClassType_descriptor() {
    return Player_ClassType_descriptor();
  }
  static inline const ::std::string& ClassType_Name(ClassType value) {
    return Player_ClassType_Name(value);
  }
  static inline bool ClassType_Parse(const ::std::string& name,
      ClassType* value) {
    return Player_ClassType_Parse(name, value);
  }
  
  typedef Player_PetType PetType;
  static const PetType DOG = Player_PetType_DOG;
  static const PetType CAT = Player_PetType_CAT;
  static inline bool PetType_IsValid(int value) {
    return Player_PetType_IsValid(value);
  }
  static const PetType PetType_MIN =
    Player_PetType_PetType_MIN;
  static const PetType PetType_MAX =
    Player_PetType_PetType_MAX;
  static inline const ::google::protobuf::EnumDescriptor*
  PetType_descriptor() {
    return Player_PetType_descriptor();
  }
  static inline const ::std::string& PetType_Name(PetType value) {
    return Player_PetType_Name(value);
  }
  static inline bool PetType_Parse(const ::std::string& name,
      PetType* value) {
    return Player_PetType_Parse(name, value);
  }
  
  // accessors -------------------------------------------------------
  
  // required string name = 1;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 1;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  
  // required int64 id = 2;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 2;
  inline ::google::protobuf::int64 id() const;
  inline void set_id(::google::protobuf::int64 value);
  
  // required .TLMP.NetworkMessages.Player.ClassType type = 3;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 3;
  inline ::TLMP::NetworkMessages::Player_ClassType type() const;
  inline void set_type(::TLMP::NetworkMessages::Player_ClassType value);
  
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* name_;
  static const ::std::string _default_name_;
  ::google::protobuf::int64 id_;
  int type_;
  friend void  protobuf_AddDesc_network_2eproto();
  friend void protobuf_AssignDesc_network_2eproto();
  friend void protobuf_ShutdownFile_network_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Player* default_instance_;
};
// ===================================================================


// ===================================================================


// ===================================================================

// Position

// required float x = 1;
inline bool Position::has_x() const {
  return _has_bit(0);
}
inline void Position::clear_x() {
  x_ = 0;
  _clear_bit(0);
}
inline float Position::x() const {
  return x_;
}
inline void Position::set_x(float value) {
  _set_bit(0);
  x_ = value;
}

// required float y = 2;
inline bool Position::has_y() const {
  return _has_bit(1);
}
inline void Position::clear_y() {
  y_ = 0;
  _clear_bit(1);
}
inline float Position::y() const {
  return y_;
}
inline void Position::set_y(float value) {
  _set_bit(1);
  y_ = value;
}

// required float z = 3;
inline bool Position::has_z() const {
  return _has_bit(2);
}
inline void Position::clear_z() {
  z_ = 0;
  _clear_bit(2);
}
inline float Position::z() const {
  return z_;
}
inline void Position::set_z(float value) {
  _set_bit(2);
  z_ = value;
}

// -------------------------------------------------------------------

// Entity

// required int32 level = 1;
inline bool Entity::has_level() const {
  return _has_bit(0);
}
inline void Entity::clear_level() {
  level_ = 0;
  _clear_bit(0);
}
inline ::google::protobuf::int32 Entity::level() const {
  return level_;
}
inline void Entity::set_level(::google::protobuf::int32 value) {
  _set_bit(0);
  level_ = value;
}

// required int64 guid = 2;
inline bool Entity::has_guid() const {
  return _has_bit(1);
}
inline void Entity::clear_guid() {
  guid_ = GOOGLE_LONGLONG(0);
  _clear_bit(1);
}
inline ::google::protobuf::int64 Entity::guid() const {
  return guid_;
}
inline void Entity::set_guid(::google::protobuf::int64 value) {
  _set_bit(1);
  guid_ = value;
}

// required bool noItems = 3;
inline bool Entity::has_noitems() const {
  return _has_bit(2);
}
inline void Entity::clear_noitems() {
  noitems_ = false;
  _clear_bit(2);
}
inline bool Entity::noitems() const {
  return noitems_;
}
inline void Entity::set_noitems(bool value) {
  _set_bit(2);
  noitems_ = value;
}

// optional .TLMP.NetworkMessages.Position destination = 4;
inline bool Entity::has_destination() const {
  return _has_bit(3);
}
inline void Entity::clear_destination() {
  if (destination_ != NULL) destination_->::TLMP::NetworkMessages::Position::Clear();
  _clear_bit(3);
}
inline const ::TLMP::NetworkMessages::Position& Entity::destination() const {
  return destination_ != NULL ? *destination_ : *default_instance_->destination_;
}
inline ::TLMP::NetworkMessages::Position* Entity::mutable_destination() {
  _set_bit(3);
  if (destination_ == NULL) destination_ = new ::TLMP::NetworkMessages::Position;
  return destination_;
}

// repeated .TLMP.NetworkMessages.Position position = 5;
inline int Entity::position_size() const {
  return position_.size();
}
inline void Entity::clear_position() {
  position_.Clear();
}
inline const ::google::protobuf::RepeatedPtrField< ::TLMP::NetworkMessages::Position >&
Entity::position() const {
  return position_;
}
inline ::google::protobuf::RepeatedPtrField< ::TLMP::NetworkMessages::Position >*
Entity::mutable_position() {
  return &position_;
}
inline const ::TLMP::NetworkMessages::Position& Entity::position(int index) const {
  return position_.Get(index);
}
inline ::TLMP::NetworkMessages::Position* Entity::mutable_position(int index) {
  return position_.Mutable(index);
}
inline ::TLMP::NetworkMessages::Position* Entity::add_position() {
  return position_.Add();
}

// -------------------------------------------------------------------

// Item

// required int64 guid = 1;
inline bool Item::has_guid() const {
  return _has_bit(0);
}
inline void Item::clear_guid() {
  guid_ = GOOGLE_LONGLONG(0);
  _clear_bit(0);
}
inline ::google::protobuf::int64 Item::guid() const {
  return guid_;
}
inline void Item::set_guid(::google::protobuf::int64 value) {
  _set_bit(0);
  guid_ = value;
}

// required int32 level = 2;
inline bool Item::has_level() const {
  return _has_bit(1);
}
inline void Item::clear_level() {
  level_ = 0;
  _clear_bit(1);
}
inline ::google::protobuf::int32 Item::level() const {
  return level_;
}
inline void Item::set_level(::google::protobuf::int32 value) {
  _set_bit(1);
  level_ = value;
}

// required int32 unk0 = 3;
inline bool Item::has_unk0() const {
  return _has_bit(2);
}
inline void Item::clear_unk0() {
  unk0_ = 0;
  _clear_bit(2);
}
inline ::google::protobuf::int32 Item::unk0() const {
  return unk0_;
}
inline void Item::set_unk0(::google::protobuf::int32 value) {
  _set_bit(2);
  unk0_ = value;
}

// required int32 unk1 = 4;
inline bool Item::has_unk1() const {
  return _has_bit(3);
}
inline void Item::clear_unk1() {
  unk1_ = 0;
  _clear_bit(3);
}
inline ::google::protobuf::int32 Item::unk1() const {
  return unk1_;
}
inline void Item::set_unk1(::google::protobuf::int32 value) {
  _set_bit(3);
  unk1_ = value;
}

// -------------------------------------------------------------------

// Player_Pet

// required int64 id = 1;
inline bool Player_Pet::has_id() const {
  return _has_bit(0);
}
inline void Player_Pet::clear_id() {
  id_ = GOOGLE_LONGLONG(0);
  _clear_bit(0);
}
inline ::google::protobuf::int64 Player_Pet::id() const {
  return id_;
}
inline void Player_Pet::set_id(::google::protobuf::int64 value) {
  _set_bit(0);
  id_ = value;
}

// required .TLMP.NetworkMessages.Player.PetType type = 2;
inline bool Player_Pet::has_type() const {
  return _has_bit(1);
}
inline void Player_Pet::clear_type() {
  type_ = 0;
  _clear_bit(1);
}
inline ::TLMP::NetworkMessages::Player_PetType Player_Pet::type() const {
  return static_cast< ::TLMP::NetworkMessages::Player_PetType >(type_);
}
inline void Player_Pet::set_type(::TLMP::NetworkMessages::Player_PetType value) {
  GOOGLE_DCHECK(::TLMP::NetworkMessages::Player_PetType_IsValid(value));
  _set_bit(1);
  type_ = value;
}

// -------------------------------------------------------------------

// Player

// required string name = 1;
inline bool Player::has_name() const {
  return _has_bit(0);
}
inline void Player::clear_name() {
  if (name_ != &_default_name_) {
    name_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& Player::name() const {
  return *name_;
}
inline void Player::set_name(const ::std::string& value) {
  _set_bit(0);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void Player::set_name(const char* value) {
  _set_bit(0);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void Player::set_name(const char* value, size_t size) {
  _set_bit(0);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Player::mutable_name() {
  _set_bit(0);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  return name_;
}

// required int64 id = 2;
inline bool Player::has_id() const {
  return _has_bit(1);
}
inline void Player::clear_id() {
  id_ = GOOGLE_LONGLONG(0);
  _clear_bit(1);
}
inline ::google::protobuf::int64 Player::id() const {
  return id_;
}
inline void Player::set_id(::google::protobuf::int64 value) {
  _set_bit(1);
  id_ = value;
}

// required .TLMP.NetworkMessages.Player.ClassType type = 3;
inline bool Player::has_type() const {
  return _has_bit(2);
}
inline void Player::clear_type() {
  type_ = 0;
  _clear_bit(2);
}
inline ::TLMP::NetworkMessages::Player_ClassType Player::type() const {
  return static_cast< ::TLMP::NetworkMessages::Player_ClassType >(type_);
}
inline void Player::set_type(::TLMP::NetworkMessages::Player_ClassType value) {
  GOOGLE_DCHECK(::TLMP::NetworkMessages::Player_ClassType_IsValid(value));
  _set_bit(2);
  type_ = value;
}


}  // namespace NetworkMessages
}  // namespace TLMP

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::TLMP::NetworkMessages::Player_ClassType>() {
  return ::TLMP::NetworkMessages::Player_ClassType_descriptor();
}
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::TLMP::NetworkMessages::Player_PetType>() {
  return ::TLMP::NetworkMessages::Player_PetType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

#endif  // PROTOBUF_network_2eproto__INCLUDED
