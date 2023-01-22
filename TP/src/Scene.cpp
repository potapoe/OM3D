#include "Scene.h"

#include <TypedBuffer.h>

#include <shader_structs.h>
#include <iostream>
#include <algorithm>

namespace OM3D {

Scene::Scene() {
}

void Scene::add_object(SceneObject obj) {
    _objects.emplace_back(std::move(obj));
}

void Scene::add_object(PointLight obj) {
    _point_lights.emplace_back(std::move(obj));
}

void Scene::sortObjects(const Camera& camera)
{
    // sort the objects based on their distance to camera
    std::sort(_objects.begin(), _objects.end(), SceneObject::Sorter(camera.position()));
}

// Render every object (without instancing) -> only way to use occlusionQuery in this code
void Scene::renderObjectsBasic(Frustum frustum, glm::vec3 camera_position)
{
    for (SceneObject& obj : _objects) {
        if (obj.is_in_frustum(frustum, camera_position))
        {
            if (obj.getVerticesCount() < minVerticesForQuery)
            {
                obj.render();
            }
            else if (obj.occlusionQuery.isAvailable())
            {
                if (obj.occlusionQuery.isVisible())
                {
                    obj.visible = true;
                    obj.occlusionQuery.beginQuery();
                    obj.render();
                    obj.occlusionQuery.endQuery();
                }
                else
                {
                    obj.visible = false;
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                    glDepthMask(GL_FALSE);
                    obj.occlusionQuery.beginQuery();
                    obj.renderOcclusionBbox();
                    obj.occlusionQuery.endQuery();
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                    glDepthMask(GL_TRUE);
                }
            }
            else
            {
                if (obj.visible)
                {
                    obj.render();
                }
            }
            
        }
    }
}

// Render every object (with instancing)
void Scene::renderObjectsInstanced(Frustum frustum, glm::vec3 camera_position)
{
    std::vector<bool> chosen_objects(_objects.size(), false);
    std::vector<glm::mat4> modelMatrices;
    for (int i = 0; i < _objects.size(); i++) {
        if (chosen_objects[i])
            continue;
        chosen_objects[i] = true;

        // get model matrices from every object of the same instance as chosen_objects[i]
        modelMatrices.clear();
        for (int j = i; j < _objects.size(); j++) {
            if (!(_objects[i].get_material().get() == _objects[j].get_material().get()))
                continue;
            chosen_objects[j] = true;
            if (!_objects[j].is_in_frustum(frustum, camera_position))
                continue;
            modelMatrices.push_back(_objects[j].get_transform());
        }

        // Fill and bind instance buffer
        if (modelMatrices.size() > 0)
        {
            TypedBuffer<shader::Instance> instance_buffer(nullptr, modelMatrices.size());
            {
                auto mapping = instance_buffer.map(AccessType::WriteOnly);
                for (size_t i = 0; i != modelMatrices.size(); ++i) {
                    const auto& model = modelMatrices[i];
                    mapping[i].model = {
                        model
                    };
                }
            }
            instance_buffer.bind(BufferUsage::Storage, 2);
            _objects[i].renderInstanced(modelMatrices.size());
        }
    }
}

void Scene::render(const Camera& camera) 
{
    // Fill and bind frame data buffer
    TypedBuffer<shader::FrameData> buffer(nullptr, 1);
    {
        auto mapping = buffer.map(AccessType::WriteOnly);
        mapping[0].camera.view_proj = camera.view_proj_matrix();
        mapping[0].point_light_count = u32(_point_lights.size());
        mapping[0].sun_color = glm::vec3(1.0f, 1.0f, 1.0f);
        mapping[0].sun_dir = glm::normalize(_sun_direction);
    }
    buffer.bind(BufferUsage::Uniform, 0);

    // Fill and bind lights buffer
    TypedBuffer<shader::PointLight> light_buffer(nullptr, std::max(_point_lights.size(), size_t(1)));
    {
        auto mapping = light_buffer.map(AccessType::WriteOnly);
        for (size_t i = 0; i != _point_lights.size(); ++i) {
            const auto& light = _point_lights[i];
            mapping[i] = {
                light.position(),
                light.radius(),
                light.color(),
                0.0f
            };
        }
    }
    light_buffer.bind(BufferUsage::Storage, 1);

    // retrieve camera frustum and position
    Frustum frustum = camera.build_frustum();
    glm::vec3 camera_position = camera.position();
    
    if (!useInstancing)
    {
        renderObjectsBasic(frustum, camera_position);
    }
    else
    {
        renderObjectsInstanced(frustum, camera_position);
    }
}

}
