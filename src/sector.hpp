#pragma once
#include "vector.hpp"
#include "intercept.hpp"
#define FMT_HEADER_ONLY
#include "../fmt/include/fmt/format.h"

#define SLEEP(time) std::this_thread::sleep_for(std::chrono::milliseconds(time))


namespace mission
{
	class sector
	{
		
	public:
		int length;
		int building_count;
		vector3* pos; //this is the pos top left;
		vector3* center_pos;
		vector2_base<int>* identifier;
		bool is_captured;
		mutable bool is_active;
		
		std::string sector_maker;
		sector(int length, vector3* pos, vector2_base<int>* identifier)
		{
			this->length = length;
			this->building_count = 0;
			this->pos = pos;
			this->is_captured = false;
			this->is_active = false;
			this->sector_maker = "";
			this->center_pos = new vector3(pos->x + length / 2, pos->y + length / 2, 0);
			this->identifier = identifier;
		}

		auto get_sector_identifier() const -> vector2_base<int>;
		void show_marker() const;
		void hide_marker() const;
		void change_marker_side(std::string side) const;
		void clean_marker() const;
		void setup_marker();
	};

	typedef struct sector_timestamp
	{
		sector* sector;
		float timestamp;
	} sector_timestamp;
	
	
	/*
	class config_values
	{
	private:
		static config_values* instance;

	public:
		static config_values* get_instance();
		void read_values();
		r_string rifleman;
		//int sectorSize;
	};*/
	
	
	class library
	{
	public:
		static std::vector<object> get_players();
	};
}
