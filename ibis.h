#ifndef __IBIS_H__
#define __IBIS_H__
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <queue>

class ibis_text_readline
{
public:
    ibis_text_readline();
    ~ibis_text_readline();
    
public:
    void load(const char *text, std::uint32_t len);
    std::string read_line();
    std::string look_line();
    
private:
    const char *_text;
    std::uint32_t _len;
    const char *_cur;
};

class ibis_readline
{
public:
    ibis_readline();
    ~ibis_readline();
    
public:
    void load(const char *text, std::uint32_t len);
    std::vector<std::string> read_line();
    std::vector<std::string> look_line();
    /* 下一行是否是tag */
    bool look_is_tag();
    static bool is_tag(const std::vector<std::string>& v_str);
    
private:
    std::vector<std::string> _split(const std::string& str);
    
private:
    ibis_text_readline _readline;
};


class ibis
{
public:
    struct value_string
    {
        std::string ref;
        std::string typ;
        std::string min;
        std::string max;
    };
    
    struct package
    {
        value_string r_pkg;
        value_string l_pkg;
        value_string c_pkg;
    };
    
    
    struct pin
    {
        std::string pin_name;
        std::string signal_name;
        std::string model_name;
        std::string r_pin;
        std::string l_pin;
        std::string c_pin;
    };
    
    
    struct pin_mapping
    {
        std::string pin_name;
        std::string pulldown_ref;
        std::string pullup_ref;
        std::string gnd_clamp_ref;
        std::string power_clamp_ref;
        std::string ext_ref;
    };
    
    
    struct diff_pin
    {
        std::string pin_name;
        std::string inv_pin_name;
        std::string vdiff;
        std::string tdelay_typ;
        std::string tdelay_min;
        std::string tdelay_max;
    };
    
    struct component
    {
        std::string name;
        std::string package_model;
        std::string manufacturer;
        package package_;
        std::vector<pin> pins;
        std::vector<pin_mapping> pin_mappings;
        std::vector<diff_pin> diff_pins;
    };
    
    struct model_selector
    {
        struct item
        {
            std::string model_name;
            std::string desc;
        };
        
        std::string name;
        std::vector<item> items;
    };
    
    struct model_spec
    {
        value_string v_in_l;
        value_string v_in_h;
        value_string v_meas;
        value_string v_ref;
    };
    
    struct receiver_thresholds
    {
        std::string v_th;
        std::string v_th_min;
        std::string v_th_max;
        std::string v_inh_ac;
        std::string v_inh_dc;
        std::string v_inl_ac;
        std::string v_inl_dc;
        std::string tslew_ac;
        std::string threshold_sensitivity;
        std::string reference_supply;
    };
    
    struct ramp
    {
        std::string r_load;
        value_string dv_dt_r;
        value_string dv_dt_f;
    };
    
    struct waveform
    {
        std::string v_fixture;
        std::string v_fixture_min;
        std::string v_fixture_max;
        std::string r_fixture;
        std::vector<value_string> values;
    };
    
    struct add_submodel
    {
        std::string submodel_name;
        std::string mode;
    };
    
    struct model
    {
        model(bool is_submodel_ = false) : is_submodel(is_submodel_) {}
        bool is_submodel;
        std::string name;
        std::string model_type;
        
        std::string v_in_l;
        std::string v_in_h;
        std::string v_meas;
        std::string v_ref;
        std::string c_ref;
        std::string r_ref;
        
        value_string c_comp;
        
        model_spec mode_specs;
        receiver_thresholds r_thresh;
        value_string voltage_range;
        value_string pullup_reference;
        value_string temperature_range;
        
        std::vector<value_string> pulldown;
        std::vector<value_string> gnd_clamp;
        std::vector<value_string> pullup;
        std::vector<value_string> power_clamp;
        ramp ramp_;
        std::vector<waveform> falling_waveforms;
        std::vector<waveform> rising_waveforms;
        std::vector<add_submodel> add_submodels;
    };
    
public:
    ibis();
    ~ibis();
    
public:
    bool load(const char *ibis_file);
    
    std::vector<std::string> get_model_names();
    std::vector<std::string> get_submodel_names();
    
    bool get_model_by_name(const std::string& name, ibis::model& model);
    bool get_component_by_name(const std::string& name, ibis::component& component);
    
private:
    std::string _make_lower(const std::string& str);
    void _load_string(std::string& str, const std::vector<std::string>& v_str);
    std::string _merge_v_str(const std::vector<std::string>& v_str, std::uint32_t begin = 0);
    std::string _read_n(const std::vector<std::string>& v_str, std::uint32_t n);
    
    void _load_ibis_ver(const std::vector<std::string>& v_str);
    void _load_file_name(const std::vector<std::string>& v_str);
    void _load_date(const std::vector<std::string>& v_str);
    void _load_file_rev(const std::vector<std::string>& v_str);
    void _load_source(const std::vector<std::string>& v_str);
    void _load_notes(const std::vector<std::string>& v_str);
    void _load_disclaimer(const std::vector<std::string>& v_str);
    void _load_copyright(const std::vector<std::string>& v_str);
    
    void _load_component(const std::vector<std::string>& v_str);
    void _load_package_model(const std::vector<std::string>& v_str);
    void _load_manufacturer(const std::vector<std::string>& v_str);
    void _load_package(const std::vector<std::string>& v_str);
    void _load_pin(const std::vector<std::string>& v_str);
    void _load_pin_mapping(const std::vector<std::string>& v_str);
    void _load_diff_pin(const std::vector<std::string>& v_str);
    void _load_model_selector(const std::vector<std::string>& v_str);
    
    void _load_model(const std::vector<std::string>& v_str);
    void _load_model_spec(const std::vector<std::string>& v_str);
    void _load_receiver_thresholds(const std::vector<std::string>& v_str);
    void _load_voltage_range(const std::vector<std::string>& v_str);
    void _load_pullup_reference(const std::vector<std::string>& v_str);
    void _load_temperature_range(const std::vector<std::string>& v_str);
    void _load_add_submodel(const std::vector<std::string>& v_str);
    void _load_pulldown(const std::vector<std::string>& v_str);
    void _load_gnd_clamp(const std::vector<std::string>& v_str);
    void _load_pullup(const std::vector<std::string>& v_str);
    void _load_power_clamp(const std::vector<std::string>& v_str);
    void _load_ramp(const std::vector<std::string>& v_str);
    void _load_falling_waveform(const std::vector<std::string>& v_str);
    void _load_rising_waveform(const std::vector<std::string>& v_str);
    
    void _load_submodel(const std::vector<std::string>& v_str);
    
private:
    ibis_readline _readline;
    std::vector<char> _text_buf;
    
    std::string _ibis_version;
    std::string _file_name;
    std::string _file_revision;
    std::string _date;
    std::string _source;
    std::string _notes;
    std::string _disclaimer;
    std::string _copyright;
    
    std::vector<component> _components;
    std::vector<model_selector> _model_selectors;
    std::vector<model> _models;
};

#endif