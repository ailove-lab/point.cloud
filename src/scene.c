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

vec3 rotate_axis;

void 
scene_render(scene_p scene) {

    static vec3 cam_pos = { 0.0, 0.0, 40.0};
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

    shader_start(shader);
    for(size_t i=0; i<scene->objects.n; i++) {
        
        obj_p o = scene->objects.a[i];
         
        mat4x4_mul(scene->vp, scene->p,   scene->v);
        static mat4x4 r, mvp;
        
        if(int_dragging_stoped) {
            mat4x4_mul(o->m, o->m, r); 
            mat4x4_identity(r);
        }
        
        if (int_dragging) {

            mat4x4_identity(r);
            // center
            float w2 = screen_width /2.0;
            float h2 = screen_height/2.0;
            vec3 a = { int_click[0]/w2 - 1.0, 1.0 - int_click[1]/h2, 0.0};         
            vec3 b = { int_mouse[0]/w2 - 1.0, 1.0 - int_mouse[1]/h2, 0.0};
            if (vec3_len(a)<1.0) a[2] = sqrtf(1.0-vec3_mul_inner(a,a)); else vec3_norm(a,a);
            if (vec3_len(b)<1.0) b[2] = sqrtf(1.0-vec3_mul_inner(b,b)); else vec3_norm(b,b);

            vec3_mul_cross(rotate_axis, a, b);
             
        	float const angle = acos(vec3_mul_inner(a, b));
            quat q; quat_rotate(q, angle, rotate_axis);
            quat_mul_vec3(cam_pos, q, cam_pos);
        }

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

