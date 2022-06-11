#pragma once

#include "../ecs/entity/entity.h"

void rbe_scene_manager_initialize();
void rbe_scene_manager_finalize();
void rbe_scene_manager_queue_entity_for_creation(Entity entity);
void rbe_scene_manager_process_queued_creation_entities();
void rbe_scene_manager_queue_entity_for_deletion(Entity entity);
void rbe_scene_manager_process_queued_deletion_entities();
