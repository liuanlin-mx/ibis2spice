#ifndef __IBIS2SPICE_GUI_H__
#define __IBIS2SPICE_GUI_H__
#include <memory>
#include <set>
#include "ibis2spice.h"
#include "ibis2spice_gui_base.h"

class ibis2spice_gui : public ibis2spice_gui_base
{
public:
    ibis2spice_gui();
    ~ibis2spice_gui();
    
public:
    virtual void m_filePickerOnFileChanged( wxFileDirPickerEvent& event );
    virtual void m_textCtrlFilterOnText( wxCommandEvent& event );
    virtual void m_buttonAddOnButtonClick( wxCommandEvent& event );
    virtual void m_buttonDelOnButtonClick( wxCommandEvent& event );
    virtual void m_buttonSaveOnButtonClick( wxCommandEvent& event );
    virtual void m_buttonConvertOnButtonClick( wxCommandEvent& event );
    
private:
    void _update_model_list();
    std::vector<std::string> _split(const std::string& str);
    void _load_cfg();
    void _save_cfg();
    
private:
    std::shared_ptr<ibis2spice> _ibis2spice;
    std::set<std::string> _model_selected;
    std::string _filter_key;
    std::string _ibs_path;
};

#endif
