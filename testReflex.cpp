#include <iostream>
#include <map>
#include <type_traits>
#include <vector>

#include "rfx.hpp"

using namespace std;
using namespace RFX;

struct BClass
{
    // int BClassa;
    // float BClassb;
    // double BClassc;
    // unsigned BClassd;
    int BClassa;
    int BClassb;
    int BClassc;
    int BClassd;

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

int main()
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
             << " val: " << *(int*)ttp.GetVal((*propsbap)[i].GetName()).GetVal()
             << endl;
             assert((*propsbap)[i].data == nullptr);
    }
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


