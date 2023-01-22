#include "SceneView.h"

namespace OM3D {

SceneView::SceneView(Scene* scene) : _scene(scene) {
}

Camera& SceneView::camera() {
    return _camera;
}

const Camera& SceneView::camera() const {
    return _camera;
}

void SceneView::render() {
    if(_scene) {
        _scene->render(_camera);
    }
}

void SceneView::sortObjects() {
    if (_scene) {
        _scene->sortObjects(_camera);
    }
}

}
