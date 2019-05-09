#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <cstdarg>

// select serialable data and unserialbe

struct Basic
{
    struct Member
    {
        const char* name;
        size_t offset;
        Basic* type;
    };
    std::string name;
    uint32_t flags;
    uint32_t size;
    Basic(std::string _name, uint32_t _size) : name(_name), size(_size){}
    virtual bool isBasicType()
    {
        return true; 
    }
    virtual std::vector<Member>* getProps()
    {
        return nullptr;
    }
};

struct User : Basic
{
    std::vector<Member> members;
    User(void(*init)(User*)) :Basic("User", 0)
    {
        init(this);
    }
    virtual bool isBasicType() override
    {
        return false; 
    }
    virtual std::vector<Member>* getProps() override
    {
        return &members;
    }
};


// Declare the function template that handles primitive types such as int, std::std::string, etc.:
template <typename T>
Basic* getPrimitiveDescriptor();



// A helper class to find Basics in different ways:
struct DefaultResolver {
    template <typename T> static char func(decltype(&T::Reflection));
    template <typename T> static int func(...);
    template <typename T>
    struct IsReflected {
        enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
    };

    // This version is called if T has a static member named "Reflection":
    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static Basic* get() {
        return &T::Reflection;
    }

    // This version is called otherwise:
    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static Basic* get() {
        return getPrimitiveDescriptor<T>();
    }
};


// This is the primary class template for finding all Basics:
template <typename T>
struct TypeResolver {
    static Basic* get() {
        return DefaultResolver::get<T>();
    }
};

struct Basic_Int : Basic {
    Basic_Int() : Basic{"int", sizeof(int)}{}
};

template <>
Basic* getPrimitiveDescriptor<int>() 
{
    static Basic_Int typeDesc;
    return &typeDesc;
}

struct Basic_double : Basic {
    Basic_double() : Basic{"double", sizeof(double)} {}
};

template <>
Basic* getPrimitiveDescriptor<double>() 
{
    static Basic_double typeDesc;
    return &typeDesc;
}


struct Basic_float : Basic {
    Basic_float() : Basic{"float", sizeof(float)} { }
};

template <>
Basic* getPrimitiveDescriptor<float>() 
{
    static Basic_float typeDesc;
    return &typeDesc;
}

struct Basic_char : Basic {
    Basic_char() : Basic{"char", sizeof(char)}{}
};

template <>
Basic* getPrimitiveDescriptor<char>() 
{
    static Basic_char typeDesc;
    return &typeDesc;
}



struct Basic_unsigned_int : Basic {
    Basic_unsigned_int() : Basic{"unsigned int", sizeof(unsigned int)} {}
};

template <>
Basic* getPrimitiveDescriptor<unsigned int>() 
{
    static Basic_unsigned_int typeDesc;
    return &typeDesc;
}


template<typename T>
Basic* MyType(T& a)
{
    return &T::Reflection;
}



struct TypePtr
{
    Basic* info;
    void* data;
    template<typename T>
    T* GetPtr()
    {
        return (T*)data;
    }
};

struct RFX
{
    static void Register(std::string _name, Basic* ptr)
    {
        typestable[_name] = ptr;
    }
    static std::map<std::string, Basic*> typestable;
    static TypePtr CreateObject(std::string _name)
    {
        TypePtr tp;
        tp.info = typestable[_name];
        tp.data = malloc(tp.info->size);
        return tp;
    }
};
std::map<std::string, Basic*> RFX::typestable;




#define REFLECT() \
    friend struct DefaultResolver; \
    static User Reflection; \
    static void initReflection(User*);

#define REFLECT_STRUCT_BEGIN(type) \
    User type::Reflection{type::initReflection}; \
    void type::initReflection(User* typeDesc) { \
        RFX::Register(#type, typeDesc);\
        using T = type; \
        typeDesc->name = #type; \
        typeDesc->size = sizeof(T); \
        typeDesc->members = {

#define REFLECT_STRUCT_MEMBER(name) \
            {#name, offsetof(T, name), TypeResolver<decltype(T::name)>::get()},

#define REFLECT_STRUCT_END() \
        }; \
    }

