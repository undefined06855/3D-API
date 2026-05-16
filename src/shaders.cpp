#include "shaders.hpp"

const GLchar* g_defaultShaderVertex = R"(
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texCoord;

uniform mat4 u_mvpMatrix;

#ifdef GL_ES
varying mediump vec2 v_texCoord;
varying mediump vec3 v_normal;
#else
varying vec2 v_texCoord;
varying vec3 v_normal;
#endif

void main() {
    gl_Position = u_mvpMatrix * vec4(a_position, 1.0);
    v_texCoord = a_texCoord;
    v_normal = a_normal;
}
)";

const GLchar* g_defaultShaderFragment = R"(
#ifdef GL_ES
precision lowp float;
#endif

varying vec2 v_texCoord;
varying vec3 v_normal;
uniform sampler2D CC_Texture0;

void main() {
    gl_FragColor = texture2D(CC_Texture0, v_texCoord);
}
)";

void loadShaders() {
    auto cache = cocos2d::CCShaderCache::sharedShaderCache();
    if (cache->programForKey("default_shader"_spr)) return;

    auto program = new cocos2d::CCGLProgram;
    bool ret = program->initWithVertexShaderByteArray(g_defaultShaderVertex, g_defaultShaderFragment);
    if (!ret) {
        geode::log::warn("shader failed to load!!!");
        geode::log::warn("{}", program->fragmentShaderLog());
        return;
    }

    program->addAttribute(kCCAttributeNamePosition, cocos2d::kCCVertexAttrib_Position);
    program->addAttribute("a_normal", 1);
    program->addAttribute(kCCAttributeNameTexCoord, cocos2d::kCCVertexAttrib_TexCoords);

    program->link();
    program->updateUniforms();

    geode::log::info("a whole THIRD dimension is about to be added by some mod are you READY");

    cache->addProgram(program, "default_shader"_spr);
}

$on_game(TexturesLoaded) { loadShaders(); }
$on_mod(Loaded) { loadShaders(); }
