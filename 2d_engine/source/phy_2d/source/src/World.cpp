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
		pbody = static_cast<Body_2d*>(new (pmem) Body_2d(body_def.m_type, body_def.m_position, body_def.m_mass, body_def.m_aabb));
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

	
	if (is_climbing_up) {
		//if the row does not have fractional part, decrement so it does not count one extra row
		if (tile_bottom_left_coord.first == (int)tile_bottom_left_coord.first) {
			--tile_bottom_left_coord.first;
		}
	}
	//else {
		//tile_bottom_left_coord.first += FLOAT_ROUNDOFF;
	//}

	Tile tile;
	unsigned id;	
	int row = tile_bottom_left_coord.first;
	for (int column = tile_bottom_left_coord.second; column <= tile_top_right_coord.second; ++column) {
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

	unsigned id;
	Tile tile;
	for (int row = tile_top_coord.first; row <= tile_bottom_coord.first; ++row) {
		id = m_pmap->get_tile_id(0, row, tile_top_coord.second);
		tile = m_pmap->get_tile(id);
		if (tile.m_is_ladder && tile.m_is_one_way) {
			ladder_bounds = m_pmap->tile_wld_space_bounds(row, tile_top_coord.second);
			return true;
		}
	}
	return false;
}


bool physics_2d::World::is_body_2d_on_ground(const physics_2d::Body_2d * pbody) const 
{
	assert(pbody != nullptr && "NULL Body_2d pointer");

	// get the bottom left and bottom right AABB world space coords
	math::vec2 bottom_left = pbody->m_aabb.p_min;
	math::vec2 bottom_right = pbody->m_aabb.p_max;
	bottom_right.y = bottom_left.y;

	//get the tile map space coordinates, i.e [row, column] of the AABB bottom left and bottom right
	std::pair<float, float> tile_bottom_left_coord = m_pmap->wld_to_tile_space(bottom_left);
	std::pair<float, float> tile_bottom_right_coord = m_pmap->wld_to_tile_space(bottom_right);
	
	float dist = MIN (1.0F, tile_bottom_right_coord.second - tile_bottom_left_coord.second);
	Tile tile;
	unsigned id;
	// search the columns o encompassed by the body's aabb and, check if any of the tiles are solid tiles
	for (float column = tile_bottom_left_coord.second  ; column <= tile_bottom_right_coord.second ; column+= dist) {
		id = m_pmap->get_tile_id(0, tile_bottom_left_coord.first, column);
		tile = m_pmap->get_tile(id);
		if ( (tile.m_is_obstacle || tile.m_is_one_way) && pbody->m_velocity.y <= 0.0f ) {
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
	//std::cout << "PLAYER WLD SPACE POS = (" << pbody->get_position().x << ", " << pbody->get_position().y << ")" << std::endl;
	//Attempt to move horizontally
	if (is_moving_right) {
		//get the right facing edge in world space
		math::vec2 up_right      =  pbody->m_aabb.p_max;
		math::vec2 bottom_right  =  pbody->m_aabb.p_min;
		bottom_right.x          =  up_right.x;

		std::pair<float, float> tile_up_right_coord = m_pmap->wld_to_tile_space(up_right)  ;
		std::pair<float, float> tile_bottom_right_coord = m_pmap->wld_to_tile_space(bottom_right);
		
		//adjust if it has no fractional part
		/*
		if (tile_up_right_coord.first == (int) tile_up_right_coord.first) {
			++tile_up_right_coord.first;
		}
		if (tile_bottom_right_coord.first == (int)tile_bottom_right_coord.first) {
			--tile_bottom_right_coord.first;
		}*/
		//std::cout << "------------------------------------------IN MOVING RIGHT TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		//std::cout << "AABB min = (" << pbody->m_aabb.p_min.x << ", " << pbody->m_aabb.p_min.y << ")" << std::endl;
		//std::cout << "AABB max = (" << pbody->m_aabb.p_max.x << ", " << pbody->m_aabb.p_max.y << ")" << std::endl;
		//std::cout << "tile_up_right_coord = [ " << tile_up_right_coord.first << ", " << tile_up_right_coord.second << "]" << std::endl;
		//std::cout << "tile_bottom_right_coord = [ " << tile_bottom_right_coord.first << ", " << tile_bottom_right_coord.second << "]" << std::endl;

		//find the closest horizontal obstacle
		unsigned closest_obstacle_row = tile_up_right_coord.first;
		unsigned closest_obstacle_column = m_pmap->width() ;
		Tile closest_obstacle_tile;
		//std::cout << "row =" << (unsigned)tile_up_right_coord.first << " row <= " << (unsigned)tile_bottom_right_coord.first << std::endl;
		//std::cout << "column =" << m_pmap->height() - 1 << " column >= " << (unsigned)tile_up_right_coord.second << std::endl;
		
		float      maximum_x_tiles_disp = m_pmap->world_to_tile_displacement_x(pbody->m_velocity.x * dt);
	//	int count = 0;
		maximum_x_tiles_disp = MAX(maximum_x_tiles_disp, 1.0f); // CHECK AT LEAST ONE TILE AHEAD
		for (float row = tile_up_right_coord.first; row <= tile_bottom_right_coord.first; ++row) {
			for (float column = tile_up_right_coord.second + maximum_x_tiles_disp; column >= tile_up_right_coord.second; --column) {
				//std::cout << "[" << row << ", " << column << "]" << std::endl;
				Tile tile;
				unsigned id = m_pmap->get_tile_id(0, row, column);
				 tile = m_pmap->get_tile(id);
		//		 count++;
				 if (tile.m_is_obstacle) {
					if ((row >= closest_obstacle_row) && (column < closest_obstacle_column)) {
						closest_obstacle_tile = tile;
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					}
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
			pbody->m_position.x    +=  pbody->m_velocity.x * dt;
			pbody->m_aabb.p_min.x  +=  pbody->m_velocity.x * dt;
			pbody->m_aabb.p_max.x  +=  pbody->m_velocity.x * dt;
		}
		//std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
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
		
		/*
		//adjust if it has no fractional part if we at the line beetwen two tiles
		if (tile_up_left_coord.first == (int)tile_up_left_coord.first) {
			++tile_up_left_coord.first;
		}
		if (tile_bottom_left_coord.first == (int)tile_bottom_left_coord.first) {
			--tile_bottom_left_coord.first;
		}*/
	//	std::cout << "tile_up_left_coord = [" << tile_up_left_coord.first << ", " << tile_up_left_coord.second << "]" << std::endl;
	//	std::cout << "tile_bottom_left_coord = [" << tile_bottom_left_coord.first << ", " << tile_bottom_left_coord.second << "]" << std::endl;
		//ceil

		// get the maximun number of tiles this object can traverse in one update
		float      maximum_x_tiles_disp = m_pmap->world_to_tile_displacement_x(pbody->m_velocity.x * dt);
		maximum_x_tiles_disp = MIN(maximum_x_tiles_disp, -1.0f); // check at least one tile ahead
		//std::cout << "maximun tiles fucking disp " << maximum_x_tiles_disp << std::endl;
		//find the closest horizontal obstacle
		unsigned closest_obstacle_row = tile_up_left_coord.first;
		unsigned closest_obstacle_column = 0;
		Tile closest_obstacle_tile;
		for (float row = tile_up_left_coord.first; row <= tile_bottom_left_coord.first; ++row) {
			for (float column = tile_up_left_coord.second + maximum_x_tiles_disp; column <= tile_up_left_coord.second; ++column) {
	//			std::cout << "[" << row << ", " << column << "]" << std::endl;
				Tile tile;
				unsigned id = m_pmap->get_tile_id(0, row, column);
					tile = m_pmap->get_tile(id);
				if (tile.m_is_obstacle) { //if this tile is a obsctale
					if ( (row >= closest_obstacle_row) && (column > closest_obstacle_column) ) {
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
			pbody->m_position.x  += pbody->m_velocity.x * dt;

			pbody->m_aabb.p_min.x += pbody->m_velocity.x * dt;
			pbody->m_aabb.p_max.x += pbody->m_velocity.x * dt;
		//	std::cout << "no collision" << std::endl;
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

		if (tile_up_right_coord.second == (int)tile_up_right_coord.second) {
			--tile_up_right_coord.second;
		}

		//std::cout << "------------------------------------------IN MOVING UP TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		
		//std::cout << std::setw(10) << number << "\n";

		//std::cout << "tile_up_left_coord = [" << tile_up_left_coord.first << ", " << tile_up_left_coord.second << "]" << std::endl;
		//std::cout << "tile_up_right_coord = [" << tile_up_right_coord.first << ", " << tile_up_right_coord.second << "]" << std::endl;
		//find the closest vertical obstacle
		float      maximum_y_tiles_disp = m_pmap->world_to_tile_displacement_y(pbody->m_velocity.y * dt);
		maximum_y_tiles_disp = MAX(maximum_y_tiles_disp, 1.0f);
		unsigned closest_obstacle_row = 0;
		unsigned closest_obstacle_column = tile_up_left_coord.second;
		float dist = MIN(1.0F, tile_up_right_coord.second - tile_up_left_coord.second);
		Tile closest_tile;
		for (float row = tile_up_left_coord.first - maximum_y_tiles_disp; row <= tile_up_left_coord.first; ++row ) {
			for (float column = tile_up_left_coord.second; column <= tile_up_right_coord.second; column += dist) {
			//	std::cout << "[" << row << ", " << column << "]" << std::endl;
				//std::cout << "column = " << column << std::endl;
				unsigned  id     =  m_pmap->get_tile_id(0, row, column);
				Tile      tile   =  m_pmap->get_tile(id);
				if (tile.m_is_obstacle) {// || one way
					if ( (row > closest_obstacle_row) && (column >= closest_obstacle_column)) {
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
			float      desired_y_position = pbody->m_aabb.p_max.y + pbody->m_velocity.y; //* g_timer.get_fixed_dt();
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
				pbody->m_position.y += pbody->m_velocity.y;

				//update the righd body's aabb
				pbody->m_aabb.p_min.y += pbody->m_velocity.y;
				pbody->m_aabb.p_max.y += pbody->m_velocity.y;
			}
		}
		else {
			pbody->m_position.y += pbody->m_velocity.y;

			//update the righd body's aabb
			pbody->m_aabb.p_min.y += pbody->m_velocity.y;
			pbody->m_aabb.p_max.y += pbody->m_velocity.y;
		}
	//	std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
	else {
		
		//std::cout << "------------------------------------------IN MOVING DOWN TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
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
		//int count = 0;
		
		unsigned closest_obstacle_row = m_pmap->height() - 1;
		unsigned closest_obstacle_column = tile_bottom_left_coord.second;
		float      maximum_y_tiles_disp = std::ceil( m_pmap->world_to_tile_displacement_y(-pbody->m_velocity.y * dt));
		//std::cout << "maximun fucking tile disp =" << maximum_y_tiles_disp << std::endl;
		//std::cout << "tile_bottom_left_coord.first + maximum_y_tiles_disp = " << tile_bottom_left_coord.first + maximum_y_tiles_disp << std::endl;
		Tile closest_tile;
		for (unsigned row = MIN(tile_bottom_left_coord.first + maximum_y_tiles_disp, closest_obstacle_row); row >= tile_bottom_left_coord.first; --row) {
			//std::cout << "in row loop" << std::endl;
			for (unsigned column = tile_bottom_left_coord.second; column <= tile_bottom_right_coord.second; ++column) {
			//	std::cout << "[" << row << ", " << column << "]" << std::endl;
				//	count++;
				Tile     tile;
				unsigned id    =  m_pmap->get_tile_id(0, row, column);
				tile  =  m_pmap->get_tile(id);
				if (tile.m_is_obstacle || tile.m_is_one_way) {
					if ((row < closest_obstacle_row) && (column >= closest_obstacle_column)) {
						closest_tile = tile;
						closest_obstacle_row     =  row;
						closest_obstacle_column  =  column;
					}
				}
			}
		}
		//std::cout << "LOOP ITERATED " << count << " TIMES " << std::endl;;
		//get the world coordinates of the bounds of the closest vertical obstacle
		math::Rect obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);

		if (closest_tile.m_is_obstacle || (closest_tile.m_is_one_way && (obstacle_border.y <= pbody->m_aabb.p_min.y))) { // treat has a obstacle

		//update the position the as the minimun of the obstacle up facing edge and the desidered posiiton
			float      desired_y_position = pbody->m_aabb.p_min.y + pbody->m_velocity.y;// *Timer::instance().get_delta();


			if (obstacle_border.y > desired_y_position) { //collied
				//std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;

				float y_offset = obstacle_border.y - pbody->m_aabb.p_min.y;

				pbody->m_position.y += y_offset;
				pbody->m_velocity.y = 0.0f;
				pbody->m_acceleration.y = 0.0f;

				//update the righd body's aabb
				pbody->m_aabb.p_min.y += y_offset;
				pbody->m_aabb.p_max.y += y_offset;
			}
			else {
				pbody->m_position.y += pbody->m_velocity.y; //* Timer::instance().get_delta();

				//update the righd body's aabb
				pbody->m_aabb.p_min.y += pbody->m_velocity.y;// * Timer::instance().get_delta();
				pbody->m_aabb.p_max.y += pbody->m_velocity.y;// * Timer::instance().get_delta();
			}
		}
		else {
			pbody->m_position.y += pbody->m_velocity.y; //* Timer::instance().get_delta();

														//update the righd body's aabb
			pbody->m_aabb.p_min.y += pbody->m_velocity.y;// * Timer::instance().get_delta();
			pbody->m_aabb.p_max.y += pbody->m_velocity.y;// * Timer::instance().get_delta();
		}
		//std::cout << "---------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
}

void physics_2d::World::update(const float dt) 
{
	 //update positions, check and solve map collision logic

	for (auto iter = m_bodies.begin(); iter != m_bodies.end(); ++iter) {
		switch ((*iter)->get_type()) {
		case Body_2d::DYNAMIC:
			if ((*iter)->m_apply_gravity) {
				(*iter)->m_velocity.x += ((*iter)->m_acceleration.x + m_gravity.x) * dt;
				(*iter)->m_velocity.y += ((*iter)->m_acceleration.y + m_gravity.y) * dt;
			//	std::cout << "m_velocity.y = " << (*iter)->m_velocity.y << std::endl;
			//	std::cout << "m_velocity.x = " << (*iter)->m_velocity.x << std::endl;

				/*
				if ((*iter)->m_vel_threshold.x != 0.0f) {
					if (std::fabs((*iter)->m_velocity.x) > (*iter)->m_vel_threshold.x) {
						(*iter)->m_velocity.x = ((*iter)->m_velocity.x > 0.0f)  ?((*iter)->m_vel_threshold.x) :(- (*iter)->m_vel_threshold.x) ;
					}
				}*/
				if ((*iter)->m_vel_threshold.y != 0.0f) {
					if ((*iter)->m_velocity.y < -0.19f) {
						(*iter)->m_velocity.y = -0.19f;

					}
					/*
					if ( std::fabs((*iter)->m_velocity.y) > (*iter)->m_vel_threshold.y) {
						(*iter)->m_velocity.y = ((*iter)->m_velocity.y > 0.0f) ? ((*iter)->m_vel_threshold.y) : (-(*iter)->m_vel_threshold.y);
											std::cout << "--------------hit velocity treshold------------------------------" << std::endl;
					}*/
				}


				//(*iter)->m_position.x += (*iter)->m_velocity.x * Timer::instance().get_delta();
				//(*iter)->m_position.y += (*iter)->m_velocity.y * Timer::instance().get_delta();
			}
			else {
				(*iter)->m_velocity.x += (*iter)->m_acceleration.x * dt;
				(*iter)->m_velocity.y += (*iter)->m_acceleration.y * dt;
				//std::cout << "m_velocity.y = " << (*iter)->m_velocity.y << std::endl;
				//std::cout << "m_velocity.x = " << (*iter)->m_velocity.x << std::endl;
			}
			if ( ( (*iter)->m_velocity.x != 0.0f) || ((*iter)->m_velocity.y != 0.0f) ) {
				check_n_solve_map_collision(*iter, dt);
			}

			break;
		case Body_2d::KINEMATIC:
			(*iter)->m_velocity.x += (*iter)->m_acceleration.x * dt;
			(*iter)->m_velocity.y += (*iter)->m_acceleration.y * dt;
			
			if ((*iter)->m_vel_threshold.x != 0.0f) {
				if (std::fabs((*iter)->m_velocity.x) > (*iter)->m_vel_threshold.x) {
					(*iter)->m_velocity.x = ((*iter)->m_velocity.x > 0.0f) ? ((*iter)->m_vel_threshold.x) : (-(*iter)->m_vel_threshold.x);
				}
			}
			if ((*iter)->m_vel_threshold.y != 0.0f) {
				if (std::fabs((*iter)->m_velocity.y) > (*iter)->m_vel_threshold.y) {
					(*iter)->m_velocity.y = ((*iter)->m_velocity.y > 0.0f) ? ((*iter)->m_vel_threshold.y) : (-(*iter)->m_vel_threshold.y);
				}
			}
			
			//(*iter)->m_position.x += (*iter)->m_velocity.x * Timer::instance().get_delta();
			//(*iter)->m_position.y += (*iter)->m_velocity.y * Timer::instance().get_delta();
			break;
		}
	}

}