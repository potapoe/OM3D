#include "Material.h"

#include <glad/glad.h>

#include <algorithm>

namespace OM3D {

std::shared_ptr<Program> Material::empty_material_program;
std::shared_ptr<Program> Material::textured_material_program;
std::shared_ptr<Program> Material::textured_normal_mapped_material_program;

Material::Material() {
}

void Material::set_program(std::shared_ptr<Program> prog) {
    _program = std::move(prog);
}

void Material::set_blend_mode(BlendMode blend) {
    _blend_mode = blend;
}

void Material::set_depth_test_mode(DepthTestMode depth) {
    _depth_test_mode = depth;
}

void Material::set_texture(u32 slot, std::shared_ptr<Texture> tex) {
    if(const auto it = std::find_if(_textures.begin(), _textures.end(), [&](const auto& t) { return t.second == tex; }); it != _textures.end()) {
        it->second = std::move(tex);
    } else {
        _textures.emplace_back(slot, std::move(tex));
    }
}

void Material::bind() const {
    switch(_blend_mode) {
        case BlendMode::None:
            glDisable(GL_BLEND);
        break;

        case BlendMode::Alpha:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;

        case BlendMode::Add:
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
        break;
    }

    switch(_depth_test_mode) {
        case DepthTestMode::None:
            glDisable(GL_DEPTH_TEST);
        break;

        case DepthTestMode::Equal:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_EQUAL);
        break;

        case DepthTestMode::Standard:
            glEnable(GL_DEPTH_TEST);
            // We are using reverse-Z
            glDepthFunc(GL_GEQUAL);
        break;

        case DepthTestMode::Reversed:
            glEnable(GL_DEPTH_TEST);
            // We are using reverse-Z
            glDepthFunc(GL_LEQUAL);
        break;
    }

    for(const auto& texture : _textures) {
        texture.second->bind(texture.first);
    }
    _program->bind();
}

std::shared_ptr<Material> Material::empty_material() {
    static std::weak_ptr<Material> weak_material;
    auto material = weak_material.lock();
    if(!material)
    {
        material = std::make_shared<Material>();
        material->_program = Material::empty_material_program;
        weak_material = material;
    }
    return material;
}

Material Material::textured_material() {
    Material material;
    material._program = Material::textured_material_program;
    return material;
}

Material Material::textured_normal_mapped_material() {
    Material material;
    material._program = Material::textured_normal_mapped_material_program;
    return material;
}

void Material::set_empty_material_program(std::shared_ptr<Program> p)
{
    Material::empty_material_program = p;
}

void Material::set_textured_material_program(std::shared_ptr<Program> p)
{
    Material::textured_material_program = p;
}

void Material::set_textured_normal_mapped_material_program(std::shared_ptr<Program> p)
{
    Material::textured_normal_mapped_material_program = p;
}


}
