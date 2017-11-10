#include "World.hpp"
#include "Body_2d.hpp"
#include "AABB_2d.hpp"
#include "Timer.hpp"
#include <algorithm>
#include <vector>
#include <utility>
#include <cmath>
#include <cassert>
#include "game.hpp"

physics_2d::World::World(const cgm::vec2 & gravity) : m_gravity(gravity), m_pmap(nullptr), m_pcoll_listener(nullptr) {}

physics_2d::World::~World() 
{
	std::for_each(m_bodies.begin(), m_bodies.end(), [](Body_2d *pbody) {delete pbody; });
}

physics_2d::Body_2d * physics_2d::World::create_body_2d(const Body_2d::Entity_types & type, const cgm::vec2 & pos, const float m, const AABB_2d & aabb)
{
	Body_2d * pbody = new Body_2d(type, pos, m, aabb);
	m_bodies.push_back(pbody);

	return pbody;
}

void physics_2d::World::destroy_body_2d(physics_2d::Body_2d *pbody) 
{
	for (auto iter = m_bodies.begin(); iter != m_bodies.end(); ++iter) {
		if (*iter == pbody) {
			delete (*iter);
			m_bodies.erase(iter);
			break;
		}
	}
}

bool physics_2d::World::is_body_2d_on_ground(const physics_2d::Body_2d * pbody) const 
{
	assert(pbody != nullptr && "NULL Body_2d pointer");

	// get the bottom left and bottom right AABB world space coords
	cgm::vec2 bottom_left = pbody->m_aabb.p_min;
	cgm::vec2 bottom_right = pbody->m_aabb.p_max;
	bottom_right.y = bottom_left.y;

	//get the tile map space coordinates, i.e [row, column] of the AABB bottom left and bottom right
	std::pair<float, float> tile_bottom_left_coord = m_pmap->wld_to_tile_space(bottom_left);
	std::pair<float, float> tile_bottom_right_coord = m_pmap->wld_to_tile_space(bottom_right);
	
	Tile tile;
	unsigned id;
	// search the columns o encompassed by the body's aabb and, check if any of the tiles are solid tiles
	for (unsigned column = tile_bottom_left_coord.second + FLOAT_ROUNDOFF ; column <= tile_bottom_right_coord.second - FLOAT_ROUNDOFF; column++) {
		id = m_pmap->get_tile_id(0, tile_bottom_left_coord.first, column);
		tile = m_pmap->get_tile(id);
		if (tile.m_is_obstacle) {
			return true;
		}
	}

	return false;
}

void physics_2d::World::check_n_solve_map_collision(physics_2d::Body_2d *pbody) 
{
	//compute the direction the body moved according to the velocity magnitude
	bool is_moving_right =  (pbody->m_velocity.x > 0.0f) ? (true) : (false);
	bool is_moving_up	= (pbody->m_velocity.y > 0.0f) ? (true) : (false);
	//std::cout << "PLAYER WLD SPACE POS = (" << pbody->get_position().x << ", " << pbody->get_position().y << ")" << std::endl;
	//Attempt to move horizontally
	if (is_moving_right) {
		//get the right facing edge in world space
		cgm::vec2 up_right      =  pbody->m_aabb.p_max;
		cgm::vec2 bottom_right  =  pbody->m_aabb.p_min;
		bottom_right.x          =  up_right.x;

		std::pair<float, float> tile_up_right_coord = m_pmap->wld_to_tile_space(up_right)  ;
		std::pair<float, float> tile_bottom_right_coord = m_pmap->wld_to_tile_space(bottom_right);
		
		//adjust if it has no fractional part
		if (tile_up_right_coord.first == (int) tile_up_right_coord.first) {
			++tile_up_right_coord.first;
		}
		if (tile_bottom_right_coord.first == (int)tile_bottom_right_coord.first) {
			--tile_bottom_right_coord.first;
		}
		//std::cout << "------------------------------------------IN MOVING RIGHT TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		//std::cout << "AABB min = (" << pbody->m_aabb.p_min.x << ", " << pbody->m_aabb.p_min.y << ")" << std::endl;
		//std::cout << "AABB max = (" << pbody->m_aabb.p_max.x << ", " << pbody->m_aabb.p_max.y << ")" << std::endl;
		//std::cout << "tile_up_right_coord = [ " << tile_up_right_coord.first << ", " << tile_up_right_coord.second << "]" << std::endl;
		//std::cout << "tile_bottom_right_coord = [ " << tile_bottom_right_coord.first << ", " << tile_bottom_right_coord.second << "]" << std::endl;

		//find the closest horizontal obstacle
		unsigned closest_obstacle_row = tile_up_right_coord.first;
		unsigned closest_obstacle_column = m_pmap->width() - 1;
		Tile tile;
		//std::cout << "row =" << (unsigned)tile_up_right_coord.first << " row <= " << (unsigned)tile_bottom_right_coord.first << std::endl;
		//std::cout << "column =" << m_pmap->height() - 1 << " column >= " << (unsigned)tile_up_right_coord.second << std::endl;
		
		float      maximum_x_tiles_disp = m_pmap->world_to_tile_displacement_x(pbody->m_velocity.x * g_timer.get_fixed_dt());
	//	int count = 0;
		
		for (unsigned row = tile_up_right_coord.first; row <= tile_bottom_right_coord.first; ++row) {
			for (unsigned column = MIN(tile_up_right_coord.second + maximum_x_tiles_disp, closest_obstacle_column); column >= tile_up_right_coord.second; --column) {
				unsigned id = m_pmap->get_tile_id(0, row, column);
				 tile = m_pmap->get_tile(id);
		//		 count++;
				 if (tile.m_is_obstacle) {
					if ((row >= closest_obstacle_row) && (column < closest_obstacle_column)) {
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					}
				}
			}
		}
		//std::cout << "LOOP ITERATED " << count << " times " << std::endl;
		//get the world coordinates of the bounds of the closest horizontal obstacle
		tgs::Rect  obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);
		
		//update the position of the aabb the as the minimun of the obstacle left facing edge and the desidered posiiton
		float      desired_x_position = pbody->m_aabb.p_max.x + pbody->m_velocity.x * g_timer.get_fixed_dt();
		//pbody->m_position.x = (obstacle_border.x < desired_x_position) ? (obstacle_border.x) : (desired_x_position);

		if (obstacle_border.x < desired_x_position && tile.m_is_obstacle ) { // if collided with the map
		//	std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;
			
			float x_offset = obstacle_border.x - pbody->m_aabb.p_max.x;
			x_offset -= AABB_COLL_OFFSET; // so is not touching the collider
			pbody->m_position.x      += x_offset;
			pbody->m_velocity.x      =  0.0f;
			pbody->m_acceleration.x  =  0.0f;

			pbody->m_aabb.p_min.x += x_offset;
			pbody->m_aabb.p_max.x += x_offset;
		}
		else {
			pbody->m_position.x    +=  pbody->m_velocity.x * g_timer.get_fixed_dt();
			pbody->m_aabb.p_min.x  +=  pbody->m_velocity.x * g_timer.get_fixed_dt();
			pbody->m_aabb.p_max.x  +=  pbody->m_velocity.x * g_timer.get_fixed_dt();
		}
		//std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
	else if (pbody->m_velocity.x != 0.0f) {
		
		//std::cout << "------------------------------------------IN MOVING LEFT TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		//get left facing edge
		cgm::vec2 up_left = pbody->m_aabb.p_max;
		cgm::vec2 bottom_left = pbody->m_aabb.p_min;
		up_left.x			  = bottom_left.x;

		//get the rows and columns that need to be checked against collision
		std::pair<float, float> tile_up_left_coord		=  m_pmap->wld_to_tile_space(up_left);
		std::pair<float, float> tile_bottom_left_coord  = m_pmap->wld_to_tile_space(bottom_left);
		
		//std::cout << "tile_up_left_coord = [" << tile_up_left_coord.first << ", " << tile_up_left_coord.second << "]" << std::endl;
		//std::cout << "tile_bottom_left_coord = [" << tile_bottom_left_coord.first << ", " << tile_bottom_left_coord.second << "]" << std::endl;
		//adjust if it has no fractional part if we at the line beetwen two tiles
		if (tile_up_left_coord.first == (int)tile_up_left_coord.first) {
			++tile_up_left_coord.first;
		}
		if (tile_bottom_left_coord.first == (int)tile_bottom_left_coord.first) {
			--tile_bottom_left_coord.first;
		}

		//ceil

		// get the maximun number of tiles this object can traverse in one update
		float      maximum_x_tiles_disp = m_pmap->world_to_tile_displacement_x(pbody->m_velocity.x * g_timer.get_fixed_dt());
		//std::cout << "maximun tiles fucking disp " << maximum_x_tiles_disp << std::endl;
		//find the closest horizontal obstacle
		unsigned closest_obstacle_row = tile_up_left_coord.first;
		unsigned closest_obstacle_column = 0;
		Tile tile;
		for (unsigned row = tile_up_left_coord.first; row <= tile_bottom_left_coord.first; ++row) {
			for (unsigned column = tile_up_left_coord.second + maximum_x_tiles_disp; column <= tile_up_left_coord.second; ++column) {
				//std::cout << "[" << row << ", " << column << "]" << std::endl;
				unsigned id = m_pmap->get_tile_id(0, row, column);
					tile = m_pmap->get_tile(id);
				if (tile.m_is_obstacle) { //if this tile is a obsctale
					if ( (row >= closest_obstacle_row) && (column > closest_obstacle_column) ) {
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					}
				}
			}
		}
		//get the world coordinates of the bounds of the closest horizontal obstacle
		tgs::Rect  obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);
		
		//update the position the as the minimun of the obstacle left facing edge and the desidered posiiton
		float      desired_x_position = pbody->m_aabb.p_min.x + pbody->m_velocity.x * g_timer.get_fixed_dt();
		//pbody->m_position.x = (obstacle_border.x + obstacle_border.width > desired_x_position) ? (obstacle_border.x + obstacle_border.width) : (desired_x_position);
		//std::cout << obstacle_border.x + obstacle_border.width + FLOAT_ROUNDOFF << " and " << desired_x_position - FLOAT_ROUNDOFF << std::endl;
		if ( obstacle_border.x + obstacle_border.width > desired_x_position ) { //collided
		//	std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;
			
			float  x_offset = obstacle_border.x + obstacle_border.width - pbody->m_aabb.p_min.x;
			x_offset += AABB_COLL_OFFSET;
			pbody->m_position.x		+= x_offset;
			pbody->m_velocity.x      =  0.0f;
			pbody->m_acceleration.x  =  0.0f;
			
			pbody->m_aabb.p_min.x += x_offset;
			pbody->m_aabb.p_max.x += x_offset;
		}
		else {
			pbody->m_position.x  += pbody->m_velocity.x * g_timer.get_fixed_dt();

			pbody->m_aabb.p_min.x += pbody->m_velocity.x * g_timer.get_fixed_dt();
			pbody->m_aabb.p_max.x += pbody->m_velocity.x * g_timer.get_fixed_dt();
		}
		//std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}

	// vertical movement
	if (is_moving_up) {
		//get up facing edge
		cgm::vec2 up_left   =  pbody->m_aabb.p_min;
		cgm::vec2 up_right  =  pbody->m_aabb.p_max;
		up_left.y           =  up_right.y;

		//get the rows and columns that need to be checked against collision
		std::pair<float, float> tile_up_left_coord  =  m_pmap->wld_to_tile_space(up_left);
		std::pair<float, float> tile_up_right_coord = m_pmap->wld_to_tile_space(up_right);
		//std::cout << "------------------------------------------IN MOVING UP TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		//std::cout << "tile_up_left_coord = [" << tile_up_left_coord.first << ", " << tile_up_left_coord.second << "]" << std::endl;
		//std::cout << "tile_up_right_coord = [" << tile_up_right_coord.first << ", " << tile_up_right_coord.second << "]" << std::endl;
		//find the closest vertical obstacle
		int      maximum_y_tiles_disp = m_pmap->world_to_tile_displacement_y(pbody->m_velocity.y * g_timer.get_fixed_dt());
		unsigned closest_obstacle_row = 0;
		unsigned closest_obstacle_column = tile_up_left_coord.second;
		for (unsigned row = tile_up_left_coord.first - maximum_y_tiles_disp; row <= tile_up_left_coord.first; ++row ) {
			for (unsigned column = tile_up_left_coord.second; column <= tile_up_right_coord.second; ++column) {
				//std::cout << "column = " << column << std::endl;
				unsigned  id     =  m_pmap->get_tile_id(0, row, column);
				Tile      tile   =  m_pmap->get_tile(id);
				if (tile.m_is_obstacle) {
					if ( (row > closest_obstacle_row) && (column >= closest_obstacle_column)) {
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					} 
				}
			}
		}
		//get the world coordinates of the bounds of the closest vertical obstacle
		tgs::Rect obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);

		//update the position the as the minimun of the obstacle up facing edge and the desidered posiiton
		float      desired_y_position = pbody->m_aabb.p_max.y + pbody->m_velocity.y * g_timer.get_fixed_dt();
		//pbody->m_position.y = (obstacle_border.y - obstacle_border.height < desired_y_position) ? (obstacle_border.y - obstacle_border.height) : (desired_y_position);

		if (obstacle_border.y - obstacle_border.height < desired_y_position) {//collided
			//std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;

			float y_offset = obstacle_border.y - obstacle_border.height - pbody->m_aabb.p_max.y;

			pbody->m_position.y      +=  y_offset;
			pbody->m_velocity.y      =  0.0f;
			pbody->m_acceleration.y  =  0.0f;

			//update the righd body's aabb
			pbody->m_aabb.p_min.y += y_offset;
			pbody->m_aabb.p_max.y += y_offset;
		}
		else {
			pbody->m_position.y  += pbody->m_velocity.y * g_timer.get_fixed_dt();
			
			//update the righd body's aabb
			pbody->m_aabb.p_min.y += pbody->m_velocity.y * g_timer.get_fixed_dt();
			pbody->m_aabb.p_max.y += pbody->m_velocity.y * g_timer.get_fixed_dt();
		}
	//	std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
	else {
		
		//std::cout << "------------------------------------------IN MOVING DOWN TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		//get down facing edge
		cgm::vec2 bottom_left   =  pbody->m_aabb.p_min;
		cgm::vec2 bottom_right  =  pbody->m_aabb.p_max;
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
		Tile     tile;
		unsigned closest_obstacle_row = m_pmap->height() - 1;
		unsigned closest_obstacle_column = tile_bottom_left_coord.second;
		float      maximum_y_tiles_disp = std::ceil( m_pmap->world_to_tile_displacement_y(-pbody->m_velocity.y * g_timer.get_fixed_dt()));
		//std::cout << "maximun fucking tile disp =" << maximum_y_tiles_disp << std::endl;
		//std::cout << "tile_bottom_left_coord.first + maximum_y_tiles_disp = " << tile_bottom_left_coord.first + maximum_y_tiles_disp << std::endl;
		for (unsigned row = MIN(tile_bottom_left_coord.first + maximum_y_tiles_disp, closest_obstacle_row); row >= tile_bottom_left_coord.first; --row) {
			//std::cout << "in row loop" << std::endl;
			for (unsigned column = tile_bottom_left_coord.second; column <= tile_bottom_right_coord.second; ++column) {
				//std::cout << "[" << row << ", " << column << "]" << std::endl;
				//	count++;
				unsigned id    =  m_pmap->get_tile_id(0, row, column);
				tile  =  m_pmap->get_tile(id);
				if (tile.m_is_obstacle) {
					if ((row < closest_obstacle_row) && (column >= closest_obstacle_column)) {
						closest_obstacle_row     =  row;
						closest_obstacle_column  =  column;
					}
				}
			}
		}
		//std::cout << "LOOP ITERATED " << count << " TIMES " << std::endl;;
		//get the world coordinates of the bounds of the closest vertical obstacle
		tgs::Rect obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);

		//update the position the as the minimun of the obstacle up facing edge and the desidered posiiton
		float      desired_y_position = pbody->m_aabb.p_min.y + pbody->m_velocity.y;// *Timer::instance().get_delta();
		

		if (obstacle_border.y > desired_y_position) { //collied
			//std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;

			float y_offset = obstacle_border.y - pbody->m_aabb.p_min.y;

			pbody->m_position.y      += y_offset;
			pbody->m_velocity.y      =  0.0f;
			pbody->m_acceleration.y  =  0.0f;

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
		//std::cout << "---------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
}

void physics_2d::World::update() 
{
	 //update positions, check and solve map collision logic

	for (auto iter = m_bodies.begin(); iter != m_bodies.end(); ++iter) {
		switch ((*iter)->get_type()) {
		case Body_2d::DYNAMIC:
			(*iter)->m_velocity.x += ((*iter)->m_acceleration.x + m_gravity.x) * g_timer.get_fixed_dt();
			(*iter)->m_velocity.y += ((*iter)->m_acceleration.y +  m_gravity.y) * g_timer.get_fixed_dt();
			//std::cout << "m_velocity.y = " << (*iter)->m_velocity.y << std::endl;
			std::cout << "m_velocity.x = " << (*iter)->m_velocity.x << std::endl;
			
			/*
			if ((*iter)->m_vel_threshold.x != 0.0f) {
				if (std::fabs((*iter)->m_velocity.x) > (*iter)->m_vel_threshold.x) {
					(*iter)->m_velocity.x = ((*iter)->m_velocity.x > 0.0f)  ?((*iter)->m_vel_threshold.x) :(- (*iter)->m_vel_threshold.x) ;
				}
			}*/
			if ((*iter)->m_vel_threshold.y != 0.0f) {
				if ((*iter)->m_velocity.y < -0.40f) {
					(*iter)->m_velocity.y = -0.40f;
					
				}
				/*
				if ( std::fabs((*iter)->m_velocity.y) > (*iter)->m_vel_threshold.y) {
					(*iter)->m_velocity.y = ((*iter)->m_velocity.y > 0.0f) ? ((*iter)->m_vel_threshold.y) : (-(*iter)->m_vel_threshold.y);
										std::cout << "--------------hit velocity treshold------------------------------" << std::endl;
				}*/
			}


			//(*iter)->m_position.x += (*iter)->m_velocity.x * Timer::instance().get_delta();
			//(*iter)->m_position.y += (*iter)->m_velocity.y * Timer::instance().get_delta();
			
			if ( ( (*iter)->m_velocity.x != 0.0f) || ((*iter)->m_velocity.y != 0.0f) ) {
				check_n_solve_map_collision(*iter);
			}

			break;
		case Body_2d::KINEMATIC:
			(*iter)->m_velocity.x += (*iter)->m_acceleration.x * g_timer.get_fixed_dt();
			(*iter)->m_velocity.y += (*iter)->m_acceleration.y * g_timer.get_fixed_dt();
			
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