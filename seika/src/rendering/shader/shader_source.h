#pragma once

static const char* SE_OPENGL_SHADER_SOURCE_VERTEX_SPRITE =
    "#version 330 core\n"
    "layout (location = 0) in float id;\n"
    "layout (location = 1) in vec2 position;\n"
    "layout (location = 2) in vec2 textureCoordinates;\n"
    "layout (location = 3) in vec4 textureColor;\n"
    "\n"
    "out float spriteId;\n"
    "out vec2 texCoord;\n"
    "out vec4 spriteColor;\n"
    "\n"
    "uniform mat4 models[100];\n"
    "uniform mat4 projection;\n"
    "\n"
    "void main() {\n"
    "    spriteId = id;\n"
    "    int spriteIntId = int(id);\n"
    "    texCoord = textureCoordinates;\n"
    "    spriteColor = textureColor;\n"
    "    gl_Position = projection * models[spriteIntId] * vec4(position, 0.0f, 1.0f);\n"
    "}\n";

static const char* SE_OPENGL_SHADER_SOURCE_FRAGMENT_SPRITE =
    "#version 330 core\n"
    "\n"
    "in float spriteId;\n"
    "in vec2 texCoord;\n"
    "in vec4 spriteColor;\n"
    "out vec4 color;\n"
    "\n"
    "uniform sampler2D sprite;\n"
    "\n"
    "void main() {\n"
    "    color = spriteColor * texture(sprite, texCoord);\n"
    "}\n";

static const char* SE_OPENGL_SHADER_SOURCE_VERTEX_FONT =
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex; // (pos, tex)\n"
    "\n"
    "out vec2 texCoords;\n"
    "\n"
    "uniform mat4 projection;\n"
    "\n"
    "void main() {\n"
    "    gl_Position = projection * vec4(vertex.xy, 0.0f, 1.0f);\n"
    "    texCoords = vertex.zw;\n"
    "}\n";

static const char* SE_OPENGL_SHADER_SOURCE_FRAGMENT_FONT =
    "#version 330 core\n"
    "in vec2 texCoords;\n"
    "out vec4 color;\n"
    "\n"
    "uniform sampler2D textValue;\n"
    "uniform vec4 textColor;\n"
    "\n"
    "void main() {\n"
    "    vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(textValue, texCoords).r);\n"
    "    color = textColor * sampled;\n"
    "}\n";

static const char* SE_OPENGL_SHADER_SOURCE_VERTEX_SCREEN =
    "#version 330 core\n"
    "layout (location = 0) in vec2 position;\n"
    "layout (location = 1) in vec2 textureCoordinates;\n"
    "\n"
    "out vec2 texCoords;\n"
    "\n"
    "void main() {\n"
    "    texCoords = textureCoordinates;\n"
    "    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);\n"
    "}\n";

static const char* SE_OPENGL_SHADER_SOURCE_FRAGMENT_SCREEN =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "\n"
    "in vec2 texCoords;\n"
    "\n"
    "uniform sampler2D screenTexture;\n"
    "\n"
    "void main() {\n"
    "    vec3 color = texture(screenTexture, texCoords).rgb;\n"
    "    fragColor = vec4(color, 1.0f);\n"
    "}\n";
