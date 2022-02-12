//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Engine/mesh_loader.h"
#include "Engine/ColorMaterial.h"

#define STB_IMAGE_IMPLEMENTATION  1

#include "3rdParty/stb/stb_image.h"

void SimpleShapeApplication::init() {
    xe::ColorMaterial::init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    set_camera(new Camera);
    set_controler(new CameraControler(camera()));

    auto pyramid = new xe::Mesh;
    pyramid = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/pyramid.obj",
                                     std::string(ROOT_DIR) + "/Models");
    add_submesh(pyramid);

    glGenBuffers(1, &pvm_buffer_handle);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, pvm_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

    int w, h;
    std::tie(w, h) = frame_buffer_size();
    float aspect = (float) w / h;
    float fov = glm::pi<float>() / 4.0;
    float near = 0.1f;
    float far = 100.0f;
    camera()->set_aspect(aspect);
    camera()->perspective(fov, aspect, near, far);
    camera()->look_at(glm::vec3{1.8f, -1.4f, 1.8f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0, 1.0, 0.0});

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    glViewport(0, 0, w, h);
}

void SimpleShapeApplication::frame() {
    auto PVM = camera()->projection() * camera()->view();
    glBindBuffer(GL_UNIFORM_BUFFER, pvm_buffer_handle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    for (auto m: meshes_)
        m->draw();
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    camera()->set_aspect((float) w / h);
}
