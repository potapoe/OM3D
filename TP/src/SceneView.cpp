#include "SceneView.h"

namespace OM3D {

SceneView::SceneView(const Scene* scene) : _scene(scene) {
}

Camera& SceneView::camera() {
    return _camera;
}

const Camera& SceneView::camera() const {
    return _camera;
}

void SceneView::render() const {
    if(_scene) {
        _scene->render(_camera);
    }
}
void SceneView::force_render_triangle() const {
    if (_scene) {
        _scene->force_render_triangle(_camera);
    }
}

void SceneView::render_lights(SceneObject& light_object) const {
    if (_scene) {
        _scene->render_lights(_camera, light_object);
    }
}

}
