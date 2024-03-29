#pragma once
//TODO(Wax) : impl a macro like IMPL(CLASS, m1, m2, m3, ...)

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <cstddef>
#include <type_traits>

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
    virtual std::vector<struct Object>* GetProperties() { return nullptr; }
};

struct Object
{
    const char* name = "root";
    uint32_t offset = 0;
    uint32_t flags = 0;
    TypeInfo* info = nullptr;
    uint8_t* data = nullptr;
    //const char* introduction?
    //const int version?
    Object() { }
    Object(const char* _name, int _offset, TypeInfo* _info):name(_name), offset(_offset), info(_info){}
    const char* GetTypeName(){ return info->GetTypeName(); }
    const uint32_t GetTypeSize(){ return info->GetTypeSize(); }
    const char* GetName() { return name;}
    const uint32_t GetOffset() {return offset;}
    void SetVal(void* v)
    {
        memcpy(data + offset, v, GetTypeSize());
    }
    void* GetVal()
    {
        return data + offset;
    }

    Object GetMemberVal(const char* name)
    {
        assert(info->GetProperties());

        Object prop(nullptr, 0, nullptr);
        auto& ps = *info->GetProperties();
        for(auto& p : ps)
        {
            if(strcmp(p.GetName(), name) == 0)
            {
                prop = p;
                prop.data = data;
                return prop;
            }
        }
        return prop;
    }

    Object SetMemberVal(const char* name, void* v)
    {
        assert(info->GetProperties());

        Object prop(nullptr, 0, nullptr);
        auto& ps = *info->GetProperties();
        for(auto& p : ps)
        {
            if(strcmp(p.GetName(), name) == 0)
            {
                prop = p;
                prop.data = data;
                prop.SetVal(v);
                return prop;
            }
        }
        return prop;
    }

    bool IsValid(){ return (data != nullptr && info != nullptr); }
    int ToInt() { assert(IsValid()); return *((int*)(data + offset));}
    float ToFloat() { assert(IsValid()); return *((float*)(data + offset));}
    double ToDouble() {assert(IsValid()); return *((double*)(data + offset));}
    uint32_t ToUint32() {assert(IsValid()); return *((uint32_t*)(data + offset));}
    char ToChar() {assert(IsValid()); return *((char*)(data + offset));}

    size_t MemberSize(){ return (*info->GetProperties()).size(); }

    bool isObject()
    {
        return !info->isBasicType();
    }
    bool isInt()
    {
        return (strcmp(info->GetTypeName(), "int") == 0);
    }
    bool isDouble()
    {
        return (strcmp(info->GetTypeName(), "double") == 0);
    }
    bool isfloat()
    {
        return (strcmp(info->GetTypeName(), "float") == 0);
    }
    bool isuint32()
    {
        return (strcmp(info->GetTypeName(), "uint32_t") == 0);
    }
    bool ischar()
    {
        return (strcmp(info->GetTypeName(), "char") == 0);
    }

    Object operator[](int i)
    {
        Object p = (*info->GetProperties())[i];
        p.data = data + offset;
        return p;
    }
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
    std::vector<Object> Props;
    UserTypeInfo(void(*init)(UserTypeInfo*)) : TypeInfo(nullptr, 0)
    {
        init(this);
    }
    virtual bool isBasicType() override { return false; }
    virtual std::vector<Object>* GetProperties() override { return &Props; }
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

struct Meta
{
    static std::map<std::string, TypeInfo*> typestable;
    static void Register(std::string _name, TypeInfo* ptr)
    {
        typestable[_name] = ptr;
    }
    template<typename T>
    static Object CreateObject(T* t)
    {
        Object o;
        o.info = GetInfo(*t);
        o.data = (uint8_t*) t;
        return o;
    }
    template<typename T>
    static Object CreateObject(T& t)
    {
        Object o;
        o.info = GetInfo(t);
        o.data = (uint8_t*) &t;
        return o;
    }
    
    static Object CreateObjectByName(std::string _name)
    {
        Object o;
        o.info = typestable[_name];
        o.data = (uint8_t*) malloc(o.info->GetTypeSize());
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