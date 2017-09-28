#include "World.hpp"
#include "Timer.hpp"
#include <vector>
#include <utility>

physics_2d::World::World(const cgm::vec2 & gravity) : m_gravity(gravity) {}

physics_2d::Body_2d * physics_2d::World::create_body_2d(const Entity_types & type, const cgm::vec2 pos, const cgm::vec2 & vel, const float m, const AABB_2d & aabb) 
{
	Body_2d * pbody = new Body_2d(type, pos, vel, m, aabb);
	m_bodies.push_back(pbody);

	return pbody;
}

///CHANGE FUNCTION NAME, IT CHECKS MAP COLLLISION AND UPDATES THE POSITION | 
void physics_2d::World::check_n_solve_map_collision(physics_2d::Body_2d *pbody)
{
	//compute the direction the body moved according to the velocity magnitude
	bool is_moving_right =  (pbody->m_velocity.x > 0.0f) ? (true) : (false);
	bool is_moving_up	= (pbody->m_velocity.y > 0.0f) ? (true) : (false);

	//Attempt to move horizontally
	if (is_moving_right) {
		//get the right facing edge in world space
		cgm::vec2 up_right      =  pbody->m_aabb.p_max;
		cgm::vec2 bottom_right  =  pbody->m_aabb.p_min;
		bottom_right.x          =  up_right.x;

		std::pair<float, float> tile_up_right_coord = m_pmap->wld_to_tile_space(up_right);
		std::pair<float, float> tile_bottom_right_coord = m_pmap->wld_to_tile_space(bottom_right);

		//find the closest horizontal obstacle
		unsigned closest_obstacle_row = tile_up_right_coord.first;
		unsigned closest_obstacle_column = m_pmap->height() - 1;
		for (unsigned row = tile_up_right_coord.first; row <= tile_bottom_right_coord.first; ++row) {
			for (unsigned column = m_pmap->height() - 1; column >= tile_up_right_coord.second; --column) {
				unsigned id = m_pmap->get_tile_id(0, row, column);
				Tile tile = m_pmap->get_tile(id);
				if (tile.m_is_obstacle) {
					if ((row >= closest_obstacle_row) && (column < closest_obstacle_column)) {
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					}
				}
			}
		}
		//get the world coordinates of the bounds of the closest horizontal obstacle
		tgs::Rect  obstacle_border = m_pmap->tile_wld_space_coord(closest_obstacle_row, closest_obstacle_column);

		//update the position the as the minimun of the obstacle left facing edge and the desidered posiiton
		float      desired_x_position = pbody->m_position.x + (*iter)->m_velocity.x * Timer::instance().get_delta();
		//pbody->m_position.x = (obstacle_border.x < desired_x_position) ? (obstacle_border.x) : (desired_x_position);

		if (obstacle_border.x < desired_x_position) { // if collided with the map
			pbody->m_position.x      =  obstacle_border.x;
			pbody->m_velocity.x      =  0.0f;
			pbody->m_acceleration.x  =  0.0f;
		}
		else {
			pbody->m_position.x = (desired_x_position);
		}

		pbody->m_aabb.p_min += pbody->m_position;
		pbody->m_aabb.p_max += pbody->m_position;
	}
	else {
		//get left facing edge
		cgm::vec2 up_left = pbody->m_aabb.p_max;
		cgm::vec2 bottom_left = pbody->m_aabb.p_min;
		up_left.x			  = bottom_left.x;

		//get the rows and columns that need to be checked against collision
		std::pair<float, float> tile_up_left_coord		=  m_pmap->wld_to_tile_space(up_left);
		std::pair<float, float> tile_bottom_left_coord  = m_pmap->wld_to_tile_space(bottom_left);
		
		//find the closest horizontal obstacle
		unsigned closest_obstacle_row = tile_up_left_coord.first;
		unsigned closest_obstacle_column = 0;
		for (unsigned row = tile_up_left_coord.first; row <= tile_bottom_left_coord.first; ++row) {
			for (unsigned column = 0; column <= tile_up_left_coord.second; ++column) {
				unsigned id = m_pmap->get_tile_id(0, row, column);
				Tile tile = m_pmap->get_tile(id);
				if (tile.m_is_obstacle) { //if this tile is a obsctale
					if ( (row >= closest_obstacle_row) && (column > closest_obstacle_column) ) {
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					}
				}
			}
		}
		//get the world coordinates of the bounds of the closest horizontal obstacle
		tgs::Rect  obstacle_border = m_pmap->tile_wld_space_coord(closest_obstacle_row, closest_obstacle_column);
		
		//update the position the as the minimun of the obstacle left facing edge and the desidered posiiton
		float      desired_x_position = pbody->m_position.x + (*iter)->m_velocity.x * Timer::instance().get_delta();
		//pbody->m_position.x = (obstacle_border.x + obstacle_border.width > desired_x_position) ? (obstacle_border.x + obstacle_border.width) : (desired_x_position);

		if (obstacle_border.x + obstacle_border.width > desired_x_position) { //collided
			pbody->m_position.x  =  obstacle_border.x + obstacle_border.width;
			pbody->m_velocity.x      =  0.0f;
			pbody->m_acceleration.x  =  0.0f;
		}
		else {
			pbody->m_position.x  =  desired_x_position;
		}

		pbody->m_aabb.p_min += pbody->m_position;
		pbody->m_aabb.p_max += pbody->m_position;
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

		//find the closest vertical obstacle
		unsigned closest_obstacle_row = 0;
		unsigned closest_obstacle_column = tile_up_left_coord.second;
		for (unsigned row = 0; row < tile_up_left_coord.first; ++row ) {
			for (unsigned column = tile_up_left_coord.second; column <= tile_up_right_coord.second; ++column) {
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
		float      desired_y_position = pbody->m_position.y + (*iter)->m_velocity.y * Timer::instance().get_delta();
		//pbody->m_position.y = (obstacle_border.y - obstacle_border.height < desired_y_position) ? (obstacle_border.y - obstacle_border.height) : (desired_y_position);

		if (obstacle_border.y - obstacle_border.height < desired_y_position) {//collided
			pbody->m_position.y      =  obstacle_border.y - obstacle_border.height;
			pbody->m_velocity.y      =  0.0f;
			pbody->m_acceleration.y  =  0.0f;
		}
		else {
			pbody->m_position.y  = desired_y_position;
		}
		//update the righd body's aabb
		pbody->m_aabb.p_min += pbody->m_position;
		pbody->m_aabb.p_max += pbody->m_position;
	}
	else {
		//get down facing edge
		cgm::vec2 bottom_left   =  pbody->m_aabb.p_min;
		cgm::vec2 bottom_right  =  pbody->m_aabb.p_max;
		bottom_right.y = bottom_left.y;

		//get the rows and columns that need to be checked against collision
		std::pair<float, float> tile_bottom_left_coord = m_pmap->wld_to_tile_space(bottom_left);
		std::pair<float, float> tile_bottom_right_coord = m_pmap->wld_to_tile_space(bottom_right);
		
		//find the closest vertical obstacle
		unsigned closest_obstacle_row = m_pmap->height() - 1;
		unsigned closest_obstacle_column = tile_bottom_left_coord.second;
		for (unsigned row = m_pmap->height() - 1; row > tile_bottom_left_coord.first; --row) {
			for (unsigned column = tile_bottom_left_coord.second; column <= tile_bottom_right_coord.second; ++column) {
				unsigned id    =  m_pmap->get_tile_id(0, row, column);
				Tile     tile  =  m_pmap->get_tile(id);
				if (tile.m_is_obstacle) {
					if ((row < closest_obstacle_row) && (column >= closest_obstacle_column)) {
						closest_obstacle_row     =  row;
						closest_obstacle_column  =  column;
					}
				}
			}
		}
		//get the world coordinates of the bounds of the closest vertical obstacle
		tgs::Rect obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);

		//update the position the as the minimun of the obstacle up facing edge and the desidered posiiton
		float      desired_y_position = pbody->m_position.y + (*iter)->m_velocity.y * Timer::instance().get_delta();
		pbody->m_position.y = (obstacle_border.y > desired_y_position) ? (obstacle_border.y) : (desired_y_position);

		if (obstacle_border.y > desired_y_position) { //collied
			pbody->m_position.y      =  obstacle_border.y;
			pbody->m_velocity.y      =  0.0f;
			pbody->m_acceleration.y  =  0.0f;
		}
		else {
			pbody->m_position.y  = desired_y_position;
		}

		pbody->m_aabb.p_min += pbody->m_position;
		pbody->m_aabb.p_max += pbody->m_position;
	}
}

void physics_2d::World::update() 
{
	 //update positions, check and solve map collision logic

	for (auto iter = m_bodies.begin(); iter != m_bodies.end(); ++iter) {
		switch ((*iter)->get_type()) {
		case DYNAMIC:
			(*iter)->m_velocity.x += (*iter)->m_acceleration.x * Timer::instance().get_delta() + m_gravity.x;
			(*iter)->m_velocity.y += (*iter)->m_acceleration.y * Timer::instance().get_delta() + m_gravity.y;
			//(*iter)->m_position.x += (*iter)->m_velocity.x * Timer::instance().get_delta();
			//(*iter)->m_position.y += (*iter)->m_velocity.y * Timer::instance().get_delta();
			
			if ( ( (*iter)->m_velocity.x != 0.0f) || ((*iter)->m_velocity.y != 0.0f) ) {
				check_n_solve_map_collision(*iter);
			}

			break;
		case KINEMATIC:
			(*iter)->m_velocity.x += (*iter)->m_acceleration.x * Timer::instance().get_delta();
			(*iter)->m_velocity.y += (*iter)->m_acceleration.y * Timer::instance().get_delta();
			//(*iter)->m_position.x += (*iter)->m_velocity.x * Timer::instance().get_delta();
			//(*iter)->m_position.y += (*iter)->m_velocity.y * Timer::instance().get_delta();
			break;
		}
	}

}