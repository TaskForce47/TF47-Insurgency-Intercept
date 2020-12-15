#include "cache_manager.hpp"

#include <random>


#include "mission_config.hpp"
#include "sector_manager.hpp"

mission::cache_manager::cache_manager()
{
}

mission::cache_manager::~cache_manager()
{
}

void mission::cache_manager::generate_caches(std::vector<sector*> sectors)
{
	srand(time(nullptr));
	std::vector<sector*> target_sectors;
	while (target_sectors.size() < mission_config::get()->max_caches)
	{
		target_sectors.push_back(sectors[rand() % sectors.size()]);
	}

	for (auto* target_sector : target_sectors)
	{
		const std::vector<std::string> items = { "HOUSE", "BUILDING", "CHURCH", "CHAPEL", "BUNKER", "HOSPITAL", "RUIN", "POWERSOLAR" };
		auto terrain_objects = intercept::sqf::nearest_terrain_objects(
			*target_sector->center_pos,
			items,
			100
		);

		for (auto terrain_object : terrain_objects)
		{
			auto positions = intercept::sqf::building_pos(terrain_object, -1);
			if (!positions.empty())
			{
				const auto random_pos = positions[rand() % positions.size()];
				spawn_cache(random_pos);
				break;
			}
		}
	}
}

void mission::cache_manager::handle_cache_destroyed(object unit, object source, float damage, object instigator)
{
	for (auto* cache : caches)
	{
		if (intercept::sqf::is_equal_to(unit, cache->cache_object))
		{
			delete &cache->event_handler;
			cache->state = false;

			return;
		}
	}
}

void mission::cache_manager::spawn_cache(vector3 pos)
{
	cache new_cache;
	new_cache.cache_pos = pos;
	new_cache.cache_object = intercept::sqf::create_vehicle(mission_config::get()->cache_classname,
		new_cache.cache_pos);

	//std::function f_handle_cache_destroyed = std::bind<void(int)>(&handle_cache_destroyed, &new_cache.cache_object);
	new_cache.event_handler =
		intercept::client::addEventHandler<intercept::client::eventhandlers_object::Hit>(
			new_cache.cache_object, handle_cache_destroyed);

	caches.push_back(&new_cache);
}

void mission::cache_manager::create_hint()
{
	std::vector<cache*> not_completed_caches;
	for (auto& cache : caches)
	{
		if (cache->state && cache->hint_markers.size() < mission_config::get()->max_hints)
		{
			not_completed_caches.push_back(cache);
		}
	}
	if (not_completed_caches.empty()) return;

	srand(time(nullptr));
	const auto random_index = rand() % not_completed_caches.size();
	auto* random_cache = not_completed_caches[random_index];

	if (random_cache->hint_markers.size() < 4) 
	{
		const auto distance = rand() % (3000 - 1200) + 1200;
		const auto marker_pos = intercept::sqf::get_pos(random_cache->cache_pos, distance, rand() % 360);
		const auto marker = intercept::sqf::create_marker(
			"df",
			marker_pos);
		intercept::sqf::set_marker_type(marker, "HD_DOT");
		random_cache->hint_markers.push_back(marker_pos);
	}
	if (random_cache->hint_markers.size() > 3 && random_cache->hint_markers.size() < 6)
	{
		const auto distance = rand() % (2400 - 800) + 800;
		const auto marker_pos = intercept::sqf::get_pos(random_cache->cache_pos, distance, rand() % 360);
		const auto marker = intercept::sqf::create_marker(
			"df",
			marker_pos);
		intercept::sqf::set_marker_type(marker, "HD_DOT");
		random_cache->hint_markers.push_back(marker_pos);
	}
	if (random_cache->hint_markers.size() > 5)
	{
		const auto distance = rand() % (1000 - 250) + 250;
		const auto marker_pos = intercept::sqf::get_pos(random_cache->cache_pos, distance, rand() % 360);
		const auto marker = intercept::sqf::create_marker(
			"df",
			marker_pos);
		intercept::sqf::set_marker_type(marker, "HD_DOT");
		random_cache->hint_markers.push_back(marker_pos);
	}
}

bool mission::cache_manager::is_completed()
{
	for (auto* cache : caches)
	{
		if (!cache->state) return false;
	}
	return true;
}

