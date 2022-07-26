#include <stdio.h>
#include <stdlib.h>
#include <wx/wx.h>
#include "ibis.h"
#include "ibis2spice.h"
#include "ibis2spice_gui.h"

class ibis2spice_app: public wxApp
{
public:
    virtual bool OnInit()
    {
        ibis2spice_gui *frame = new ibis2spice_gui();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(ibis2spice_app);

int mainx(int argc, char **argv)
{
    ibis2spice ibs2spice;
    ibs2spice.load("v89c.ibs");
    
    std::string subckt = ibs2spice.gen_spice_model_by_model("MT41J128M16JT", "DQ_34_1866", "typ");
    subckt = subckt + ibs2spice.gen_spice_model_by_model("MT41J128M16JT", "DQ_34_ODT20_1866", "typ");
    subckt = subckt + ibs2spice.gen_spice_model_by_model("MT41J128M16JT", "DQ_34_ODT30_1866", "typ");
    subckt = subckt + ibs2spice.gen_spice_model_by_model("MT41J128M16JT", "DQ_34_ODT40_1866", "typ");
    subckt = subckt + ibs2spice.gen_spice_model_by_model("MT41J128M16JT", "DQ_34_ODT60_1866", "typ");
    subckt = subckt + ibs2spice.gen_spice_model_by_model("MT41J128M16JT", "DQ_34_ODT120_1866", "typ");
    
    FILE *fp = fopen("test.lib", "wb");
    fwrite(subckt.c_str(), 1, subckt.length(), fp);
    fclose(fp);
    /*
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
    ibs.get_model_by_name("120ohm_ODT_2133", model);*/
    
	printf("hello world\n");
	return 0;
}
