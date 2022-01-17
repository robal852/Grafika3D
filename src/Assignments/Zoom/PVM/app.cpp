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

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }



    //Uniforms
    //2
    GLuint uniformy(0);
    glGenBuffers(1, &uniformy);
    glBindBuffer(GL_UNIFORM_BUFFER, uniformy);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformy);
    //3
    float strength = 0.2f;
    float color[3] = {1.0f, 0.0f, 0.0f};
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 3 * sizeof(float), &color);


    //Moving houses :)
    GLuint moving(0);
    glGenBuffers(1, &moving);
    glBindBuffer(GL_UNIFORM_BUFFER, moving);
    glBufferData(GL_UNIFORM_BUFFER, 12 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, moving);
    float theta = 1.0 * glm::pi<float>() / 6.0f;
    auto cs = std::cos(theta);
    auto ss = std::sin(theta);
    glm::mat2 rot{cs, ss, -ss, cs};
    glm::vec2 trans{0.0, -0.25};
    glm::vec2 scale{0.5, 0.5};
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec2), &scale);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec2), sizeof(glm::vec2), &trans);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 4 * sizeof(float), &rot[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 8 * sizeof(float), 4 * sizeof(float), &rot[1]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    std::vector<GLushort> indices = {0, 1, 2, 3, 4, 5, 3, 5, 6};
    GLuint v_buffer_handle2;
    glGenBuffers(1, &v_buffer_handle2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_buffer_handle2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::vector<GLfloat> vertices = {
            0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  //gora
            0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  //prawo
            -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, //lewo

            -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // t1 ten sam lewo inny kolor
            0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, //ten sam prawo inny kolor


            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  //t2  prawa dol



            // -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  //t1
            //  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  //t2
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f  //lewa dol

    };
    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_buffer_handle2);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);

    // This indicates that the data for attribute 0 should be read from a vertex buffer.
    glEnableVertexAttribArray(0);
    // and this specifies how the data is layout in the buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

    glEnableVertexAttribArray(1);
    // and this specifies how the data is layout in the buffer.
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //end of vao "recording"






    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    auto[w, h] = frame_buffer_size();
    GLuint pvm;
    glGenBuffers(1, &pvm);
    glBindBuffer(GL_UNIFORM_BUFFER, pvm);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glm::mat4 Projection = glm::perspective(glm::half_pi<float>(), (float) w / h, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3{1.0f, 1.0f, 1.0f}, glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec3{0.0f, 0.0f, 1.0f});
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 PVM = Projection * View * Model;
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, pvm);
//    auto[w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    auto PVM = P_ * V_;
    glBindBuffer(GL_UNIFORM_BUFFER, pvm);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid *>(0));
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDrawArrays(GL_TRIANGLES, 3, 6);
    //glDrawArrays(GL_TRIANGLES, 5, 8);
    glBindVertexArray(0);
}

//RESIZING
void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    aspect_ = (float) w / h;
    P_ = glm::perspective(fov_, aspect_, near_, far_);
}

