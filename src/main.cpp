#include <intercept.hpp>

using namespace intercept;

intercept::types::registered_sqf_function _nular_func_example;


int intercept::api_version() { //This is required for the plugin to work.
    return INTERCEPT_SDK_API_VERSION;
}

void intercept::register_interfaces() {
    
}



void intercept::on_frame() {
    intercept::sqf::all_players();
    for(auto player : players) {

    }
    auto player = intercept::sqf::player();

}

void intercept::post_init() {

}

game_value test_nular_fnc() {
    return "test me oh yeah";
}

void intercept::pre_start() {
    _nular_func_example = intercept::client::host::register_sqf_command(
            "testcommand",
            "gives you some awesome string",
            userFunctionWrapper<test_nular_fnc>,
            intercept::types::GameDataType::STRING);
}