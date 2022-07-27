#include <algorithm>
#include "ibis2spice.h"

ibis2spice::ibis2spice()
{
}


ibis2spice::~ibis2spice()
{
}



bool ibis2spice::load(const char * ibis_file)
{
    return _ibs.load(ibis_file);
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
    else if (model.model_type == "Output")
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
    
    rising = _read_two_col(model.rising_waveforms[0].values, type);
    falling = _read_two_col(model.falling_waveforms[0].values, type);
    
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
        r_pkg = component.package_.r_pkg.typ;
        l_pkg = component.package_.l_pkg.typ;
        c_pkg = component.package_.c_pkg.typ;
        c_comp = model.c_comp.typ;
        voltage_range = std::atof(model.voltage_range.typ.c_str());
        
    }
    else if (type == "min")
    {
        r_pkg = component.package_.r_pkg.min;
        l_pkg = component.package_.l_pkg.min;
        c_pkg = component.package_.c_pkg.min;
        c_comp = model.c_comp.min;
        voltage_range = std::atof(model.voltage_range.min.c_str());
    }
    else if (type == "max")
    {
        r_pkg = component.package_.r_pkg.max;
        l_pkg = component.package_.l_pkg.max;
        c_pkg = component.package_.c_pkg.max;
        c_comp = model.c_comp.max;
        voltage_range = std::atof(model.voltage_range.max.c_str());
    }
        
    sprintf(buf, "*** vinh:%s vinl:%s ***\n", model.v_in_h.c_str(), model.v_in_l.c_str());
    subckt = subckt + buf;
    
    sprintf(buf, ".SUBCKT %s_%s_OUTPUT OUT VCC VSS freq=800Meg duty=0.5\n", _make_upper(model.name).c_str(), _make_upper(type).c_str());
    subckt = subckt + buf;
    subckt = subckt + "\n";

    subckt = subckt + "V1 S VSS PWL(0 1 {(duty / freq)} 1 {(duty / freq)} 0 {(1 / freq)} 0) r=0\n";
    subckt = subckt + "V2 T VSS PWL(0 0 {(duty / freq)} {(duty / freq)} {(duty / freq)} 0 {(1 / freq)} {((1 - duty) / freq)} ) r=0\n";
        
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
    
    subckt = subckt + "B4 FALLING VSS V = PWL(V(T),\n";
    for (const auto& v: falling)
    {
        sprintf(buf, "+ %e,%e,\n", v.first, (v.second - minv) / (maxv - minv));
        subckt = subckt + buf;
    }
    subckt.pop_back();
    subckt.pop_back();
    subckt = subckt + ")\n\n\n";
        
    subckt = subckt + "B5 RISING VSS V = PWL(V(T),\n";
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
        r_pkg = component.package_.r_pkg.typ;
        l_pkg = component.package_.l_pkg.typ;
        c_pkg = component.package_.c_pkg.typ;
        c_comp = model.c_comp.typ;
        
    }
    else if (type == "min")
    {
        r_pkg = component.package_.r_pkg.min;
        l_pkg = component.package_.l_pkg.min;
        c_pkg = component.package_.c_pkg.min;
        c_comp = model.c_comp.min;
    }
    else if (type == "max")
    {
        r_pkg = component.package_.r_pkg.max;
        l_pkg = component.package_.l_pkg.max;
        c_pkg = component.package_.c_pkg.max;
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
            if (_ibs.get_model_by_name(model.add_submodels[0].submodel_name, submodel))
            {
                subckt = subckt + _gen_ngspice_add_submodel(component, submodel, type);
            }
        }
    }
    subckt = subckt + ".ENDS\n";
    subckt = subckt + "\n";
    subckt = subckt + "\n";
    
    return subckt;
}



std::string ibis2spice::_gen_ngspice_add_submodel(ibis::component& component, ibis::model submodel, std::string type)
{
    if (submodel.model_type == "Dynamic_clamp")
    {
        return _gen_ngspice_add_submodel_dynamic_clamp(component, submodel, type);
    }
    return "";
}

std::string ibis2spice::_gen_ngspice_add_submodel_dynamic_clamp(ibis::component& component, ibis::model submodel, std::string type)
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



std::vector<std::pair<float, float> > ibis2spice::_read_two_col(std::vector<ibis::value_string> values, const std::string& type)
{
    std::vector<std::pair<float, float> > out;
    
    for (const auto& value: values)
    {
        std::pair<float, float>  v;
        v.first = std::atof(value.ref.c_str());
        
        if (type == "min")
        {
            if (value.min == "NA")
            {
                continue;
            }
            v.second = std::atof(value.min.c_str());
        }
        else if (type == "max")
        {
            if (value.max == "NA")
            {
                continue;
            }
            v.second = std::atof(value.max.c_str());
        }
        else
        {
            if (value.typ == "NA")
            {
                continue;
            }
            v.second = std::atof(value.typ.c_str());
        }
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