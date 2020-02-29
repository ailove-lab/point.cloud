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

void 
scene_render(scene_p scene) {

    static vec3 cam_pos = {80.0, 0.0, 0.0};
    static vec3 look_at = { 0.0, 0.0, 0.0};
    static vec3 cam_up  = { 0.0, 1.0, 0.0};

    // quat q;
    // quat_rotate(q, M_PI/900.0, (vec3){0.0,0.0,1.0});
    // quat_mul_vec3(cam_pos, q, cam_pos);
    // char buf[256] = {0};
    // vec4_sprint(buf            , "      q", q);
    // vec3_sprint(buf+strlen(buf), "cam_pos", cam_pos);
    // printf(buf); 
   
    mat4x4_perspective(scene->p, scene->fov, ratio, scene->n, scene->f);
    mat4x4_look_at(scene->v, cam_pos, look_at, cam_up);

    static mat4x4 m; 
    if(int_dragging_stoped) {
       mat4x4_mul(scene->m, scene->m, m);
    }
    mat4x4_identity(m);
    if (int_dragging) {
        float w2 = screen_width /2.0;
        float h2 = screen_height/2.0;
        vec2 a = { int_click[0]/w2 - 1.0, 1.0 - int_click[1]/h2 };         
        vec2 b = { int_mouse[0]/w2 - 1.0, 1.0 - int_mouse[1]/h2 };
        mat4x4_arcball(m, m, a, b, 1.0);
    }

    shader_start(shader);
    for(size_t i=0; i<scene->objects.n; i++) {
        
        obj_p o = scene->objects.a[i];
         
        mat4x4_mul(scene->mvp, scene->p, scene->v);
        mat4x4_mul(scene->mvp, scene->mvp, scene->m); 
        mat4x4_mul(scene->mvp, scene->mvp, m);

        glUniformMatrix4fv(shader->mvp, 1, GL_FALSE, (const GLfloat*) scene->mvp);
        glUniformMatrix4fv(shader->rot, 1, GL_FALSE, (const GLfloat*) scene->rot);
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

