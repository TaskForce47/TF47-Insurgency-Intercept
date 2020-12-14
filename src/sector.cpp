#include "sector.hpp"

std::vector<object> mission::library::get_players()
{
	std::vector<intercept::types::object> result;
	auto players = intercept::sqf::all_players();
	auto headless_clients = intercept::sqf::entities("HeadlessClient_F");
	for (auto player : players)
	{
		if (std::find(headless_clients.begin(), headless_clients.end(), player) != headless_clients.end()) continue;
		result.push_back(player);
	}
	return result;
}

vector2_base<int> mission::sector::get_sector_identifier() const
{
	return *identifier;
}

void mission::sector::show_marker() const
{
	if (sector_maker.empty()) return;

	intercept::sqf::set_marker_alpha(sector_maker, 0.8);
}

void mission::sector::hide_marker() const
{
	if (sector_maker.empty()) return;

	intercept::sqf::set_marker_alpha(sector_maker, 0.2);
}

void mission::sector::change_marker_side(const std::string side) const
{
	if (sector_maker.empty()) return;

	if (side == "Opfor")
		intercept::sqf::set_marker_color(sector_maker, "COLOROPFOR");
	if (side == "Bluefor")
		intercept::sqf::set_marker_color(sector_maker, "COLORBLUEFOR");
}

void mission::sector::clean_marker() const
{
	if (sector_maker.empty()) return;

	intercept::sqf::delete_marker(sector_maker);
}

void mission::sector::setup_marker()
{
	if (!sector_maker.empty())
		clean_marker();
	sector_maker = intercept::sqf::create_marker(fmt::format("SECTOR:{}-{}", pos->x, pos->y), *center_pos);
	intercept::sqf::set_marker_color(sector_maker, "colorOPFOR");
	intercept::sqf::set_marker_shape(sector_maker, "RECTANGLE");
	intercept::sqf::set_marker_size(sector_maker, vector2_base<float>(length / 2, length / 2));

	if (is_captured)
		intercept::sqf::set_marker_color(sector_maker, "COLORBLUEFOR");
	else
		intercept::sqf::set_marker_color(sector_maker, "COLOROPFOR");

	if (is_active || is_captured)
		show_marker();
	else
		hide_marker();
}



