#include "World.hpp"
#include "Collision_listener.hpp"
#include "Body_2d.hpp"
#include "Body_2d_def.hpp"
#include "AABB_2d.hpp"
#include "Tile_map.hpp"
#include "Rect.hpp"
#include "Tile.hpp"

#include <algorithm>
#include <vector>
#include <utility>
#include <cmath>
#include <cassert>


physics_2d::World::World(const math::vec2 & gravity, const math::vec2 & solid_tile_sensor_line) :
	m_gravity(gravity), m_solid_tile_sensor_line(solid_tile_sensor_line) ,m_pmap(nullptr), m_pcoll_listener(nullptr), m_mem_pool(sizeof(physics_2d::Body_2d), MAX_BODY_2DS, 4) 
{
	m_bodies.reserve(MAX_BODY_2DS);
}

physics_2d::World::~World() 
{
	for (auto it = m_bodies.begin(); it != m_bodies.end(); ++it) {
		m_mem_pool.free_element(static_cast<void*>(*it));
	}

	//memory pool destructor is called implicitly to return the memory to the system
}

physics_2d::Body_2d * physics_2d::World::create_body_2d(const Body_2d::Entity_types & type, const math::vec2 & pos, const float m, const AABB_2d & aabb)
{
	Body_2d *pbody = nullptr;
	void *pmem = m_mem_pool.get_element();
	if (pmem != nullptr) {
		pbody = static_cast<Body_2d*>( new (pmem) Body_2d(type, pos, m, aabb) );
		m_bodies.push_back(pbody);
	}
	
	return pbody;
}

physics_2d::Body_2d * physics_2d::World::create_body_2d(const Body_2d_def & body_def)
{
	Body_2d *pbody = nullptr;
	void *pmem = m_mem_pool.get_element();
	if (pmem != nullptr) {
		pbody = static_cast<Body_2d*>(new (pmem) Body_2d(&body_def));
		m_bodies.push_back(pbody);
	}

	return pbody;
}

void physics_2d::World::destroy_body_2d(physics_2d::Body_2d *pbody) 
{
	for (auto iter = m_bodies.begin(); iter != m_bodies.end(); ++iter) {
		if (*iter == pbody) {
			m_mem_pool.free_element(static_cast<void*>(*iter));
			m_bodies.erase(iter);
			break;
		}
	}
}

bool physics_2d::World::try_climbing_ladder(physics_2d::Body_2d * pbody, const bool is_climbing_up) 
{
	assert(pbody != nullptr && "NULL Body_2d pointer");

	//world space bottom left and top right coordinates of the body's aabb
	math::vec2 bottom_left = pbody->m_aabb.p_min;
	math::vec2 top_right   = pbody->m_aabb.p_max;

	//scale down the aabb horizontal size, so it can only climb if it is close to the ladder column
	float x_magnitude = top_right.x - bottom_left.x;
	bottom_left.x += 0.2f * x_magnitude;
	top_right.x -= 0.2f * x_magnitude;

	if (is_climbing_up) {
		bottom_left.y = (pbody->m_aabb.p_min.y + pbody->m_aabb.p_max.y) / 2.0f;
	}

	// tile map space coordinates, [row, column] of aabb bottom left and top right
	std::pair<float, float> tile_bottom_left_coord =  m_pmap->wld_to_tile_space(bottom_left);
	std::pair<float, float> tile_top_right_coord   =  m_pmap->wld_to_tile_space(top_right);

	std::pair<unsigned, unsigned> columns_to_check{ tile_bottom_left_coord.second, tile_top_right_coord.second };
	unsigned row = tile_bottom_left_coord.first;

	//std::cout << "columns interval to check [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;
	//std::cout << "row to check: " << row << std::endl;

	Tile tile;
	unsigned id;	
	for (unsigned column = columns_to_check.first; column <= columns_to_check.second; ++column) {
		id = m_pmap->get_tile_id(0, row, column);
		tile = m_pmap->get_tile(id);
		if(tile.m_is_ladder) {
			//reposition the character to be inside the ladder
			math::Rect ladder_tile_bounds = m_pmap->tile_wld_space_bounds(row, column);
			
			float aabb_center_x = (pbody->m_aabb.p_max.x + pbody->m_aabb.p_min.x) / 2.0f;
			float tile_center_x = (ladder_tile_bounds.x + (ladder_tile_bounds.x + ladder_tile_bounds.width)) / 2.0f;
			float  x_offset = tile_center_x - aabb_center_x;
			if ( (top_right.x < ladder_tile_bounds.x + ladder_tile_bounds.width ) || (bottom_left.x > ladder_tile_bounds.x)) { // on the right
				pbody->m_aabb.p_max.x += x_offset;
				pbody->m_aabb.p_min.x += x_offset;
				pbody->m_position.x += x_offset;
			}
			if (!is_climbing_up) { // if is climbing down, we need to position the body inside the ladder
				float y_offset = 0.8f * ( (ladder_tile_bounds.y - ladder_tile_bounds.height) - pbody->m_aabb.p_min.y );
				pbody->m_aabb.p_max.y += y_offset;
				pbody->m_aabb.p_min.y += y_offset;
				pbody->m_position.y += y_offset;

			}
			return true;
		}
	}
	return false;
}

bool physics_2d::World::is_body_on_ladder(physics_2d::Body_2d * pbody) 
{
	assert(pbody != nullptr && "NULL Body_2d pointer");
	
	// get the bottom left and bottom right AABB world space coords
	math::vec2 top_right = pbody->m_aabb.p_max;
	math::vec2 bottom_left = pbody->m_aabb.p_min;
	top_right.x = bottom_left.x = (pbody->m_aabb.p_min.x + pbody->m_aabb.p_max.x) / 2.0f;
	top_right.y = pbody->m_aabb.p_min.y * 0.3f + pbody->m_aabb.p_max.y * 0.7f;
	std::pair<float, float> tile_top_right_coord = m_pmap->wld_to_tile_space(top_right);
	//std::pair<float, float> tile_bottom_left_coord = m_pmap->wld_to_tile_space(bottom_left);

	unsigned id;
	Tile tile;	
	//for (int row = tile_top_right_coord.first; row <= tile_bottom_left_coord.first; ++row) {
		id = m_pmap->get_tile_id(0, tile_top_right_coord.first, tile_top_right_coord.second);
		tile = m_pmap->get_tile(id);
		if (tile.m_is_ladder) {
			return true;
		}
//	}
	return false;
}

bool physics_2d::World::is_on_ladder_top_tile(const physics_2d::Body_2d * pbody, math::Rect & ladder_bounds) const
{
	assert(pbody != nullptr && "NULL Body_2d pointer");

	// get the bottom left and bottom right AABB world space coords
	math::vec2 top_center = pbody->m_aabb.p_max;
	math::vec2 bottom_center = pbody->m_aabb.p_min;
	bottom_center.x = top_center.x = (pbody->m_aabb.p_min.x + pbody->m_aabb.p_max.x) / 2.0f;
	 

	std::pair<float, float> tile_top_coord = m_pmap->wld_to_tile_space(top_center);
	std::pair<float, float> tile_bottom_coord = m_pmap->wld_to_tile_space(bottom_center);
	
	std::pair<unsigned, unsigned> rows_to_check{ tile_top_coord.first, tile_bottom_coord.first };
	unsigned column = tile_top_coord.second;
	
	unsigned id;
	Tile tile;
	for (unsigned row = rows_to_check.first; row <= rows_to_check.second; ++row) {
		id = m_pmap->get_tile_id(0, row, column);
		tile = m_pmap->get_tile(id);
		if (tile.m_is_ladder && tile.m_is_one_way) {
			ladder_bounds = m_pmap->tile_wld_space_bounds(row, tile_top_coord.second);
			return true;
		}
	}
	return false;
}

//STARTED WORKING ON IS_BODY_ON_THE_GROUND !!!!!

bool physics_2d::World::is_body_2d_on_ground(const physics_2d::Body_2d * pbody) const 
{
	assert(pbody != nullptr && "NULL Body_2d pointer");

	if (pbody->m_velocity.y > 0.0f) {
		return false;
	}

	// get the bottom left and bottom right AABB world space coords
	math::vec2 bottom_left = pbody->m_aabb.p_min;
	math::vec2 bottom_right = pbody->m_aabb.p_max;
	bottom_right.y = bottom_left.y;

	//get the tile map space coordinates, i.e [row, column] of the AABB bottom left and bottom right
	std::pair<float, float> tile_bottom_left_coord = m_pmap->wld_to_tile_space(bottom_left);
	std::pair<float, float> tile_bottom_right_coord = m_pmap->wld_to_tile_space(bottom_right);
	
	//const float pixel_wld_units = 1.0f / m_pmap->pixels_per_world_unit();
	//float tile_extra_height = pixel_wld_units * 3.0f;

	std::pair<unsigned, unsigned> columns_to_check = { tile_bottom_left_coord.second, tile_bottom_right_coord.second };
	unsigned row = (tile_bottom_left_coord.first > m_pmap->height() - 1) ?(m_pmap->height() - 1) :(tile_bottom_left_coord.first);
	//std::cout << "columns = [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;
	
	//float dist = MIN (1.0F, tile_bottom_right_coord.second - tile_bottom_left_coord.second);
	Tile tile;
	unsigned id;
	math::Rect border;
	// search the columns o encompassed by the body's aabb and, check if any of the tiles are solid tiles
	for (unsigned column = columns_to_check.first; column <= columns_to_check.second; column++) {
		id = m_pmap->get_tile_id(0, row, column);
		tile = m_pmap->get_tile(id);
		border = m_pmap->tile_wld_space_bounds(row, column);
		if ( tile.m_is_obstacle || (tile.m_is_one_way && border.y <= pbody->m_aabb.p_min.y))  {
			return true;
		}
	}

	return false;
}

void physics_2d::World::check_n_solve_map_collision(physics_2d::Body_2d *pbody, const float dt) 
{
	//compute the direction the body moved according to the velocity magnitude
	bool is_moving_right =  (pbody->m_velocity.x > 0.0f) ? (true) : (false);
	bool is_moving_up	= (pbody->m_velocity.y > 0.0f) ? (true) : (false);
	float sensor_line_width  =  m_solid_tile_sensor_line.x / m_pmap->pixels_per_world_unit();
	float sensor_line_height = m_solid_tile_sensor_line.y / m_pmap->pixels_per_world_unit();
	const float pixel_wld_units = 1.0f / m_pmap->pixels_per_world_unit();
	//std::cout << "PLAYER WLD SPACE POS = (" << pbody->get_position().x << ", " << pbody->get_position().y << ")" << std::endl;
	//Attempt to move horizontally
	if (is_moving_right) {
		//get the right facing edge in world space
		math::vec2 up_right      =  pbody->m_aabb.p_max;
		math::vec2 bottom_right  =  pbody->m_aabb.p_min;
		bottom_right.x          =  up_right.x;

		std::pair<float, float> tile_up_right_coord = m_pmap->wld_to_tile_space(up_right)  ;
		std::pair<float, float> tile_bottom_right_coord = m_pmap->wld_to_tile_space(bottom_right);
		
		float height_offset = 2.0f / m_pmap->pixels_per_world_unit();;
		tile_bottom_right_coord.first -= height_offset;

		//std::cout << "tile_up_right_coord = [" << tile_up_right_coord.first << ", " << tile_up_right_coord.second << "]" << std::endl;
		//std::cout << "tile_bottom_right_coord = [" << tile_bottom_right_coord.first << ", " << tile_bottom_right_coord.second << "]" << std::endl;
		//std::cout << "------------------------------------------IN MOVING RIGHT TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		
		//check if the body is beyond the map's border. If so, set the coordinates to the "last" or "first" row 
		tile_up_right_coord.first = (tile_up_right_coord.first > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_up_right_coord.first);
		tile_bottom_right_coord.first = (tile_bottom_right_coord.first > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_bottom_right_coord.first);

		tile_up_right_coord.first = (tile_up_right_coord.first < 0.0f) ? (0.0f) : (tile_up_right_coord.first);
		tile_bottom_right_coord.first = (tile_bottom_right_coord.first < 0.0f) ? (0.0f) : (tile_bottom_right_coord.first);

		//       Find the closest horizontal obstacle

		// get how many columns of tiles ahead, this body wants to traverse
		float      maximum_x_tiles_disp = m_pmap->world_to_tile_displacement_x(pbody->m_velocity.x * dt) + sensor_line_width;

		//calculate the farthest column to check
		float farthest_column = (tile_up_right_coord.second + maximum_x_tiles_disp > m_pmap->width() - 1) ?(m_pmap->width() - 1) :(tile_up_right_coord.second + maximum_x_tiles_disp);
		
		// start the closest_obstacle column with the farthest possible column
		unsigned closest_obstacle_row = tile_up_right_coord.first;
		unsigned closest_obstacle_column = farthest_column;

		//get the farthest tile, i.e, the one with a column = map_width - 1, this is used if no obstacle is found ahead of the body
		unsigned  id = m_pmap->get_tile_id(0, closest_obstacle_row, closest_obstacle_column);
		Tile      closest_obstacle_tile = m_pmap->get_tile(id);

		std::pair<unsigned, unsigned> rows_to_check = { tile_up_right_coord.first, tile_bottom_right_coord.first };
		std::pair<unsigned, unsigned> columns_to_check = { tile_up_right_coord.second, farthest_column};
		//std::cout << "Rows interval to check = [" << rows_to_check.first << ", " << rows_to_check.second << "]" << std::endl;
		//std::cout << "columns interval to check = [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;
		
		
		//start checking from farthest calculated column to the bodys current column
		for (unsigned row = rows_to_check.first; row <= rows_to_check.second; ++row) {
			for (unsigned column = columns_to_check.second; column >= columns_to_check.first; --column) {
			//	std::cout << "[" << row << ", " << column << "]" << std::endl;
				Tile tile;
				unsigned id = m_pmap->get_tile_id(0, row, column);
				 tile = m_pmap->get_tile(id);
				 if (tile.m_is_obstacle) {
					if ((row >= closest_obstacle_row) && (column <= closest_obstacle_column)) {
						closest_obstacle_tile = tile;
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					}
				}
				 if (column == 0) {
					 break;
				}
			}
		}
		//std::cout << "LOOP ITERATED " << count << " times " << std::endl;
		//get the world coordinates of the bounds of the closest horizontal obstacle
		math::Rect  obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);
		
		//update the position of the aabb the as the minimun of the obstacle left facing edge and the desidered posiiton
		float      desired_x_position = pbody->m_aabb.p_max.x + pbody->m_velocity.x * dt;
		//pbody->m_position.x = (obstacle_border.x < desired_x_position) ? (obstacle_border.x) : (desired_x_position);

		if ( (obstacle_border.x - sensor_line_width) < desired_x_position && closest_obstacle_tile.m_is_obstacle ) { // if collided with the map
		//	std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;
			
			float x_offset = (obstacle_border.x - sensor_line_width) - pbody->m_aabb.p_max.x;

			//check if we can push the player in the other direction or if it is going to collide with a tile on their left
			math::vec2 up_left = pbody->m_aabb.p_max;
			math::vec2 bottom_left = pbody->m_aabb.p_min;
			bottom_left.x += x_offset;
			up_left.x = bottom_left.x;

			bottom_left.y += sensor_line_height;

			std::pair<float, float> tile_up_left = m_pmap->wld_to_tile_space(up_left);
			std::pair<float, float> tile_bottom_left = m_pmap->wld_to_tile_space(bottom_left);
			
			tile_up_left.first = (tile_up_left.first > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_up_left.first);
			tile_bottom_left.first = (tile_bottom_left.first > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_bottom_left.first);

			tile_up_left.first =     (tile_up_left.first < 0.0f) ? (0.0f) : (tile_up_left.first);
			tile_bottom_left.first = (tile_bottom_left.first < 0.0f) ? (0.0f) : (tile_bottom_left.first);

			Tile bottom_tile = m_pmap->get_tile(m_pmap->get_tile_id(0, tile_up_left.first, tile_up_left.second));
		    Tile up_tile     = m_pmap->get_tile(m_pmap->get_tile_id(0, tile_bottom_left.first, tile_bottom_left.second));

			if ((!bottom_tile.m_is_obstacle && !bottom_tile.m_is_one_way) && (!up_tile.m_is_obstacle && !up_tile.m_is_one_way)) {
				pbody->m_position.x += x_offset;
				pbody->m_velocity.x = 0.0f;
				pbody->m_acceleration.x = 0.0f;

				pbody->m_aabb.p_min.x += x_offset;
				pbody->m_aabb.p_max.x += x_offset;
			}
		}
		else {
			//check collision against the map border, i.e, if this body is going beyond the last column of tiles of the map
			if ( (closest_obstacle_column == m_pmap->width() - 1) && (desired_x_position >obstacle_border.x + obstacle_border.width - sensor_line_width) ) {
				//trying to go off map, set the position to the maps border
				float x_offset = (obstacle_border.x + obstacle_border.width - sensor_line_width) - pbody->m_aabb.p_max.x;
				math::vec2 t(x_offset, 0);
				pbody->translate_by(t);
			
				pbody->m_velocity.x = 0.0f;
				pbody->m_acceleration.x = 0.0f;
			}
			else {
				pbody->m_position.x += pbody->m_velocity.x * dt;
				pbody->m_aabb.p_min.x += pbody->m_velocity.x * dt;
				pbody->m_aabb.p_max.x += pbody->m_velocity.x * dt;
			}
		}
	//	std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
	else if (pbody->m_velocity.x != 0.0f) {
		
		//std::cout << "------------------------------------------IN MOVING LEFT TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		//get left facing edge
		math::vec2 up_left = pbody->m_aabb.p_max;
		math::vec2 bottom_left = pbody->m_aabb.p_min;
		up_left.x			  = bottom_left.x;

		//get the rows and columns that need to be checked against collision
		std::pair<float, float> tile_up_left_coord		=  m_pmap->wld_to_tile_space(up_left);
		std::pair<float, float> tile_bottom_left_coord  = m_pmap->wld_to_tile_space(bottom_left);

		float height_offset = 2.0f / m_pmap->pixels_per_world_unit();;
		tile_bottom_left_coord.first -= height_offset;
		
		//check if the body is beyond the map's border. If so, set the coordinates to the "last" or "first" row
		tile_up_left_coord.first = (tile_up_left_coord.first > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_up_left_coord.first);
		tile_bottom_left_coord.first = (tile_bottom_left_coord.first > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_bottom_left_coord.first);

		tile_up_left_coord.first = (tile_up_left_coord.first < 0.0f) ? (0.0f) : (tile_up_left_coord.first);
		tile_bottom_left_coord.first = (tile_bottom_left_coord.first < 0.0f) ? (0.0f) : (tile_bottom_left_coord.first);

		//find the closest horizontal obstacle

		// get the maximun number of tiles this object can traverse in one update
		float      maximum_x_tiles_disp = m_pmap->world_to_tile_displacement_x(pbody->m_velocity.x * dt) - sensor_line_width;
	
		//calculate the farthest column to check
		float farthest_column = (tile_up_left_coord.second + maximum_x_tiles_disp < 0) ? (0) : (tile_up_left_coord.second + maximum_x_tiles_disp);

		// start the closest_obstacle's row and column variables with the farthest obstacle's row and column
		unsigned closest_obstacle_row = tile_up_left_coord.first;
		unsigned closest_obstacle_column = farthest_column;

		//get the farthest tile to the left , i.e, the one with a column = 0, this is used if no obstacle is found ahead of the body
		unsigned id = m_pmap->get_tile_id(0, closest_obstacle_row, closest_obstacle_column);
		Tile closest_obstacle_tile = m_pmap->get_tile(id);
		
		std::pair<unsigned, unsigned> rows_to_check{ tile_up_left_coord.first, tile_bottom_left_coord.first };
		std::pair<unsigned, unsigned> columns_to_check{ farthest_column, tile_up_left_coord.second };

		//std::cout << "Rows interval to check = [" << rows_to_check.first << ", " << rows_to_check.second << "]" << std::endl;
		//std::cout << "columns interval to check = [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;

		for (unsigned row = rows_to_check.first; row <= rows_to_check.second; ++row) {
			for (unsigned column = columns_to_check.first; column <= columns_to_check.second; ++column) {
				//std::cout << "[" << row << ", " << column << "]" << std::endl;
				Tile tile;
				id = m_pmap->get_tile_id(0, row, column);
				tile = m_pmap->get_tile(id);
				if (tile.m_is_obstacle) { //if this tile is a obsctale
					if ( (row >= closest_obstacle_row) && (column >= closest_obstacle_column) ) {
						closest_obstacle_tile = tile;
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					}
				}
			}
		}
		//get the world coordinates of the bounds of the closest horizontal obstacle
		math::Rect  obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);
		
		//update the position the as the minimun of the obstacle left facing edge and the desidered posiiton
		float      desired_x_position = pbody->m_aabb.p_min.x + pbody->m_velocity.x * dt;
		//pbody->m_position.x = (obstacle_border.x + obstacle_border.width > desired_x_position) ? (obstacle_border.x + obstacle_border.width) : (desired_x_position);
		//std::cout << obstacle_border.x + obstacle_border.width + FLOAT_ROUNDOFF << " and " << desired_x_position - FLOAT_ROUNDOFF << std::endl;
		if ( (obstacle_border.x + obstacle_border.width + sensor_line_width > desired_x_position ) && closest_obstacle_tile.m_is_obstacle){ //collided
		//	std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;
			
			float  x_offset = (obstacle_border.x + obstacle_border.width + sensor_line_width) - pbody->m_aabb.p_min.x;
			
			math::vec2 top_right     =  pbody->m_aabb.p_max;
			math::vec2 bottom_right  =  pbody->m_aabb.p_min;
			// displace the bodie's aabb to the right to check against collision with a tile on their right
			top_right.x += x_offset;
			bottom_right.x = top_right.x;

			bottom_right.y += sensor_line_height; //ignore the floor tile

			//get the aabb's coordinates on tile map space
			std::pair<float, float> tile_top_right	   =  m_pmap->wld_to_tile_space(top_right);
			std::pair<float, float> tile_bottom_right  =  m_pmap->wld_to_tile_space(bottom_right);

			tile_top_right.first = (tile_top_right.first > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_top_right.first);
			tile_bottom_right.first = (tile_bottom_right.first > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_bottom_right.first);

			tile_top_right.first = (tile_top_right.first < 0.0f) ? (0.0f) : (tile_top_right.first);
			tile_bottom_right.first = (tile_bottom_right.first < 0.0f) ? (0.0f) : (tile_bottom_right.first);

			Tile top_tile    =  m_pmap->get_tile(m_pmap->get_tile_id(0, tile_top_right.first, tile_top_right.second));
			Tile bottom_tile =  m_pmap->get_tile(m_pmap->get_tile_id(0, tile_bottom_right.first, tile_bottom_right.second));

			if ((!top_tile.m_is_obstacle && !top_tile.m_is_one_way) && (!bottom_tile.m_is_obstacle && !bottom_tile.m_is_one_way)) {
				pbody->m_position.x += x_offset;
				pbody->m_velocity.x = 0.0f;
				pbody->m_acceleration.x = 0.0f;

				pbody->m_aabb.p_min.x += x_offset;
				pbody->m_aabb.p_max.x += x_offset;
			}
		}
		else {
			if ((closest_obstacle_column == 0) && (desired_x_position < obstacle_border.x + sensor_line_width)) {
				//trying to go off map, set the position to the maps border
				float x_offset = (obstacle_border.x + sensor_line_width) - pbody->m_aabb.p_min.x;
				math::vec2 t(x_offset, 0);
				pbody->translate_by(t);

				pbody->m_velocity.x = 0.0f;
				pbody->m_acceleration.x = 0.0f;
			}
			else {
				pbody->m_position.x += pbody->m_velocity.x * dt;

				pbody->m_aabb.p_min.x += pbody->m_velocity.x * dt;
				pbody->m_aabb.p_max.x += pbody->m_velocity.x * dt;
				//	std::cout << "no collision" << std::endl;
			}
		}
		//std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}

	// vertical movement
	if (is_moving_up) {
		//get up facing edge
		math::vec2 up_left   =  pbody->m_aabb.p_min;
		math::vec2 up_right  =  pbody->m_aabb.p_max;
		up_left.y           =  up_right.y;

		//get the rows and columns that need to be checked against collision
		std::pair<float, float> tile_up_left_coord  =  m_pmap->wld_to_tile_space(up_left);
		std::pair<float, float> tile_up_right_coord = m_pmap->wld_to_tile_space(up_right);

	//	std::cout << "------------------------------------------IN MOVING UP TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		
		tile_up_left_coord.first = (tile_up_left_coord.first < 0.0f) ? (0.0f) : (tile_up_left_coord.first);
		//find the closest vertical obstacle
		float      maximum_y_tiles_disp = m_pmap->world_to_tile_displacement_y(pbody->m_velocity.y * dt) + sensor_line_height;
		//maximum_y_tiles_disp = MAX(maximum_y_tiles_disp, 1.0f);
		
		float farthest_row = (tile_up_left_coord.first - maximum_y_tiles_disp < 0) ? (0) : (tile_up_left_coord.first - maximum_y_tiles_disp);

		//set the closest obstacle to default with the farthest posible obstacle
		unsigned closest_obstacle_row = farthest_row;
		unsigned closest_obstacle_column = tile_up_left_coord.second;
		unsigned  id = m_pmap->get_tile_id(0, closest_obstacle_row, closest_obstacle_column);
		Tile closest_tile = m_pmap->get_tile(id);
		
		std::pair<unsigned, unsigned> rows_to_check{ farthest_row, tile_up_left_coord.first };
		std::pair<unsigned, unsigned> columns_to_check{ tile_up_left_coord.second, tile_up_right_coord.second };

	//	std::cout << "Rows interval to check = [" << rows_to_check.first << ", " << rows_to_check.second << "]" << std::endl;
	//	std::cout << "columns interval to check = [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;

		for (unsigned row = rows_to_check.first; row <= rows_to_check.second; ++row ) {
			for (unsigned column = columns_to_check.first; column <= columns_to_check.second; column++) {
				id     =  m_pmap->get_tile_id(0, row, column);
				Tile      tile   =  m_pmap->get_tile(id);
				if (tile.m_is_obstacle) {// || one way
					if ( (row >= closest_obstacle_row) && (column >= closest_obstacle_column)) {
						closest_tile = tile;
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					} 
				}
			}
		}
		//get the world coordinates of the bounds of the closest vertical obstacle
		math::Rect obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);

		if (closest_tile.m_is_obstacle || (closest_tile.m_is_one_way && (obstacle_border.y <= pbody->m_aabb.p_min.y))) { // treat has a obstacle

		//update the position the as the minimun of the obstacle up facing edge and the desidered posiiton
			float      desired_y_position = pbody->m_aabb.p_max.y + pbody->m_velocity.y * dt; //* g_timer.get_fixed_dt();
			//pbody->m_position.y = (obstacle_border.y - obstacle_border.height < desired_y_position) ? (obstacle_border.y - obstacle_border.height) : (desired_y_position);

			if (obstacle_border.y - obstacle_border.height - sensor_line_height < desired_y_position) {//collided
			//	std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;

				float y_offset = (obstacle_border.y - obstacle_border.height - sensor_line_height) - pbody->m_aabb.p_max.y;

				pbody->m_position.y += y_offset;
				pbody->m_velocity.y = 0.0f;
				pbody->m_acceleration.y = 0.0f;

				//update the righd body's aabb
				pbody->m_aabb.p_min.y += y_offset;
				pbody->m_aabb.p_max.y += y_offset;
			}
			else {
				pbody->m_position.y += pbody->m_velocity.y * dt;

				//update the righd body's aabb
				pbody->m_aabb.p_min.y += pbody->m_velocity.y * dt;
				pbody->m_aabb.p_max.y += pbody->m_velocity.y * dt;
			}
		}
		else {
			pbody->m_position.y += pbody->m_velocity.y * dt;

			//update the righd body's aabb
			pbody->m_aabb.p_min.y += pbody->m_velocity.y * dt;
			pbody->m_aabb.p_max.y += pbody->m_velocity.y * dt;
		}
	//	std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
	else {
		
		// STARTING WORKING ON MOVE DOWN COLLLISION DETECTION AND RESOLUTION!!!!!!!!

	//	std::cout << "------------------------------------------IN MOVING DOWN TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		//get down facing edge
		math::vec2 bottom_left   =  pbody->m_aabb.p_min;
		math::vec2 bottom_right  =  pbody->m_aabb.p_max;
		bottom_right.y = bottom_left.y;

		//get the rows and columns that need to be checked against collision
		std::pair<float, float> tile_bottom_left_coord = m_pmap->wld_to_tile_space(bottom_left);
		std::pair<float, float> tile_bottom_right_coord = m_pmap->wld_to_tile_space(bottom_right);
		
		//std::cout << "AABB min = (" << pbody->m_aabb.p_min.x << ", " << pbody->m_aabb.p_min.y << ")" << std::endl;
		//std::cout << "AABB max = (" << pbody->m_aabb.p_max.x << ", " << pbody->m_aabb.p_max.y << ")" << std::endl;
		//std::cout << "tile_bottom_left_coord = [ " << tile_bottom_left_coord.first << ", " << tile_bottom_left_coord.second << "]" << std::endl;
		//std::cout << "tile_bottom_right_coord = [ " << tile_bottom_right_coord.first << ", " << tile_bottom_right_coord.second << "]" << std::endl;

		//find the closest vertical obstacle
		//start the closest obstacle with the farthest obstacle, i.e, tile row = height -1
		
	

		// get the farthest row we need to check for obstacles, based on the body's position and velocity
		float     maximum_y_tiles_disp = m_pmap->world_to_tile_displacement_y(-pbody->m_velocity.y * dt);
		float     farthest_row = (tile_bottom_left_coord.first + maximum_y_tiles_disp > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_bottom_left_coord.first + maximum_y_tiles_disp);
		
		//start the closest obstacle with the farthest possible obstacle
		unsigned  closest_obstacle_row = farthest_row;
		unsigned  closest_obstacle_column = tile_bottom_left_coord.second;

		unsigned  id = m_pmap->get_tile_id(0, closest_obstacle_row, closest_obstacle_column);
		Tile      closest_tile = m_pmap->get_tile(id);

		std::pair<unsigned, unsigned> rows_to_check{ tile_bottom_left_coord.first, farthest_row };
		std::pair<unsigned, unsigned> columns_to_check{ tile_bottom_left_coord.second, tile_bottom_right_coord.second };
		
		//std::cout << "Rows interval to check = [" << rows_to_check.first << ", " << rows_to_check.second << "]" << std::endl;
	//	std::cout << "columns interval to check = [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;

		for (unsigned row = rows_to_check.second; row >= rows_to_check.first; --row) {
			for (unsigned column = columns_to_check.first; column <= columns_to_check.second; ++column) {
			//	std::cout << "[" << row << ", " << column << "]" << std::endl;
				Tile     tile;
				id    =  m_pmap->get_tile_id(0, row, column);
				tile  =  m_pmap->get_tile(id);
				if (tile.m_is_obstacle || tile.m_is_one_way) {
					if ((row <= closest_obstacle_row) && (column >= closest_obstacle_column)) {
						closest_tile = tile;
						closest_obstacle_row     =  row;
						closest_obstacle_column  =  column;
					}
				}
			}
			if (row == 0) {
				break;
			}
		}
		//std::cout << "LOOP ITERATED " << count << " TIMES " << std::endl;;
		//get the world coordinates of the bounds of the closest vertical obstacle
		//std::cout << "closest tile = [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;

		math::Rect obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);
		float      desired_y_position = pbody->m_aabb.p_min.y + pbody->m_velocity.y * dt;

		//std::cout << "AABB.p_min.y = " << pbody->m_aabb.p_min.y +  sensor_line_height << " | obstacle_border.y " << obstacle_border.y << std::endl;

		if ( (closest_tile.m_is_obstacle && obstacle_border.y >= desired_y_position) || (closest_tile.m_is_one_way && (obstacle_border.y <= pbody->m_aabb.p_min.y + sensor_line_height))) { // treat has a obstacle
		//update the position the as the minimun of the obstacle up facing edge and the desidered posiiton
		
			float y_offset = obstacle_border.y - pbody->m_aabb.p_min.y;

			pbody->m_position.y += y_offset;
			pbody->m_velocity.y = 0.0f;
			pbody->m_acceleration.y = 0.0f;

			//update the righd body's aabb
			pbody->m_aabb.p_min.y += y_offset;
			pbody->m_aabb.p_max.y += y_offset;
		}
		else {
			pbody->m_position.y += pbody->m_velocity.y * dt; //* Timer::instance().get_delta();

														//update the righd body's aabb
			pbody->m_aabb.p_min.y += pbody->m_velocity.y * dt;// * Timer::instance().get_delta();
			pbody->m_aabb.p_max.y += pbody->m_velocity.y * dt;// * Timer::instance().get_delta();
		}
	//	std::cout << "---------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
}

void physics_2d::World::update(const float dt) 
{
	 //update positions, check and solve map collision logic

	for (auto iter = m_bodies.begin(); iter != m_bodies.end(); ++iter) {
		if (!(*iter)->is_active()) {
			continue;
		}
		
		switch ((*iter)->get_type()) {
		case Body_2d::DYNAMIC:
			(*iter)->m_velocity.x += ((*iter)->m_acceleration.x * dt) + (m_gravity.x * (*iter)->m_gravity_scale * dt);
			(*iter)->m_velocity.y += ((*iter)->m_acceleration.y * dt) + (m_gravity.y * (*iter)->m_gravity_scale * dt);
	
			if ( ( ( (*iter)->m_velocity.x != 0.0f) || ((*iter)->m_velocity.y != 0.0f) ) && (*iter)->m_map_collision) {
				check_n_solve_map_collision(*iter, dt);
			}
			else if (!(*iter)->m_map_collision) {
				(*iter)->m_position.x += (*iter)->m_velocity.x * dt;
				(*iter)->m_aabb.p_min.x += (*iter)->m_velocity.x * dt;
				(*iter)->m_aabb.p_max.x += (*iter)->m_velocity.x * dt;

				(*iter)->m_position.y += (*iter)->m_velocity.y * dt; 
				(*iter)->m_aabb.p_min.y += (*iter)->m_velocity.y * dt;
				(*iter)->m_aabb.p_max.y += (*iter)->m_velocity.y * dt;

			}

			break;
		case Body_2d::KINEMATIC:
			(*iter)->m_velocity.x += (*iter)->m_acceleration.x * dt;
			(*iter)->m_velocity.y += (*iter)->m_acceleration.y * dt;
			break;
		}
	}

}