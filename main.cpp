#include <stdio.h>
#include <stdlib.h>
#include <wx/wx.h>
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