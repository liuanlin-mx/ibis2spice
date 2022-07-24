#include <stdio.h>
#include "ibis.h"

int main(int argc, char **argv)
{
    ibis ibs;
    ibs.load("v89c.ibs");
    std::vector<std::string> names = ibs.get_model_names();
    for (auto& name: names)
    {
        printf("%s\n", name.c_str());
    }
    
    names = ibs.get_submodel_names();
    for (auto& name: names)
    {
        printf("%s\n", name.c_str());
    }
    
    ibis::model model;
    ibs.get_model_by_name("120ohm_ODT_2133", model);
    
	printf("hello world\n");
	return 0;
}
