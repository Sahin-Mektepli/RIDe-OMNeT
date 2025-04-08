//
// Generated file, do not edit! Created by opp_msgtool 6.0 from
// src/BlockchainMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#pragma warning(disable : 4101)
#pragma warning(disable : 4065)
#endif

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wunreachable-code-break"
#pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include "BlockchainMessage_m.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <type_traits>


namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack
// functions for multiple reasons. They are in the omnetpp namespace, to allow
// them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template <typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T, A> &v) {
  int n = v.size();
  doParsimPacking(buffer, n);
  for (int i = 0; i < n; i++)
    doParsimPacking(buffer, v[i]);
}

template <typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T, A> &v) {
  int n;
  doParsimUnpacking(buffer, n);
  v.resize(n);
  for (int i = 0; i < n; i++)
    doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template <typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T, A> &l) {
  doParsimPacking(buffer, (int)l.size());
  for (typename std::list<T, A>::const_iterator it = l.begin(); it != l.end();
       ++it)
    doParsimPacking(buffer, (T &)*it);
}

template <typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T, A> &l) {
  int n;
  doParsimUnpacking(buffer, n);
  for (int i = 0; i < n; i++) {
    l.push_back(T());
    doParsimUnpacking(buffer, l.back());
  }
}

// Packing/unpacking an std::set
template <typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer,
                     const std::set<T, Tr, A> &s) {
  doParsimPacking(buffer, (int)s.size());
  for (typename std::set<T, Tr, A>::const_iterator it = s.begin();
       it != s.end(); ++it)
    doParsimPacking(buffer, *it);
}

template <typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T, Tr, A> &s) {
  int n;
  doParsimUnpacking(buffer, n);
  for (int i = 0; i < n; i++) {
    T x;
    doParsimUnpacking(buffer, x);
    s.insert(x);
  }
}

// Packing/unpacking an std::map
template <typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer,
                     const std::map<K, V, Tr, A> &m) {
  doParsimPacking(buffer, (int)m.size());
  for (typename std::map<K, V, Tr, A>::const_iterator it = m.begin();
       it != m.end(); ++it) {
    doParsimPacking(buffer, it->first);
    doParsimPacking(buffer, it->second);
  }
}

template <typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K, V, Tr, A> &m) {
  int n;
  doParsimUnpacking(buffer, n);
  for (int i = 0; i < n; i++) {
    K k;
    V v;
    doParsimUnpacking(buffer, k);
    doParsimUnpacking(buffer, v);
    m[k] = v;
  }
}

// Default pack/unpack function for arrays
template <typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n) {
  for (int i = 0; i < n; i++)
    doParsimPacking(b, t[i]);
}

template <typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n) {
  for (int i = 0; i < n; i++)
    doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking()
// function
template <typename T> void doParsimPacking(omnetpp::cCommBuffer *, const T &t) {
  throw omnetpp::cRuntimeError(
      "Parsim error: No doParsimPacking() function for type %s",
      omnetpp::opp_typename(typeid(t)));
}

template <typename T> void doParsimUnpacking(omnetpp::cCommBuffer *, T &t) {
  throw omnetpp::cRuntimeError(
      "Parsim error: No doParsimUnpacking() function for type %s",
      omnetpp::opp_typename(typeid(t)));
}

} // namespace omnetpp

Register_Class(BlockchainTransaction)

    BlockchainTransaction::BlockchainTransaction(const char *name, short kind)
    : ::omnetpp::cMessage(name, kind) {}

BlockchainTransaction::BlockchainTransaction(const BlockchainTransaction &other)
    : ::omnetpp::cMessage(other) {
  copy(other);
}

BlockchainTransaction::~BlockchainTransaction() {}

BlockchainTransaction &
BlockchainTransaction::operator=(const BlockchainTransaction &other) {
  if (this == &other)
    return *this;
  ::omnetpp::cMessage::operator=(other);
  copy(other);
  return *this;
}

void BlockchainTransaction::copy(const BlockchainTransaction &other) {
  this->requesterId = other.requesterId;
  this->providerId = other.providerId;
  this->rating = other.rating;
}

void BlockchainTransaction::parsimPack(omnetpp::cCommBuffer *b) const {
  ::omnetpp::cMessage::parsimPack(b);
  doParsimPacking(b, this->requesterId);
  doParsimPacking(b, this->providerId);
  doParsimPacking(b, this->rating);
}

void BlockchainTransaction::parsimUnpack(omnetpp::cCommBuffer *b) {
  ::omnetpp::cMessage::parsimUnpack(b);
  doParsimUnpacking(b, this->requesterId);
  doParsimUnpacking(b, this->providerId);
  doParsimUnpacking(b, this->rating);
}

int BlockchainTransaction::getRequesterId() const { return this->requesterId; }

void BlockchainTransaction::setRequesterId(int requesterId) {
  this->requesterId = requesterId;
}

int BlockchainTransaction::getProviderId() const { return this->providerId; }

void BlockchainTransaction::setProviderId(int providerId) {
  this->providerId = providerId;
}

double BlockchainTransaction::getRating() const { return this->rating; }

void BlockchainTransaction::setRating(double rating) { this->rating = rating; }

class BlockchainTransactionDescriptor : public omnetpp::cClassDescriptor {
private:
  mutable const char **propertyNames;
  enum FieldConstants {
    FIELD_requesterId,
    FIELD_providerId,
    FIELD_rating,
  };

public:
  BlockchainTransactionDescriptor();
  virtual ~BlockchainTransactionDescriptor();

  virtual bool doesSupport(omnetpp::cObject *obj) const override;
  virtual const char **getPropertyNames() const override;
  virtual const char *getProperty(const char *propertyName) const override;
  virtual int getFieldCount() const override;
  virtual const char *getFieldName(int field) const override;
  virtual int findField(const char *fieldName) const override;
  virtual unsigned int getFieldTypeFlags(int field) const override;
  virtual const char *getFieldTypeString(int field) const override;
  virtual const char **getFieldPropertyNames(int field) const override;
  virtual const char *getFieldProperty(int field,
                                       const char *propertyName) const override;
  virtual int getFieldArraySize(omnetpp::any_ptr object,
                                int field) const override;
  virtual void setFieldArraySize(omnetpp::any_ptr object, int field,
                                 int size) const override;

  virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object,
                                                int field,
                                                int i) const override;
  virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field,
                                            int i) const override;
  virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i,
                                     const char *value) const override;
  virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field,
                                        int i) const override;
  virtual void setFieldValue(omnetpp::any_ptr object, int field, int i,
                             const omnetpp::cValue &value) const override;

  virtual const char *getFieldStructName(int field) const override;
  virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object,
                                                      int field,
                                                      int i) const override;
  virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field,
                                          int i,
                                          omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(BlockchainTransactionDescriptor)

    BlockchainTransactionDescriptor::BlockchainTransactionDescriptor()
    : omnetpp::cClassDescriptor(
          omnetpp::opp_typename(typeid(BlockchainTransaction)),
          "omnetpp::cMessage") {
  propertyNames = nullptr;
}

BlockchainTransactionDescriptor::~BlockchainTransactionDescriptor() {
  delete[] propertyNames;
}

bool BlockchainTransactionDescriptor::doesSupport(omnetpp::cObject *obj) const {
  return dynamic_cast<BlockchainTransaction *>(obj) != nullptr;
}

const char **BlockchainTransactionDescriptor::getPropertyNames() const {
  if (!propertyNames) {
    static const char *names[] = {nullptr};
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    const char **baseNames = base ? base->getPropertyNames() : nullptr;
    propertyNames = mergeLists(baseNames, names);
  }
  return propertyNames;
}

const char *
BlockchainTransactionDescriptor::getProperty(const char *propertyName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  return base ? base->getProperty(propertyName) : nullptr;
}

int BlockchainTransactionDescriptor::getFieldCount() const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  return base ? 3 + base->getFieldCount() : 3;
}

unsigned int
BlockchainTransactionDescriptor::getFieldTypeFlags(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldTypeFlags(field);
    field -= base->getFieldCount();
  }
  static unsigned int fieldTypeFlags[] = {
      FD_ISEDITABLE, // FIELD_requesterId
      FD_ISEDITABLE, // FIELD_providerId
      FD_ISEDITABLE, // FIELD_rating
  };
  return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *BlockchainTransactionDescriptor::getFieldName(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldName(field);
    field -= base->getFieldCount();
  }
  static const char *fieldNames[] = {
      "requesterId",
      "providerId",
      "rating",
  };
  return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int BlockchainTransactionDescriptor::findField(const char *fieldName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  int baseIndex = base ? base->getFieldCount() : 0;
  if (strcmp(fieldName, "requesterId") == 0)
    return baseIndex + 0;
  if (strcmp(fieldName, "providerId") == 0)
    return baseIndex + 1;
  if (strcmp(fieldName, "rating") == 0)
    return baseIndex + 2;
  return base ? base->findField(fieldName) : -1;
}

const char *
BlockchainTransactionDescriptor::getFieldTypeString(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldTypeString(field);
    field -= base->getFieldCount();
  }
  static const char *fieldTypeStrings[] = {
      "int",    // FIELD_requesterId
      "int",    // FIELD_providerId
      "double", // FIELD_rating
  };
  return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **
BlockchainTransactionDescriptor::getFieldPropertyNames(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldPropertyNames(field);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  }
}

const char *BlockchainTransactionDescriptor::getFieldProperty(
    int field, const char *propertyName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldProperty(field, propertyName);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  }
}

int BlockchainTransactionDescriptor::getFieldArraySize(omnetpp::any_ptr object,
                                                       int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldArraySize(object, field);
    field -= base->getFieldCount();
  }
  BlockchainTransaction *pp =
      omnetpp::fromAnyPtr<BlockchainTransaction>(object);
  (void)pp;
  switch (field) {
  default:
    return 0;
  }
}

void BlockchainTransactionDescriptor::setFieldArraySize(omnetpp::any_ptr object,
                                                        int field,
                                                        int size) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldArraySize(object, field, size);
      return;
    }
    field -= base->getFieldCount();
  }
  BlockchainTransaction *pp =
      omnetpp::fromAnyPtr<BlockchainTransaction>(object);
  (void)pp;
  switch (field) {
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set array size of field %d of class 'BlockchainTransaction'",
        field);
  }
}

const char *BlockchainTransactionDescriptor::getFieldDynamicTypeString(
    omnetpp::any_ptr object, int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldDynamicTypeString(object, field, i);
    field -= base->getFieldCount();
  }
  BlockchainTransaction *pp =
      omnetpp::fromAnyPtr<BlockchainTransaction>(object);
  (void)pp;
  switch (field) {
  default:
    return nullptr;
  }
}

std::string
BlockchainTransactionDescriptor::getFieldValueAsString(omnetpp::any_ptr object,
                                                       int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldValueAsString(object, field, i);
    field -= base->getFieldCount();
  }
  BlockchainTransaction *pp =
      omnetpp::fromAnyPtr<BlockchainTransaction>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    return long2string(pp->getRequesterId());
  case FIELD_providerId:
    return long2string(pp->getProviderId());
  case FIELD_rating:
    return double2string(pp->getRating());
  default:
    return "";
  }
}

void BlockchainTransactionDescriptor::setFieldValueAsString(
    omnetpp::any_ptr object, int field, int i, const char *value) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldValueAsString(object, field, i, value);
      return;
    }
    field -= base->getFieldCount();
  }
  BlockchainTransaction *pp =
      omnetpp::fromAnyPtr<BlockchainTransaction>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    pp->setRequesterId(string2long(value));
    break;
  case FIELD_providerId:
    pp->setProviderId(string2long(value));
    break;
  case FIELD_rating:
    pp->setRating(string2double(value));
    break;
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set field %d of class 'BlockchainTransaction'", field);
  }
}

omnetpp::cValue
BlockchainTransactionDescriptor::getFieldValue(omnetpp::any_ptr object,
                                               int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldValue(object, field, i);
    field -= base->getFieldCount();
  }
  BlockchainTransaction *pp =
      omnetpp::fromAnyPtr<BlockchainTransaction>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    return pp->getRequesterId();
  case FIELD_providerId:
    return pp->getProviderId();
  case FIELD_rating:
    return pp->getRating();
  default:
    throw omnetpp::cRuntimeError(
        "Cannot return field %d of class 'BlockchainTransaction' as cValue -- "
        "field index out of range?",
        field);
  }
}

void BlockchainTransactionDescriptor::setFieldValue(
    omnetpp::any_ptr object, int field, int i,
    const omnetpp::cValue &value) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldValue(object, field, i, value);
      return;
    }
    field -= base->getFieldCount();
  }
  BlockchainTransaction *pp =
      omnetpp::fromAnyPtr<BlockchainTransaction>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    pp->setRequesterId(omnetpp::checked_int_cast<int>(value.intValue()));
    break;
  case FIELD_providerId:
    pp->setProviderId(omnetpp::checked_int_cast<int>(value.intValue()));
    break;
  case FIELD_rating:
    pp->setRating(value.doubleValue());
    break;
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set field %d of class 'BlockchainTransaction'", field);
  }
}

const char *
BlockchainTransactionDescriptor::getFieldStructName(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldStructName(field);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  };
}

omnetpp::any_ptr BlockchainTransactionDescriptor::getFieldStructValuePointer(
    omnetpp::any_ptr object, int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldStructValuePointer(object, field, i);
    field -= base->getFieldCount();
  }
  BlockchainTransaction *pp =
      omnetpp::fromAnyPtr<BlockchainTransaction>(object);
  (void)pp;
  switch (field) {
  default:
    return omnetpp::any_ptr(nullptr);
  }
}

void BlockchainTransactionDescriptor::setFieldStructValuePointer(
    omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldStructValuePointer(object, field, i, ptr);
      return;
    }
    field -= base->getFieldCount();
  }
  BlockchainTransaction *pp =
      omnetpp::fromAnyPtr<BlockchainTransaction>(object);
  (void)pp;
  switch (field) {
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set field %d of class 'BlockchainTransaction'", field);
  }
}

Register_Class(ServiceRequest)

    ServiceRequest::ServiceRequest(const char *name, short kind)
    : ::omnetpp::cMessage(name, kind) {}

ServiceRequest::ServiceRequest(const ServiceRequest &other)
    : ::omnetpp::cMessage(other) {
  copy(other);
}

ServiceRequest::~ServiceRequest() {}

ServiceRequest &ServiceRequest::operator=(const ServiceRequest &other) {
  if (this == &other)
    return *this;
  ::omnetpp::cMessage::operator=(other);
  copy(other);
  return *this;
}

void ServiceRequest::copy(const ServiceRequest &other) {
  this->requesterId = other.requesterId;
  this->providerId = other.providerId;
}

void ServiceRequest::parsimPack(omnetpp::cCommBuffer *b) const {
  ::omnetpp::cMessage::parsimPack(b);
  doParsimPacking(b, this->requesterId);
  doParsimPacking(b, this->providerId);
}

void ServiceRequest::parsimUnpack(omnetpp::cCommBuffer *b) {
  ::omnetpp::cMessage::parsimUnpack(b);
  doParsimUnpacking(b, this->requesterId);
  doParsimUnpacking(b, this->providerId);
}

int ServiceRequest::getRequesterId() const { return this->requesterId; }

void ServiceRequest::setRequesterId(int requesterId) {
  this->requesterId = requesterId;
}

int ServiceRequest::getProviderId() const { return this->providerId; }

void ServiceRequest::setProviderId(int providerId) {
  this->providerId = providerId;
}

class ServiceRequestDescriptor : public omnetpp::cClassDescriptor {
private:
  mutable const char **propertyNames;
  enum FieldConstants {
    FIELD_requesterId,
    FIELD_providerId,
  };

public:
  ServiceRequestDescriptor();
  virtual ~ServiceRequestDescriptor();

  virtual bool doesSupport(omnetpp::cObject *obj) const override;
  virtual const char **getPropertyNames() const override;
  virtual const char *getProperty(const char *propertyName) const override;
  virtual int getFieldCount() const override;
  virtual const char *getFieldName(int field) const override;
  virtual int findField(const char *fieldName) const override;
  virtual unsigned int getFieldTypeFlags(int field) const override;
  virtual const char *getFieldTypeString(int field) const override;
  virtual const char **getFieldPropertyNames(int field) const override;
  virtual const char *getFieldProperty(int field,
                                       const char *propertyName) const override;
  virtual int getFieldArraySize(omnetpp::any_ptr object,
                                int field) const override;
  virtual void setFieldArraySize(omnetpp::any_ptr object, int field,
                                 int size) const override;

  virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object,
                                                int field,
                                                int i) const override;
  virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field,
                                            int i) const override;
  virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i,
                                     const char *value) const override;
  virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field,
                                        int i) const override;
  virtual void setFieldValue(omnetpp::any_ptr object, int field, int i,
                             const omnetpp::cValue &value) const override;

  virtual const char *getFieldStructName(int field) const override;
  virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object,
                                                      int field,
                                                      int i) const override;
  virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field,
                                          int i,
                                          omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ServiceRequestDescriptor)

    ServiceRequestDescriptor::ServiceRequestDescriptor()
    : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ServiceRequest)),
                                "omnetpp::cMessage") {
  propertyNames = nullptr;
}

ServiceRequestDescriptor::~ServiceRequestDescriptor() {
  delete[] propertyNames;
}

bool ServiceRequestDescriptor::doesSupport(omnetpp::cObject *obj) const {
  return dynamic_cast<ServiceRequest *>(obj) != nullptr;
}

const char **ServiceRequestDescriptor::getPropertyNames() const {
  if (!propertyNames) {
    static const char *names[] = {nullptr};
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    const char **baseNames = base ? base->getPropertyNames() : nullptr;
    propertyNames = mergeLists(baseNames, names);
  }
  return propertyNames;
}

const char *
ServiceRequestDescriptor::getProperty(const char *propertyName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  return base ? base->getProperty(propertyName) : nullptr;
}

int ServiceRequestDescriptor::getFieldCount() const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  return base ? 2 + base->getFieldCount() : 2;
}

unsigned int ServiceRequestDescriptor::getFieldTypeFlags(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldTypeFlags(field);
    field -= base->getFieldCount();
  }
  static unsigned int fieldTypeFlags[] = {
      FD_ISEDITABLE, // FIELD_requesterId
      FD_ISEDITABLE, // FIELD_providerId
  };
  return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *ServiceRequestDescriptor::getFieldName(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldName(field);
    field -= base->getFieldCount();
  }
  static const char *fieldNames[] = {
      "requesterId",
      "providerId",
  };
  return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int ServiceRequestDescriptor::findField(const char *fieldName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  int baseIndex = base ? base->getFieldCount() : 0;
  if (strcmp(fieldName, "requesterId") == 0)
    return baseIndex + 0;
  if (strcmp(fieldName, "providerId") == 0)
    return baseIndex + 1;
  return base ? base->findField(fieldName) : -1;
}

const char *ServiceRequestDescriptor::getFieldTypeString(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldTypeString(field);
    field -= base->getFieldCount();
  }
  static const char *fieldTypeStrings[] = {
      "int", // FIELD_requesterId
      "int", // FIELD_providerId
  };
  return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **ServiceRequestDescriptor::getFieldPropertyNames(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldPropertyNames(field);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  }
}

const char *
ServiceRequestDescriptor::getFieldProperty(int field,
                                           const char *propertyName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldProperty(field, propertyName);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  }
}

int ServiceRequestDescriptor::getFieldArraySize(omnetpp::any_ptr object,
                                                int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldArraySize(object, field);
    field -= base->getFieldCount();
  }
  ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object);
  (void)pp;
  switch (field) {
  default:
    return 0;
  }
}

void ServiceRequestDescriptor::setFieldArraySize(omnetpp::any_ptr object,
                                                 int field, int size) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldArraySize(object, field, size);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object);
  (void)pp;
  switch (field) {
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set array size of field %d of class 'ServiceRequest'", field);
  }
}

const char *
ServiceRequestDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object,
                                                    int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldDynamicTypeString(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object);
  (void)pp;
  switch (field) {
  default:
    return nullptr;
  }
}

std::string
ServiceRequestDescriptor::getFieldValueAsString(omnetpp::any_ptr object,
                                                int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldValueAsString(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    return long2string(pp->getRequesterId());
  case FIELD_providerId:
    return long2string(pp->getProviderId());
  default:
    return "";
  }
}

void ServiceRequestDescriptor::setFieldValueAsString(omnetpp::any_ptr object,
                                                     int field, int i,
                                                     const char *value) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldValueAsString(object, field, i, value);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    pp->setRequesterId(string2long(value));
    break;
  case FIELD_providerId:
    pp->setProviderId(string2long(value));
    break;
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set field %d of class 'ServiceRequest'", field);
  }
}

omnetpp::cValue ServiceRequestDescriptor::getFieldValue(omnetpp::any_ptr object,
                                                        int field,
                                                        int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldValue(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    return pp->getRequesterId();
  case FIELD_providerId:
    return pp->getProviderId();
  default:
    throw omnetpp::cRuntimeError(
        "Cannot return field %d of class 'ServiceRequest' as cValue -- field "
        "index out of range?",
        field);
  }
}

void ServiceRequestDescriptor::setFieldValue(
    omnetpp::any_ptr object, int field, int i,
    const omnetpp::cValue &value) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldValue(object, field, i, value);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    pp->setRequesterId(omnetpp::checked_int_cast<int>(value.intValue()));
    break;
  case FIELD_providerId:
    pp->setProviderId(omnetpp::checked_int_cast<int>(value.intValue()));
    break;
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set field %d of class 'ServiceRequest'", field);
  }
}

const char *ServiceRequestDescriptor::getFieldStructName(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldStructName(field);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  };
}

omnetpp::any_ptr
ServiceRequestDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object,
                                                     int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldStructValuePointer(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object);
  (void)pp;
  switch (field) {
  default:
    return omnetpp::any_ptr(nullptr);
  }
}

void ServiceRequestDescriptor::setFieldStructValuePointer(
    omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldStructValuePointer(object, field, i, ptr);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object);
  (void)pp;
  switch (field) {
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set field %d of class 'ServiceRequest'", field);
  }
}

Register_Class(ServiceResponse)

    ServiceResponse::ServiceResponse(const char *name, short kind)
    : ::omnetpp::cMessage(name, kind) {}

ServiceResponse::ServiceResponse(const ServiceResponse &other)
    : ::omnetpp::cMessage(other) {
  copy(other);
}

ServiceResponse::~ServiceResponse() {}

ServiceResponse &ServiceResponse::operator=(const ServiceResponse &other) {
  if (this == &other)
    return *this;
  ::omnetpp::cMessage::operator=(other);
  copy(other);
  return *this;
}

void ServiceResponse::copy(const ServiceResponse &other) {
  this->requesterId = other.requesterId;
  this->providerId = other.providerId;
}

void ServiceResponse::parsimPack(omnetpp::cCommBuffer *b) const {
  ::omnetpp::cMessage::parsimPack(b);
  doParsimPacking(b, this->requesterId);
  doParsimPacking(b, this->providerId);
}

void ServiceResponse::parsimUnpack(omnetpp::cCommBuffer *b) {
  ::omnetpp::cMessage::parsimUnpack(b);
  doParsimUnpacking(b, this->requesterId);
  doParsimUnpacking(b, this->providerId);
}

int ServiceResponse::getRequesterId() const { return this->requesterId; }

void ServiceResponse::setRequesterId(int requesterId) {
  this->requesterId = requesterId;
}

int ServiceResponse::getProviderId() const { return this->providerId; }

void ServiceResponse::setProviderId(int providerId) {
  this->providerId = providerId;
}

class ServiceResponseDescriptor : public omnetpp::cClassDescriptor {
private:
  mutable const char **propertyNames;
  enum FieldConstants {
    FIELD_requesterId,
    FIELD_providerId,
  };

public:
  ServiceResponseDescriptor();
  virtual ~ServiceResponseDescriptor();

  virtual bool doesSupport(omnetpp::cObject *obj) const override;
  virtual const char **getPropertyNames() const override;
  virtual const char *getProperty(const char *propertyName) const override;
  virtual int getFieldCount() const override;
  virtual const char *getFieldName(int field) const override;
  virtual int findField(const char *fieldName) const override;
  virtual unsigned int getFieldTypeFlags(int field) const override;
  virtual const char *getFieldTypeString(int field) const override;
  virtual const char **getFieldPropertyNames(int field) const override;
  virtual const char *getFieldProperty(int field,
                                       const char *propertyName) const override;
  virtual int getFieldArraySize(omnetpp::any_ptr object,
                                int field) const override;
  virtual void setFieldArraySize(omnetpp::any_ptr object, int field,
                                 int size) const override;

  virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object,
                                                int field,
                                                int i) const override;
  virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field,
                                            int i) const override;
  virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i,
                                     const char *value) const override;
  virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field,
                                        int i) const override;
  virtual void setFieldValue(omnetpp::any_ptr object, int field, int i,
                             const omnetpp::cValue &value) const override;

  virtual const char *getFieldStructName(int field) const override;
  virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object,
                                                      int field,
                                                      int i) const override;
  virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field,
                                          int i,
                                          omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ServiceResponseDescriptor)

    ServiceResponseDescriptor::ServiceResponseDescriptor()
    : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ServiceResponse)),
                                "omnetpp::cMessage") {
  propertyNames = nullptr;
}

ServiceResponseDescriptor::~ServiceResponseDescriptor() {
  delete[] propertyNames;
}

bool ServiceResponseDescriptor::doesSupport(omnetpp::cObject *obj) const {
  return dynamic_cast<ServiceResponse *>(obj) != nullptr;
}

const char **ServiceResponseDescriptor::getPropertyNames() const {
  if (!propertyNames) {
    static const char *names[] = {nullptr};
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    const char **baseNames = base ? base->getPropertyNames() : nullptr;
    propertyNames = mergeLists(baseNames, names);
  }
  return propertyNames;
}

const char *
ServiceResponseDescriptor::getProperty(const char *propertyName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  return base ? base->getProperty(propertyName) : nullptr;
}

int ServiceResponseDescriptor::getFieldCount() const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  return base ? 2 + base->getFieldCount() : 2;
}

unsigned int ServiceResponseDescriptor::getFieldTypeFlags(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldTypeFlags(field);
    field -= base->getFieldCount();
  }
  static unsigned int fieldTypeFlags[] = {
      FD_ISEDITABLE, // FIELD_requesterId
      FD_ISEDITABLE, // FIELD_providerId
  };
  return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *ServiceResponseDescriptor::getFieldName(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldName(field);
    field -= base->getFieldCount();
  }
  static const char *fieldNames[] = {
      "requesterId",
      "providerId",
  };
  return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int ServiceResponseDescriptor::findField(const char *fieldName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  int baseIndex = base ? base->getFieldCount() : 0;
  if (strcmp(fieldName, "requesterId") == 0)
    return baseIndex + 0;
  if (strcmp(fieldName, "providerId") == 0)
    return baseIndex + 1;
  return base ? base->findField(fieldName) : -1;
}

const char *ServiceResponseDescriptor::getFieldTypeString(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldTypeString(field);
    field -= base->getFieldCount();
  }
  static const char *fieldTypeStrings[] = {
      "int", // FIELD_requesterId
      "int", // FIELD_providerId
  };
  return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **ServiceResponseDescriptor::getFieldPropertyNames(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldPropertyNames(field);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  }
}

const char *
ServiceResponseDescriptor::getFieldProperty(int field,
                                            const char *propertyName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldProperty(field, propertyName);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  }
}

int ServiceResponseDescriptor::getFieldArraySize(omnetpp::any_ptr object,
                                                 int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldArraySize(object, field);
    field -= base->getFieldCount();
  }
  ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object);
  (void)pp;
  switch (field) {
  default:
    return 0;
  }
}

void ServiceResponseDescriptor::setFieldArraySize(omnetpp::any_ptr object,
                                                  int field, int size) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldArraySize(object, field, size);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object);
  (void)pp;
  switch (field) {
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set array size of field %d of class 'ServiceResponse'", field);
  }
}

const char *
ServiceResponseDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object,
                                                     int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldDynamicTypeString(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object);
  (void)pp;
  switch (field) {
  default:
    return nullptr;
  }
}

std::string
ServiceResponseDescriptor::getFieldValueAsString(omnetpp::any_ptr object,
                                                 int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldValueAsString(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    return long2string(pp->getRequesterId());
  case FIELD_providerId:
    return long2string(pp->getProviderId());
  default:
    return "";
  }
}

void ServiceResponseDescriptor::setFieldValueAsString(omnetpp::any_ptr object,
                                                      int field, int i,
                                                      const char *value) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldValueAsString(object, field, i, value);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    pp->setRequesterId(string2long(value));
    break;
  case FIELD_providerId:
    pp->setProviderId(string2long(value));
    break;
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set field %d of class 'ServiceResponse'", field);
  }
}

omnetpp::cValue
ServiceResponseDescriptor::getFieldValue(omnetpp::any_ptr object, int field,
                                         int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldValue(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    return pp->getRequesterId();
  case FIELD_providerId:
    return pp->getProviderId();
  default:
    throw omnetpp::cRuntimeError(
        "Cannot return field %d of class 'ServiceResponse' as cValue -- field "
        "index out of range?",
        field);
  }
}

void ServiceResponseDescriptor::setFieldValue(
    omnetpp::any_ptr object, int field, int i,
    const omnetpp::cValue &value) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldValue(object, field, i, value);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    pp->setRequesterId(omnetpp::checked_int_cast<int>(value.intValue()));
    break;
  case FIELD_providerId:
    pp->setProviderId(omnetpp::checked_int_cast<int>(value.intValue()));
    break;
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set field %d of class 'ServiceResponse'", field);
  }
}

const char *ServiceResponseDescriptor::getFieldStructName(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldStructName(field);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  };
}

omnetpp::any_ptr
ServiceResponseDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object,
                                                      int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldStructValuePointer(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object);
  (void)pp;
  switch (field) {
  default:
    return omnetpp::any_ptr(nullptr);
  }
}

void ServiceResponseDescriptor::setFieldStructValuePointer(
    omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldStructValuePointer(object, field, i, ptr);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object);
  (void)pp;
  switch (field) {
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set field %d of class 'ServiceResponse'", field);
  }
}

Register_Class(ServiceRating)

    ServiceRating::ServiceRating(const char *name, short kind)
    : ::omnetpp::cMessage(name, kind) {}

ServiceRating::ServiceRating(const ServiceRating &other)
    : ::omnetpp::cMessage(other) {
  copy(other);
}

ServiceRating::~ServiceRating() {}

ServiceRating &ServiceRating::operator=(const ServiceRating &other) {
  if (this == &other)
    return *this;
  ::omnetpp::cMessage::operator=(other);
  copy(other);
  return *this;
}

void ServiceRating::copy(const ServiceRating &other) {
  this->requesterId = other.requesterId;
  this->providerId = other.providerId;
  this->rating = other.rating;
}

void ServiceRating::parsimPack(omnetpp::cCommBuffer *b) const {
  ::omnetpp::cMessage::parsimPack(b);
  doParsimPacking(b, this->requesterId);
  doParsimPacking(b, this->providerId);
  doParsimPacking(b, this->rating);
}

void ServiceRating::parsimUnpack(omnetpp::cCommBuffer *b) {
  ::omnetpp::cMessage::parsimUnpack(b);
  doParsimUnpacking(b, this->requesterId);
  doParsimUnpacking(b, this->providerId);
  doParsimUnpacking(b, this->rating);
}

int ServiceRating::getRequesterId() const { return this->requesterId; }

void ServiceRating::setRequesterId(int requesterId) {
  this->requesterId = requesterId;
}

int ServiceRating::getProviderId() const { return this->providerId; }

void ServiceRating::setProviderId(int providerId) {
  this->providerId = providerId;
}

double ServiceRating::getRating() const { return this->rating; }

void ServiceRating::setRating(double rating) { this->rating = rating; }

class ServiceRatingDescriptor : public omnetpp::cClassDescriptor {
private:
  mutable const char **propertyNames;
  enum FieldConstants {
    FIELD_requesterId,
    FIELD_providerId,
    FIELD_rating,
  };

public:
  ServiceRatingDescriptor();
  virtual ~ServiceRatingDescriptor();

  virtual bool doesSupport(omnetpp::cObject *obj) const override;
  virtual const char **getPropertyNames() const override;
  virtual const char *getProperty(const char *propertyName) const override;
  virtual int getFieldCount() const override;
  virtual const char *getFieldName(int field) const override;
  virtual int findField(const char *fieldName) const override;
  virtual unsigned int getFieldTypeFlags(int field) const override;
  virtual const char *getFieldTypeString(int field) const override;
  virtual const char **getFieldPropertyNames(int field) const override;
  virtual const char *getFieldProperty(int field,
                                       const char *propertyName) const override;
  virtual int getFieldArraySize(omnetpp::any_ptr object,
                                int field) const override;
  virtual void setFieldArraySize(omnetpp::any_ptr object, int field,
                                 int size) const override;

  virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object,
                                                int field,
                                                int i) const override;
  virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field,
                                            int i) const override;
  virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i,
                                     const char *value) const override;
  virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field,
                                        int i) const override;
  virtual void setFieldValue(omnetpp::any_ptr object, int field, int i,
                             const omnetpp::cValue &value) const override;

  virtual const char *getFieldStructName(int field) const override;
  virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object,
                                                      int field,
                                                      int i) const override;
  virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field,
                                          int i,
                                          omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ServiceRatingDescriptor)

    ServiceRatingDescriptor::ServiceRatingDescriptor()
    : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ServiceRating)),
                                "omnetpp::cMessage") {
  propertyNames = nullptr;
}

ServiceRatingDescriptor::~ServiceRatingDescriptor() { delete[] propertyNames; }

bool ServiceRatingDescriptor::doesSupport(omnetpp::cObject *obj) const {
  return dynamic_cast<ServiceRating *>(obj) != nullptr;
}

const char **ServiceRatingDescriptor::getPropertyNames() const {
  if (!propertyNames) {
    static const char *names[] = {nullptr};
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    const char **baseNames = base ? base->getPropertyNames() : nullptr;
    propertyNames = mergeLists(baseNames, names);
  }
  return propertyNames;
}

const char *
ServiceRatingDescriptor::getProperty(const char *propertyName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  return base ? base->getProperty(propertyName) : nullptr;
}

int ServiceRatingDescriptor::getFieldCount() const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  return base ? 3 + base->getFieldCount() : 3;
}

unsigned int ServiceRatingDescriptor::getFieldTypeFlags(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldTypeFlags(field);
    field -= base->getFieldCount();
  }
  static unsigned int fieldTypeFlags[] = {
      FD_ISEDITABLE, // FIELD_requesterId
      FD_ISEDITABLE, // FIELD_providerId
      FD_ISEDITABLE, // FIELD_rating
  };
  return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *ServiceRatingDescriptor::getFieldName(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldName(field);
    field -= base->getFieldCount();
  }
  static const char *fieldNames[] = {
      "requesterId",
      "providerId",
      "rating",
  };
  return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int ServiceRatingDescriptor::findField(const char *fieldName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  int baseIndex = base ? base->getFieldCount() : 0;
  if (strcmp(fieldName, "requesterId") == 0)
    return baseIndex + 0;
  if (strcmp(fieldName, "providerId") == 0)
    return baseIndex + 1;
  if (strcmp(fieldName, "rating") == 0)
    return baseIndex + 2;
  return base ? base->findField(fieldName) : -1;
}

const char *ServiceRatingDescriptor::getFieldTypeString(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldTypeString(field);
    field -= base->getFieldCount();
  }
  static const char *fieldTypeStrings[] = {
      "int",    // FIELD_requesterId
      "int",    // FIELD_providerId
      "double", // FIELD_rating
  };
  return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **ServiceRatingDescriptor::getFieldPropertyNames(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldPropertyNames(field);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  }
}

const char *
ServiceRatingDescriptor::getFieldProperty(int field,
                                          const char *propertyName) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldProperty(field, propertyName);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  }
}

int ServiceRatingDescriptor::getFieldArraySize(omnetpp::any_ptr object,
                                               int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldArraySize(object, field);
    field -= base->getFieldCount();
  }
  ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object);
  (void)pp;
  switch (field) {
  default:
    return 0;
  }
}

void ServiceRatingDescriptor::setFieldArraySize(omnetpp::any_ptr object,
                                                int field, int size) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldArraySize(object, field, size);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object);
  (void)pp;
  switch (field) {
  default:
    throw omnetpp::cRuntimeError(
        "Cannot set array size of field %d of class 'ServiceRating'", field);
  }
}

const char *
ServiceRatingDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object,
                                                   int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldDynamicTypeString(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object);
  (void)pp;
  switch (field) {
  default:
    return nullptr;
  }
}

std::string
ServiceRatingDescriptor::getFieldValueAsString(omnetpp::any_ptr object,
                                               int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldValueAsString(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    return long2string(pp->getRequesterId());
  case FIELD_providerId:
    return long2string(pp->getProviderId());
  case FIELD_rating:
    return double2string(pp->getRating());
  default:
    return "";
  }
}

void ServiceRatingDescriptor::setFieldValueAsString(omnetpp::any_ptr object,
                                                    int field, int i,
                                                    const char *value) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldValueAsString(object, field, i, value);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    pp->setRequesterId(string2long(value));
    break;
  case FIELD_providerId:
    pp->setProviderId(string2long(value));
    break;
  case FIELD_rating:
    pp->setRating(string2double(value));
    break;
  default:
    throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceRating'",
                                 field);
  }
}

omnetpp::cValue ServiceRatingDescriptor::getFieldValue(omnetpp::any_ptr object,
                                                       int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldValue(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    return pp->getRequesterId();
  case FIELD_providerId:
    return pp->getProviderId();
  case FIELD_rating:
    return pp->getRating();
  default:
    throw omnetpp::cRuntimeError(
        "Cannot return field %d of class 'ServiceRating' as cValue -- field "
        "index out of range?",
        field);
  }
}

void ServiceRatingDescriptor::setFieldValue(
    omnetpp::any_ptr object, int field, int i,
    const omnetpp::cValue &value) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldValue(object, field, i, value);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object);
  (void)pp;
  switch (field) {
  case FIELD_requesterId:
    pp->setRequesterId(omnetpp::checked_int_cast<int>(value.intValue()));
    break;
  case FIELD_providerId:
    pp->setProviderId(omnetpp::checked_int_cast<int>(value.intValue()));
    break;
  case FIELD_rating:
    pp->setRating(value.doubleValue());
    break;
  default:
    throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceRating'",
                                 field);
  }
}

const char *ServiceRatingDescriptor::getFieldStructName(int field) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldStructName(field);
    field -= base->getFieldCount();
  }
  switch (field) {
  default:
    return nullptr;
  };
}

omnetpp::any_ptr
ServiceRatingDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object,
                                                    int field, int i) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount())
      return base->getFieldStructValuePointer(object, field, i);
    field -= base->getFieldCount();
  }
  ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object);
  (void)pp;
  switch (field) {
  default:
    return omnetpp::any_ptr(nullptr);
  }
}

void ServiceRatingDescriptor::setFieldStructValuePointer(
    omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const {
  omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
  if (base) {
    if (field < base->getFieldCount()) {
      base->setFieldStructValuePointer(object, field, i, ptr);
      return;
    }
    field -= base->getFieldCount();
  }
  ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object);
  (void)pp;
  switch (field) {
  default:
    throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceRating'",
                                 field);
  }
}

namespace omnetpp {} // namespace omnetpp
