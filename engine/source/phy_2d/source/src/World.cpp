#include "World.hpp"
#include "Collision_listener.hpp"
#include "Body_2d.hpp"
#include "Body_2d_def.hpp"
#include "Collider_2d.hpp"
#include "AABB_2d.hpp"

#include "Broadphase.hpp"
#include "N_squared_broadphase.hpp"

#include "Tile_map.hpp"
#include "Rect.hpp"
#include "ray3.hpp"
#include "Tile.hpp"

#include <algorithm>
#include <vector>
#include <utility>
#include <cmath>
#include <cassert>


physics_2d::World::World(const math::vec2 & gravity, const math::vec2 & solid_tile_sensor_line) :
	m_gravity(gravity), m_solid_tile_sensor_line(solid_tile_sensor_line) ,m_pmap(nullptr), m_pcoll_listener(nullptr), m_pbody_list(nullptr), m_pcontact_list(nullptr), m_contact_cnt(0),
	m_body_cnt(0) ,m_body_allocator(sizeof(physics_2d::Body_2d), MAX_BODY_2DS, 4), m_collider_allocator(sizeof(Collider_2d), MAX_BODY_2DS, 4), 
	m_collider_proxy_allocator(sizeof(Collider_2d_proxy), MAX_BODY_2DS, 4), m_contact_allocator(sizeof(Contact_node), MAX_CONTACTS, 4)
{
	m_pbroadphase = new N_squared_broadphase();
}

physics_2d::World::~World() 
{
	/*
	for (auto it = m_bodies.begin(); it != m_bodies.end(); ++it) {
		m_mem_pool.free_element(static_cast<void*>(*it));
	}*/
	Body_2d *pbody = m_pbody_list;
	while (pbody) {
		Body_2d *pb = pbody->m_pnext;
		destroy_body_2d(pbody);
		pbody = pb;
	}
	m_pbody_list = nullptr;
	m_body_cnt   = 0; 
	//memory pool destructor is called implicitly to return the memory to the system
}
/*
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
*/

physics_2d::Body_2d * physics_2d::World::create_body_2d(const Body_2d_def & body_def)
{
	Body_2d *pbody = nullptr;
	void *pmem = m_body_allocator.get_element();
	if (pmem != nullptr) {
		pbody = static_cast<Body_2d*>(new (pmem) Body_2d(&body_def, this));
		
		//add it to the start of the list
		pbody->m_pprev = nullptr;
		pbody->m_pnext = m_pbody_list;
		if (m_pbody_list) {
			m_pbody_list->m_pprev = pbody;
		}
		m_pbody_list = pbody;
		++m_body_cnt;
		//m_bodies.push_back(pbody);

	}

	return pbody;
}

void physics_2d::World::destroy_body_2d(physics_2d::Body_2d *pbody) 
{
	//remove from the doubled linked list
	if (pbody->m_pprev) {
		// if not the first element in the list
		(pbody->m_pprev)->m_pnext = pbody->m_pnext;
	}
	else {
		m_pbody_list = pbody->m_pnext;
	}

	if (pbody->m_pnext) {
		(pbody->m_pnext)->m_pprev = pbody->m_pprev;
	}

	--m_body_cnt;

	//remove the contacts that have to body's collider proxy, if body_2d is active
	if (pbody->is_active()) {
		int32_t proxy_id = pbody->get_collider()->get_proxy_id();
		Collider_2d_proxy *pcoll_proxy = static_cast<Collider_2d_proxy*>(m_pbroadphase->get_user_data(proxy_id));
		remove_contacts_with_proxy(pcoll_proxy);
	}
	//destroy collider_2d, remove proxy from broadphase and deallocate resources
	pbody->~Body_2d();
	m_body_allocator.free_element(static_cast<void*>(pbody));
}	


void physics_2d::World::add_to_contact_list(physics_2d::Collider_2d_proxy *pproxy_a, physics_2d::Collider_2d_proxy *pproxy_b) 
{
	//allocate a new node from the pool
	void *pmem = m_contact_allocator.get_element();
	if (pmem) {
		Contact_node *pnode = static_cast<Contact_node*>(new (pmem) Contact_node);
		pnode->pnext = m_pcontact_list;
		pnode->pprev = nullptr;
		if (m_pcontact_list) {
			m_pcontact_list->pprev = pnode;
		}
		m_pcontact_list = pnode;

		pnode->pproxy_a = pproxy_a;
		pnode->pproxy_b = pproxy_b;
		++m_contact_cnt;
	}	
}

void physics_2d::World::remove_from_contact_list(physics_2d::Contact_node *pcontact) 
{
	if (pcontact->pprev) {
		pcontact->pprev->pnext = pcontact->pnext;
	}
	else {
		m_pcontact_list = pcontact->pnext;
	}
	if (pcontact->pnext) {
		pcontact->pnext->pprev = pcontact->pprev;
	}
	//deallocate
	m_contact_allocator.free_element(static_cast<void*>(pcontact));
	--m_contact_cnt;
}

void physics_2d::World::remove_contacts_with_proxy(physics_2d::Collider_2d_proxy *pcoll_proxy) 
{
	//iterate through all the contacts, and remove all that have the proxy's collider
	Contact_node *pcontact = m_pcontact_list;
	while (pcontact) {
		if ((pcontact->pproxy_a == pcoll_proxy) || (pcontact->pproxy_b == pcoll_proxy)) {
			Contact_node *pnext_contact = pcontact->pnext;
			remove_from_contact_list(pcontact);
			pcontact = pnext_contact;
		}
		else {
			pcontact = pcontact->pnext;
		}
	}
}

bool physics_2d::World::try_climbing_ladder(physics_2d::Body_2d * pbody, const bool is_climbing_up) 
{
	assert(pbody != nullptr && "NULL Body_2d pointer");

	//world space bottom left and top right coordinates of the body's aabb
	const AABB_2d & aabb = pbody->get_collider()->get_aabb();

	math::vec2 bottom_left = aabb.p_min; // pbody->m_aabb.p_min;
	math::vec2 top_right   = aabb.p_max; //pbody->m_aabb.p_max;

	//scale down the aabb horizontal size, so it can only climb if it is close to the ladder column
	float x_magnitude = top_right.x - bottom_left.x;
	bottom_left.x += 0.2f * x_magnitude;
	top_right.x -= 0.2f * x_magnitude;

	if (is_climbing_up) {
		bottom_left.y = (aabb.p_min.y + aabb.p_max.y) / 2.0f;
	}

	// tile map space coordinates, [row, column] of aabb bottom left and top right
	std::pair<float, float> tile_bottom_left_coord =  m_pmap->wld_to_tile_space(bottom_left);
	std::pair<float, float> tile_top_right_coord   =  m_pmap->wld_to_tile_space(top_right);

	std::pair<unsigned, unsigned> columns_to_check{ tile_bottom_left_coord.second, tile_top_right_coord.second };
	unsigned row = tile_bottom_left_coord.first;

	//std::cout << "columns interval to check [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;
	//std::cout << "row to check: " << row << std::endl;

	const Tile *ptile;
	unsigned id;	
	for (unsigned column = columns_to_check.first; column <= columns_to_check.second; ++column) {
		id = m_pmap->get_tile_id(0, row, column);
		ptile = m_pmap->get_tile(id);
		if(ptile->m_is_ladder) {
			//reposition the character to be inside the ladder
			math::Rect ladder_tile_bounds = m_pmap->tile_wld_space_bounds(row, column);
			
			float aabb_center_x = (aabb.p_max.x + aabb.p_min.x) / 2.0f;
			float tile_center_x = (ladder_tile_bounds.x + (ladder_tile_bounds.x + ladder_tile_bounds.width)) / 2.0f;
			float  x_offset = tile_center_x - aabb_center_x;
			if ( (top_right.x < ladder_tile_bounds.x + ladder_tile_bounds.width ) || (bottom_left.x > ladder_tile_bounds.x)) { // on the right
				math::vec2 displacement(x_offset, 0.0f);
				pbody->translate_by(displacement);
				/*
				pbody->m_aabb.p_max.x += x_offset;
				pbody->m_aabb.p_min.x += x_offset;
				pbody->m_position.x += x_offset;
				*/
			}
			if (!is_climbing_up) { // if is climbing down, we need to position the body inside the ladder
				float y_offset = 0.8f * ( (ladder_tile_bounds.y - ladder_tile_bounds.height) - aabb.p_min.y );
				math::vec2 displacement(0.0f, y_offset);
				pbody->translate_by(displacement);
				/*
				pbody->m_aabb.p_max.y += y_offset;
				pbody->m_aabb.p_min.y += y_offset;
				pbody->m_position.y += y_offset;
				*/
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
	const AABB_2d & aabb = pbody->get_collider()->get_aabb();
	
	math::vec2 top_right = aabb.p_max;
	math::vec2 bottom_left = aabb.p_min;
	top_right.x = bottom_left.x = (aabb.p_min.x + aabb.p_max.x) / 2.0f;
	top_right.y = aabb.p_min.y * 0.3f + aabb.p_max.y * 0.7f;
	std::pair<float, float> tile_top_right_coord = m_pmap->wld_to_tile_space(top_right);
	//std::pair<float, float> tile_bottom_left_coord = m_pmap->wld_to_tile_space(bottom_left);

	unsigned id;
	const Tile *ptile;	
	//for (int row = tile_top_right_coord.first; row <= tile_bottom_left_coord.first; ++row) {
		id = m_pmap->get_tile_id(0, tile_top_right_coord.first, tile_top_right_coord.second);
		ptile = m_pmap->get_tile(id);
		if (ptile->m_is_ladder) {
			return true;
		}
//	}
	return false;
}

bool physics_2d::World::is_on_ladder_top_tile(const physics_2d::Body_2d * pbody, math::Rect & ladder_bounds) const
{
	assert(pbody != nullptr && "NULL Body_2d pointer");

	// get the bottom left and bottom right AABB world space coords
	const AABB_2d & aabb = pbody->get_collider()->get_aabb();

	math::vec2 top_center = aabb.p_max;
	math::vec2 bottom_center = aabb.p_min;
	bottom_center.x = top_center.x = (aabb.p_min.x + aabb.p_max.x) / 2.0f;
	 

	std::pair<float, float> tile_top_coord = m_pmap->wld_to_tile_space(top_center);
	std::pair<float, float> tile_bottom_coord = m_pmap->wld_to_tile_space(bottom_center);
	
	std::pair<unsigned, unsigned> rows_to_check{ tile_top_coord.first, tile_bottom_coord.first };
	unsigned column = tile_top_coord.second;
	
	unsigned id;
	const Tile *ptile;
	for (unsigned row = rows_to_check.first; row <= rows_to_check.second; ++row) {
		id = m_pmap->get_tile_id(0, row, column);
		ptile = m_pmap->get_tile(id);
		if (ptile->m_is_ladder && ptile->m_is_one_way) {
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

	const AABB_2d & aabb = pbody->get_collider()->get_aabb();
	// get the bottom left and bottom right AABB world space coords
	math::vec2 bottom_left = aabb.p_min;
	math::vec2 bottom_right = aabb.p_max;
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
	const Tile *ptile;
	unsigned id;
	math::Rect border;
	// search the columns o encompassed by the body's aabb and, check if any of the tiles are solid tiles
	for (unsigned column = columns_to_check.first; column <= columns_to_check.second; column++) {
		id = m_pmap->get_tile_id(0, row, column);
		ptile = m_pmap->get_tile(id);
		border = m_pmap->tile_wld_space_bounds(row, column);
		if (ptile->m_is_obstacle || (ptile->m_is_one_way && border.y <= aabb.p_min.y))  {
			return true;
		}
	}

	return false;
}

bool physics_2d::World::trace_on_map(const math::ray3 & r, const Tile **pphit, const unsigned steps) const
{
        math::vec3 p(math::g_zero_vec3);
        std::pair<float, float> tile_coord;
        const Tile *ptile;
        const float tile_sz_wld_units = m_pmap->tile_height() / m_pmap->pixels_per_world_unit();

        for (unsigned i = 0; i != steps; ++i) {
                p = r.origin + r.direction * (i * tile_sz_wld_units);
                if (!m_pmap->is_inside_map(p)) { //is outside
                        return true;
                }
                //get point in tile space and corresponding tile
                tile_coord = m_pmap->wld_to_tile_space(p);
                ptile = m_pmap->get_tile(m_pmap->get_tile_id(0, tile_coord.first, tile_coord.second));
                if (ptile->m_is_obstacle || ptile->m_is_one_way) {
                        pphit = &ptile;
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
		math::vec2 up_right =     pbody->get_collider()->get_aabb().p_max;//  pbody->m_aabb.p_max;
		math::vec2 bottom_right = pbody->get_collider()->get_aabb().p_min;// pbody->m_aabb.p_min;
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
		const Tile      *pclosest_obstacle_tile = m_pmap->get_tile(id);

		std::pair<unsigned, unsigned> rows_to_check = { tile_up_right_coord.first, tile_bottom_right_coord.first };
		std::pair<unsigned, unsigned> columns_to_check = { tile_up_right_coord.second, farthest_column};
		//std::cout << "Rows interval to check = [" << rows_to_check.first << ", " << rows_to_check.second << "]" << std::endl;
		//std::cout << "columns interval to check = [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;
		
		
		//start checking from farthest calculated column to the bodys current column
		for (unsigned row = rows_to_check.first; row <= rows_to_check.second; ++row) {
			for (unsigned column = columns_to_check.second; column >= columns_to_check.first; --column) {
			//	std::cout << "[" << row << ", " << column << "]" << std::endl;
				const Tile *ptile;
				unsigned id = m_pmap->get_tile_id(0, row, column);
				ptile = m_pmap->get_tile(id);
				 if (ptile->m_is_obstacle) {
					if ((row >= closest_obstacle_row) && (column <= closest_obstacle_column)) {
						pclosest_obstacle_tile = ptile;
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
		float      desired_x_position = pbody->get_collider()->get_aabb().p_max.x + pbody->m_velocity.x * dt;
		//pbody->m_position.x = (obstacle_border.x < desired_x_position) ? (obstacle_border.x) : (desired_x_position);

		if ( (obstacle_border.x - sensor_line_width) < desired_x_position && pclosest_obstacle_tile->m_is_obstacle ) { // if collided with the map
		//	std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;
        //        std::cout << "Body Collided moving RIGHT, WITH TILE: " << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;
			
			float x_offset = (obstacle_border.x - sensor_line_width) - pbody->get_collider()->get_aabb().p_max.x;

			//check if we can push the player in the other direction or if it is going to collide with a tile on their left
			math::vec2 up_left = pbody->get_collider()->get_aabb().p_max;
			math::vec2 bottom_left = pbody->get_collider()->get_aabb().p_min;
			bottom_left.x += x_offset;
			up_left.x = bottom_left.x;

			bottom_left.y += sensor_line_height;

			std::pair<float, float> tile_up_left = m_pmap->wld_to_tile_space(up_left);
			std::pair<float, float> tile_bottom_left = m_pmap->wld_to_tile_space(bottom_left);
			
			tile_up_left.first = (tile_up_left.first > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_up_left.first);
			tile_bottom_left.first = (tile_bottom_left.first > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_bottom_left.first);

			tile_up_left.first =     (tile_up_left.first < 0.0f) ? (0.0f) : (tile_up_left.first);
			tile_bottom_left.first = (tile_bottom_left.first < 0.0f) ? (0.0f) : (tile_bottom_left.first);

			const Tile *pbottom_tile = m_pmap->get_tile(m_pmap->get_tile_id(0, tile_up_left.first, tile_up_left.second));
		        const Tile *pup_tile     = m_pmap->get_tile(m_pmap->get_tile_id(0, tile_bottom_left.first, tile_bottom_left.second));

			if (!pbottom_tile->m_is_obstacle && !pup_tile->m_is_obstacle) {
				pbody->m_velocity.x = 0.0f;
				pbody->m_acceleration.x = 0.0f;

				math::vec2 t(x_offset, 0.0f);
				pbody->translate_by(t);
				/*
				pbody->m_position.x += x_offset;
				pbody->m_aabb.p_min.x += x_offset;
				pbody->m_aabb.p_max.x += x_offset;
				*/
			}
		}
		else {
			//check collision against the map border, i.e, if this body is going beyond the last column of tiles of the map
			if ( (closest_obstacle_column == m_pmap->width() - 1) && (desired_x_position >obstacle_border.x + obstacle_border.width - sensor_line_width) ) {
				//trying to go off map, set the position to the maps border
				float x_offset = (obstacle_border.x + obstacle_border.width - sensor_line_width) - pbody->get_collider()->get_aabb().p_max.x;
				math::vec2 t(x_offset, 0);
				pbody->translate_by(t);
			
				pbody->m_velocity.x = 0.0f;
				pbody->m_acceleration.x = 0.0f;
			}
			else {
				math::vec2 t(pbody->m_velocity.x * dt, 0.0f);
				pbody->translate_by(t);
				/*
				pbody->m_position.x += pbody->m_velocity.x * dt;
				pbody->m_aabb.p_min.x += pbody->m_velocity.x * dt;
				pbody->m_aabb.p_max.x += pbody->m_velocity.x * dt;
				*/
			}
		}
	//	std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
	else if (pbody->m_velocity.x != 0.0f) {
		
		//std::cout << "------------------------------------------IN MOVING LEFT TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		//get left facing edge
		math::vec2 up_left = pbody->get_collider()->get_aabb().p_max;// pbody->m_aabb.p_max;
		math::vec2 bottom_left = pbody->get_collider()->get_aabb().p_min;// pbody->m_aabb.p_min;
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
		const Tile *pclosest_obstacle_tile = m_pmap->get_tile(id);
		
		std::pair<unsigned, unsigned> rows_to_check{ tile_up_left_coord.first, tile_bottom_left_coord.first };
		std::pair<unsigned, unsigned> columns_to_check{ farthest_column, tile_up_left_coord.second };

		//std::cout << "Rows interval to check = [" << rows_to_check.first << ", " << rows_to_check.second << "]" << std::endl;
		//std::cout << "columns interval to check = [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;

		for (unsigned row = rows_to_check.first; row <= rows_to_check.second; ++row) {
			for (unsigned column = columns_to_check.first; column <= columns_to_check.second; ++column) {
				//std::cout << "[" << row << ", " << column << "]" << std::endl;
				const Tile *ptile;
				id = m_pmap->get_tile_id(0, row, column);
				ptile = m_pmap->get_tile(id);
				if (ptile->m_is_obstacle) { //if this tile is a obsctale
					if ( (row >= closest_obstacle_row) && (column >= closest_obstacle_column) ) {
						pclosest_obstacle_tile = ptile;
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					}
				}
			}
		}
		//get the world coordinates of the bounds of the closest horizontal obstacle
		math::Rect  obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);
		
		//update the position the as the minimun of the obstacle left facing edge and the desidered posiiton
		float      desired_x_position = pbody->get_collider()->get_aabb().p_min.x + pbody->m_velocity.x * dt;
		//pbody->m_position.x = (obstacle_border.x + obstacle_border.width > desired_x_position) ? (obstacle_border.x + obstacle_border.width) : (desired_x_position);
		//std::cout << obstacle_border.x + obstacle_border.width + FLOAT_ROUNDOFF << " and " << desired_x_position - FLOAT_ROUNDOFF << std::endl;
		if ( (obstacle_border.x + obstacle_border.width + sensor_line_width > desired_x_position ) && pclosest_obstacle_tile->m_is_obstacle){ //collided
		//	std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;
       //         std::cout << "BODY COLLIDED MOVING LEFT, WITH TILE:" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;
			
			float  x_offset = (obstacle_border.x + obstacle_border.width + sensor_line_width) - pbody->get_collider()->get_aabb().p_min.x;
			
			math::vec2 top_right = pbody->get_collider()->get_aabb().p_max;// pbody->m_aabb.p_max;
			math::vec2 bottom_right = pbody->get_collider()->get_aabb().p_min;// pbody->m_aabb.p_min;
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

			const Tile *ptop_tile    =  m_pmap->get_tile(m_pmap->get_tile_id(0, tile_top_right.first, tile_top_right.second));
			const Tile *pbottom_tile =  m_pmap->get_tile(m_pmap->get_tile_id(0, tile_bottom_right.first, tile_bottom_right.second));

			if (!ptop_tile->m_is_obstacle && !pbottom_tile->m_is_obstacle) {
				pbody->m_velocity.x = 0.0f;
				pbody->m_acceleration.x = 0.0f;

				math::vec2 t(x_offset, 0.0f);
				pbody->translate_by(t);
				/*
				pbody->m_position.x += x_offset;
				pbody->m_aabb.p_min.x += x_offset;
				pbody->m_aabb.p_max.x += x_offset;
				*/
			}
		}
		else {
			if ((closest_obstacle_column == 0) && (desired_x_position < obstacle_border.x + sensor_line_width)) {
				//trying to go off map, set the position to the maps border
				float x_offset = (obstacle_border.x + sensor_line_width) - pbody->get_collider()->get_aabb().p_min.x;// pbody->m_aabb.p_min.x;
				math::vec2 t(x_offset, 0);
				pbody->translate_by(t);

				pbody->m_velocity.x = 0.0f;
				pbody->m_acceleration.x = 0.0f;
			}
			else {
				math::vec2 t(pbody->m_velocity.x * dt, 0.0f);
				pbody->translate_by(t);
				/*
				pbody->m_position.x += pbody->m_velocity.x * dt;

				pbody->m_aabb.p_min.x += pbody->m_velocity.x * dt;
				pbody->m_aabb.p_max.x += pbody->m_velocity.x * dt;
				//	std::cout << "no collision" << std::endl;
				*/
			}
		}
		//std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}

	// vertical movement
	if (is_moving_up) {
		//get up facing edge
		math::vec2 up_left = pbody->get_collider()->get_aabb().p_min;//  pbody->m_aabb.p_min;
		math::vec2 up_right = pbody->get_collider()->get_aabb().p_max;// pbody->m_aabb.p_max;
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
		const Tile *pclosest_tile = m_pmap->get_tile(id);
		
		std::pair<unsigned, unsigned> rows_to_check{ farthest_row, tile_up_left_coord.first };
		std::pair<unsigned, unsigned> columns_to_check{ tile_up_left_coord.second, tile_up_right_coord.second };

	//	std::cout << "Rows interval to check = [" << rows_to_check.first << ", " << rows_to_check.second << "]" << std::endl;
	//	std::cout << "columns interval to check = [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;

		for (unsigned row = rows_to_check.first; row <= rows_to_check.second; ++row ) {
			for (unsigned column = columns_to_check.first; column <= columns_to_check.second; column++) {
				id     =  m_pmap->get_tile_id(0, row, column);
				const Tile      *ptile = m_pmap->get_tile(id);
				if (ptile->m_is_obstacle) {// || one way
					if ( (row >= closest_obstacle_row) && (column >= closest_obstacle_column)) {
						pclosest_tile = ptile;
						closest_obstacle_row = row;
						closest_obstacle_column = column;
					} 
				}
			}
		}
		//get the world coordinates of the bounds of the closest vertical obstacle
		math::Rect obstacle_border = m_pmap->tile_wld_space_bounds(closest_obstacle_row, closest_obstacle_column);

		if (pclosest_tile->m_is_obstacle || (pclosest_tile->m_is_one_way && (obstacle_border.y <= pbody->get_collider()->get_aabb().p_min.y))) { // treat has a obstacle

		//update the position the as the minimun of the obstacle up facing edge and the desidered posiiton
			float      desired_y_position = pbody->get_collider()->get_aabb().p_max.y + pbody->m_velocity.y * dt; //* g_timer.get_fixed_dt();
			//pbody->m_position.y = (obstacle_border.y - obstacle_border.height < desired_y_position) ? (obstacle_border.y - obstacle_border.height) : (desired_y_position);

			if (obstacle_border.y - obstacle_border.height - sensor_line_height < desired_y_position) {//collided
			//	std::cout << "Body collided with tile [" << closest_obstacle_row << ", " << closest_obstacle_column << "]" << std::endl;

				float y_offset = (obstacle_border.y - obstacle_border.height - sensor_line_height) - pbody->get_collider()->get_aabb().p_max.y;

				pbody->m_velocity.y = 0.0f;
				pbody->m_acceleration.y = 0.0f;

				math::vec2 t(0.0f, y_offset);
				pbody->translate_by(t);
				/*
				pbody->m_position.y += y_offset;
				//update the righd body's aabb
				pbody->m_aabb.p_min.y += y_offset;
				pbody->m_aabb.p_max.y += y_offset;
				*/
			}
			else {
				math::vec2 t(0.0f, pbody->m_velocity.y * dt);
				pbody->translate_by(t);

				/*
				pbody->m_position.y += pbody->m_velocity.y * dt;
				//update the righd body's aabb
				pbody->m_aabb.p_min.y += pbody->m_velocity.y * dt;
				pbody->m_aabb.p_max.y += pbody->m_velocity.y * dt;
				*/
			}
		}
		else {
			math::vec2 t(0.0f, pbody->m_velocity.y * dt);
			pbody->translate_by(t);
			/*
			pbody->m_position.y += pbody->m_velocity.y * dt;
			//update the righd body's aabb
			pbody->m_aabb.p_min.y += pbody->m_velocity.y * dt;
			pbody->m_aabb.p_max.y += pbody->m_velocity.y * dt;
			*/
		}
	//	std::cout << "------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
	else {
		
		// STARTING WORKING ON MOVE DOWN COLLLISION DETECTION AND RESOLUTION!!!!!!!!

	//	std::cout << "------------------------------------------IN MOVING DOWN TILE COLLISION DETECTION--------------------------------------------------------------" << std::endl;
		//get down facing edge
		math::vec2 bottom_left = pbody->get_collider()->get_aabb().p_min;// pbody->m_aabb.p_min;
		math::vec2 bottom_right = pbody->get_collider()->get_aabb().p_max;// pbody->m_aabb.p_max;
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
		float		maximum_y_tiles_disp = m_pmap->world_to_tile_displacement_y(-pbody->m_velocity.y * dt);
		float		farthest_row = (tile_bottom_left_coord.first + maximum_y_tiles_disp > m_pmap->height() - 1) ? (m_pmap->height() - 1) : (tile_bottom_left_coord.first + maximum_y_tiles_disp);
		
		//start the closest obstacle with the farthest possible obstacle
		unsigned	closest_obstacle_row = farthest_row;
		unsigned	closest_obstacle_column = tile_bottom_left_coord.second;

		unsigned	id = m_pmap->get_tile_id(0, closest_obstacle_row, closest_obstacle_column);
		const Tile      *pclosest_tile = m_pmap->get_tile(id);

		std::pair<unsigned, unsigned> rows_to_check{ tile_bottom_left_coord.first, farthest_row };
		std::pair<unsigned, unsigned> columns_to_check{ tile_bottom_left_coord.second, tile_bottom_right_coord.second };
		
		//std::cout << "Rows interval to check = [" << rows_to_check.first << ", " << rows_to_check.second << "]" << std::endl;
	//	std::cout << "columns interval to check = [" << columns_to_check.first << ", " << columns_to_check.second << "]" << std::endl;

		for (unsigned row = rows_to_check.second; row >= rows_to_check.first; --row) {
			for (unsigned column = columns_to_check.first; column <= columns_to_check.second; ++column) {
			//	std::cout << "[" << row << ", " << column << "]" << std::endl;
				const Tile     *ptile;
				id    =  m_pmap->get_tile_id(0, row, column);
				ptile  =  m_pmap->get_tile(id);
				if (ptile->m_is_obstacle || ptile->m_is_one_way) {
					if ((row <= closest_obstacle_row) && (column >= closest_obstacle_column)) {
						pclosest_tile = ptile;
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
		float      desired_y_position = pbody->get_collider()->get_aabb().p_min.y + pbody->m_velocity.y * dt;

		//std::cout << "AABB.p_min.y = " << pbody->m_aabb.p_min.y +  sensor_line_height << " | obstacle_border.y " << obstacle_border.y << std::endl;

		if ( (pclosest_tile->m_is_obstacle && obstacle_border.y >= desired_y_position) || (pclosest_tile->m_is_one_way && (obstacle_border.y <= pbody->get_collider()->get_aabb().p_min.y + sensor_line_height))) { // treat has a obstacle
		//update the position the as the minimun of the obstacle up facing edge and the desidered posiiton
		
			float y_offset = obstacle_border.y - pbody->get_collider()->get_aabb().p_min.y;

			pbody->m_velocity.y = 0.0f;
			pbody->m_acceleration.y = 0.0f;

			math::vec2 t(0.0f, y_offset);
			pbody->translate_by(t);
			/*
			pbody->m_position.y += y_offset;
			//update the righd body's aabb
			pbody->m_aabb.p_min.y += y_offset;
			pbody->m_aabb.p_max.y += y_offset;
			*/
		}
		else {
			math::vec2 t(0.0f, pbody->m_velocity.y * dt);
			pbody->translate_by(t);

			/*
			pbody->m_position.y += pbody->m_velocity.y * dt; //* Timer::instance().get_delta();
			//update the righd body's aabb
			pbody->m_aabb.p_min.y += pbody->m_velocity.y * dt;// * Timer::instance().get_delta();
			pbody->m_aabb.p_max.y += pbody->m_velocity.y * dt;// * Timer::instance().get_delta();
			*/
		}
	//	std::cout << "---------------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
}

void physics_2d::World::update(const float dt) 
{
	 //UPDATE velocity and positions. Check map collision.

	for (Body_2d *pbody = m_pbody_list; pbody != nullptr; pbody = pbody->get_next()) {
		if (!pbody->is_active()) {
			continue;
		}
		math::vec2 disp = math::g_zero_vec2;
		switch (pbody->get_type()) {
		case Body_2d::DYNAMIC:
			pbody->m_velocity.x += (pbody->m_acceleration.x * dt) + (m_gravity.x * pbody->m_gravity_scale * dt);
			pbody->m_velocity.y += (pbody->m_acceleration.y * dt) + (m_gravity.y * pbody->m_gravity_scale * dt);
	
			if ( ( (pbody->m_velocity.x != 0.0f) || (pbody->m_velocity.y != 0.0f) ) && pbody->m_map_collision) {
				check_n_solve_map_collision(pbody, dt);
			}
			else if (!pbody->m_map_collision) {
			    disp = math::vec2(pbody->m_velocity.x * dt, pbody->m_velocity.y * dt);
				pbody->translate_by(disp);
				/*
				pbody->m_position.x += pbody->m_velocity.x * dt;
				pbody->m_aabb.p_min.x += pbody->m_velocity.x * dt;
				pbody->m_aabb.p_max.x += pbody->m_velocity.x * dt;

				pbody->m_position.y += pbody->m_velocity.y * dt;
				pbody->m_aabb.p_min.y += pbody->m_velocity.y * dt;
				pbody->m_aabb.p_max.y += pbody->m_velocity.y * dt;
				*/

			}
			//mark that this body's collider moved, so it is checked against collision
			m_pbroadphase->move_proxy(pbody->get_collider()->get_proxy_id());

			break;
		case Body_2d::KINEMATIC:
			pbody->m_velocity.x += pbody->m_acceleration.x * dt;
			pbody->m_velocity.y += pbody->m_acceleration.y * dt;

		    disp = math::vec2(pbody->m_velocity.x * dt, pbody->m_velocity.y * dt);
			pbody->translate_by(disp);

			/*
			pbody->m_position.x += pbody->m_velocity.x * dt;
			pbody->m_aabb.p_min.x += pbody->m_velocity.x * dt;
			pbody->m_aabb.p_max.x += pbody->m_velocity.x * dt;

			pbody->m_position.y += pbody->m_velocity.y * dt;
			pbody->m_aabb.p_min.y += pbody->m_velocity.y * dt;
			pbody->m_aabb.p_max.y += pbody->m_velocity.y * dt;
			*/

			if (pbody->m_velocity.x != 0.0f || pbody->m_velocity.y != 0.0f) {
				m_pbroadphase->move_proxy(pbody->get_collider()->get_proxy_id());
			}

			break;
		case Body_2d::STATIC:
			break;
		}
	}

	//---------------------BROADPHASE: generate possible pairs of colliding bodies-----------------------------------
	Proxy_pair *ppair_buff = m_pbroadphase->compute_pairs();

	//check if any of the generated pairs are already on the contact list
	size_t  pair_cnt  =  m_pbroadphase->get_pair_count();

	for (size_t i = 0; i != pair_cnt; ++i) {
		Proxy_pair *ppair = &ppair_buff[i];
		//get the colliders
		Collider_2d_proxy *pcollider_proxy_a  =  static_cast<Collider_2d_proxy*>(m_pbroadphase->get_user_data(ppair->a_id));
		Collider_2d_proxy *pcollider_proxy_b = static_cast<Collider_2d_proxy*>(m_pbroadphase->get_user_data(ppair->b_id));

		//check if bodies types allow collision, i.e, at least one should be dynamic
		Body_2d *pbody_a = pcollider_proxy_a->pcollider->get_body();
		Body_2d *pbody_b = pcollider_proxy_b->pcollider->get_body();

		if (pbody_a->get_type() != Body_2d::DYNAMIC && pbody_b->get_type() != Body_2d::DYNAMIC) {
			continue;
		}

		//iterate through all  the contacts
		bool is_duplicate = false;
		for (Contact_node *pcontact = m_pcontact_list; pcontact != nullptr; pcontact = pcontact->pnext) {
		
			if ((pcontact->pproxy_a->proxy_id == pcollider_proxy_a->proxy_id) && (pcontact->pproxy_b->proxy_id == pcollider_proxy_b->proxy_id)) {
				is_duplicate = true;
				break;
			}
			if ((pcontact->pproxy_a->proxy_id == pcollider_proxy_b->proxy_id) && (pcontact->pproxy_b->proxy_id == pcollider_proxy_a->proxy_id)) {
				is_duplicate = true;
				break;
			}
		}
		if (!is_duplicate) {
			// new pair of colliding Collider_2ds, add it to the contact list
			add_to_contact_list(pcollider_proxy_a, pcollider_proxy_b);
			// CALL BEGING CONTACT ON THE BODY_2DS !!! USE CONTACT LISTENER
			m_pcoll_listener->begin_body_collision(pcollider_proxy_a->pcollider->get_body(), pcollider_proxy_b->pcollider->get_body());
		}
	}

	//-----------------------NARROWPHASE: SOLVE COLLISIONS---------------------------------------
	//Iterate through all the contacts solving the necessary collisions
	Body_2d *pbody_a;
	Body_2d *pbody_b;
	Collider_2d *pcollider_a;
	Collider_2d *pcollider_b;
	for (Contact_node *pcontact = m_pcontact_list; pcontact != nullptr;) {
		pcollider_a = pcontact->pproxy_a->pcollider;
		pcollider_b = pcontact->pproxy_b->pcollider;
		//are the bodies still colliding?
		if (test_overlap(pcollider_a->get_aabb(), pcollider_b->get_aabb())) {
			//check if one is a trigger
			if (pcollider_a->is_trigger() || pcollider_b->is_trigger()) {
				pcontact = pcontact->pnext;
				continue;
			}
			//none is a trigger, SOLVE THE COLLISION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			pbody_a = pcollider_a->get_body();
			pbody_b = pcollider_b->get_body();

			//vector from a to b
			math::vec2 n = pbody_b->get_position() - pbody_a->get_position();

			//calculate half the extent along the x axis 
			float a_x_axis_extent = (pcollider_a->get_aabb().p_max.x - pcollider_a->get_aabb().p_min.x) / 2.0f;
			float a_y_axis_extent = (pcollider_a->get_aabb().p_max.y - pcollider_a->get_aabb().p_min.y) / 2.0f;
			
			//calculate half the extent along the y axis
			float b_x_axis_extent = (pcollider_b->get_aabb().p_max.x - pcollider_b->get_aabb().p_min.x) / 2.0f;
			float b_y_axis_extent = (pcollider_b->get_aabb().p_max.y - pcollider_b->get_aabb().p_min.y) / 2.0f;
			
			//calculate overlap on x axis
			float x_overlap = a_x_axis_extent + b_x_axis_extent - abs(n.x);

			//calculate overlap on the y axis
			float y_overlap = a_y_axis_extent + b_y_axis_extent - abs(n.y);

			//find out which axis is the axis of least penetration, and calculate the displacement vector
			math::vec2 displacement = math::g_zero_vec2;
			float pixel_sz = 1.0f / m_pmap->pixels_per_world_unit();
			if (x_overlap > y_overlap) {
				if (n.x < 0.0f) {
					displacement.x = -1.0f;
				}
				else {
					displacement.x = 1.0f;
				}
				displacement *= x_overlap + pixel_sz;
			}
			else {
				if (n.y < 0.0f) {
					displacement.y = -1.0f;
				}
				else {
					displacement.y = 1.0f;
				}
				displacement *= y_overlap + pixel_sz;
			}

			if (pbody_a->get_type() == Body_2d::STATIC || pbody_a->get_type() == Body_2d::KINEMATIC) {
				//We must move B out of A
				pbody_b->translate_by(displacement);
			}
			else if (pbody_b->get_type() == Body_2d::STATIC || pbody_b->get_type() == Body_2d::KINEMATIC) {
				//We must move A out of B
				pbody_b->translate_by(-displacement);
			}
			
			// the bodies are no longer colliding
			// notify through the collision listener
			m_pcoll_listener->end_body_collision(pcollider_a->get_body(), pcollider_b->get_body());

			//aux pointer, use to set pcontact to the node after the one removed
			Contact_node *pnode = pcontact->pnext;

			//remove from the list
			remove_from_contact_list(pcontact);

			pcontact = pnode;
		}
		else {
			// the bodies are no longer colliding
			// notify through the collision listener
			m_pcoll_listener->end_body_collision(pcollider_a->get_body(), pcollider_b->get_body());

			//aux pointer, use to set pcontact to the node after the one removed
			Contact_node *pnode = pcontact->pnext;
			
			//remove from the list
			remove_from_contact_list(pcontact);
			
			pcontact = pnode;
		}	
	}
}