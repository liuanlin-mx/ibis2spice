#ifndef __IBIS2SPICE_H__
#define __IBIS2SPICE_H__

#include "ibis.h"

class ibis2spice
{
public:
    ibis2spice();
    ~ibis2spice();
    
public:
    bool load(const char *ibis_content);
    std::vector<std::string> get_component_names() { return _ibs.get_component_names(); }
    std::vector<std::string> get_model_names() { return _ibs.get_model_names(); }
    std::vector<std::string> get_submodel_names() { return _ibs.get_submodel_names(); }
    
    std::string gen_spice_model_by_model(const std::string& component_name, const std::string& model_name, const std::string& type);
    
private:
    std::string _gen_ngspice_model_by_model_output(ibis::component& component, ibis::model model, std::string type);
    std::string _gen_ngspice_model_by_model_input(ibis::component& component, ibis::model model, std::string type);
    
    std::string _gen_ngspice_add_submodel_input(ibis::component& component, ibis::model& model, ibis::model& submodel, const std::string& type);
    std::string _gen_ngspice_add_submodel_output(ibis::component& component, ibis::model& model, ibis::model& submodel, const std::string& type);
    
    std::vector<std::string> _string_split(std::string str, const std::string& key);
    float _suffix_to_value(const std::string& suffix);
    float _string_to_float(const std::string& str);
    std::vector<std::pair<float, float> > _ramp_to_waveform(const ibis::ramp& ramp, bool rising_or_falling, const std::string& type);
    std::string _read_value_string(const ibis::value_string& value, const std::string& type);
    std::vector<std::pair<float, float> > _read_two_col(std::vector<ibis::value_string> values, const std::string& type);
    
    std::string _make_upper(const std::string& str);
private:
    ibis _ibs;
};

#endif