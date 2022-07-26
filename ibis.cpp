#include <algorithm>
#include "ibis.h"

ibis_text_readline::ibis_text_readline()
    : _text(NULL)
    , _len(0)
    , _cur(NULL)
{
}

ibis_text_readline::~ibis_text_readline()
{
}



void ibis_text_readline::load(const char *text, std::uint32_t len)
{
    _text = text;
    _len = len;
    _cur = _text;
}


std::string ibis_text_readline::read_line()
{
    const char *str = _cur;
    const char *end = _text + _len;
    
    while (_cur < end)
    {
        if (*_cur == '\r' || *_cur == '\n')
        {
            while (*_cur == '\r' || *_cur == '\n')
            {
                _cur++;
            }
            break;
        }
        _cur++;
    }
    return std::string(str, _cur - str);
}



std::string ibis_text_readline::look_line()
{
    const char *str = _cur;
    const char *end = _text + _len;
    const char *cur = _cur;
    while (cur < end)
    {
        if (*cur == '\r' || *cur == '\n')
        {
            while (*cur == '\r' || *cur == '\n')
            {
                cur++;
            }
            break;
        }
        cur++;
    }
    return std::string(str, cur - str);
}



ibis_readline::ibis_readline()
    : _readline()
{
}



ibis_readline::~ibis_readline()
{
}



void ibis_readline::load(const char *text, std::uint32_t len)
{
    _readline.load(text, len);
}


std::vector<std::string> ibis_readline::read_line()
{
    while (1)
    {
        std::string line = _readline.read_line();
        if (line.length() == 0)
        {
            return std::vector<std::string>();
        }
        
        std::vector<std::string> v_str = _split(line);
        if (v_str.empty())
        {
            return std::vector<std::string>();
        }
        
        if (v_str[0].front() == '|')
        {
            continue;
        }
        return v_str;
    }
}


std::vector<std::string> ibis_readline::look_line()
{
    while (1)
    {
        std::string line = _readline.look_line();
        if (line.length() == 0)
        {
            return std::vector<std::string>();
        }
        
        std::vector<std::string> v_str = _split(line);
        if (v_str.empty())
        {
            return std::vector<std::string>();
        }
        
        if (v_str[0].front() == '|')
        {
            _readline.read_line();
            continue;
        }
        return v_str;
    }
}



bool ibis_readline::look_is_tag()
{
    std::vector<std::string> v_str = look_line();
    return v_str.empty() || v_str[0].front() == '['; 
}


bool ibis_readline::is_tag(const std::vector<std::string>& v_str)
{
    return v_str.empty() || v_str[0].front() == '['; 
}


std::vector<std::string> ibis_readline::_split(const std::string& str)
{
    std::vector<std::string> result;
    
    std::string s;
    std::int32_t tag = 0;
    for (const auto& c: str)
    {
        if (c == '[')
        {
            tag++;
        }
        else if (c == ']')
        {
            tag--;
        }
        
        if (std::isspace(c) && tag == 0)
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


ibis::ibis()
{
}

ibis::~ibis()
{
}

bool ibis::load(const char * ibis_file)
{
    FILE *fp = fopen(ibis_file, "rb");
    if (fp == NULL)
    {
        return false;
    }
    fseek(fp, 0, SEEK_END);
    std::int32_t flen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    _text_buf.resize(flen + 1);
    
    std::int32_t rlen = fread(&_text_buf[0], 1, flen, fp);
    fclose(fp);
    if (rlen <= 0)
    {
        return false;
    }
    
    _text_buf.resize(rlen);
    _readline.load(&_text_buf[0], rlen);
    
    while (1)
    {
        std::vector<std::string> v_str = _readline.read_line();
        if (v_str.empty())
        {
            break;
        }
        
        if (ibis_readline::is_tag(v_str) == false)
        {
            continue;
        }
        
        std::string tag = _make_lower(_read_n(v_str, 0));
        //printf("tag:%s\n", tag.c_str());
        if (tag == "[ibis ver]")
        {
            _load_ibis_ver(v_str);
        }
        else if (tag == "[file name]")
        {
            _load_file_name(v_str);
        }
        else if (tag == "[date]")
        {
            _load_date(v_str);
        }
        else if (tag == "[file rev]")
        {
            _load_file_rev(v_str);
        }
        else if (tag == "[source]")
        {
            _load_source(v_str);
        }
        else if (tag == "[notes]")
        {
            _load_notes(v_str);
        }
        else if (tag == "[disclaimer]")
        {
            _load_disclaimer(v_str);
        }
        else if (tag == "[copyright]")
        {
            _load_copyright(v_str);
        }
        else if (tag == "[component]")
        {
            _load_component(v_str);
        }
        else if (tag == "[package model]")
        {
            _load_package_model(v_str);
        }
        else if (tag == "[manufacturer]")
        {
            _load_manufacturer(v_str);
        }
        else if (tag == "[package]")
        {
            _load_package(v_str);
        }
        else if (tag == "[pin]")
        {
            _load_pin(v_str);
        }
        else if (tag == "[pin mapping]")
        {
            _load_pin_mapping(v_str);
        }
        else if (tag == "[diff_pin]")
        {
            _load_diff_pin(v_str);
        }
        else if (tag == "[model selector]")
        {
            _load_model_selector(v_str);
        }
        else if (tag == "[model]")
        {
            _load_model(v_str);
        }
        else if (tag == "[model spec]")
        {
            _load_model_spec(v_str);
        }
        else if (tag == "[receiver thresholds]")
        {
            _load_receiver_thresholds(v_str);
        }
        else if (tag == "[voltage range]")
        {
            _load_voltage_range(v_str);
        }
        else if (tag == "[pullup reference]")
        {
            _load_pullup_reference(v_str);
        }
        else if (tag == "[temperature range]")
        {
            _load_temperature_range(v_str);
        }
        else if (tag == "[add submodel]")
        {
            _load_add_submodel(v_str);
        }
        else if (tag == "[pulldown]")
        {
            _load_pulldown(v_str);
        }
        else if (tag == "[gnd clamp]")
        {
            _load_gnd_clamp(v_str);
        }
        else if (tag == "[pullup]")
        {
            _load_pullup(v_str);
        }
        else if (tag == "[power clamp]")
        {
            _load_power_clamp(v_str);
        }
        else if (tag == "[ramp]")
        {
            _load_ramp(v_str);
        }
        else if (tag == "[falling waveform]")
        {
            _load_falling_waveform(v_str);
        }
        else if (tag == "[rising waveform]")
        {
            _load_rising_waveform(v_str);
        }
        else if (tag == "[submodel]")
        {
            _load_submodel(v_str);
        }
    }
    
    return true;
}




std::vector<std::string> ibis::get_component_names()
{
    std::vector<std::string> names;
    for (const auto& component: _components)
    {
        names.push_back(component.name);
    }
    return names;
}



std::vector<std::string> ibis::get_model_names()
{
    std::vector<std::string> names;
    for (const auto& model: _models)
    {
        if (model.is_submodel == false)
        {
            names.push_back(model.name);
        }
    }
    return names;
}


std::vector<std::string> ibis::get_submodel_names()
{
    std::vector<std::string> names;
    for (const auto& model: _models)
    {
        if (model.is_submodel)
        {
            names.push_back(model.name);
        }
    }
    return names;
}



bool ibis::get_model_by_name(const std::string& name, ibis::model& model)
{
    for (const auto& model_: _models)
    {
        if (model_.name == name)
        {
            model = model_;
            return true;
        }
    }
    return false;
}



bool ibis::get_component_by_name(const std::string& name, ibis::component& component)
{
    for (const auto& component_: _components)
    {
        if (component_.name == name)
        {
            component = component_;
            return true;
        }
    }
    return false;
}



std::string ibis::_make_lower(const std::string& str)
{
    std::string out = str;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}


void ibis::_load_string(std::string& str, const std::vector<std::string>& v_str)
{
    for (std::uint32_t i = 1; i < v_str.size(); i++)
    {
        str.append(v_str[i]);
        str.append(" ");
    }
    
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        std::vector<std::string> v_str_ = _readline.read_line();
        
        for (std::uint32_t i = 0; i < v_str_.size(); i++)
        {
            str.append(v_str_[i]);
            str.append(" ");
        }
        str.append("\n");
    }
}



std::string ibis::_merge_v_str(const std::vector<std::string>& v_str, std::uint32_t begin)
{
    std::string str;
    for (std::uint32_t i = begin; i < v_str.size(); i++)
    {
        str.append(v_str[i]);
        if (i < v_str.size() - 1)
        {
            str.append(" ");
        }
    }
    return str;
}



std::string ibis::_read_n(const std::vector<std::string>& v_str, std::uint32_t n)
{
    if (n >= v_str.size())
    {
        return "null";
    }
    return v_str[n];
}


void ibis::_load_ibis_ver(const std::vector<std::string>& v_str)
{
    for (std::uint32_t i = 1; i < v_str.size(); i++)
    {
        _ibis_version.append(v_str[i]);
    }
}



void ibis::_load_file_name(const std::vector<std::string>& v_str)
{
    for (std::uint32_t i = 1; i < v_str.size(); i++)
    {
        _file_name.append(v_str[i]);
    }
}


void ibis::_load_date(const std::vector<std::string>& v_str)
{
    for (std::uint32_t i = 1; i < v_str.size(); i++)
    {
        _date.append(v_str[i]);
    }
}



void ibis::_load_file_rev(const std::vector<std::string>& v_str)
{
    for (std::uint32_t i = 1; i < v_str.size(); i++)
    {
        _file_revision.append(v_str[i]);
    }
}



void ibis::_load_source(const std::vector<std::string>& v_str)
{
    _load_string(_source, v_str);
}



void ibis::_load_notes(const std::vector<std::string>& v_str)
{
    _load_string(_notes, v_str);
}



void ibis::_load_disclaimer(const std::vector<std::string>& v_str)
{
    _load_string(_disclaimer, v_str);
}



void ibis::_load_copyright(const std::vector<std::string>& v_str)
{
    _load_string(_copyright, v_str);
}


void ibis::_load_component(const std::vector<std::string>& v_str)
{
    component c;
    c.name = _read_n(v_str, 1);
    _components.push_back(c);
}


void ibis::_load_package_model(const std::vector<std::string>& v_str)
{
    _load_string(_components.back().package_model, v_str);
}


void ibis::_load_manufacturer(const std::vector<std::string>& v_str)
{
    _load_string(_components.back().manufacturer, v_str);
}


void ibis::_load_package(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        if (type == "r_pkg")
        {
            _components.back().package_.r_pkg.typ = _read_n(v_str_, 1);
            _components.back().package_.r_pkg.min = _read_n(v_str_, 2);
            _components.back().package_.r_pkg.max = _read_n(v_str_, 3);
        }
        else if (type == "l_pkg")
        {
            _components.back().package_.l_pkg.typ = _read_n(v_str_, 1);
            _components.back().package_.l_pkg.min = _read_n(v_str_, 2);
            _components.back().package_.l_pkg.max = _read_n(v_str_, 3);
        }
        else if (type == "c_pkg")
        {
            _components.back().package_.c_pkg.typ = _read_n(v_str_, 1);
            _components.back().package_.c_pkg.min = _read_n(v_str_, 2);
            _components.back().package_.c_pkg.max = _read_n(v_str_, 3);
        }
    }
}

void ibis::_load_pin(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        pin pin_;
        pin_.pin_name = _read_n(v_str_, 0);
        pin_.signal_name = _read_n(v_str_, 1);
        pin_.model_name = _read_n(v_str_, 2);
        pin_.r_pin = _read_n(v_str_, 3);
        pin_.l_pin = _read_n(v_str_, 4);
        pin_.c_pin = _read_n(v_str_, 5);
        _components.back().pins.push_back(pin_);
    }
}



void ibis::_load_pin_mapping(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        pin_mapping pin_mapping_;
        pin_mapping_.pin_name = _read_n(v_str_, 0);
        pin_mapping_.pulldown_ref = _read_n(v_str_, 1);
        pin_mapping_.pullup_ref = _read_n(v_str_, 2);
        pin_mapping_.gnd_clamp_ref = _read_n(v_str_, 3);
        pin_mapping_.power_clamp_ref = _read_n(v_str_, 4);
        pin_mapping_.ext_ref = _read_n(v_str_, 5);
        _components.back().pin_mappings.push_back(pin_mapping_);
    }
}



void ibis::_load_diff_pin(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        diff_pin diff_pin_;
        diff_pin_.pin_name = _read_n(v_str_, 0);
        diff_pin_.inv_pin_name = _read_n(v_str_, 1);
        diff_pin_.vdiff = _read_n(v_str_, 2);
        diff_pin_.tdelay_typ = _read_n(v_str_, 3);
        diff_pin_.tdelay_min = _read_n(v_str_, 4);
        diff_pin_.tdelay_max = _read_n(v_str_, 5);
        _components.back().diff_pins.push_back(diff_pin_);
    }
}


void ibis::_load_model_selector(const std::vector<std::string>& v_str)
{
    model_selector ms;
    ms.name = _read_n(v_str, 1);
    
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        model_selector::item item;
        item.model_name = _read_n(v_str_, 0);
        item.desc = _merge_v_str(v_str_, 1);
        ms.items.push_back(item);
    }
    _model_selectors.push_back(ms);
    
}




void ibis::_load_model(const std::vector<std::string>& v_str)
{
    model m;
    m.name = _read_n(v_str, 1);
    
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        
        if (type == "model_type")
        {
            m.model_type = _read_n(v_str_, 1);
        }
        else if (type == "vinl")
        {
            m.v_in_l = _read_n(v_str_, 2);
        }
        else if (type == "vinh")
        {
            m.v_in_h = _read_n(v_str_, 2);
        }
        else if (type == "vmeas")
        {
            m.v_meas = _read_n(v_str_, 2);
        }
        else if (type == "vref")
        {
            m.v_ref = _read_n(v_str_, 2);
        }
        else if (type == "cref")
        {
            m.c_ref = _read_n(v_str_, 2);
        }
        else if (type == "rref")
        {
            m.r_ref = _read_n(v_str_, 2);
        }
        else if (type == "c_comp")
        {
            m.c_comp.typ = _read_n(v_str_, 1);
            m.c_comp.min = _read_n(v_str_, 2);
            m.c_comp.max = _read_n(v_str_, 3);
        }
    }
    _models.push_back(m);
}



void ibis::_load_model_spec(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        
        if (type == "vinl")
        {
            _models.back().mode_specs.v_in_l.typ = _read_n(v_str_, 1);
            _models.back().mode_specs.v_in_l.min = _read_n(v_str_, 2);
            _models.back().mode_specs.v_in_l.max = _read_n(v_str_, 3);
        }
        else if (type == "vinh")
        {
            _models.back().mode_specs.v_in_h.typ = _read_n(v_str_, 1);
            _models.back().mode_specs.v_in_h.min = _read_n(v_str_, 2);
            _models.back().mode_specs.v_in_h.max = _read_n(v_str_, 3);
        }
        else if (type == "vmeas")
        {
            _models.back().mode_specs.v_meas.typ = _read_n(v_str_, 1);
            _models.back().mode_specs.v_meas.min = _read_n(v_str_, 2);
            _models.back().mode_specs.v_meas.max = _read_n(v_str_, 3);
        }
        else if (type == "vref")
        {
            _models.back().mode_specs.v_ref.typ = _read_n(v_str_, 1);
            _models.back().mode_specs.v_ref.min = _read_n(v_str_, 2);
            _models.back().mode_specs.v_ref.max = _read_n(v_str_, 3);
        }
    }
}


void ibis::_load_receiver_thresholds(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        
        if (type == "vth")
        {
            _models.back().r_thresh.v_th = _read_n(v_str_, 2);
        }
        else if (type == "vth_min")
        {
            _models.back().r_thresh.v_th_min = _read_n(v_str_, 2);
        }
        else if (type == "vth_max")
        {
            _models.back().r_thresh.v_th_max = _read_n(v_str_, 2);
        }
        else if (type == "vinh_ac")
        {
            _models.back().r_thresh.v_inh_ac = _read_n(v_str_, 2);
        }
        else if (type == "vinh_dc")
        {
            _models.back().r_thresh.v_inh_dc = _read_n(v_str_, 2);
        }
        else if (type == "vinl_ac")
        {
            _models.back().r_thresh.v_inl_ac = _read_n(v_str_, 2);
        }
        else if (type == "vinl_dc")
        {
            _models.back().r_thresh.v_inl_dc = _read_n(v_str_, 2);
        }
        else if (type == "tslew_ac")
        {
            _models.back().r_thresh.tslew_ac = _read_n(v_str_, 2);
        }
        else if (type == "threshold_sensitivity")
        {
            _models.back().r_thresh.threshold_sensitivity = _read_n(v_str_, 2);
        }
        else if (type == "reference_supply")
        {
            _models.back().r_thresh.reference_supply = _read_n(v_str_, 2);
        }
    }
}



void ibis::_load_voltage_range(const std::vector<std::string>& v_str)
{
    _models.back().voltage_range.typ = _read_n(v_str, 1);
    _models.back().voltage_range.min= _read_n(v_str, 2);
    _models.back().voltage_range.max = _read_n(v_str, 3);
}



void ibis::_load_pullup_reference(const std::vector<std::string>& v_str)
{
    _models.back().pullup_reference.typ = _read_n(v_str, 1);
    _models.back().pullup_reference.min= _read_n(v_str, 2);
    _models.back().pullup_reference.max = _read_n(v_str, 3);
}



void ibis::_load_temperature_range(const std::vector<std::string>& v_str)
{
    _models.back().temperature_range.typ = _read_n(v_str, 1);
    _models.back().temperature_range.min= _read_n(v_str, 2);
    _models.back().temperature_range.max = _read_n(v_str, 3);
}



void ibis::_load_add_submodel(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        add_submodel sub;
        sub.submodel_name = _read_n(v_str_, 0);
        sub.mode = _read_n(v_str_, 1);
        _models.back().add_submodels.push_back(sub);
    }
}



void ibis::_load_pulldown(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        value_string val;
        val.ref = _read_n(v_str_, 0);
        val.typ = _read_n(v_str_, 1);
        val.min = _read_n(v_str_, 2);
        val.max = _read_n(v_str_, 3);
        _models.back().pulldown.push_back(val);
    }
}


void ibis::_load_gnd_clamp(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        value_string val;
        val.ref = _read_n(v_str_, 0);
        val.typ = _read_n(v_str_, 1);
        val.min = _read_n(v_str_, 2);
        val.max = _read_n(v_str_, 3);
        _models.back().gnd_clamp.push_back(val);
    }
}

void ibis::_load_pullup(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        value_string val;
        val.ref = _read_n(v_str_, 0);
        val.typ = _read_n(v_str_, 1);
        val.min = _read_n(v_str_, 2);
        val.max = _read_n(v_str_, 3);
        _models.back().pullup.push_back(val);
    }
}



void ibis::_load_power_clamp(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        value_string val;
        val.ref = _read_n(v_str_, 0);
        val.typ = _read_n(v_str_, 1);
        val.min = _read_n(v_str_, 2);
        val.max = _read_n(v_str_, 3);
        _models.back().power_clamp.push_back(val);
    }
}



void ibis::_load_ramp(const std::vector<std::string>& v_str)
{
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        
        if (type == "r_load")
        {
            _models.back().ramp_.r_load = _read_n(v_str_, 2);
        }
        else if (type == "dv/dt_r")
        {
            _models.back().ramp_.dv_dt_r.typ = _read_n(v_str_, 1);
            _models.back().ramp_.dv_dt_r.min = _read_n(v_str_, 2);
            _models.back().ramp_.dv_dt_r.max = _read_n(v_str_, 3);
        }
        else if (type == "dv/dt_f")
        {
            _models.back().ramp_.dv_dt_f.typ = _read_n(v_str_, 1);
            _models.back().ramp_.dv_dt_f.min = _read_n(v_str_, 2);
            _models.back().ramp_.dv_dt_f.max = _read_n(v_str_, 3);
        }
    }
}


void ibis::_load_falling_waveform(const std::vector<std::string>& v_str)
{
    waveform wave;
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        
        if (type == "v_fixture")
        {
            wave.r_fixture = _read_n(v_str_, 2);
        }
        else if (type == "v_fixture_min")
        {
            wave.v_fixture_min = _read_n(v_str_, 2);
        }
        else if (type == "v_fixture_max")
        {
            wave.v_fixture_max = _read_n(v_str_, 2);
        }
        else if (type == "r_fixture")
        {
            wave.r_fixture = _read_n(v_str_, 2);
        }
        else if (!type.empty()
                    && (type[0] == '+'
                        || type[0] == '-'
                        || (type[0] >= '0' && type[0] <= '9')))
        {
            value_string val;
            val.ref = _read_n(v_str_, 0);
            val.typ = _read_n(v_str_, 1);
            val.min = _read_n(v_str_, 2);
            val.max = _read_n(v_str_, 3);
            wave.values.push_back(val);
        }
    }
    _models.back().falling_waveforms.push_back(wave);
}



void ibis::_load_rising_waveform(const std::vector<std::string>& v_str)
{
    waveform wave;
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        
        if (type == "v_fixture")
        {
            wave.r_fixture = _read_n(v_str_, 2);
        }
        else if (type == "v_fixture_min")
        {
            wave.v_fixture_min = _read_n(v_str_, 2);
        }
        else if (type == "v_fixture_max")
        {
            wave.v_fixture_max = _read_n(v_str_, 2);
        }
        else if (type == "r_fixture")
        {
            wave.r_fixture = _read_n(v_str_, 2);
        }
        else if (!type.empty()
                    && (type[0] == '+'
                        || type[0] == '-'
                        || (type[0] >= '0' && type[0] <= '9')))
        {
            value_string val;
            val.ref = _read_n(v_str_, 0);
            val.typ = _read_n(v_str_, 1);
            val.min = _read_n(v_str_, 2);
            val.max = _read_n(v_str_, 3);
            wave.values.push_back(val);
        }
    }
    _models.back().rising_waveforms.push_back(wave);
}



void ibis::_load_submodel(const std::vector<std::string>& v_str)
{
    model m(true);
    m.name = _read_n(v_str, 1);
    
    while (1)
    {
        if (_readline.look_is_tag())
        {
            break;
        }
        const std::vector<std::string>& v_str_ = _readline.read_line();
        std::string type = _make_lower(_read_n(v_str_, 0));
        
        if (type == "submodel_type")
        {
            m.model_type = _read_n(v_str_, 1);
        }
    }
    _models.push_back(m);
}