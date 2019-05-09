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
    REFLECT()
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
    REFLECT()
};
REFLECT_STRUCT_BEGIN(AClass)
REFLECT_STRUCT_MEMBER(AClasse)
REFLECT_STRUCT_MEMBER(eAClass2)
REFLECT_STRUCT_MEMBER(eAClass3)
REFLECT_STRUCT_MEMBER(eAClass4)
REFLECT_STRUCT_MEMBER(AClassa)
REFLECT_STRUCT_MEMBER(AClassb)
REFLECT_STRUCT_MEMBER(AClassc)
REFLECT_STRUCT_MEMBER(AClassd)
REFLECT_STRUCT_MEMBER(bAClassc)
REFLECT_STRUCT_END()


REFLECT_STRUCT_BEGIN(BClass)
REFLECT_STRUCT_MEMBER(BClassa)
REFLECT_STRUCT_MEMBER(BClassb)
REFLECT_STRUCT_MEMBER(BClassc)
REFLECT_STRUCT_MEMBER(BClassd)
REFLECT_STRUCT_END()

int main()
{
    BClass b;
    Basic * bb = DefaultResolver::get<AClass>();
    auto props = bb->getProps();
    cout << "Struct " << bb->name << " with size: " << bb->size << endl;
    for(int i = 0; i < props->size(); i++)
    {
        cout << "name: " << (*props)[i].name
             << " offset: " << (*props)[i].offset
             << " type: " << (*props)[i].type->name
             << " size: " << (*props)[i].type->size << endl;
    }

    Basic * ab = MyType(b);
    auto propsb = ab->getProps();
    cout << "\nStruct " << ab->name << " with size: " << ab->size << endl;
    for(int i = 0; i < propsb->size(); i++)
    {
        cout << "name: " << (*propsb)[i].name
             << " offset: " << (*propsb)[i].offset
             << " type: " << (*propsb)[i].type->name
             << " size: " << (*propsb)[i].type->size << endl;
    }


    TypePtr tp = RFX::CreateObject("AClass");
    cout << "\nStruct " << tp.info->name << " with size: " << tp.info->size << endl;
    auto propsba =  tp.info->getProps();
    for(int i = 0; i < propsba->size(); i++)
    {
        cout << "name: " << (*propsba)[i].name
             << " offset: " << (*propsba)[i].offset
             << " type: " << (*propsba)[i].type->name
             << " size: " << (*propsba)[i].type->size << endl;
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


