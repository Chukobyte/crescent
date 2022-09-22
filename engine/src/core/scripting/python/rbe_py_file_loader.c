#include "rbe_py_file_loader.h"

#include <Python.h>

#include "py_helper.h"
#include "../../scene/scene_manager.h"
#include "../../asset_manager.h"
#include "../../memory/rbe_mem.h"
#include "../../utils/rbe_assert.h"

// Components
#include "../../ecs/component/animated_sprite_component.h"
#include "../../ecs/component/collider2d_component.h"
#include "../../ecs/component/color_rect_component.h"
#include "../../ecs/component/script_component.h"
#include "../../ecs/component/sprite_component.h"
#include "../../ecs/component/text_label_component.h"

//--- FileSceneNode ---//
FileSceneNode* cachedFileSceneNode = NULL;

void file_scene_node_load_component(FileSceneNode* node, PyObject* pComponent) {
    const char* className = Py_TYPE(pComponent)->tp_name; // TODO: Should probably Py_DecRed()?
    if (strcmp(className, "Transform2DComponent") == 0) {
        rbe_logger_debug("Building transform 2d component");
        PyObject* pPosition = PyObject_GetAttrString(pComponent, "position");
        const float positionX = phy_get_float_from_var(pPosition, "x");
        const float positionY = phy_get_float_from_var(pPosition, "y");
        PyObject* pScale = PyObject_GetAttrString(pComponent, "scale");
        const float scaleX = phy_get_float_from_var(pScale, "x");
        const float scaleY = phy_get_float_from_var(pScale, "y");
        const float rotation = phy_get_float_from_var(pComponent, "rotation");
        const int zIndex = phy_get_int_from_var(pComponent, "z_index");
        const bool zIndexRelativeToParent = phy_get_bool_from_var(pComponent, "z_index_relative_to_parent");
        const bool ignoreCamera = phy_get_bool_from_var(pComponent, "ignore_camera");
        Transform2DComponent* transform2DComponent = transform2d_component_create();
        transform2DComponent->localTransform.position.x = positionX;
        transform2DComponent->localTransform.position.y = positionY;
        transform2DComponent->localTransform.scale.x = scaleX;
        transform2DComponent->localTransform.scale.y = scaleY;
        transform2DComponent->localTransform.rotation = rotation;
        transform2DComponent->zIndex = zIndex;
        transform2DComponent->isZIndexRelativeToParent = zIndexRelativeToParent;
        transform2DComponent->ignoreCamera = ignoreCamera;

        node->components[ComponentDataIndex_TRANSFORM_2D] = transform2DComponent;
        rbe_logger_debug("position: (%f, %f), scale: (%f, %f), rotation: %f, z_index: %d, z_index_relative: %d, ignore_camera: %d",
                         positionX, positionY, scaleX, scaleY, rotation, zIndex, zIndexRelativeToParent, ignoreCamera);
        Py_DECREF(pPosition);
        Py_DECREF(pScale);
    } else if (strcmp(className, "SpriteComponent") == 0) {
        rbe_logger_debug("Building sprite component");
        const char* texturePath = phy_get_string_from_var(pComponent, "texture_path");
        PyObject* pDrawSource = PyObject_GetAttrString(pComponent, "draw_source");
        const float drawSourceX = phy_get_float_from_var(pDrawSource, "x");
        const float drawSourceY = phy_get_float_from_var(pDrawSource, "y");
        const float drawSourceW = phy_get_float_from_var(pDrawSource, "w");
        const float drawSourceH = phy_get_float_from_var(pDrawSource, "h");
        PyObject* pOrigin = PyObject_GetAttrString(pComponent, "origin");
        const float originX = phy_get_float_from_var(pOrigin, "x");
        const float originY = phy_get_float_from_var(pOrigin, "y");
        const bool flipX = phy_get_bool_from_var(pComponent, "flip_x");
        const bool flipY = phy_get_bool_from_var(pComponent, "flip_y");
        PyObject* pModulate = PyObject_GetAttrString(pComponent, "modulate");
        const int modulateR = phy_get_int_from_var(pModulate, "r");
        const int modulateG = phy_get_int_from_var(pModulate, "g");
        const int modulateB = phy_get_int_from_var(pModulate, "b");
        const int modulateA = phy_get_int_from_var(pModulate, "a");
        SpriteComponent* spriteComponent = sprite_component_create();
        node->spriteTexturePath = strdup(texturePath); // TODO: Clean up
        spriteComponent->drawSource.x = drawSourceX;
        spriteComponent->drawSource.y = drawSourceY;
        spriteComponent->drawSource.w = drawSourceW;
        spriteComponent->drawSource.h = drawSourceH;
        spriteComponent->origin.x = originX;
        spriteComponent->origin.y = originY;
        spriteComponent->flipX = flipX;
        spriteComponent->flipY = flipY;
        const Color modulateColor = rbe_color_get_normalized_color(modulateR, modulateG, modulateB, modulateA);
        spriteComponent->modulate.r = modulateColor.r;
        spriteComponent->modulate.g = modulateColor.g;
        spriteComponent->modulate.b = modulateColor.b;
        spriteComponent->modulate.a = modulateColor.a;

        node->components[ComponentDataIndex_SPRITE] = spriteComponent;
        rbe_logger_debug("texture_path = %s, draw_source = (%f, %f, %f, %f), origin: (%f, %f), flip_x: %d, flip_y: %d, modulate: (%d, %d, %d, %d)",
                         texturePath, drawSourceX, drawSourceY, drawSourceW, drawSourceH, originX, originY, flipX, flipY, modulateR, modulateG, modulateB, modulateA);
        Py_DECREF(pDrawSource);
        Py_DECREF(pModulate);
        Py_DECREF(pOrigin);
        Py_DECREF(pModulate);
    } else if (strcmp(className, "AnimatedSpriteComponent") == 0) {
        rbe_logger_debug("Building animated sprite component");
        AnimatedSpriteComponentData* animatedSpriteComponent = animated_sprite_component_data_create();
        const char* currentAnimationName = phy_get_string_from_var(pComponent, "current_animation_name");
        const bool isPlaying = phy_get_bool_from_var(pComponent, "is_playing");
        PyObject* pOrigin = PyObject_GetAttrString(pComponent, "origin");

        PyObject* pModulate = PyObject_GetAttrString(pComponent, "modulate");
        const int modulateRed = phy_get_int_from_var(pModulate, "r");
        const int modulateGreen = phy_get_int_from_var(pModulate, "g");
        const int modulateBlue = phy_get_int_from_var(pModulate, "b");
        const int modulateAlpha = phy_get_int_from_var(pModulate, "a");

        const float originX = phy_get_float_from_var(pOrigin, "x");
        const float originY = phy_get_float_from_var(pOrigin, "y");
        const bool flipX = phy_get_bool_from_var(pComponent, "flip_x");
        const bool flipY = phy_get_bool_from_var(pComponent, "flip_y");
        rbe_logger_debug("current_animation_name: '%s', is_playing: '%d', origin: (%f, %f), flip_x: '%d', flip_y: '%d'",
                         currentAnimationName, isPlaying, originX, originY, flipX, flipY);
        animatedSpriteComponent->isPlaying = isPlaying;
        animatedSpriteComponent->modulate = rbe_color_get_normalized_color(modulateRed, modulateGreen, modulateBlue, modulateAlpha);
        animatedSpriteComponent->origin.x = originX;
        animatedSpriteComponent->origin.y = originY;
        animatedSpriteComponent->flipX = flipX;
        animatedSpriteComponent->flipY = flipY;

        PyObject* pyAnimationsList = PyObject_GetAttrString(pComponent, "animations");
        RBE_ASSERT(PyList_Check(pyAnimationsList));
        for (Py_ssize_t animationIndex = 0; animationIndex < PyList_Size(pyAnimationsList); animationIndex++) {
            AnimationData animation;
            animation.frameCount = 0;
            animation.currentFrame = 0;
            animation.isValid = true;
            PyObject* pyAnimation = PyList_GetItem(pyAnimationsList, animationIndex);
            RBE_ASSERT(pyAnimation != NULL);
            const char* animationName = phy_get_string_from_var(pyAnimation, "name");
            const int animationSpeed = phy_get_int_from_var(pyAnimation, "speed");
            const bool animationLoops = phy_get_bool_from_var(pyAnimation, "loops");
            rbe_logger_debug("building anim - name: '%s', speed: '%d', loops: '%d'", animationName, animationSpeed, animationLoops);
            strcpy(animation.name, animationName);
            animation.speed = animationSpeed;
            animation.doesLoop = animationLoops;

            PyObject* pyAnimationFramesList = PyObject_GetAttrString(pyAnimation, "frames");
            RBE_ASSERT(PyList_Check(pyAnimationFramesList));
            for (Py_ssize_t animationFrameIndex = 0; animationFrameIndex < PyList_Size(pyAnimationFramesList); animationFrameIndex++) {
                PyObject* pyAnimationFrame = PyList_GetItem(pyAnimationFramesList, animationFrameIndex);
                const int animationFrameNumber = phy_get_int_from_var(pyAnimationFrame, "frame");
                const char* animationFrameTexturePath = phy_get_string_from_var(pyAnimationFrame, "texture_path");
                PyObject* pyDrawSource = PyObject_GetAttrString(pyAnimationFrame, "draw_source");
                RBE_ASSERT(pyDrawSource != NULL);
                const float drawSourceX = phy_get_float_from_var(pyDrawSource, "x");
                const float drawSourceY = phy_get_float_from_var(pyDrawSource, "y");
                const float drawSourceW = phy_get_float_from_var(pyDrawSource, "w");
                const float drawSourceH = phy_get_float_from_var(pyDrawSource, "h");
                rbe_logger_debug("frame: %d, texture_path: %s, draw_source: (%f, %f, %f, %f)",
                                 animationFrameNumber, animationFrameTexturePath, drawSourceX, drawSourceY, drawSourceW, drawSourceH);
                AnimationFrameData animationFrame;
                strcpy(animationFrame.texturePath, animationFrameTexturePath);
                animationFrame.frame = animationFrameNumber;
                const Rect2 frameDrawSource = { drawSourceX, drawSourceY, drawSourceW, drawSourceH };
                animationFrame.drawSource = frameDrawSource;
                animation.animationFrames[animationFrame.frame] = animationFrame;
                animation.frameCount++;

                Py_DECREF(pyDrawSource);
            }

            // Set current animation if the name matches
            animated_sprite_component_data_add_animation(animatedSpriteComponent, animation);
            if (strcmp(animation.name, currentAnimationName) == 0) {
                animatedSpriteComponent->currentAnimation = animation;
            }
        }

        node->components[ComponentDataIndex_ANIMATED_SPRITE] = animatedSpriteComponent;

        Py_DECREF(pModulate);
        Py_DECREF(pOrigin);
    } else if (strcmp(className, "TextLabelComponent") == 0) {
        rbe_logger_debug("Building text label component");
        const char* textLabelUID = phy_get_string_from_var(pComponent, "uid");
        const char* textLabelText = phy_get_string_from_var(pComponent, "text");
        PyObject* pColor = PyObject_GetAttrString(pComponent, "color");
        const int colorR = phy_get_int_from_var(pColor, "r");
        const int colorG = phy_get_int_from_var(pColor, "g");
        const int colorB = phy_get_int_from_var(pColor, "b");
        const int colorA = phy_get_int_from_var(pColor, "a");
        const Color textLabelColor = rbe_color_get_normalized_color(colorR, colorG, colorB, colorA);
        TextLabelComponent* textLabelComponent = text_label_component_create();
        node->fontUID = strdup(textLabelUID);
        strcpy(textLabelComponent->text, textLabelText);
        textLabelComponent->color = textLabelColor;

        node->components[ComponentDataIndex_TEXT_LABEL] = textLabelComponent;
        rbe_logger_debug("uid: %s, text: %s, color(%d, %d, %d, %d)", textLabelUID, textLabelText, colorR, colorG, colorB, colorA);
        Py_DECREF(pColor);
    } else if (strcmp(className, "ScriptComponent") == 0) {
        rbe_logger_debug("Building script component");
        const char* scriptClassPath = phy_get_string_from_var(pComponent, "class_path");
        const char* scriptClassName = phy_get_string_from_var(pComponent, "class_name");
        ScriptComponent* scriptComponent = script_component_create();
        scriptComponent->classPath = scriptClassPath;
        scriptComponent->className = scriptClassName;
        scriptComponent->contextType = ScriptContextType_PYTHON;

        node->components[ComponentDataIndex_SCRIPT] = scriptComponent;
        rbe_logger_debug("class_path: %s, class_name: %s", scriptClassPath, scriptClassName);
    } else if (strcmp(className, "Collider2DComponent") == 0) {
        rbe_logger_debug("Building collider2d component");
        PyObject* pyExtents = PyObject_GetAttrString(pComponent, "extents");
        RBE_ASSERT(pyExtents != NULL);
        const float rectW = phy_get_float_from_var(pyExtents, "w");
        const float rectH = phy_get_float_from_var(pyExtents, "h");
        PyObject* pyColor = PyObject_GetAttrString(pComponent, "color");
        RBE_ASSERT(pyColor != NULL);
        const int colorR = phy_get_int_from_var(pyColor, "r");
        const int colorG = phy_get_int_from_var(pyColor, "g");
        const int colorB = phy_get_int_from_var(pyColor, "b");
        const int colorA = phy_get_int_from_var(pyColor, "a");
        Collider2DComponent* collider2DComponent = collider2d_component_create();
        collider2DComponent->extents.w = rectW;
        collider2DComponent->extents.h = rectH;
        collider2DComponent->color.r = (float) colorR / 255.0f;
        collider2DComponent->color.g = (float) colorG / 255.0f;
        collider2DComponent->color.b = (float) colorB / 255.0f;
        collider2DComponent->color.a = (float) colorA / 255.0f;
        collider2DComponent->collisionExceptionCount = 0;

        node->components[ComponentDataIndex_COLLIDER_2D] = collider2DComponent;
        rbe_logger_debug("extents: (%f, %f), color: (%f, %f, %f, %f)",
                         rectW, rectH, collider2DComponent->color.r, collider2DComponent->color.g, collider2DComponent->color.b, collider2DComponent->color.a);
        Py_DECREF(pyExtents);
        Py_DECREF(pyColor);
    } else if (strcmp(className, "ColorRectComponent") == 0) {
        rbe_logger_debug("Building color rect component");
        PyObject* pySize = PyObject_GetAttrString(pComponent, "size");
        RBE_ASSERT(pySize != NULL);
        const float rectW = phy_get_float_from_var(pySize, "w");
        const float rectH = phy_get_float_from_var(pySize, "h");
        PyObject* pyColor = PyObject_GetAttrString(pComponent, "color");
        RBE_ASSERT(pyColor != NULL);
        const int colorR = phy_get_int_from_var(pyColor, "r");
        const int colorG = phy_get_int_from_var(pyColor, "g");
        const int colorB = phy_get_int_from_var(pyColor, "b");
        const int colorA = phy_get_int_from_var(pyColor, "a");
        ColorRectComponent* colorRectComponent = color_rect_component_create();
        colorRectComponent->size.w = rectW;
        colorRectComponent->size.h = rectH;
        colorRectComponent->color.r = (float) colorR / 255.0f;
        colorRectComponent->color.g = (float) colorG / 255.0f;
        colorRectComponent->color.b = (float) colorB / 255.0f;
        colorRectComponent->color.a = (float) colorA / 255.0f;

        node->components[ComponentDataIndex_COLOR_RECT] = colorRectComponent;
        rbe_logger_debug("size: (%f, %f), color: (%f, %f, %f, %f)",
                         rectW, rectH, colorRectComponent->color.r, colorRectComponent->color.g,
                         colorRectComponent->color.b, colorRectComponent->color.a);
        Py_DECREF(pySize);
        Py_DECREF(pyColor);
    } else {
        rbe_logger_error("Invalid component class name: '%s'", className);
    }
}

void file_scene_node_delete_components(FileSceneNode* node) {
    if (node->components[ComponentDataIndex_TRANSFORM_2D] != NULL) {
        transform2d_component_delete(node->components[ComponentDataIndex_TRANSFORM_2D]);
    }
    if (node->components[ComponentDataIndex_SPRITE] != NULL) {
        sprite_component_delete(node->components[ComponentDataIndex_SPRITE]);
    }
    if (node->components[ComponentDataIndex_ANIMATED_SPRITE] != NULL) {
        animated_sprite_component_delete(node->components[ComponentDataIndex_ANIMATED_SPRITE]);
    }
    if (node->components[ComponentDataIndex_TEXT_LABEL] != NULL) {
        text_label_component_delete(node->components[ComponentDataIndex_TEXT_LABEL]);
    }
    if (node->components[ComponentDataIndex_SCRIPT] != NULL) {
        script_component_delete(node->components[ComponentDataIndex_SCRIPT]);
    }
    if (node->components[ComponentDataIndex_COLLIDER_2D] != NULL) {
        collider2d_component_delete(node->components[ComponentDataIndex_COLLIDER_2D]);
    }
    if (node->components[ComponentDataIndex_COLOR_RECT] != NULL) {
        color_rect_component_delete(node->components[ComponentDataIndex_COLOR_RECT]);
    }
}

FileSceneNode* file_scene_node_load_stage_node(FileSceneNode* parent, PyObject* pStageNode) {
    FileSceneNode* node = RBE_MEM_ALLOCATE(FileSceneNode);
    node->parent = parent;
    node->childrenCount = 0;
    node->spriteTexturePath = NULL;
    node->fontUID = NULL;

    // Node component is used for all scene nodes
    const char* nodeName = phy_get_string_from_var(pStageNode, "name");
    const char* nodeType = phy_get_string_from_var(pStageNode, "type");
    node->name = strdup(nodeName); // TODO: Clean up
    node->type = node_get_base_type(nodeType);
    RBE_ASSERT_FMT(node->type != NodeBaseType_INVALID, "Node '%s' has an invalid node type '%s'", nodeName, nodeType);

    // TODO: Process tags if tags var is a list
    PyObject* tagsListVar = PyObject_GetAttrString(pStageNode, "tags");
    if (PyList_Check(tagsListVar)) {
        for (Py_ssize_t tagIndex = 0; tagIndex < PyList_Size(tagsListVar); tagIndex++) {}
    }
    // TODO: Actually load external scene file here and updated variables
    PyObject* externalNodeSourceVar = PyObject_GetAttrString(pStageNode, "external_node_source");
    if (externalNodeSourceVar != Py_None) {
        const char* externalNodeSourcePath = phy_get_string_from_var(externalNodeSourceVar, "external_node_source");
    }
    // Components
    for (size_t i = 0; i < MAX_COMPONENTS; i++) {
        node->components[i] = NULL;
    }
    PyObject* componentsListVar = PyObject_GetAttrString(pStageNode, "components");
    if (PyList_Check(componentsListVar)) {
        const Py_ssize_t pComponentsListSize = PyList_Size(componentsListVar);
        for (Py_ssize_t componentIndex = 0; componentIndex < PyList_Size(componentsListVar); componentIndex++) {
            PyObject* pComponent = PyList_GetItem(componentsListVar, componentIndex);
            RBE_ASSERT(pComponent != NULL);
            file_scene_node_load_component(node, pComponent);
        }
    }
    // Children Nodes
    PyObject* childrenListVar = PyObject_GetAttrString(pStageNode, "children");
    if (PyList_Check(childrenListVar)) {
        // Recurse through children nodes
        for (Py_ssize_t i = 0; i < PyList_Size(childrenListVar); i++) {
            PyObject* pChildStageNode = PyList_GetItem(childrenListVar, i);
            FileSceneNode* childNode = file_scene_node_load_stage_node(node, pChildStageNode);
            node->children[node->childrenCount++] = childNode;
        }
    }

    rbe_logger_debug("node_name = %s, node_type = %s", nodeName, nodeType);
    Py_DECREF(externalNodeSourceVar);

    return node;
}

FileSceneNode* file_scene_node_create_cached_file_scene_nodes_from_list(PyObject* stageNodeList) {
    file_scene_node_delete_cached_file_scene_node();
    RBE_ASSERT_FMT(PyList_Size(stageNodeList) == 1, "Stage Node Root has more than one!");
    PyObject* pRootStageNode = PyList_GetItem(stageNodeList, 0);
    FileSceneNode* rootNode = file_scene_node_load_stage_node(NULL, pRootStageNode);
    cachedFileSceneNode = rootNode;
    return rootNode;
}

void file_scene_node_erase_scene_node(FileSceneNode* node) {
    for (size_t i = 0; i < node->childrenCount; i++) {
        file_scene_node_erase_scene_node(node->children[i]);
    }
    file_scene_node_delete_components(node);
    RBE_MEM_FREE(node->name);
    RBE_MEM_FREE(node->spriteTexturePath);
    RBE_MEM_FREE(node->fontUID);
    RBE_MEM_FREE(node);
}

void file_scene_node_delete_cached_file_scene_node() {
    if (cachedFileSceneNode != NULL) {
        file_scene_node_erase_scene_node(cachedFileSceneNode);
        cachedFileSceneNode = NULL;
    }
}

FileSceneNode* file_scene_node_get_cached_file_scene_node() {
    return cachedFileSceneNode;
}

//--- Py File Loader ---//
RBEGameProperties* rbe_py_load_game_properties(const char* filePath) {
    bool load_success = pyh_run_python_file(filePath);
    RBE_ASSERT_FMT(load_success == true, "Failed to load config at '%s'", filePath);
    return rbe_game_props_get();
}

//SceneTreeNode* rbe_py_load_scene(const char* filePath) {
//    bool load_success = pyh_run_python_file(filePath);
//    RBE_ASSERT_FMT(load_success == true, "Failed to scene file at '%s'", filePath);
//    SceneTreeNode* rootNode = rbe_scene_manager_get_active_scene_root();
//    RBE_ASSERT_FMT(rootNode != NULL, "Root node is NULL for file path at '%s'", filePath);
//    return rootNode;
//}
