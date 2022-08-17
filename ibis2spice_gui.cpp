#include <algorithm>
#include <wx/textfile.h>
#include "ibis2spice_gui.h"

ibis2spice_gui::ibis2spice_gui()
    : ibis2spice_gui_base(NULL)
{
}


ibis2spice_gui::~ibis2spice_gui()
{
}


void ibis2spice_gui::m_filePickerOnFileChanged( wxFileDirPickerEvent& event )
{
    m_listBoxComponent->Clear();
    m_listBoxModel->Clear();
    m_listBoxModelSelected->Clear();
    _model_selected.clear();
    _filter_key.clear();
    
    if (event.GetPath().IsEmpty())
    {
        return;
    }
    
    _ibs_path = event.GetPath();
    wxFile file;
    file.Open(_ibs_path,  wxFile::read);
    if (!file.IsOpened())
    {
        return;
    }
    wxString ibis_content;
    file.ReadAll(&ibis_content);
    file.Close();
    
    _ibis2spice.reset(new ibis2spice());
    _ibis2spice->load(ibis_content.data());
    
    _load_cfg();
    
    std::vector<std::string> component_names = _ibis2spice->get_component_names();
    
    for (auto& component_name: component_names)
    {
        m_listBoxComponent->Append(component_name);
    }
    
    _update_model_list();
}


void ibis2spice_gui::m_textCtrlFilterOnText( wxCommandEvent& event )
{
    _filter_key = m_textCtrlFilter->GetValue().ToStdString();
    _update_model_list();
}




void ibis2spice_gui::m_buttonAddOnButtonClick( wxCommandEvent& event )
{
    wxArrayInt selections;
    std::int32_t n = m_listBoxModel->GetSelections(selections);
    for (std::int32_t i = 0; i < n; i++)
    {
        std::string name = m_listBoxModel->GetString(selections[i]).ToStdString();
        if (_model_selected.count(name) == 0)
        {
            _model_selected.insert(name);
            m_listBoxModelSelected->Append(name);
        }
    }
    _update_model_list();
}


void ibis2spice_gui::m_buttonDelOnButtonClick( wxCommandEvent& event )
{
    
    wxArrayInt selections;
    std::int32_t n = m_listBoxModelSelected->GetSelections(selections);
    for (std::int32_t i = n - 1; i >= 0; i--)
    {
        std::string name = m_listBoxModelSelected->GetString(selections[i]).ToStdString();
        m_listBoxModelSelected->Delete(selections[i]);
        _model_selected.erase(name);
    }
    
    if (n > 0)
    {
        _update_model_list();
    }
}


void ibis2spice_gui::m_buttonSaveOnButtonClick( wxCommandEvent& event )
{
    _save_cfg();
}


void ibis2spice_gui::m_buttonConvertOnButtonClick( wxCommandEvent& event )
{
    if (m_listBoxComponent->IsEmpty())
    {
        return;
    }
    std::int32_t select = m_listBoxComponent->GetSelection();
    if (select < 0)
    {
        select = 0;
    }
    
    std::string component_name = m_listBoxComponent->GetString(select).ToStdString();
    std::string subckt;
    for (const auto& model_name: _model_selected)
    {
        if (m_checkBoxTyp->GetValue())
        {
            subckt += _ibis2spice->gen_spice_model_by_model(component_name, model_name, "typ");
        }
        if (m_checkBoxMin->GetValue())
        {
            subckt += _ibis2spice->gen_spice_model_by_model(component_name, model_name, "min");
        }
        if (m_checkBoxMax->GetValue())
        {
            subckt += _ibis2spice->gen_spice_model_by_model(component_name, model_name, "max");
        }
    }
    
    wxFile file;
    file.Open((_ibs_path + ".lib").c_str(), wxFile::write);
    if (file.IsOpened())
    {
        file.Write(subckt.c_str());
        file.Close();
    }
}



void ibis2spice_gui::_update_model_list()
{
    std::vector<std::string> model_names = _ibis2spice->get_model_names();
    m_listBoxModel->Clear();
    for (std::string& model_name: model_names)
    {
        if (_model_selected.count(model_name) == 0)
        {
            std::string name = model_name;
            std::string key = _filter_key;
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            
            std::vector<std::string> keys = _split(key);
            bool flag = true;
            for (auto &s: keys)
            {
                if (name.find(s) == name.npos)
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
            {
                m_listBoxModel->Append(model_name);
            }
        }
    }
}


std::vector<std::string> ibis2spice_gui::_split(const std::string& str)
{
    std::vector<std::string> result;
    
    std::string s;
    for (const auto& c: str)
    {
        if (std::isspace(c))
        {
            if (!s.empty())
            {
                result.push_back(s);
                s.clear();
            }
            continue;
        }
        s.push_back(c);
    }
    
    if (!s.empty())
    {
        result.push_back(s);
    }
    return result;
}


std::vector<std::string> ibis2spice_gui::_string_split(std::string str, const std::string& key)
{
    std::vector<std::string> out;
    std::string::size_type begin = 0;
    std::string::size_type end = 0;
    while ((end = str.find(key, begin)) != str.npos)
    {
        out.push_back(str.substr(begin, end - begin));
        begin = end + key.size();
    }
    if (begin < str.size())
    {
        out.push_back(str.substr(begin, end - begin));
    }
    
    return out;
}

void ibis2spice_gui::_load_cfg()
{
    wxFile file;
    file.Open(_ibs_path + ".ibis2spice", wxFile::read);
    if (!file.IsOpened())
    {
        return;
    }
    wxString str;
    file.ReadAll(&str);
    file.Close();
    std::vector<std::string> vstr = _string_split(str.ToStdString(), "\n");
    for (auto& s: vstr)
    {
        while (s.back() == '\n' || s.back() == '\r')
        {
            s.pop_back();
        }
        if (s.length() > 0)
        {
            _model_selected.insert(s);
            m_listBoxModelSelected->Append(s);
        }
    }
}

void ibis2spice_gui::_save_cfg()
{
    wxFile file;
    file.Open(_ibs_path + ".ibis2spice", wxFile::write);
    if (file.IsOpened())
    {
        for (const std::string& model_name: _model_selected)
        {
            file.Write((model_name + "\r\n"));
        }
        file.Close();
    }
}