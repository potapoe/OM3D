#include "SceneObject.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace OM3D {

SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh, std::shared_ptr<Material> material) :
    mesh(std::move(mesh)),
    _material(std::move(material)) {
}

void SceneObject::render() const {
    if(!_material || !mesh) {
        std::cerr << "rendering object without materiel or mesh" << std::endl;
        return;
    }

   _material->set_uniform(HASH("model"), transform());
   _material->bind();
   mesh->draw();
}

void SceneObject::set_transform(const glm::mat4& tr) {
    _transform = tr;
}

const glm::mat4& SceneObject::transform() const {
    return _transform;
}

void SceneObject::adapt_to_light(const PointLight& light)
{
    glm::mat4 t = glm::mat4(1.0f);
    t = glm::translate(t, light.position());
    t = glm::scale(t, glm::vec3(light.radius()));
    set_transform(t);

    _material->set_uniform("current_light_position", light.position());
    _material->set_uniform("current_light_radius", light.radius());
    _material->set_uniform("current_light_color", light.color());
}

}
