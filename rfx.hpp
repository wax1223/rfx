#pragma once

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <cstddef>

// select serialable data and unserialbe

struct Member
{
    const char* name;
    size_t offset;
    uint32_t flags;
    struct TypeInfo* typeinfo;
    //const char* introduction?
    //const int version?
    Member(const char* _name, int _offset, TypeInfo* _typeinfo):name(_name), offset(_offset), typeinfo(_typeinfo){}
};

template <typename T> struct TypeInfo* getType();

struct TypeInfo
{
    const char* name;
    uint32_t size;
    TypeInfo(const char* _name, uint32_t _size) : name(_name), size(_size){}
    virtual bool isBasicType() { return true; }
    virtual std::vector<Member>* getProps() { return nullptr; }
};

struct TypeInt    : TypeInfo { TypeInt()    : TypeInfo{ "int",         sizeof(int)     } { } };
struct TypeDouble : TypeInfo { TypeDouble() : TypeInfo{ "double",      sizeof(double)  } { } };
struct TypeFloat  : TypeInfo { TypeFloat()  : TypeInfo{ "float",       sizeof(float)   } { } };
struct TypeChar   : TypeInfo { TypeChar()   : TypeInfo{ "char",        sizeof(char)    } { } };
struct TypeUInt32 : TypeInfo { TypeUInt32() : TypeInfo{ "uint32_t",    sizeof(uint32_t)} { } };

template <> TypeInfo* getType<int>()      { static TypeInt typeDesc;    return &typeDesc; }
template <> TypeInfo* getType<double>()   { static TypeDouble typeDesc; return &typeDesc; }
template <> TypeInfo* getType<float>()    { static TypeFloat typeDesc;  return &typeDesc; }
template <> TypeInfo* getType<char>()     { static TypeChar typeDesc;   return &typeDesc; }
template <> TypeInfo* getType<uint32_t>() { static TypeUInt32 typeDesc; return &typeDesc; }

struct UserTypeInfo : TypeInfo
{
    std::vector<Member> members;
    UserTypeInfo(void(*init)(UserTypeInfo*)) : TypeInfo(nullptr, 0)
    {
        init(this);
    }
    virtual bool isBasicType() override { return false; }
    virtual std::vector<Member>* getProps() override { return &members; }
};


// A helper class to find Types in different ways:
struct DefaultResolver
{
    template <typename T> static char func(decltype(&T::Reflection));
    template <typename T> static int func(...);
    template <typename T>
    struct IsReflected { enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) }; };

    // This version is called if T has a static member named "Reflection":
    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static TypeInfo* get() { return &T::Reflection; }

    // This version is called otherwise:
    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static TypeInfo* get() { return getType<T>(); }
};

// This is the primary class template for finding all Types:
template <typename T>
struct TypeResolver 
{
    static TypeInfo* get() { return DefaultResolver::get<T>(); }
};

template<typename T>
TypeInfo* GetTypeInfo(T& a) { return &T::Reflection; }

struct Type
{
    TypeInfo* info;
    void* data;
    template<typename T>
    T* GetPtr() { return (T*)data; }
};

struct RFX
{
    static void Register(std::string _name, TypeInfo* ptr)
    {
        typestable[_name] = ptr;
    }
    static std::map<std::string, TypeInfo*> typestable;
    static Type CreateObject(std::string _name)
    {
        Type t;
        t.info = typestable[_name];
        t.data = malloc(t.info->size);
        return t;
    }
};
std::map<std::string, TypeInfo*> RFX::typestable;

#ifdef RFX
    #define STR_HELPER(x) #x
    #define STR(x) STR_HELPER(x)
    static_assert(0, "RFX already defined!!!! at " __FILE__ " " STR(__LINE__)) ;
    #undef STR_HELPER
    #undef STR
#endif

#define RFX() \
    friend struct DefaultResolver; \
    static UserTypeInfo Reflection; \
    static void initReflection(UserTypeInfo*);

#define RFX_IMPL(T) \
    UserTypeInfo T::Reflection{T::initReflection}; \
    void T::initReflection(UserTypeInfo* typeDesc) { \
        RFX::Register(#T, typeDesc);\
        using Tn = T; \
        typeDesc->name = #T; \
        typeDesc->size = sizeof(T); \
        typeDesc->members = { 

#define RFX_M(name) \
            {#name, offsetof(Tn, name), TypeResolver<decltype(name)>::get()},

#define RFX_END()\
    };}

//Fail to impl a macro like IMPL(CLASS, m1, m2, m3, ...)
// void Rgi(int begin, ...)
// {
//     va_list args;
//     va_start(args, begin);
//     int i = 0;
//     i = va_arg(args, int);
//     while (i >= 0)
//     {
//         cout << i << " ";
//         i = va_arg(args, int);
//     }
// }

// {#name, offsetof(Tn, name), TypeResolver<decltype(Tn::name)>::get()},
// template<typename T, typename... Targs>
// void Rgi(vector<Member>& vm, const char* name, int offset, TypeInfo* ti)
// {
//     vm.push_back
//     (
//         {name, offsetof(T, name), TypeResolver<decltype(name)>::get()}
//     );
// }

// #define RFX_IMPL(T, M, ...) \
//     UserTypeInfo T::Reflection{T::initReflection}; \
//     void T::initReflection(UserTypeInfo* typeDesc) { \
//         RFX::Register(#T, typeDesc);\
//         using Tn = T; \
//         typeDesc->name = #T; \
//         typeDesc->size = sizeof(T); \
//         typeDesc->members = Rgi(0, __VA_ARGS__, {});
