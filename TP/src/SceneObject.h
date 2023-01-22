#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <StaticMesh.h>
#include <Material.h>

#include <memory>
#include <Camera.h>

#include <glm/matrix.hpp>

#include <OcclusionQuery.h>
#pragma warning (disable : 4005)

namespace OM3D {

class SceneObject : NonCopyable {

    public:
        
        SceneObject(std::shared_ptr<StaticMesh> mesh = nullptr, std::shared_ptr<Material> material = nullptr);

        void buildBbox();

        bool is_in_frustum(Frustum f, glm::vec3 camera_position) const;

        void render() const;
        void renderInstanced(size_t instancesCount) const;
        void renderOcclusionBbox();

        void set_transform(const glm::mat4& tr);
        const glm::mat4& transform() const;

        const std::shared_ptr<Material> get_material() const;
        const glm::mat4 get_transform() const;
        const size_t getVerticesCount() const;

        OcclusionQuery occlusionQuery;
        bool visible = true;

        struct Sorter {

        public:
            Sorter(glm::vec3 camera_position) : _camera_position(camera_position) {}
            bool operator()(const SceneObject& s1, const SceneObject& s2) const {
                return glm::distance(glm::vec3(s1.transform()[3]) + s1._mesh->bbox_center, _camera_position) < glm::distance(glm::vec3(s2.transform()[3]) + s2._mesh->bbox_center, _camera_position);
            }

        private:
            glm::vec3 _camera_position;
        };

    private:
        glm::mat4 _transform = glm::mat4(1.0f);

        std::shared_ptr<StaticMesh> _mesh;
        std::shared_ptr<Material> _material;

        std::shared_ptr<StaticMesh> _bbox_mesh;
};

}

#endif // SCENEOBJECT_H
