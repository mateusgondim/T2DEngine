#ifndef _DAMAGE_REGION_HPP
#define _DAMAGE_REGION_HPP

#include "Game_object.hpp"
#include "Rect.hpp"

class Event;

class Damage_region : public gom::Game_object {
public:
    Damage_region(const math::Rect & bounds, const int damage);
    Damage_region(const Damage_region &) = delete;

    Damage_region & operator=(const Damage_region & rhs) = delete;
    void set_damage_points(const float damage) { m_damage_points = damage; }

    float get_damage_points() const { return m_damage_points; }

    virtual void update(const float dt) override;
    virtual void on_event(Event & event) override;

private:
    math::Rect m_bounds;
    int m_damage_points;
};
#endif