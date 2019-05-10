#pragma once
//TODO(Wax) : impl a macro like IMPL(CLASS, m1, m2, m3, ...)

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <cstddef>

namespace RFX
{

template <typename T> struct TypeInfo* ThisType();

struct TypeInfo
{
    const char* name;
    uint32_t size;
    const char* GetTypeName() { return name; }
    const uint32_t GetTypeSize(){ return size; }
    TypeInfo(const char* _name, uint32_t _size) : name(_name), size(_size){}
    virtual bool isBasicType() { return true; }
    virtual std::vector<struct Property>* GetProperties() { return nullptr; }
};

struct Property
{
    const char* name;
    uint32_t offset;
    uint32_t flags;
    TypeInfo* info;
    //const char* introduction?
    //const int version?
    Property(const char* _name, int _offset, TypeInfo* _info):name(_name), offset(_offset), info(_info){}
    const char* GetTypeName(){ return info->GetTypeName(); }
    const uint32_t GetTypeSize(){ return info->GetTypeSize(); }
    const char* GetName() { return name;}
    const uint32_t GetOffset() {return offset;}
};

struct TypeInt    : TypeInfo { TypeInt()    : TypeInfo{ "int",         sizeof(int)     } { } };
struct TypeDouble : TypeInfo { TypeDouble() : TypeInfo{ "double",      sizeof(double)  } { } };
struct TypeFloat  : TypeInfo { TypeFloat()  : TypeInfo{ "float",       sizeof(float)   } { } };
struct TypeChar   : TypeInfo { TypeChar()   : TypeInfo{ "char",        sizeof(char)    } { } };
struct TypeUInt32 : TypeInfo { TypeUInt32() : TypeInfo{ "uint32_t",    sizeof(uint32_t)} { } };

template <> TypeInfo* ThisType<int>()      { static TypeInt typeDesc;    return &typeDesc; }
template <> TypeInfo* ThisType<double>()   { static TypeDouble typeDesc; return &typeDesc; }
template <> TypeInfo* ThisType<float>()    { static TypeFloat typeDesc;  return &typeDesc; }
template <> TypeInfo* ThisType<char>()     { static TypeChar typeDesc;   return &typeDesc; }
template <> TypeInfo* ThisType<uint32_t>() { static TypeUInt32 typeDesc; return &typeDesc; }

struct UserTypeInfo : TypeInfo
{
    std::vector<Property> Props;
    UserTypeInfo(void(*init)(UserTypeInfo*)) : TypeInfo(nullptr, 0)
    {
        init(this);
    }
    virtual bool isBasicType() override { return false; }
    virtual std::vector<Property>* GetProperties() override { return &Props; }
};

struct ThisTypeInfo
{
    template <typename T> static char func(decltype(&T::Reflection));
    template <typename T> static int func(...);
    template <typename T>
    struct IsReflected { enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) }; };

    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static TypeInfo* get() { return &T::Reflection; }

    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static TypeInfo* get(T& t) { return &T::Reflection; }

    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static TypeInfo* get() { return ThisType<T>(); }

    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static TypeInfo* get(T& t) { return ThisType<T>(); }
};

struct Object
{
    TypeInfo* info;
    void* data;
    template<typename T>
    T* GetPtr() { return (T*)data; }
    const char* GetName() { return info->GetTypeName(); }
    uint32_t GetSize() { return info->GetTypeSize(); }
};

struct Meta
{
    static std::map<std::string, TypeInfo*> typestable;
    static void Register(std::string _name, TypeInfo* ptr)
    {
        typestable[_name] = ptr;
    }
    static Object CreateObject(std::string _name)
    {
        Object o;
        o.info = typestable[_name];
        o.data = malloc(o.info->size);
        return o;
    }
    template <typename T>
    static TypeInfo* GetInfo() { return ThisTypeInfo::get<T>(); }
    template <typename T>
    static TypeInfo* GetInfo(T& a) { return ThisTypeInfo::get<T>(a); }
};
std::map<std::string, TypeInfo*> Meta::typestable;

#ifdef RFX
    #define STR_HELPER(x) #x
    #define STR(x) STR_HELPER(x)
    static_assert(0, "RFX already defined!!!! at " __FILE__ " " STR(__LINE__)) ;
    #undef STR_HELPER
    #undef STR
#endif

#define RFX() \
    friend struct ThisTypeInfo; \
    static UserTypeInfo Reflection; \
    static void initReflection(UserTypeInfo*);

#define RFX_IMPL(T) \
    UserTypeInfo T::Reflection{T::initReflection}; \
    void T::initReflection(UserTypeInfo* typeDesc) { \
        Meta::Register(#T, typeDesc);\
        using Tn = T; \
        typeDesc->name = #T; \
        typeDesc->size = sizeof(T); \
        typeDesc->Props = { 

#define RFX_M(name) \
            {#name, offsetof(Tn, name), Meta::GetInfo<decltype(name)>()},

#define RFX_END()\
    };}
    
} // namespace RFX