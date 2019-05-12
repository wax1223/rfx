#include <iostream>
#include <map>
#include <type_traits>
#include <vector>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/reader.h"

#include "rfx.hpp"

using namespace std;
using namespace RFX;
using namespace rapidjson;

struct BClass
{
    int i;
    float f;
    double d;
    unsigned ui;

    void BclassFunc(int i)
    {
        int b = i;
        b++;
        std::cout << "BclassFunc" << std::endl;
    }
    RFX()
};


struct AClass
{
    char c1;
    char c2;
    char c3;
    char c4;
    char c5;
    int i;
    float f;
    double d;
    BClass bc1;
    unsigned ui;
    BClass bc2;
    void AClassFunc()
    {
        std::cout << "AClassFunc" << std::endl;
    }
    RFX()
};

struct CClass
{
    int Begin = 520;
    AClass a1;
    BClass b1;
    AClass a2;
    BClass b2;
    AClass a3;
    int End = 584;
    RFX()
};
RFX_IMPL(AClass)
    RFX_M(c1) RFX_M(c2) RFX_M(c3) RFX_M(c4) RFX_M(c5)
    RFX_M(i) RFX_M(f) RFX_M(d) RFX_M(ui) 
    RFX_M(bc1) RFX_M(bc2)
RFX_END()


RFX_IMPL(BClass)
    RFX_M(i) RFX_M(f)
    RFX_M(d) RFX_M(ui)
RFX_END()


RFX_IMPL(CClass)
    RFX_M(Begin)
    RFX_M(a1)
    RFX_M(b1)
    RFX_M(a2)
    RFX_M(b2)
    RFX_M(a3)
    RFX_M(End)
RFX_END()

void TestGetTypeInfo()
{
    BClass b;
    TypeInfo * bb = Meta::GetInfo<AClass>();
    auto props = bb->GetProperties();
    cout << "Struct " << bb->name << " with size: " << bb->size << endl;
    for(int i = 0; i < props->size(); i++)
    {
        cout << "name: " << (*props)[i].GetName()
             << " offset: " << (*props)[i].GetOffset()
             << " type: " << (*props)[i].GetTypeName()
             << " size: " << (*props)[i].GetTypeSize() << endl;
    }

    TypeInfo * ab = Meta::GetInfo(b);
    auto propsb = ab->GetProperties();
    cout << "\nStruct " << ab->GetTypeName() << " with size: " << ab->GetTypeSize() << endl;
    for(int i = 0; i < propsb->size(); i++)
    {
        cout << "name: " << (*propsb)[i].GetName()
             << " offset: " << (*propsb)[i].GetOffset()
             << " type: " << (*propsb)[i].GetTypeName()
             << " size: " << (*propsb)[i].GetTypeSize() << endl;
    }

/*
    Object tp = Meta::CreateObjectByName("AClass");

    cout << "\nStruct " << tp.GetName() << " with size: " << tp.GetSize() << endl;
    auto propsba =  tp.info->GetProperties();
    for(int i = 0; i < propsba->size(); i++)
    {
        cout << "name: " << (*propsba)[i].GetName()
             << " offset: " << (*propsba)[i].GetOffset()
             << " type: " << (*propsba)[i].GetTypeName()
             << " size: " << (*propsba)[i].GetTypeSize() 
             << endl;
    }

    BClass cbc;
    cbc.i = 1;
    cbc.f = 12;
    cbc.d = 123;
    cbc.ui = 1234;

    Object ttp = Meta::CreateObject(cbc);
    int v = 12345;
    ttp.SetVal("ui", &v);
    cout << "\nStruct " << ttp.GetName() << " with size: " << ttp.GetSize() << endl;
    auto propsbap = ttp.info->GetProperties();
    for(int i = 0; i < propsbap->size(); i++)
    {
        cout << "name: " << (*propsbap)[i].GetName()
             << " offset: " << (*propsbap)[i].GetOffset()
             << " type: " << (*propsbap)[i].GetTypeName()
             << " size: " << (*propsbap)[i].GetTypeSize()
            //  << " val: " << *(int*)ttp.GetVal((*propsbap)[i].GetName()).GetVal()
             << endl;
             assert((*propsbap)[i].data == nullptr);
    }
    */
}

char* MakeNameAndType(char* buffer, const char* name, const char* type, const char sep = '|')
{
    int nameLen = strlen(name);
    strcpy(buffer, name);
    buffer[nameLen] = sep;
    strcpy(buffer + nameLen + 1, type);
    return buffer;
}

void toJson(PrettyWriter<StringBuffer>& w, Object p, char* buffer)
{
    // if(p is not serialable) continue
    w.Key(MakeNameAndType(buffer, p.GetName(), p.GetTypeName()));
    if(p.isObject())
    {
        w.StartObject();
        for(int i = 0; i < p.MemberSize(); i++)
        {
            toJson(w, p[i], buffer);
        }
        w.EndObject();
    }
    else
    {
        if(p.isInt())
        {
            w.Int(p.ToInt());
        }
        else if(p.isDouble())
        {
            w.Double(p.ToDouble());
        }
        else if(p.isfloat())
        {
            w.Double(p.ToFloat());
        }
        else if(p.isuint32())
        {
            w.Uint(p.ToUint32());
        }
        else
        {
            w.Int(p.ToChar());
        }
    }
}


void ToJson(PrettyWriter<StringBuffer>& w, Object& p)
{
    char buffer[1024];
    w.StartObject();
    toJson(w, p, buffer);
    w.EndObject();
}

void Serial()
{
    StringBuffer s;
    PrettyWriter<StringBuffer> writer(s);
    
    BClass b;
    b.i = 520;
    b.f = 5.210f;
    b.d = 0.520808;
    b.ui = 321123;
    Object o = Meta::CreateObject(b);
    ToJson(writer, o);
    cout << s.GetString() << endl;

    s.Clear();
    writer.Reset(s);

    cout << "\n\n";

    AClass a;
    a.i = 1;
    a.f = 3.1415926f;
    a.d = 2.163182731823;
    a.ui = 520584;
    a.c1 = 'h';
    a.bc1 = b;
    a.c2 = 'u';
    a.c3 = 'g';
    a.c4 = 'u';
    a.c5 = 'o';
    a.bc2 = b;

    Object o2 = Meta::CreateObject(a);
    ToJson(writer, o2);
    cout << s.GetString() << endl;


    CClass c;
    c.a1 = a;
    c.a2 = a;
    c.a3 = a;
    c.b1 = b;
    c.b2 = b;
    Object o3 = Meta::CreateObject(c);

    s.Clear();
    writer.Reset(s);
    ToJson(writer, o3);
    cout << s.GetString() << endl;
}

void fromJson(Value& val, Object o)
{
    if(val.IsObject())
    {
        int i = 0; 
        for (auto& m : val.GetObject())
        {
            fromJson(m.value, o[i++]);
        }
    }
    else 
    {
        if(val.IsInt() || val.IsUint())
        {
            int i = val.GetInt();
            if(o.isInt())
            {
                o.SetVal(&i);
            }
            else if(o.ischar())
            {
                char c = (char)i;
                o.SetVal((void*)&c);
            }
            else if(o.isuint32())
            {
                o.SetVal((void*)&i);
            }
        }
        else if(val.IsInt64() || val.IsUint64())
        {
            int64_t i = val.GetInt64();
            // Signed or unsigned is way to interpre the memory
            // and will not affect the memory itself if we do not
            // make any modification.
            o.SetVal((void*)&i);
        }
        else if(val.IsDouble())
        {
            double d = val.GetDouble();
            if(o.isfloat())
            {
                float f = (float)d;
                o.SetVal((void*)&f);
            }
            else if(o.isDouble())
            {
                o.SetVal((void*)&d);
            }
        }
    }
}


void FromJson(const char* json, Object& o)
{
    Document d; 
    d.Parse(json);
    Value v = d.GetObject();
    for (auto& m : v.GetObject())
    {
        fromJson(m.value, o);
    }
}

void DeSerial()
{
    StringBuffer s;
    PrettyWriter<StringBuffer> writer(s);

    BClass b;
    b.i = 520;
    b.f = 5.210f;
    b.d = 0.520808;
    b.ui = 321123;
    Object ob = Meta::CreateObject(b);
    ToJson(writer, ob);

    BClass bb;
    Object obb = Meta::CreateObject(bb);
    FromJson(s.GetString(), obb);

    assert(bb.i == 520);
    assert((bb.f - 5.210) <= 0.001);
    assert((bb.d - 0.520808) <= 0.000001);
    assert(bb.ui == 321123);


    AClass a;
    a.i = 1;
    a.f = 3.1415926f;
    //    3.1415925
    //    3.141592502593994
    a.d = 2.163182731823;
    //    2.1631827318229999
    //    2.1631827354431154
    a.ui = 520584;
    a.c1 = 'h';
    a.bc1 = b;
    a.c2 = 'u';
    a.c3 = 'g';
    a.c4 = 'u';
    a.c5 = 'o';
    a.bc2 = b;

    Object oa = Meta::CreateObject(a);
    s.Clear();
    writer.Reset(s);
    ToJson(writer, oa);

    AClass aa;
    Object oaa = Meta::CreateObject(aa);
    const char* json = s.GetString();
    FromJson(json, oaa);
    assert(aa.i == 1);
    assert((aa.f - 3.1415926f) <= 0.0000001f);
    assert((aa.d - 2.163182731823) <= 0.00000000001);
    assert(aa.ui == 520584);
    assert(aa.c1 == 'h');
    assert(aa.c2 == 'u');
    assert(aa.c3 == 'g');
    assert(aa.c4 == 'u');
    assert(aa.c5 == 'o');

    assert(aa.bc1.i == 520);
    assert((aa.bc1.f - 5.210) <= 0.001);
    assert((aa.bc1.d - 0.520808) <= 0.000001);
    assert(aa.bc1.ui == 321123);

    assert(aa.bc2.i == 520);
    assert((aa.bc2.f - 5.210) <= 0.001);
    assert((aa.bc2.d - 0.520808) <= 0.000001);
    assert(aa.bc2.ui == 321123);
    
    CClass c;
    c.a1 = a;
    c.a1.c1 = 'u';
    c.a2 = a;
    c.a2.c1 = 'b';
    c.a3 = a;
    c.a3.c1 = 's';
    c.b1 = b;
    c.b2 = b;

    Object oc = Meta::CreateObject(c);
    s.Clear();
    writer.Reset(s);
    ToJson(writer, oc);

    CClass cc;
    Object occ = Meta::CreateObject(cc);
    FromJson(s.GetString(), occ);

    assert(cc.a1.i == 1);
    assert((cc.a1.f - 3.1415926f) <= 0.0000001f);
    assert((cc.a1.d - 2.163182731823) <= 0.00000000001);
    assert(cc.a1.ui == 520584);
    assert(cc.a1.c1 == 'u');
    assert(cc.a1.c2 == 'u');
    assert(cc.a1.c3 == 'g');
    assert(cc.a1.c4 == 'u');
    assert(cc.a1.c5 == 'o');
    assert(cc.a1.bc1.i == 520);
    assert((cc.a1.bc1.f - 5.210) <= 0.001);
    assert((cc.a1.bc1.d - 0.520808) <= 0.000001);
    assert(cc.a1.bc1.ui == 321123);
    assert(cc.a1.bc2.i == 520);
    assert((cc.a1.bc2.f - 5.210) <= 0.001);
    assert((cc.a1.bc2.d - 0.520808) <= 0.000001);
    assert(cc.a1.bc2.ui == 321123);


    assert(cc.a2.i == 1);
    assert((cc.a2.f - 3.1415926f) <= 0.0000001f);
    assert((cc.a2.d - 2.163182731823) <= 0.00000000001);
    assert(cc.a2.ui == 520584);
    assert(cc.a2.c1 == 'b');
    assert(cc.a2.c2 == 'u');
    assert(cc.a2.c3 == 'g');
    assert(cc.a2.c4 == 'u');
    assert(cc.a2.c5 == 'o');
    assert(cc.a2.bc1.i == 520);
    assert((cc.a2.bc1.f - 5.210) <= 0.001);
    assert((cc.a2.bc1.d - 0.520808) <= 0.000001);
    assert(cc.a2.bc1.ui == 321123);
    assert(cc.a2.bc2.i == 520);
    assert((cc.a2.bc2.f - 5.210) <= 0.001);
    assert((cc.a2.bc2.d - 0.520808) <= 0.000001);
    assert(cc.a2.bc2.ui == 321123);


    assert(cc.a3.i == 1);
    assert((cc.a3.f - 3.1415926f) <= 0.0000001f);
    assert((cc.a3.d - 2.163182731823) <= 0.00000000001);
    assert(cc.a3.ui == 520584);
    assert(cc.a3.c1 == 's');
    assert(cc.a3.c2 == 'u');
    assert(cc.a3.c3 == 'g');
    assert(cc.a3.c4 == 'u');
    assert(cc.a3.c5 == 'o');
    assert(cc.a3.bc1.i == 520);
    assert((cc.a3.bc1.f - 5.210) <= 0.001);
    assert((cc.a3.bc1.d - 0.520808) <= 0.000001);
    assert(cc.a3.bc1.ui == 321123);
    assert(cc.a3.bc2.i == 520);
    assert((cc.a3.bc2.f - 5.210) <= 0.001);
    assert((cc.a3.bc2.d - 0.520808) <= 0.000001);
    assert(cc.a3.bc2.ui == 321123);

    assert(cc.b1.i == 520);
    assert((cc.b1.f - 5.210) <= 0.001);
    assert((cc.b1.d - 0.520808) <= 0.000001);
    assert(cc.b1.ui == 321123);

    assert(cc.b2.i == 520);
    assert((cc.b2.f - 5.210) <= 0.001);
    assert((cc.b2.d - 0.520808) <= 0.000001);
    assert(cc.b2.ui == 321123);

}

int main()
{
    // Serial();
    DeSerial();
}