#pragma once
#include "sector.hpp"
#include "vector.hpp"

namespace mission
{
	class sector_manager
	{
	private:
		bool sector_scanner_running = false;
		bool sector_scanner_stop_signal = false;
		std::thread* scanner_thread;

		void sector_scanner();
	public:
		int sector_length;
		int cell_count;
		int max_active_sectors;
		int sector_trigger_distance;
		float aggression;
		float time_to_deactivate;

		std::vector<std::vector<mission::sector*>> sectors;
		std::vector<sector*> game_sectors;
		std::vector<mission::sector_timestamp> active_sectors;

		void generate_sectors(const int sector_length, int building_threshold);
		void cleanup();
		mission::sector* find_nearest_sector(vector3_base<float> pos);
		mission::sector* get_sector_by_id(vector2_base<int> sector);
		mission::sector* get_sector_by_id(vector2_base<float> sector);
		std::vector<mission::sector*> find_adjacent_sectors(const intercept::types::vector2_base<int> sector, const int max_range, const int max);

		bool start_sector_scanner();
		bool stop_sector_scanner();

		sector_manager();
		~sector_manager();
	};
}
