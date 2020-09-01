#include "Damage_region_creator.hpp"

#include "Game_object.hpp"
#include "vec3.hpp"
#include "Rect.hpp"
#include "Damage_region.hpp"
#include "Graphics_manager.hpp"
#include "runtime_memory_allocator.hpp"
#include "Object.hpp"
#include "Property.hpp"
#include "crc32.hpp"

Damage_region_creator::Damage_region_creator() : gom::Creator() {}

gom::Game_object * Damage_region_creator::create(const Object & obj_description)
{
    const math::Rect bounds(obj_description.get_x(), obj_description.get_y(),
                            obj_description.get_width(), obj_description.get_height());

    const Property *pproperty = obj_description.get_property(SID('DAMAGE'));
    int damage = 20;
    if (pproperty) {
        damage = pproperty->get_int();
    }

    std::size_t obj_sz = sizeof(Damage_region);
    void *pmem = mem::allocate(obj_sz);
    gom::Game_object *pgame_object = static_cast<gom::Game_object*>(
        new (pmem) Damage_region(bounds, damage));

    return pgame_object;
}

