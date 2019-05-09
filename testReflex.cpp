#include <iostream>
#include <map>
#include <type_traits>
#include <vector>

#include "rfx.hpp"

using namespace std;

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

int main()
{
    // Rgi(123);
    BClass b;
    TypeInfo * bb = TypeResolver<AClass>::get();
    auto props = bb->getProps();
    cout << "Struct " << bb->name << " with size: " << bb->size << endl;
    for(int i = 0; i < props->size(); i++)
    {
        cout << "name: " << (*props)[i].name
             << " offset: " << (*props)[i].offset
             << " type: " << (*props)[i].typeinfo->name
             << " size: " << (*props)[i].typeinfo->size << endl;
    }

    TypeInfo * ab = GetTypeInfo(b);
    // int ai;
    // TypeInfo* aiiti = GetTypeInfo(ai);
    auto propsb = ab->getProps();
    cout << "\nStruct " << ab->name << " with size: " << ab->size << endl;
    for(int i = 0; i < propsb->size(); i++)
    {
        cout << "name: " << (*propsb)[i].name
             << " offset: " << (*propsb)[i].offset
             << " type: " << (*propsb)[i].typeinfo->name
             << " size: " << (*propsb)[i].typeinfo->size << endl;
    }


    Type tp = RFX::CreateObject("AClass");
    cout << "\nStruct " << tp.info->name << " with size: " << tp.info->size << endl;
    auto propsba =  tp.info->getProps();
    for(int i = 0; i < propsba->size(); i++)
    {
        cout << "name: " << (*propsba)[i].name
             << " offset: " << (*propsba)[i].offset
             << " type: " << (*propsba)[i].typeinfo->name
             << " size: " << (*propsba)[i].typeinfo->size << endl;
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


