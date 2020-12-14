#pragma once

#include "intercept.hpp"
#include "sector.hpp"
#include "sector_manager.hpp"
#include "../json/single_include/nlohmann/json.hpp"

using namespace intercept;

intercept::types::registered_sqf_function nular_func_example;
intercept::types::registered_sqf_function nular_func_start;
intercept::types::registered_sqf_function binary_func_generate_sectors;
intercept::types::registered_sqf_function nular_func_cleanup_sectors;
intercept::types::registered_sqf_function unary_func_generate_sectors;
intercept::types::registered_sqf_function binary_func_get_adjacent_sectors;


bool enableFrameNumbers = false;
mission::sector_manager manager;

int intercept::api_version() { //This is required for the plugin to work.
    return INTERCEPT_SDK_API_VERSION;
}

void intercept::register_interfaces() {
    
}

void intercept::on_frame() {
	if(enableFrameNumbers)
		sqf::system_chat(fmt::format("Frame number {}", sqf::diag_frameno()));
}

void intercept::post_init() {
    sqf::diag_log(fmt::format("Game world is: {} and its size is", sqf::world_name(), sqf::world_size()));
}

game_value generate_sectors(game_value_parameter sector_length, game_value_parameter building_threshold)
{
	intercept::sqf::system_chat(fmt::format("length: {}, threshold: {}", static_cast<int>(sector_length), static_cast<int>(building_threshold)));
    manager.generate_sectors();
    return "Done!";
}

game_value cleanup_sectors()
{
    manager.cleanup();
    return "Done!";
}

game_value find_closest_sector(game_value_parameter pos)
{
	mission::sector* result = manager.find_nearest_sector(pos);
    return {result->pos->x, result->pos->y};
}

game_value get_adjacent_sectors(game_state& gs, game_value_parameter left, game_value_parameter right)
{
    if (right.size() < 2) throw;
    const vector3 left_arg = left;
    auto result = manager.find_adjacent_sectors(vector2_base<int>(left_arg.x, left_arg.y), right[0], right[1]);
    std::vector<vector3> output;
    for (auto value : result)
    {
        output.push_back(*value->pos);
    }
    return output;
}


game_value test_nular_fnc() {
    enableFrameNumbers = !enableFrameNumbers;
    if (enableFrameNumbers)
        return "enabled";
    else
        return "disabled";
}

game_value start_scan()
{
    return manager.start_sector_scanner();
}

void intercept::pre_start() {
    nular_func_example = intercept::client::host::register_sqf_command(
            "enableFrameDiag",
            "Enables or disables the output of the current frame number",
            userFunctionWrapper<test_nular_fnc>,
            intercept::types::GameDataType::STRING);
    nular_func_cleanup_sectors = intercept::client::host::register_sqf_command(
        "missionCleanupSectors",
        "cleans up all sector markers",
        userFunctionWrapper<cleanup_sectors>,
        game_data_type::STRING
    );
    binary_func_generate_sectors = intercept::client::host::register_sqf_command(
        "missionGenerateSectors",
        "Generates new sectors",
        userFunctionWrapper<generate_sectors>,
        game_data_type::STRING,
        game_data_type::SCALAR,
        game_data_type::SCALAR
    );
    unary_func_generate_sectors = intercept::client::host::register_sqf_command(
        "missionFindClosestSector",
        "finds the closest sector to a passed position",
        userFunctionWrapper<find_closest_sector>,
        game_data_type::ARRAY,
        game_data_type::ARRAY
    );
    binary_func_get_adjacent_sectors = intercept::client::host::register_sqf_command(
        "missionGenerateSectors",
        "Generates new sectors",
        get_adjacent_sectors,
        game_data_type::ARRAY,
        game_data_type::ARRAY,
        game_data_type::ARRAY
    );
    nular_func_start = intercept::client::host::register_sqf_command(
        "missionStartScan",
        "start background scan for sector activation",
        userFunctionWrapper<start_scan>,
        game_data_type::BOOL
    );
	/*
	 *
    _binary_func_get_adjacent_sectors = client::host::register_sqf_command(
		"missionGetAdjacentSectors",
        "Get all adjacent sectors in range passed",
        get_adjacent_sectors,
        game_data_type::ARRAY,
        game_data_type::ARRAY
    );*/
}