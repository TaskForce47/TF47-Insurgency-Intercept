#include "sector_manager.hpp"

#include "mission_config.hpp"

void mission::sector_manager::sector_scanner()
{
	intercept::client::invoker_lock thread_lock;
	while (! sector_scanner_stop_signal)
	{
		thread_lock.lock();

		auto players = mission::library::get_players();
		const auto time = intercept::sqf::time();
		std::vector<vector3> players_pos;
		for (auto& player : players)
		{
			players_pos.push_back(intercept::sqf::get_pos(player));
		}

		intercept::sqf::system_chat("Running scan loop!");
		thread_lock.unlock();
		for (auto& player_pos : players_pos)
		{
			auto* const nearest_sector = find_nearest_sector(player_pos);
			if (active_sectors.size() < max_active_sectors &&
				nearest_sector->center_pos->distance_2d(player_pos) < sector_trigger_distance)
			{
				const auto sectors_to_trigger = std::min(std::max(static_cast<int>(active_sectors.size()), 10), 10);

				auto sectors_to_activate = find_adjacent_sectors(nearest_sector->get_sector_identifier(), 500, sectors_to_trigger);

				for (auto& sector : sectors_to_activate)
				{
					if (sector->is_active) continue;

					sector->is_active = true;
					sector->show_marker();
					mission::sector_timestamp timestamp;
					timestamp.timestamp = time;
					timestamp.sector = sector;

					active_sectors.push_back(timestamp);
				}
			}
		}

		//scan for sectors to deactivate!
		for (auto i = 0; i < static_cast<int>(active_sectors.size()); i++)
		{
			if (active_sectors[i].timestamp + time_to_deactivate < time)
			{
				auto* const sector_to_test = active_sectors[i].sector;
				auto flag = false;
				for (auto& player_pos : players_pos)
				{
					if (sector_to_test->center_pos->distance_2d(player_pos) < sector_trigger_distance)
					{
						flag = true;
						break;
					}
				}

				if (!flag)
				{
					sector_to_test->is_active = false;
					sector_to_test->hide_marker();
					active_sectors.erase(active_sectors.begin() + i);
					i--;
				}
			}
		}
		SLEEP(1000 * 5);
	}
	std::cout << "Stopping worker Thread!";
	sector_scanner_stop_signal = false;
}

void mission::sector_manager::generate_sectors()
{
	auto* const config_data = mission_config::get();
	const auto world_size = intercept::sqf::world_size();
	cell_count = static_cast<int>(world_size / config_data->sector_size);
	sectors.resize(cell_count);
	for (auto i = 0; i < cell_count; i++)
		sectors[i].resize(cell_count);

	for (auto i = 0; i < cell_count; i++)
	{
		for (auto j = 0; j < cell_count; j++)
		{
			sectors[i][j] = new mission::sector(
				config_data->sector_size,
				new vector3(i * config_data->sector_size, j * config_data->sector_size, 0),
				new vector2_base<int>(i, j));
		}
	}
	const std::vector<std::string> items = { "HOUSE", "BUILDING", "CHURCH", "CHAPEL", "BUNKER", "CROSS", "FORTRESS", "FOUNTAIN", "LIGHTHOUSE", "VIEW-TOWER", "FUELSTATION", "HOSPITAL", "RUIN", "POWERSOLAR" };

	auto terrain_objects = intercept::sqf::nearest_terrain_objects(
		vector3(world_size / 2, world_size / 2, 0),
		items,
		world_size
	);

	intercept::sqf::system_chat(fmt::format("We have {} objects", terrain_objects.capacity()));

	for (auto& terrain_object : terrain_objects)
	{
		const auto pos = intercept::sqf::get_pos(terrain_object);
		if (pos.x < 0 || pos.x > world_size || pos.y < 0 || pos.y > world_size) continue;
		int x = pos.x / config_data->sector_size;
		int y = pos.y / config_data->sector_size;
		sectors[x][y]->building_count++;
	}


	for (auto i = 0; i < cell_count; i++)
	{
		for (auto j = 0; j < cell_count; j++)
		{
			sector* sector = sectors[i][j];
			if (sector->building_count > config_data->sector_building_threshold)
			{
				game_sectors.push_back(sector);
				sector->setup_marker();
			}
		}
	}
}

void mission::sector_manager::cleanup()
{
	for (auto i = 0; i < cell_count; i++)
	{
		for (auto j = 0; j < cell_count; j++)
		{
			if (sectors[i][j]->sector_maker.empty()) continue;
			intercept::sqf::delete_marker(sectors[i][j]->sector_maker);
		}
	}
}

mission::sector* mission::sector_manager::find_nearest_sector(const vector3_base<float> pos)
{
	mission::sector* closest = nullptr;
	auto distance = 9999999;
	for (auto& game_sector : game_sectors)
	{
		const auto temp = pos.distance_2d(*game_sector->center_pos);
		if (temp <= distance)
		{
			closest = game_sector;
			distance = temp;
		}
	}
	return closest;
}

mission::sector* mission::sector_manager::get_sector_by_id(vector2_base<int> sector)
{
	return sectors[sector.x][sector.y];
}

mission::sector* mission::sector_manager::get_sector_by_id(vector2_base<float> sector)
{
	return sectors[(int)sector.x][(int)sector.y];
}

std::vector<mission::sector*> mission::sector_manager::find_adjacent_sectors(const vector2_base<int> sector, const int max_range = 500, const int max = 10)
{
	mission::sector* const starting_sector = get_sector_by_id(sector);
	const auto starting_pos = starting_sector->get_sector_identifier();

	auto left_bound = starting_pos.x - max_range / 100 / 2;
	if (left_bound < 0) left_bound = 0;
	auto right_bound = starting_pos.x + max_range / 100 / 2;
	if (right_bound > cell_count) right_bound = cell_count;
	auto lower_bound = starting_pos.y + max_range / 100 / 2;
	if (lower_bound > cell_count) lower_bound = cell_count;
	auto upper_bound = starting_pos.y - max_range / 100 / 2;
	if (upper_bound < 0) upper_bound = 0;

	std::vector<mission::sector*> output;

	for (auto i = left_bound; i < right_bound; i++)
	{
		for (auto j = upper_bound; j < lower_bound; j++)
		{
			if (sectors[i][j]->sector_maker.empty()) continue;

			output.push_back(sectors[i][j]);
		}
	}

	while (max < (int)output.size())
	{
		//find the most far sector from starting pos and eliminate
		auto distance = -1;
		auto sector_to_eliminate = -1;
		for (auto i = 0; i < (int)output.size(); i++)
		{
			const auto current_distance = output[i]->pos->distance_2d(*starting_sector->pos);
			if (current_distance > distance) {
				distance = current_distance;
				sector_to_eliminate = i;
			}
		}
		output.erase(output.begin() + sector_to_eliminate);
	}

	return output;
}

bool mission::sector_manager::start_sector_scanner()
{
	if (sector_scanner_running) return false;

	sector_scanner_running = true;
	scanner_thread = new std::thread([this] { sector_scanner();  });
	scanner_thread->detach();
	return true;
}

bool mission::sector_manager::stop_sector_scanner()
{
	if (!sector_scanner_running) return false;

	sector_scanner_stop_signal = true;
	while (sector_scanner_stop_signal)
		SLEEP(10);
	sector_scanner_running = false;

	return true;
}

mission::sector_manager::sector_manager()
{
	sector_length = 100;
	max_active_sectors = 100;
	sector_trigger_distance = 500;
	aggression = 0;
	time_to_deactivate = 30;
}

mission::sector_manager::~sector_manager()
{
	if (sector_scanner_running)
		delete scanner_thread;
}
