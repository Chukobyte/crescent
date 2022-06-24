#include "collision_ec_system.h"

#include "ec_system.h"
#include "../../utils/rbe_string_util.h"
#include "../../utils/rbe_assert.h"

EntitySystem* collisionSystem = NULL;

EntitySystem* collision_ec_system_create() {
    RBE_ASSERT(collisionSystem == NULL);
    collisionSystem = rbe_ec_system_create();
    collisionSystem->name = rbe_strdup("Collision");
    collisionSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_COLLIDER_2D;
    return collisionSystem;
}

EntitySystem* collision_ec_system_get() {
    return collisionSystem;
}
