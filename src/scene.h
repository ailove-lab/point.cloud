#pragma once

#include <kvec.h>
#include <linmath.h>

#include "obj.h"

typedef kvec_t(obj_p) objects_v;

typedef struct scene_t {
    objects_v objects;
    mat4x4 mvp;
    mat4x4 rot;
    mat4x4 p,v;
    float n,f; // near far
    float fov;
} scene_t;
typedef scene_t* scene_p;

scene_p scene_ctor();
void scene_dtor(scene_p scene);
void scene_render(scene_p scene);
void scene_add_obj(scene_p scene, obj_p obj);

