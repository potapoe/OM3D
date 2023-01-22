#ifndef SCENE_H
#define SCENE_H

#include <SceneObject.h>
#include <PointLight.h>
#include <Camera.h>

#include <vector>
#include <memory>

namespace OM3D {

class Scene : NonMovable {

    public:
        Scene();

        static Result<std::unique_ptr<Scene>> from_gltf(const std::string& file_name);

        void render(const Camera& camera);

        void add_object(SceneObject obj);
        void add_object(PointLight obj);

        void sortObjects(const Camera& camera);

        // rendering without instancing also means rendering with occlusionCulling
        bool useInstancing = true;

        // from what number of vertices do we use queries on an object
        size_t minVerticesForQuery = 0;

    private:

        void renderObjectsBasic(Frustum frustum, glm::vec3 camera_position);
        void renderObjectsInstanced(Frustum frustum, glm::vec3 camera_position);

        std::vector<SceneObject> _objects;
        std::vector<PointLight> _point_lights;
        glm::vec3 _sun_direction = glm::vec3(0.2f, 1.0f, 0.1f);
};

}

#endif // SCENE_H
