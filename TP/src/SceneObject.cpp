#include "SceneObject.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace OM3D {

SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh, std::shared_ptr<Material> material) :
    _mesh(std::move(mesh)),
    _material(std::move(material)) 
{
    buildBbox();
}

void SceneObject::buildBbox()
{
    // compute the max/min positions for the model mesh
    float maxX, maxY, maxZ, minX, minY, minZ;
    auto oVertices = _mesh->meshData.vertices;
    maxX = oVertices[0].position[0];
    maxY = oVertices[0].position[1];
    maxZ = oVertices[0].position[2];
    minX = oVertices[0].position[0];
    minY = oVertices[0].position[1];
    minZ = oVertices[0].position[2];

    for (int i = 1; i < oVertices.size(); i++)
    {
        Vertex current_vertex = oVertices[i];
        maxX = current_vertex.position[0] > maxX ? current_vertex.position[0] : maxX;
        maxY = current_vertex.position[1] > maxY ? current_vertex.position[1] : maxY;
        maxZ = current_vertex.position[2] > maxZ ? current_vertex.position[2] : maxZ;

        minX = current_vertex.position[0] < minX ? current_vertex.position[0] : minX;
        minY = current_vertex.position[1] < minY ? current_vertex.position[1] : minY;
        minZ = current_vertex.position[2] < minZ ? current_vertex.position[2] : minZ;
    }

    // create the bounding box for occlusion culling
    std::vector<Vertex> vertices = {
        { glm::vec3(minX, minY, minZ), glm::vec3(-0.57735, -0.57735, -0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(maxX, minY, minZ), glm::vec3(0.57735, -0.57735, -0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(maxX, maxY, minZ), glm::vec3(0.57735,  0.57735, -0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(minX, maxY, minZ), glm::vec3(-0.57735,  0.57735, -0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(minX, minY, maxZ), glm::vec3(-0.57735, -0.57735,  0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(maxX, minY, maxZ), glm::vec3(0.57735, -0.57735,  0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(maxX, maxY, maxZ), glm::vec3(0.57735,  0.57735,  0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(minX, maxY, maxZ), glm::vec3(-0.57735,  0.57735,  0.57735), glm::vec2(0.f, 0.f)},
    };

    std::vector<u32> indices = {
        0, 2, 1,
        0, 3, 2,
        0, 1, 5,
        0, 5, 4,
        1, 2, 6,
        1, 6, 5,
        4, 5, 6,
        4, 6, 7,
        3, 0, 4,
        3, 4, 7,
        2, 3, 7,
        2, 7, 6,
    };

    MeshData bboxMeshData{ std::move(vertices), std::move(indices) };
    _bbox_mesh = std::make_shared<StaticMesh>(StaticMesh(bboxMeshData));
}

bool SceneObject::is_in_frustum(Frustum f, glm::vec3 camera_position) const {

    if (!_material || !_mesh) {
        return false;
    }

    glm::vec4 a = _transform * glm::vec4(_mesh->bbox_center, 1);
    glm::vec3 b = glm::vec3(a);

    float one = glm::length(glm::vec3(_transform[0]));
    float two = glm::length(glm::vec3(_transform[1]));
    float three = glm::length(glm::vec3(_transform[2]));

    float scaling = three;
    if (one > two)
        if (one > three)
            scaling = one;
    else
        if (two > three)
            scaling = two;

    float actual_radius = _mesh->bbox_radius * scaling;

    if (glm::dot(f._bottom_normal, b - camera_position) < -actual_radius
     || glm::dot(f._left_normal, b - camera_position) < -actual_radius
     || glm::dot(f._near_normal, b - camera_position) < -actual_radius
     || glm::dot(f._right_normal, b - camera_position) < -actual_radius
     || glm::dot(f._top_normal, b - camera_position) < -actual_radius)
        return false;

    return true;
}

void SceneObject::render() const {
    if (!_material || !_mesh) {
        return;
    }

    _material->set_uniform(HASH("model"), transform());
    _material->set_uniform(HASH("instancing"), 0.f);
    _material->bind();
    _mesh->draw();
}

void SceneObject::renderInstanced(size_t instancesCount) const {
    if (!_material || !_mesh) {
        return;
    }

    _material->set_uniform(HASH("instancing"), 1.f);
    _material->bind();
    _mesh->drawInstanced(instancesCount);
}

void SceneObject::renderOcclusionBbox() {
    if (!_material || !_mesh || !_bbox_mesh) {
        return;
    }

    _material->set_uniform(HASH("model"), transform());
    _material->set_uniform(HASH("instancing"), 0.f);
    _material->bind();
    _bbox_mesh->draw();
}

void SceneObject::set_transform(const glm::mat4& tr) {
    _transform = tr;
}

const std::shared_ptr<Material> SceneObject::get_material() const {
    return _material;
}

const glm::mat4 SceneObject::get_transform() const {
    return _transform;
}

const size_t SceneObject::getVerticesCount() const {
    return _mesh->verticesCount;
}

const glm::mat4& SceneObject::transform() const {
    return _transform;
}

}
