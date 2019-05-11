#include <iostream>
#include <map>
#include <type_traits>
#include <vector>

// #include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "rfx.hpp"

using namespace std;
using namespace RFX;
using namespace rapidjson;

struct BClass
{
    int BClassa;
    float BClassb;
    double BClassc;
    unsigned BClassd;

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
    char AClasse;
    char eAClass2;
    char eAClass3;
    char eAClass4;
    char eAClass5;
    int AClassa;
    float AClassb;
    double AClassc;
    BClass bAClassc;
    unsigned AClassd;
    void AClassFunc()
    {
        std::cout << "AClassFunc" << std::endl;
    }
    RFX()
};
RFX_IMPL(AClass)
    RFX_M(AClasse) RFX_M(eAClass2) RFX_M(eAClass3) RFX_M(eAClass4) 
    RFX_M(AClassa) RFX_M(AClassb) RFX_M(AClassc) RFX_M(AClassd) 
    RFX_M(bAClassc)
RFX_END()


RFX_IMPL(BClass)
    RFX_M(BClassa) RFX_M(BClassb)
    RFX_M(BClassc) RFX_M(BClassd)
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
    cbc.BClassa = 1;
    cbc.BClassb = 12;
    cbc.BClassc = 123;
    cbc.BClassd = 1234;

    Object ttp = Meta::CreateObject(cbc);
    int v = 12345;
    ttp.SetVal("BClassd", &v);
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
}


void testRapidJson()
{
    StringBuffer s;
    PrettyWriter<StringBuffer> writer(s);
    writer.StartObject();
    writer.Key("Object");
        writer.StartObject();
        writer.Key("hello");
        writer.String("world");
        writer.Key("t");
        writer.Bool(true);
        writer.Key("f");
        writer.Bool(false);
        writer.Key("n");
        writer.Null();
        writer.Key("i");
        writer.Uint(123);
        writer.Key("pi");
        writer.Double(3.1416);
    writer.EndObject();
    writer.Key("hello");
    writer.String("world");
    writer.Key("t");
    writer.Bool(true);
    writer.Key("f");
    writer.Bool(false);
    writer.Key("n");
    writer.Null();
    writer.Key("i");
    writer.Uint(123);
    writer.Key("pi");
    writer.Double(3.1416);

    writer.Key("a");
    writer.StartArray();
    for (unsigned i = 0; i < 4; i++)
        writer.Uint(i);
    writer.EndArray();

    writer.EndObject();
    cout << s.GetString() << endl;
}
void toJson(PrettyWriter<StringBuffer>& w, Property p)
{
    if(p.isObject())
    {
        w.Key(p.GetName());
        w.StartObject();
        for(int i = 0; i < p.MemberSize(); i++)
        {
            toJson(w, p[i]);
        }
        w.EndObject();
    }
    else
    {
        w.Key(p.GetName());
        w.StartObject();
        w.Key("Type");
        w.String(p.GetTypeName());
        w.Key("Value");
        if(p.isInt())
        {
            int a = p.ToInt();
            w.Int(a);
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
        w.EndObject();
    }
}

void Serial()
{
    StringBuffer s;
    PrettyWriter<StringBuffer> writer(s);
    
    BClass b;
    b.BClassa = 520;
    b.BClassb = 5.210f;
    b.BClassc = 0.520808;
    b.BClassd = 321123;
    Object o = Meta::CreateObject(b);

    writer.StartObject();

    writer.Key(o.GetName());
        writer.StartObject();
        for(int i = 0; i < o.MemberSize(); i++)
        {
            toJson(writer, o[i]);
        }
        writer.EndObject();
    writer.EndObject();
    cout << s.GetString() << endl;

    s.Clear();
    writer.Reset(s);

    cout << "\n\n";

    AClass a;
    a.AClassa = 1;
    a.AClassb = 3.1415926f;
    a.AClassc = 2.163182731823;
    a.AClassd = 520584;
    a.AClasse = 'h';
    a.bAClassc = b;
    a.eAClass2 = 'u';
    a.eAClass3 = 'g';
    a.eAClass4 = 'u';
    a.eAClass5 = 'o';
    
    Object o2 = Meta::CreateObject(a);
    writer.StartObject();
    writer.Key(o2.GetName());
        writer.StartObject();
        for(int i = 0; i < o2.MemberSize(); i++)
        {
            toJson(writer, o2[i]);
        }
        writer.EndObject();
    writer.EndObject();
    cout << s.GetString() << endl;
}

int main()
{
    Serial();
    // testRapidJson();
}



//Load
/*
Json js;
Object root = createObject("root")

void save(Object& j, Json& js)
{
    for(auto p : root.props)
    {
        if(p is not serialable) continue
        if(p is obj)
        {
            save(p, js);
        }
        else if(p is array)
        {
            for(pp : [])
            {
                save(pp);
            }

        }else if(p is permitiy)
        {
            js.save(p.getValue());
        }
    }
}

*/


/*
Load

void Load(js,)
{
    for(t : js)
    {
        Object o = create(t[type]);
        if(t[type] is object)
        {
            for(o)
            {
                type(o).set(t["name"], t["value"]);
            }
        }
        else if(t[type] is array)
        {

        }else
        {
            Object o = create(t[type]);
            o.value = t[value];
        }
    }
}

*/


// 