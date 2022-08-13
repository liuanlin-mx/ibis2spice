#include <algorithm>
#include "ibis2spice.h"

ibis2spice::ibis2spice()
{
}


ibis2spice::~ibis2spice()
{
}



bool ibis2spice::load(const char *ibis_content)
{
    return _ibs.load(ibis_content);
}



std::string ibis2spice::gen_spice_model_by_model(const std::string& component_name, const std::string& model_name, const std::string& type)
{
    ibis::model model;
    ibis::component component;
    std::string subckt;
    if (!_ibs.get_model_by_name(model_name, model))
    {
        return "";
    }
    
    if (!_ibs.get_component_by_name(component_name, component))
    {
        return "";
    }
    
    if (model.model_type.find("I/O") != model.model_type.npos)
    {
        subckt = _gen_ngspice_model_by_model_output(component, model, type);
        subckt = subckt + _gen_ngspice_model_by_model_input(component, model, type);
    }
    else if (model.model_type == "Input")
    {
        subckt = _gen_ngspice_model_by_model_input(component, model, type);
    }
    else if (model.model_type == "Output" || model.model_type == "Open_drain")
    {
        subckt = _gen_ngspice_model_by_model_output(component, model, type);
    }
    return subckt;
}



std::string ibis2spice::_gen_ngspice_model_by_model_output(ibis::component& component, ibis::model model, std::string type)
{
    std::string subckt;
    std::string r_pkg;
    std::string l_pkg;
    std::string c_pkg;
    std::string c_comp;
    float voltage_range = 0;
    
    std::vector<std::pair<float, float> > rising;
    std::vector<std::pair<float, float> > falling;
    std::vector<std::pair<float, float> > pulldown;
    std::vector<std::pair<float, float> > pullup;
    std::vector<std::pair<float, float> > gnd_clamp;
    std::vector<std::pair<float, float> > power_clamp;
    
    char buf[2048];
    
    if (model.rising_waveforms.empty())
    {
        rising = _ramp_to_waveform(model.ramp, true, type);
    }
    else
    {
        rising = _read_two_col(model.rising_waveforms[0].values, type);
    }
    
    if (model.falling_waveforms.empty())
    {
        falling = _ramp_to_waveform(model.ramp, false, type);
    }
    else
    {
        falling = _read_two_col(model.falling_waveforms[0].values, type);
    }
    
    if (rising.empty() || falling.empty())
    {
        return "";
    }
    
    if (!model.pullup.empty())
    {
        pullup = _read_two_col(model.pullup, type);
    }
    
    if (!model.pulldown.empty())
    {
        pulldown = _read_two_col(model.pulldown, type);
    }
    
    if (!model.power_clamp.empty())
    {
        power_clamp = _read_two_col(model.power_clamp, type);
    }
    
    if (!model.gnd_clamp.empty())
    {
        gnd_clamp = _read_two_col(model.gnd_clamp, type);
    }
        
    if (type == "typ")
    {
        r_pkg = component.package.r_pkg.typ;
        l_pkg = component.package.l_pkg.typ;
        c_pkg = component.package.c_pkg.typ;
        c_comp = model.c_comp.typ;
        voltage_range = std::atof(model.voltage_range.typ.c_str());
        
    }
    else if (type == "min")
    {
        r_pkg = component.package.r_pkg.min;
        l_pkg = component.package.l_pkg.min;
        c_pkg = component.package.c_pkg.min;
        c_comp = model.c_comp.min;
        voltage_range = std::atof(model.voltage_range.min.c_str());
    }
    else if (type == "max")
    {
        r_pkg = component.package.r_pkg.max;
        l_pkg = component.package.l_pkg.max;
        c_pkg = component.package.c_pkg.max;
        c_comp = model.c_comp.max;
        voltage_range = std::atof(model.voltage_range.max.c_str());
    }
        
    sprintf(buf, "*** vinh:%s vinl:%s ***\n", model.v_in_h.c_str(), model.v_in_l.c_str());
    subckt = subckt + buf;
    
    sprintf(buf, ".SUBCKT %s_%s_OUTPUT OUT VCC VSS freq=500Meg duty=0.5 td=0 invert=0\n", _make_upper(model.name).c_str(), _make_upper(type).c_str());
    subckt = subckt + buf;
    subckt = subckt + "\n";
    subckt = subckt + ".param SH={invert < 0.5? 1: 0}\n";
    subckt = subckt + ".param SL={invert < 0.5? 0: 1}\n";
    
    subckt = subckt + "V1 S 0 PWL(0 {SH} {(duty / freq)} {SH} {(duty / freq)} {SL} {(1 / freq)} {SL}) r=0 td={td}\n";
    subckt = subckt + "V2 T 0 PWL(0 0 {(duty / freq)} {(duty / freq)} {(duty / freq)} 0 {(1 / freq)} {((1 - duty) / freq)} ) r=0 td={td}\n";
        
    subckt = subckt + "\n";
    
    
    sprintf(buf, "Cpkg OUT VSS %s\n", c_pkg.c_str());
    subckt = subckt + buf;
    sprintf(buf, "Rpkg OUT MID %s\n", r_pkg.c_str());
    subckt = subckt + buf;
    sprintf(buf, "Lpkg DIE MID %s\n", l_pkg.c_str());
    subckt = subckt + buf;
    sprintf(buf, "Ccomp DIE VSS %s\n", c_comp.c_str());
    subckt = subckt + buf;
    subckt = subckt + "\n";
    
    
    sprintf(buf, "B1 WAVEFORM VSS V=(V(S) > 0.5)? V(RISING) * %f: V(FALLING) * %f\n", voltage_range, voltage_range);
    subckt = subckt + buf;
    
    if (!pullup.empty())
    {
        subckt = subckt + "B2 PULLUP1 VSS V =(V(WAVEFORM, DIE) > 0)? V(PULLUP, VSS): 0\n";
        subckt = subckt + "G1 DIE VCC PULLUP1 VSS 1\n";
    }
    
    if (!pulldown.empty())
    {
        subckt = subckt + "B3 PULLDOWN1 VSS V =(V(DIE, WAVEFORM) > 0)? V(PULLDOWN, VSS): 0\n";
        subckt = subckt + "G2 DIE VSS PULLDOWN1 VSS 1\n";
    }
    
    subckt = subckt + "\n";
    subckt = subckt + "\n";
        
    
    float maxv = -100000;
    float minv = 10000;
    
    for (const auto& tmp: rising)
    {
        if (maxv < tmp.second)
        {
            maxv = tmp.second;
        }
        if (minv > tmp.second)
        {
            minv = tmp.second;
        }
    }
    
    for (const auto& tmp: falling)
    {
        if (maxv < tmp.second)
        {
            maxv = tmp.second;
        }
        if (minv > tmp.second)
        {
            minv = tmp.second;
        }
    }
    
    subckt = subckt + "B4 FALLING 0 V = PWL(V(T),\n";
    for (const auto& v: falling)
    {
        sprintf(buf, "+ %e,%e,\n", v.first, (v.second - minv) / (maxv - minv));
        subckt = subckt + buf;
    }
    subckt.pop_back();
    subckt.pop_back();
    subckt = subckt + ")\n\n\n";
        
    subckt = subckt + "B5 RISING 0 V = PWL(V(T),\n";
    for (const auto& v: rising)
    {
        sprintf(buf, "+ %e,%e,\n", v.first, (v.second - minv) / (maxv - minv));
        subckt = subckt + buf;
    }
    subckt.pop_back();
    subckt.pop_back();
    subckt = subckt + ")\n\n\n";
        
        
    if (!pullup.empty())
    {
        subckt = subckt + "Bpullup PULLUP VSS V= PWL(V(WAVEFORM, DIE),\n";
        for (const auto& v: pullup)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, v.second);
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
    }
    
    if (!pulldown.empty())
    {
        subckt = subckt + "Bpulldown PULLDOWN VSS V= PWL(V(DIE, WAVEFORM),\n";
        for (const auto& v: pulldown)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, v.second);
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
    }

    if (!power_clamp.empty())
    {
        subckt = subckt + "Bpower_clamp DIE VCC I= PWL(V(VCC, DIE),\n";
        for (const auto& v: power_clamp)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, v.second);
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
    }
        
    if (!gnd_clamp.empty())
    {
        subckt = subckt + "Bgnd_clamp DIE VSS I= PWL(V(DIE, VSS),\n";
        for (const auto& v: gnd_clamp)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, v.second);
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
    }
        
    for (const auto& add_submodel: model.add_submodels)
    {
        if (add_submodel.mode == "All")
        {
            ibis::model submodel;
            if (_ibs.get_model_by_name(add_submodel.submodel_name, submodel))
            {
                subckt = subckt + _gen_ngspice_add_submodel_output(component, model, submodel, type);
            }
        }
    }
    
    subckt = subckt + ".ENDS\n";
    subckt = subckt + "\n";
    subckt = subckt + "\n";
    return subckt;
}



std::string ibis2spice::_gen_ngspice_model_by_model_input(ibis::component& component, ibis::model model, std::string type)
{
    std::string subckt;
    std::string r_pkg;
    std::string l_pkg;
    std::string c_pkg;
    std::string c_comp;
    
    std::vector<std::pair<float, float> > power_clamp;
    std::vector<std::pair<float, float> > gnd_clamp;
    
    char buf[2048];
    if (!model.power_clamp.empty())
    {
        power_clamp = _read_two_col(model.power_clamp, type);
    }
    
    if(!model.gnd_clamp.empty())
    {
        gnd_clamp = _read_two_col(model.gnd_clamp, type);
    }
        
    if (type == "typ")
    {
        r_pkg = component.package.r_pkg.typ;
        l_pkg = component.package.l_pkg.typ;
        c_pkg = component.package.c_pkg.typ;
        c_comp = model.c_comp.typ;
        
    }
    else if (type == "min")
    {
        r_pkg = component.package.r_pkg.min;
        l_pkg = component.package.l_pkg.min;
        c_pkg = component.package.c_pkg.min;
        c_comp = model.c_comp.min;
    }
    else if (type == "max")
    {
        r_pkg = component.package.r_pkg.max;
        l_pkg = component.package.l_pkg.max;
        c_pkg = component.package.c_pkg.max;
        c_comp = model.c_comp.max;
    }
        
    sprintf(buf, "*** vinh:%s vinl:%s ***\n", model.v_in_h.c_str(), model.v_in_l.c_str());
    subckt = subckt + buf;
    
    sprintf(buf, ".SUBCKT %s_%s_INPUT IN VCC VSS\n", _make_upper(model.name).c_str(), _make_upper(type).c_str());
    subckt = subckt + buf;
    subckt = subckt + "\n";
    
    
    sprintf(buf, "Cpkg IN VSS %s\n", c_pkg.c_str());
    subckt = subckt + buf;
    sprintf(buf, "Rpkg IN MID %s\n", r_pkg.c_str());
    subckt = subckt + buf;
    sprintf(buf, "Lpkg DIE MID %s\n", l_pkg.c_str());
    subckt = subckt + buf;
    sprintf(buf, "Ccomp DIE VSS %s\n", c_comp.c_str());
    subckt = subckt + buf;
    subckt = subckt + "\n";
    
    
    if (!power_clamp.empty())
    {
        subckt = subckt + "Bpower_clamp DIE VCC I= PWL(V(VCC, DIE),\n";
        for (const auto& v: power_clamp)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, v.second);
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
    }
    
        
    if (!gnd_clamp.empty())
    {
        subckt = subckt + "Bgnd_clamp DIE VSS I= PWL(V(DIE, VSS),\n";
        for (const auto& v: gnd_clamp)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, v.second);
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
    }
    
    for (const auto& add_submodel: model.add_submodels)
    {
        if (add_submodel.mode == "Non-Driving"
            || add_submodel.mode == "All")
        {
            ibis::model submodel;
            if (_ibs.get_model_by_name(add_submodel.submodel_name, submodel))
            {
                subckt = subckt + _gen_ngspice_add_submodel_input(component, model, submodel, type);
            }
        }
    }
    subckt = subckt + ".ENDS\n";
    subckt = subckt + "\n";
    subckt = subckt + "\n";
    
    return subckt;
}



std::string ibis2spice::_gen_ngspice_add_submodel_input(ibis::component& component, ibis::model& model, ibis::model& submodel, const std::string& type)
{
    std::string subckt;
    
    std::vector<std::pair<float, float> > gnd_clamp;
    std::vector<std::pair<float, float> > power_clamp;
    
    char buf[2048];
    
    if (!submodel.power_clamp.empty())
    {
        power_clamp = _read_two_col(submodel.power_clamp, type);
    }
    
    if (!submodel.gnd_clamp.empty())
    {
        gnd_clamp = _read_two_col(submodel.gnd_clamp, type);
    }
    
    if (!power_clamp.empty())
    {
        sprintf(buf, "B%s_power_clamp DIE VCC I= PWL(V(VCC, DIE),\n", submodel.name.c_str());
        subckt = subckt + buf;
        for (const auto& v: power_clamp)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, v.second);
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
    }
    
    if (!gnd_clamp.empty())
    {
        sprintf(buf, "B%s_gnd_clamp DIE VSS I= PWL(V(DIE, VSS),\n", submodel.name.c_str());
        subckt = subckt + buf;
        for (const auto& v: gnd_clamp)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, v.second);
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
    }
    return subckt;
}


std::string ibis2spice::_gen_ngspice_add_submodel_output(ibis::component& component, ibis::model& model, ibis::model& submodel, const std::string& type)
{
    std::string subckt;
    float voltage_range = 0;
    
    std::vector<std::pair<float, float> > rising;
    std::vector<std::pair<float, float> > falling;
    std::vector<std::pair<float, float> > pulldown;
    std::vector<std::pair<float, float> > pullup;
    std::vector<std::pair<float, float> > gnd_clamp;
    std::vector<std::pair<float, float> > power_clamp;
    
    char buf[2048];
    
    if (submodel.rising_waveforms.empty())
    {
        rising = _ramp_to_waveform(submodel.ramp, true, type);
    }
    else
    {
        rising = _read_two_col(submodel.rising_waveforms[0].values, type);
    }
    
    if (submodel.falling_waveforms.empty())
    {
        falling = _ramp_to_waveform(submodel.ramp, false, type);
    }
    else
    {
        falling = _read_two_col(submodel.falling_waveforms[0].values, type);
    }
    
    if (!submodel.pullup.empty())
    {
        pullup = _read_two_col(submodel.pullup, type);
    }
    
    if (!submodel.pulldown.empty())
    {
        pulldown = _read_two_col(submodel.pulldown, type);
    }
    
    if (!submodel.power_clamp.empty())
    {
        power_clamp = _read_two_col(submodel.power_clamp, type);
    }
    
    if (!submodel.gnd_clamp.empty())
    {
        gnd_clamp = _read_two_col(submodel.gnd_clamp, type);
    }
        
    if (type == "typ")
    {
        voltage_range = std::atof(model.voltage_range.typ.c_str());
    }
    else if (type == "min")
    {
        voltage_range = std::atof(model.voltage_range.min.c_str());
    }
    else if (type == "max")
    {
        voltage_range = std::atof(model.voltage_range.max.c_str());
    }
    
    if (!rising.empty() && !falling.empty())
    {
        sprintf(buf, "B%s1 WAVEFORM%s VSS V=(V(S) > 0.5)? V(RISING%s) * %f: V(FALLING%s) * %f\n", 
                _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str(),
                _make_upper(submodel.name).c_str(), voltage_range,
                _make_upper(submodel.name).c_str(), voltage_range);
    
        subckt = subckt + buf;
        
        if (!pullup.empty())
        {
            sprintf(buf, "B%s2 PULLUP%s1 VSS V =(V(WAVEFORM%s, DIE) > 0)? V(PULLUP%s, VSS): 0\n", 
                    _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str(),
                    _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str());
            subckt = subckt + buf;
            
            sprintf(buf, "G%s1 DIE VCC PULLUP%s1 VSS 1\n", _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str());
            subckt = subckt + buf;
        }
        
        if (!pulldown.empty())
        {
            sprintf(buf, "B%s3 PULLDOWN%s1 VSS V =(V(DIE, WAVEFORM%s) > 0)? V(PULLDOWN%s, VSS): 0\n",
                        _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str(),
                        _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str());
            subckt = subckt + buf;
            
            sprintf(buf, "G%s2 DIE VSS PULLDOWN%s1 VSS 1\n", _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str());
            subckt = subckt + buf;
        }
        
        subckt = subckt + "\n";
        subckt = subckt + "\n";
    }
        
    
    
    if (!rising.empty() && !falling.empty())
    {
        float maxv = -100000;
        float minv = 10000;
        
        for (const auto& tmp: rising)
        {
            if (maxv < tmp.second)
            {
                maxv = tmp.second;
            }
            if (minv > tmp.second)
            {
                minv = tmp.second;
            }
        }
        
        for (const auto& tmp: falling)
        {
            if (maxv < tmp.second)
            {
                maxv = tmp.second;
            }
            if (minv > tmp.second)
            {
                minv = tmp.second;
            }
        }
        
        sprintf(buf, "B%s4 FALLING%s 0 V = PWL(V(T),\n", _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str());
        subckt = subckt + buf;
        for (const auto& v: falling)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, (v.second - minv) / (maxv - minv));
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
        
        sprintf(buf, "B%s5 RISING%s 0 V = PWL(V(T),\n", _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str());
        subckt = subckt + buf;
        for (const auto& v: rising)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, (v.second - minv) / (maxv - minv));
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
        
        
        if (!pullup.empty())
        {
            sprintf(buf, "B%spullup PULLUP%s VSS V= PWL(V(WAVEFORM%s, DIE),\n",
                _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str());
            subckt = subckt + buf;
            for (const auto& v: pullup)
            {
                sprintf(buf, "+ %e,%e,\n", v.first, v.second);
                subckt = subckt + buf;
            }
            subckt.pop_back();
            subckt.pop_back();
            subckt = subckt + ")\n\n\n";
        }
        
        if (!pulldown.empty())
        {
            sprintf(buf, "B%spulldown PULLDOWN%s VSS V= PWL(V(DIE, WAVEFORM%s),\n",
                _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str(), _make_upper(submodel.name).c_str());
            subckt = subckt + buf;
            for (const auto& v: pulldown)
            {
                sprintf(buf, "+ %e,%e,\n", v.first, v.second);
                subckt = subckt + buf;
            }
            subckt.pop_back();
            subckt.pop_back();
            subckt = subckt + ")\n\n\n";
        }
    }
    
    if (!power_clamp.empty())
    {
        sprintf(buf, "B%spower_clamp DIE VCC I= PWL(V(VCC, DIE),\n", _make_upper(submodel.name).c_str());
        subckt = subckt + buf;
        for (const auto& v: power_clamp)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, v.second);
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
    }
        
    if (!gnd_clamp.empty())
    {
        sprintf(buf, "B%sgnd_clamp DIE VSS I= PWL(V(DIE, VSS),\n", _make_upper(submodel.name).c_str());
        subckt = subckt + buf;
        for (const auto& v: gnd_clamp)
        {
            sprintf(buf, "+ %e,%e,\n", v.first, v.second);
            subckt = subckt + buf;
        }
        subckt.pop_back();
        subckt.pop_back();
        subckt = subckt + ")\n\n\n";
    }
    return subckt;
}


std::vector<std::string> ibis2spice::_string_split(std::string str, const std::string& key)
{
    std::vector<std::string> out;
    std::string::size_type begin = 0;
    std::string::size_type end = 0;
    while ((end = str.find(key, begin)) != str.npos)
    {
        out.push_back(str.substr(begin, end));
        begin = end + key.size();
    }
    if (begin < str.size())
    {
        out.push_back(str.substr(begin, end));
    }
    
    return out;
}



float ibis2spice::_suffix_to_value(const std::string& suffix)
{
    switch (suffix[0])
    {
        case 'T': return 1e12;
        case 'G': return 1e9;
        case 'M': return 1e6;
        case 'k': return 1e3;
        case 'm': return 1e-3;
        case 'u': return 1e-6;
        case 'n': return 1e-9;
        case 'p': return 1e-12;
        case 'f': return 1e-15;
    }
    return 1;
}


float ibis2spice::_string_to_float(const std::string& str)
{
    std::string num;
    std::string suffix;
    for (const auto& c: str)
    {
        if (c == '+' || c == '-' || c == '.'
            || c == 'e' || c == 'E' 
            || (c >= '0' && c <= '9'))
        {
            num.push_back(c);
        }
        else
        {
            break;
        }
    }
    suffix = str.substr(num.size(), str.npos);
    
    return std::atof(num.c_str()) * _suffix_to_value(suffix);
    
}

std::vector<std::pair<float, float> > ibis2spice::_ramp_to_waveform(const ibis::ramp& ramp, bool rising_or_falling, const std::string& type)
{
    std::vector<std::pair<float, float> > out;
    if (rising_or_falling)
    {
        std::string dv_dt_r = _read_value_string(ramp.dv_dt_r, type);
        std::vector<std::string> v_str = _string_split(dv_dt_r, "/");
        if (v_str.size() != 2)
        {
            return out;
        }
        float dt = _string_to_float(v_str[1]);
        dt = dt + dt * 0.4;
        out.push_back(std::pair<float, float>(0, 0));
        out.push_back(std::pair<float, float>(dt, 1.0));
        out.push_back(std::pair<float, float>(dt + dt * 0.1, 1.0));
    }
    else
    {
        std::string dv_dt_f = _read_value_string(ramp.dv_dt_f, type);
        std::vector<std::string> v_str = _string_split(dv_dt_f, "/");
        if (v_str.size() != 2)
        {
            return out;
        }
        float dt = _string_to_float(v_str[1]);
        dt = dt + dt * 0.4;
        out.push_back(std::pair<float, float>(0, 1.0));
        out.push_back(std::pair<float, float>(dt, 0));
        out.push_back(std::pair<float, float>(dt + dt * 0.1, 0));
    }
    return out;
}


std::string ibis2spice::_read_value_string(const ibis::value_string& value, const std::string& type)
{
    if (type == "min")
    {
        return value.min;
    }
    else if (type == "max")
    {
        return value.max;
    }
    return value.typ;
}


std::vector<std::pair<float, float> > ibis2spice::_read_two_col(std::vector<ibis::value_string> values, const std::string& type)
{
    std::vector<std::pair<float, float> > out;
    
    for (const auto& value: values)
    {
        std::pair<float, float>  v;
        v.first = _string_to_float(value.ref);
        
        std::string val = _read_value_string(value, type);
        if (val == "NA")
        {
            continue;
        }
        v.second = _string_to_float(val);
        out.push_back(v);
    }
    return out;
}



std::string ibis2spice::_make_upper(const std::string& str)
{
    std::string out = str;
    std::transform(out.begin(), out.end(), out.begin(), ::toupper);
    return out;
}