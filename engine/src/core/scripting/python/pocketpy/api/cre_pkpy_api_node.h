#pragma once

struct pkpy_vm_handle;

// Node
int cre_pkpy_api_node_new(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node_get_name(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node_add_child(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node_get_child(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node_get_children(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node_get_parent(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node_queue_deletion(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node_is_queued_for_deletion(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node_set_time_dilation(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node_get_time_dilation(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node_get_total_time_dilation(struct pkpy_vm_handle* vm);

// Node2D
int cre_pkpy_api_node2d_set_position(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_add_to_position(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_get_position(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_get_global_position(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_set_scale(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_add_to_scale(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_get_scale(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_set_rotation(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_add_to_rotation(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_get_rotation(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_set_z_index(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_get_z_index(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_set_z_index_relative_to_parent(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_get_z_index_relative_to_parent(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_set_ignore_camera(struct pkpy_vm_handle* vm);
int cre_pkpy_api_node2d_get_ignore_camera(struct pkpy_vm_handle* vm);

// Sprite
int cre_pkpy_api_sprite_get_texture(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_set_texture(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_get_draw_source(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_set_draw_source(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_get_flip_h(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_set_flip_h(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_get_flip_v(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_set_flip_v(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_get_modulate(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_set_modulate(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_get_origin(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_set_origin(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_get_shader_instance(struct pkpy_vm_handle* vm);
int cre_pkpy_api_sprite_set_shader_instance(struct pkpy_vm_handle* vm);

// Animated Sprite
int cre_pkpy_api_animated_sprite_play(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_stop(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_set_current_animation_frame(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_add_animation(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_get_stagger_animation_start_times(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_set_stagger_animation_start_times(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_get_flip_h(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_set_flip_h(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_get_flip_v(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_set_flip_v(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_get_modulate(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_set_modulate(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_get_origin(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_set_origin(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_get_shader_instance(struct pkpy_vm_handle* vm);
int cre_pkpy_api_animated_sprite_set_shader_instance(struct pkpy_vm_handle* vm);

// Text Label
int cre_pkpy_api_text_label_get_text(struct pkpy_vm_handle* vm);
int cre_pkpy_api_text_label_set_text(struct pkpy_vm_handle* vm);
int cre_pkpy_api_text_label_get_color(struct pkpy_vm_handle* vm);
int cre_pkpy_api_text_label_set_color(struct pkpy_vm_handle* vm);
int cre_pkpy_api_text_label_set_font_uid(struct pkpy_vm_handle* vm);

// Collider2D
int cre_pkpy_api_collider2d_get_extents(struct pkpy_vm_handle* vm);
int cre_pkpy_api_collider2d_set_extents(struct pkpy_vm_handle* vm);
int cre_pkpy_api_collider2d_get_color(struct pkpy_vm_handle* vm);
int cre_pkpy_api_collider2d_set_color(struct pkpy_vm_handle* vm);

// Color Rect
int cre_pkpy_api_color_rect_get_size(struct pkpy_vm_handle* vm);
int cre_pkpy_api_color_rect_set_size(struct pkpy_vm_handle* vm);
int cre_pkpy_api_color_rect_get_color(struct pkpy_vm_handle* vm);
int cre_pkpy_api_color_rect_set_color(struct pkpy_vm_handle* vm);

// Parallax
int cre_pkpy_api_parallax_get_scroll_speed(struct pkpy_vm_handle* vm);
int cre_pkpy_api_parallax_set_scroll_speed(struct pkpy_vm_handle* vm);
