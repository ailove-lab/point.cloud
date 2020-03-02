#include "bbgl.h"
#include "scene.h"
#include "obj.h"
#include "shader.h"
#include "interactive.h"
#include "gui.h"

shader_p shader;

scene_p
scene_ctor() {
    scene_p scene = calloc(1, sizeof(scene_t));
    kv_init(scene->objects);
    scene->fov = 60.0/180.0*3.14159;
    scene->f = 100.0;
    scene->n =   1.0;
    mat4x4_identity(scene->m);
    mat4x4_identity(scene->r);
    quat_identity(scene->q);
    shader = shader_ctor("simple");
    obj_p o = obj_cloud();
    scene_add_obj(scene, o);
    
	return scene;
}

void 
scene_dtor(scene_p scene) { 
    // free objects
    for(size_t i=0; i<scene->objects.n; i++) {
        obj_dtor(scene->objects.a[i]);
    }
	kv_destroy(scene->objects);
	free(scene);
}

void 
scene_add_obj(scene_p scene, obj_p obj) {
    kv_push(obj_p, scene->objects, obj);
}

vec3 rotate_axis;



void 
scene_render(scene_p scene) {


    // P
    mat4x4_perspective(scene->p, scene->fov, ratio, scene->n, scene->f);

    // V    
    mat4x4_translate(scene->v, 0, 0, -100);
    static float a = 0.01;
    // a+=0.01;
    static vec3 axis = {1.0, 1.0, 1.0};
    vec3_norm(axis, axis);
    quat q; 
    quat_rotate(q, a, axis);
    quat_norm(q, q);

    quat_printf(scene->q);
    quat_printf(q);
    quat sq = {scene->q[0],scene->q[1],scene->q[2],scene->q[3]};
    quat_mul(scene->q, q, sq);
    quat_norm(scene->q, scene->q);
    quat_printf(scene->q);
    printf("\n");

    mat4x4_from_quat(scene->r, scene->q);
    mat4x4_mul(scene->v, scene->v, scene->r);

    // P*V
    mat4x4_mul(scene->vp, scene->p, scene->v);
        
    shader_start(shader);
    for(size_t i=0; i<scene->objects.n; i++) {
        
        obj_p o = scene->objects.a[i];
        

        if (int_dragging) {            
            float w2 = screen_width /2.0;
            float h2 = screen_height/2.0;
            vec3 a = { int_click[0]/w2 - 1.0, 1.0 - int_click[1]/h2, 0.0 };         
            vec3 b = { int_mouse[0]/w2 - 1.0, 1.0 - int_mouse[1]/h2, 0.0 };
            if (vec3_len(a)<1.0) a[2] = sqrtf(1.0-vec3_mul_inner(a,a)); else vec3_norm(a,a);
            if (vec3_len(b)<1.0) b[2] = sqrtf(1.0-vec3_mul_inner(b,b)); else vec3_norm(b,b);
            // mat4x4_arcball(mvp, scene->p, a, b, 1.0);
            // mat4x4_mul(mvp, mvp, scene->r);
        }
        mat4x4 mvp;
        mat4x4_mul(mvp, scene->vp, o->m);
        
        glUniformMatrix4fv(shader->mvp, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform1f(shader->off,        (const GLfloat) gui_off_u);
        glUniform1f(shader->min,        (const GLfloat) gui_min);
        glUniform1f(shader->max,        (const GLfloat) gui_max);
        glUniform1f(shader->alpha_1,    (const GLfloat) gui_alpha_1);
        glUniform1f(shader->alpha_2,    (const GLfloat) gui_alpha_2);
        glUniform1f(shader->point_size, (const GLfloat) gui_point_size);
            
        obj_render(o);
    }
    shader_stop(shader);
}

