#include "transform2d_component.h"

#include "../../memory/rbe_mem.h"

Transform2DComponent* transform2d_component_create() {
    Transform2DComponent* transform2DComponent = RBE_MEM_ALLOCATE(Transform2DComponent);
    transform2DComponent->position.x = 0.0f;
    transform2DComponent->position.y = 0.0f;
    transform2DComponent->scale.x = 1.0f;
    transform2DComponent->scale.y = 1.0f;
    transform2DComponent->rotation = 0.0f;
    transform2DComponent->zIndex = 0;
    transform2DComponent->isZIndexRelativeToParent = true;
    transform2DComponent->ignoreCamera = false;
    return transform2DComponent;
}
