//
// Generated file, do not edit! Created by opp_msgtool 6.0 from src/BlockchainMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "BlockchainMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(BlockchainTransaction)

BlockchainTransaction::BlockchainTransaction(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

BlockchainTransaction::BlockchainTransaction(const BlockchainTransaction& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

BlockchainTransaction::~BlockchainTransaction()
{
}

BlockchainTransaction& BlockchainTransaction::operator=(const BlockchainTransaction& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void BlockchainTransaction::copy(const BlockchainTransaction& other)
{
    this->requesterId = other.requesterId;
    this->providerId = other.providerId;
    this->rating = other.rating;
}

void BlockchainTransaction::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->requesterId);
    doParsimPacking(b,this->providerId);
    doParsimPacking(b,this->rating);
}

void BlockchainTransaction::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->requesterId);
    doParsimUnpacking(b,this->providerId);
    doParsimUnpacking(b,this->rating);
}

int BlockchainTransaction::getRequesterId() const
{
    return this->requesterId;
}

void BlockchainTransaction::setRequesterId(int requesterId)
{
    this->requesterId = requesterId;
}

int BlockchainTransaction::getProviderId() const
{
    return this->providerId;
}

void BlockchainTransaction::setProviderId(int providerId)
{
    this->providerId = providerId;
}

double BlockchainTransaction::getRating() const
{
    return this->rating;
}

void BlockchainTransaction::setRating(double rating)
{
    this->rating = rating;
}

class BlockchainTransactionDescriptor : public omnetpp::cClassDescriptor
{
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
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(BlockchainTransactionDescriptor)

BlockchainTransactionDescriptor::BlockchainTransactionDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(BlockchainTransaction)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

BlockchainTransactionDescriptor::~BlockchainTransactionDescriptor()
{
    delete[] propertyNames;
}

bool BlockchainTransactionDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<BlockchainTransaction *>(obj)!=nullptr;
}

const char **BlockchainTransactionDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *BlockchainTransactionDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int BlockchainTransactionDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int BlockchainTransactionDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_requesterId
        FD_ISEDITABLE,    // FIELD_providerId
        FD_ISEDITABLE,    // FIELD_rating
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *BlockchainTransactionDescriptor::getFieldName(int field) const
{
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

int BlockchainTransactionDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "requesterId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "providerId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "rating") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *BlockchainTransactionDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_requesterId
        "int",    // FIELD_providerId
        "double",    // FIELD_rating
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **BlockchainTransactionDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *BlockchainTransactionDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int BlockchainTransactionDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    BlockchainTransaction *pp = omnetpp::fromAnyPtr<BlockchainTransaction>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void BlockchainTransactionDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    BlockchainTransaction *pp = omnetpp::fromAnyPtr<BlockchainTransaction>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'BlockchainTransaction'", field);
    }
}

const char *BlockchainTransactionDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    BlockchainTransaction *pp = omnetpp::fromAnyPtr<BlockchainTransaction>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string BlockchainTransactionDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    BlockchainTransaction *pp = omnetpp::fromAnyPtr<BlockchainTransaction>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: return long2string(pp->getRequesterId());
        case FIELD_providerId: return long2string(pp->getProviderId());
        case FIELD_rating: return double2string(pp->getRating());
        default: return "";
    }
}

void BlockchainTransactionDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    BlockchainTransaction *pp = omnetpp::fromAnyPtr<BlockchainTransaction>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: pp->setRequesterId(string2long(value)); break;
        case FIELD_providerId: pp->setProviderId(string2long(value)); break;
        case FIELD_rating: pp->setRating(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'BlockchainTransaction'", field);
    }
}

omnetpp::cValue BlockchainTransactionDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    BlockchainTransaction *pp = omnetpp::fromAnyPtr<BlockchainTransaction>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: return pp->getRequesterId();
        case FIELD_providerId: return pp->getProviderId();
        case FIELD_rating: return pp->getRating();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'BlockchainTransaction' as cValue -- field index out of range?", field);
    }
}

void BlockchainTransactionDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    BlockchainTransaction *pp = omnetpp::fromAnyPtr<BlockchainTransaction>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: pp->setRequesterId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_providerId: pp->setProviderId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_rating: pp->setRating(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'BlockchainTransaction'", field);
    }
}

const char *BlockchainTransactionDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr BlockchainTransactionDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    BlockchainTransaction *pp = omnetpp::fromAnyPtr<BlockchainTransaction>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void BlockchainTransactionDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    BlockchainTransaction *pp = omnetpp::fromAnyPtr<BlockchainTransaction>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'BlockchainTransaction'", field);
    }
}

Register_Class(ServiceRequest)

ServiceRequest::ServiceRequest(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

ServiceRequest::ServiceRequest(const ServiceRequest& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ServiceRequest::~ServiceRequest()
{
}

ServiceRequest& ServiceRequest::operator=(const ServiceRequest& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ServiceRequest::copy(const ServiceRequest& other)
{
    this->requesterId = other.requesterId;
    this->providerId = other.providerId;
    this->serviceType = other.serviceType;
}

void ServiceRequest::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->requesterId);
    doParsimPacking(b,this->providerId);
    doParsimPacking(b,this->serviceType);
}

void ServiceRequest::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->requesterId);
    doParsimUnpacking(b,this->providerId);
    doParsimUnpacking(b,this->serviceType);
}

int ServiceRequest::getRequesterId() const
{
    return this->requesterId;
}

void ServiceRequest::setRequesterId(int requesterId)
{
    this->requesterId = requesterId;
}

int ServiceRequest::getProviderId() const
{
    return this->providerId;
}

void ServiceRequest::setProviderId(int providerId)
{
    this->providerId = providerId;
}

const char * ServiceRequest::getServiceType() const
{
    return this->serviceType.c_str();
}

void ServiceRequest::setServiceType(const char * serviceType)
{
    this->serviceType = serviceType;
}

class ServiceRequestDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_requesterId,
        FIELD_providerId,
        FIELD_serviceType,
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
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ServiceRequestDescriptor)

ServiceRequestDescriptor::ServiceRequestDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ServiceRequest)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

ServiceRequestDescriptor::~ServiceRequestDescriptor()
{
    delete[] propertyNames;
}

bool ServiceRequestDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ServiceRequest *>(obj)!=nullptr;
}

const char **ServiceRequestDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ServiceRequestDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ServiceRequestDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int ServiceRequestDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_requesterId
        FD_ISEDITABLE,    // FIELD_providerId
        FD_ISEDITABLE,    // FIELD_serviceType
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *ServiceRequestDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "requesterId",
        "providerId",
        "serviceType",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int ServiceRequestDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "requesterId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "providerId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "serviceType") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *ServiceRequestDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_requesterId
        "int",    // FIELD_providerId
        "string",    // FIELD_serviceType
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **ServiceRequestDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ServiceRequestDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ServiceRequestDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ServiceRequestDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ServiceRequest'", field);
    }
}

const char *ServiceRequestDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ServiceRequestDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: return long2string(pp->getRequesterId());
        case FIELD_providerId: return long2string(pp->getProviderId());
        case FIELD_serviceType: return oppstring2string(pp->getServiceType());
        default: return "";
    }
}

void ServiceRequestDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: pp->setRequesterId(string2long(value)); break;
        case FIELD_providerId: pp->setProviderId(string2long(value)); break;
        case FIELD_serviceType: pp->setServiceType((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceRequest'", field);
    }
}

omnetpp::cValue ServiceRequestDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: return pp->getRequesterId();
        case FIELD_providerId: return pp->getProviderId();
        case FIELD_serviceType: return pp->getServiceType();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ServiceRequest' as cValue -- field index out of range?", field);
    }
}

void ServiceRequestDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: pp->setRequesterId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_providerId: pp->setProviderId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_serviceType: pp->setServiceType(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceRequest'", field);
    }
}

const char *ServiceRequestDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ServiceRequestDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ServiceRequestDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceRequest *pp = omnetpp::fromAnyPtr<ServiceRequest>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceRequest'", field);
    }
}

Register_Class(ServiceResponse)

ServiceResponse::ServiceResponse(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

ServiceResponse::ServiceResponse(const ServiceResponse& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ServiceResponse::~ServiceResponse()
{
}

ServiceResponse& ServiceResponse::operator=(const ServiceResponse& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ServiceResponse::copy(const ServiceResponse& other)
{
    this->requesterId = other.requesterId;
    this->providerId = other.providerId;
    this->serviceType = other.serviceType;
}

void ServiceResponse::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->requesterId);
    doParsimPacking(b,this->providerId);
    doParsimPacking(b,this->serviceType);
}

void ServiceResponse::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->requesterId);
    doParsimUnpacking(b,this->providerId);
    doParsimUnpacking(b,this->serviceType);
}

int ServiceResponse::getRequesterId() const
{
    return this->requesterId;
}

void ServiceResponse::setRequesterId(int requesterId)
{
    this->requesterId = requesterId;
}

int ServiceResponse::getProviderId() const
{
    return this->providerId;
}

void ServiceResponse::setProviderId(int providerId)
{
    this->providerId = providerId;
}

const char * ServiceResponse::getServiceType() const
{
    return this->serviceType.c_str();
}

void ServiceResponse::setServiceType(const char * serviceType)
{
    this->serviceType = serviceType;
}

class ServiceResponseDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_requesterId,
        FIELD_providerId,
        FIELD_serviceType,
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
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ServiceResponseDescriptor)

ServiceResponseDescriptor::ServiceResponseDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ServiceResponse)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

ServiceResponseDescriptor::~ServiceResponseDescriptor()
{
    delete[] propertyNames;
}

bool ServiceResponseDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ServiceResponse *>(obj)!=nullptr;
}

const char **ServiceResponseDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ServiceResponseDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ServiceResponseDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int ServiceResponseDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_requesterId
        FD_ISEDITABLE,    // FIELD_providerId
        FD_ISEDITABLE,    // FIELD_serviceType
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *ServiceResponseDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "requesterId",
        "providerId",
        "serviceType",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int ServiceResponseDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "requesterId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "providerId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "serviceType") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *ServiceResponseDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_requesterId
        "int",    // FIELD_providerId
        "string",    // FIELD_serviceType
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **ServiceResponseDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ServiceResponseDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ServiceResponseDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ServiceResponseDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ServiceResponse'", field);
    }
}

const char *ServiceResponseDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ServiceResponseDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: return long2string(pp->getRequesterId());
        case FIELD_providerId: return long2string(pp->getProviderId());
        case FIELD_serviceType: return oppstring2string(pp->getServiceType());
        default: return "";
    }
}

void ServiceResponseDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: pp->setRequesterId(string2long(value)); break;
        case FIELD_providerId: pp->setProviderId(string2long(value)); break;
        case FIELD_serviceType: pp->setServiceType((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceResponse'", field);
    }
}

omnetpp::cValue ServiceResponseDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: return pp->getRequesterId();
        case FIELD_providerId: return pp->getProviderId();
        case FIELD_serviceType: return pp->getServiceType();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ServiceResponse' as cValue -- field index out of range?", field);
    }
}

void ServiceResponseDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: pp->setRequesterId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_providerId: pp->setProviderId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_serviceType: pp->setServiceType(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceResponse'", field);
    }
}

const char *ServiceResponseDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ServiceResponseDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ServiceResponseDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceResponse *pp = omnetpp::fromAnyPtr<ServiceResponse>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceResponse'", field);
    }
}

Register_Class(FinalServiceRequest)

FinalServiceRequest::FinalServiceRequest(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

FinalServiceRequest::FinalServiceRequest(const FinalServiceRequest& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

FinalServiceRequest::~FinalServiceRequest()
{
}

FinalServiceRequest& FinalServiceRequest::operator=(const FinalServiceRequest& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void FinalServiceRequest::copy(const FinalServiceRequest& other)
{
    this->requesterId = other.requesterId;
    this->providerId = other.providerId;
    this->serviceType = other.serviceType;
}

void FinalServiceRequest::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->requesterId);
    doParsimPacking(b,this->providerId);
    doParsimPacking(b,this->serviceType);
}

void FinalServiceRequest::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->requesterId);
    doParsimUnpacking(b,this->providerId);
    doParsimUnpacking(b,this->serviceType);
}

int FinalServiceRequest::getRequesterId() const
{
    return this->requesterId;
}

void FinalServiceRequest::setRequesterId(int requesterId)
{
    this->requesterId = requesterId;
}

int FinalServiceRequest::getProviderId() const
{
    return this->providerId;
}

void FinalServiceRequest::setProviderId(int providerId)
{
    this->providerId = providerId;
}

const char * FinalServiceRequest::getServiceType() const
{
    return this->serviceType.c_str();
}

void FinalServiceRequest::setServiceType(const char * serviceType)
{
    this->serviceType = serviceType;
}

class FinalServiceRequestDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_requesterId,
        FIELD_providerId,
        FIELD_serviceType,
    };
  public:
    FinalServiceRequestDescriptor();
    virtual ~FinalServiceRequestDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(FinalServiceRequestDescriptor)

FinalServiceRequestDescriptor::FinalServiceRequestDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(FinalServiceRequest)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

FinalServiceRequestDescriptor::~FinalServiceRequestDescriptor()
{
    delete[] propertyNames;
}

bool FinalServiceRequestDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<FinalServiceRequest *>(obj)!=nullptr;
}

const char **FinalServiceRequestDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *FinalServiceRequestDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int FinalServiceRequestDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int FinalServiceRequestDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_requesterId
        FD_ISEDITABLE,    // FIELD_providerId
        FD_ISEDITABLE,    // FIELD_serviceType
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *FinalServiceRequestDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "requesterId",
        "providerId",
        "serviceType",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int FinalServiceRequestDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "requesterId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "providerId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "serviceType") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *FinalServiceRequestDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_requesterId
        "int",    // FIELD_providerId
        "string",    // FIELD_serviceType
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **FinalServiceRequestDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *FinalServiceRequestDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int FinalServiceRequestDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    FinalServiceRequest *pp = omnetpp::fromAnyPtr<FinalServiceRequest>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void FinalServiceRequestDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    FinalServiceRequest *pp = omnetpp::fromAnyPtr<FinalServiceRequest>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'FinalServiceRequest'", field);
    }
}

const char *FinalServiceRequestDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    FinalServiceRequest *pp = omnetpp::fromAnyPtr<FinalServiceRequest>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string FinalServiceRequestDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    FinalServiceRequest *pp = omnetpp::fromAnyPtr<FinalServiceRequest>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: return long2string(pp->getRequesterId());
        case FIELD_providerId: return long2string(pp->getProviderId());
        case FIELD_serviceType: return oppstring2string(pp->getServiceType());
        default: return "";
    }
}

void FinalServiceRequestDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    FinalServiceRequest *pp = omnetpp::fromAnyPtr<FinalServiceRequest>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: pp->setRequesterId(string2long(value)); break;
        case FIELD_providerId: pp->setProviderId(string2long(value)); break;
        case FIELD_serviceType: pp->setServiceType((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FinalServiceRequest'", field);
    }
}

omnetpp::cValue FinalServiceRequestDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    FinalServiceRequest *pp = omnetpp::fromAnyPtr<FinalServiceRequest>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: return pp->getRequesterId();
        case FIELD_providerId: return pp->getProviderId();
        case FIELD_serviceType: return pp->getServiceType();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'FinalServiceRequest' as cValue -- field index out of range?", field);
    }
}

void FinalServiceRequestDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    FinalServiceRequest *pp = omnetpp::fromAnyPtr<FinalServiceRequest>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: pp->setRequesterId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_providerId: pp->setProviderId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_serviceType: pp->setServiceType(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FinalServiceRequest'", field);
    }
}

const char *FinalServiceRequestDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr FinalServiceRequestDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    FinalServiceRequest *pp = omnetpp::fromAnyPtr<FinalServiceRequest>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void FinalServiceRequestDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    FinalServiceRequest *pp = omnetpp::fromAnyPtr<FinalServiceRequest>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FinalServiceRequest'", field);
    }
}

Register_Class(FinalServiceResponse)

FinalServiceResponse::FinalServiceResponse(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

FinalServiceResponse::FinalServiceResponse(const FinalServiceResponse& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

FinalServiceResponse::~FinalServiceResponse()
{
}

FinalServiceResponse& FinalServiceResponse::operator=(const FinalServiceResponse& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void FinalServiceResponse::copy(const FinalServiceResponse& other)
{
    this->requesterId = other.requesterId;
    this->providerId = other.providerId;
    this->serviceType = other.serviceType;
    this->serviceQuality = other.serviceQuality;
}

void FinalServiceResponse::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->requesterId);
    doParsimPacking(b,this->providerId);
    doParsimPacking(b,this->serviceType);
    doParsimPacking(b,this->serviceQuality);
}

void FinalServiceResponse::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->requesterId);
    doParsimUnpacking(b,this->providerId);
    doParsimUnpacking(b,this->serviceType);
    doParsimUnpacking(b,this->serviceQuality);
}

int FinalServiceResponse::getRequesterId() const
{
    return this->requesterId;
}

void FinalServiceResponse::setRequesterId(int requesterId)
{
    this->requesterId = requesterId;
}

int FinalServiceResponse::getProviderId() const
{
    return this->providerId;
}

void FinalServiceResponse::setProviderId(int providerId)
{
    this->providerId = providerId;
}

const char * FinalServiceResponse::getServiceType() const
{
    return this->serviceType.c_str();
}

void FinalServiceResponse::setServiceType(const char * serviceType)
{
    this->serviceType = serviceType;
}

double FinalServiceResponse::getServiceQuality() const
{
    return this->serviceQuality;
}

void FinalServiceResponse::setServiceQuality(double serviceQuality)
{
    this->serviceQuality = serviceQuality;
}

class FinalServiceResponseDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_requesterId,
        FIELD_providerId,
        FIELD_serviceType,
        FIELD_serviceQuality,
    };
  public:
    FinalServiceResponseDescriptor();
    virtual ~FinalServiceResponseDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(FinalServiceResponseDescriptor)

FinalServiceResponseDescriptor::FinalServiceResponseDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(FinalServiceResponse)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

FinalServiceResponseDescriptor::~FinalServiceResponseDescriptor()
{
    delete[] propertyNames;
}

bool FinalServiceResponseDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<FinalServiceResponse *>(obj)!=nullptr;
}

const char **FinalServiceResponseDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *FinalServiceResponseDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int FinalServiceResponseDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int FinalServiceResponseDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_requesterId
        FD_ISEDITABLE,    // FIELD_providerId
        FD_ISEDITABLE,    // FIELD_serviceType
        FD_ISEDITABLE,    // FIELD_serviceQuality
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *FinalServiceResponseDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "requesterId",
        "providerId",
        "serviceType",
        "serviceQuality",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int FinalServiceResponseDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "requesterId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "providerId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "serviceType") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "serviceQuality") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *FinalServiceResponseDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_requesterId
        "int",    // FIELD_providerId
        "string",    // FIELD_serviceType
        "double",    // FIELD_serviceQuality
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **FinalServiceResponseDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *FinalServiceResponseDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int FinalServiceResponseDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    FinalServiceResponse *pp = omnetpp::fromAnyPtr<FinalServiceResponse>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void FinalServiceResponseDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    FinalServiceResponse *pp = omnetpp::fromAnyPtr<FinalServiceResponse>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'FinalServiceResponse'", field);
    }
}

const char *FinalServiceResponseDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    FinalServiceResponse *pp = omnetpp::fromAnyPtr<FinalServiceResponse>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string FinalServiceResponseDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    FinalServiceResponse *pp = omnetpp::fromAnyPtr<FinalServiceResponse>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: return long2string(pp->getRequesterId());
        case FIELD_providerId: return long2string(pp->getProviderId());
        case FIELD_serviceType: return oppstring2string(pp->getServiceType());
        case FIELD_serviceQuality: return double2string(pp->getServiceQuality());
        default: return "";
    }
}

void FinalServiceResponseDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    FinalServiceResponse *pp = omnetpp::fromAnyPtr<FinalServiceResponse>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: pp->setRequesterId(string2long(value)); break;
        case FIELD_providerId: pp->setProviderId(string2long(value)); break;
        case FIELD_serviceType: pp->setServiceType((value)); break;
        case FIELD_serviceQuality: pp->setServiceQuality(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FinalServiceResponse'", field);
    }
}

omnetpp::cValue FinalServiceResponseDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    FinalServiceResponse *pp = omnetpp::fromAnyPtr<FinalServiceResponse>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: return pp->getRequesterId();
        case FIELD_providerId: return pp->getProviderId();
        case FIELD_serviceType: return pp->getServiceType();
        case FIELD_serviceQuality: return pp->getServiceQuality();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'FinalServiceResponse' as cValue -- field index out of range?", field);
    }
}

void FinalServiceResponseDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    FinalServiceResponse *pp = omnetpp::fromAnyPtr<FinalServiceResponse>(object); (void)pp;
    switch (field) {
        case FIELD_requesterId: pp->setRequesterId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_providerId: pp->setProviderId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_serviceType: pp->setServiceType(value.stringValue()); break;
        case FIELD_serviceQuality: pp->setServiceQuality(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FinalServiceResponse'", field);
    }
}

const char *FinalServiceResponseDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr FinalServiceResponseDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    FinalServiceResponse *pp = omnetpp::fromAnyPtr<FinalServiceResponse>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void FinalServiceResponseDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    FinalServiceResponse *pp = omnetpp::fromAnyPtr<FinalServiceResponse>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FinalServiceResponse'", field);
    }
}

Register_Class(ServiceRating)

ServiceRating::ServiceRating(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

ServiceRating::ServiceRating(const ServiceRating& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ServiceRating::~ServiceRating()
{
}

ServiceRating& ServiceRating::operator=(const ServiceRating& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ServiceRating::copy(const ServiceRating& other)
{
    this->isPropagated_ = other.isPropagated_;
    this->requesterId = other.requesterId;
    this->providerId = other.providerId;
    this->rating = other.rating;
}

void ServiceRating::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->isPropagated_);
    doParsimPacking(b,this->requesterId);
    doParsimPacking(b,this->providerId);
    doParsimPacking(b,this->rating);
}

void ServiceRating::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->isPropagated_);
    doParsimUnpacking(b,this->requesterId);
    doParsimUnpacking(b,this->providerId);
    doParsimUnpacking(b,this->rating);
}

bool ServiceRating::isPropagated() const
{
    return this->isPropagated_;
}

void ServiceRating::setIsPropagated(bool isPropagated)
{
    this->isPropagated_ = isPropagated;
}

int ServiceRating::getRequesterId() const
{
    return this->requesterId;
}

void ServiceRating::setRequesterId(int requesterId)
{
    this->requesterId = requesterId;
}

int ServiceRating::getProviderId() const
{
    return this->providerId;
}

void ServiceRating::setProviderId(int providerId)
{
    this->providerId = providerId;
}

double ServiceRating::getRating() const
{
    return this->rating;
}

void ServiceRating::setRating(double rating)
{
    this->rating = rating;
}

class ServiceRatingDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_isPropagated,
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
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ServiceRatingDescriptor)

ServiceRatingDescriptor::ServiceRatingDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ServiceRating)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

ServiceRatingDescriptor::~ServiceRatingDescriptor()
{
    delete[] propertyNames;
}

bool ServiceRatingDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ServiceRating *>(obj)!=nullptr;
}

const char **ServiceRatingDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ServiceRatingDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ServiceRatingDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int ServiceRatingDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_isPropagated
        FD_ISEDITABLE,    // FIELD_requesterId
        FD_ISEDITABLE,    // FIELD_providerId
        FD_ISEDITABLE,    // FIELD_rating
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *ServiceRatingDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "isPropagated",
        "requesterId",
        "providerId",
        "rating",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int ServiceRatingDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "isPropagated") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "requesterId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "providerId") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "rating") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *ServiceRatingDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "bool",    // FIELD_isPropagated
        "int",    // FIELD_requesterId
        "int",    // FIELD_providerId
        "double",    // FIELD_rating
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **ServiceRatingDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ServiceRatingDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ServiceRatingDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ServiceRatingDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ServiceRating'", field);
    }
}

const char *ServiceRatingDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ServiceRatingDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object); (void)pp;
    switch (field) {
        case FIELD_isPropagated: return bool2string(pp->isPropagated());
        case FIELD_requesterId: return long2string(pp->getRequesterId());
        case FIELD_providerId: return long2string(pp->getProviderId());
        case FIELD_rating: return double2string(pp->getRating());
        default: return "";
    }
}

void ServiceRatingDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object); (void)pp;
    switch (field) {
        case FIELD_isPropagated: pp->setIsPropagated(string2bool(value)); break;
        case FIELD_requesterId: pp->setRequesterId(string2long(value)); break;
        case FIELD_providerId: pp->setProviderId(string2long(value)); break;
        case FIELD_rating: pp->setRating(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceRating'", field);
    }
}

omnetpp::cValue ServiceRatingDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object); (void)pp;
    switch (field) {
        case FIELD_isPropagated: return pp->isPropagated();
        case FIELD_requesterId: return pp->getRequesterId();
        case FIELD_providerId: return pp->getProviderId();
        case FIELD_rating: return pp->getRating();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ServiceRating' as cValue -- field index out of range?", field);
    }
}

void ServiceRatingDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object); (void)pp;
    switch (field) {
        case FIELD_isPropagated: pp->setIsPropagated(value.boolValue()); break;
        case FIELD_requesterId: pp->setRequesterId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_providerId: pp->setProviderId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_rating: pp->setRating(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceRating'", field);
    }
}

const char *ServiceRatingDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ServiceRatingDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ServiceRatingDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ServiceRating *pp = omnetpp::fromAnyPtr<ServiceRating>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ServiceRating'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

