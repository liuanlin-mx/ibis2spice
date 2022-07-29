#include <stdio.h>
#include <stdlib.h>
#include <wx/wx.h>
#include "ibis2spice_gui.h"
#include "ibis2spice.h"


void test()
{
    const char *ibs_file[] = {
    };
    
    for (std::uint32_t i = 0; i < sizeof(ibs_file) / sizeof(ibs_file[0]); i++)
    {
        std::string subckt;
        ibis2spice ibs;
        ibs.load(ibs_file[i]);
        std::vector<std::string> model_names = ibs.get_model_names();
        std::vector<std::string> component_names = ibs.get_component_names();
        for (auto& model_name: model_names)
        {
            subckt += ibs.gen_spice_model_by_model(component_names[0], model_name, "typ");
        }
        
        FILE *fp = fopen("test.lib", "wb");
        if (fp)
        {
            fwrite(subckt.c_str(), 1, subckt.length(), fp);
            fclose(fp);
        }
        printf("%s", subckt.c_str());
    }
}

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
