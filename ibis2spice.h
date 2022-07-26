#ifndef __IBIS2SPICE_H__
#define __IBIS2SPICE_H__

#include "ibis.h"

class ibis2spice
{
public:
    ibis2spice();
    ~ibis2spice();
    
public:
    bool load(const char *ibis_file);
    std::vector<std::string> get_component_names() { return _ibs.get_component_names(); }
    std::vector<std::string> get_model_names() { return _ibs.get_model_names(); }
    std::vector<std::string> get_submodel_names() { return _ibs.get_submodel_names(); }
    
    std::string gen_spice_model_by_model(const std::string& component_name, const std::string& model_name, const std::string& type);
    
private:
    std::string _gen_ngspice_model_by_model_output(ibis::component& component, ibis::model model, std::string type);
    std::string _gen_ngspice_model_by_model_input(ibis::component& component, ibis::model model, std::string type);
    
    std::string _gen_ngspice_add_submodel(ibis::component& component, ibis::model model, std::string type);
    std::string _gen_ngspice_add_submodel_dynamic_clamp(ibis::component& component, ibis::model model, std::string type);
    
    std::vector<std::pair<float, float> > _read_two_col(std::vector<ibis::value_string> values, const std::string& type);
    
    std::string _make_upper(const std::string& str);
private:
    ibis _ibs;
};

#endif