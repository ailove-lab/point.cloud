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
    scene->fov = 30.0;
    scene->f = 100.0;
    scene->n = 1.0;
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

    static float alpha = 0.0, betta=0.0;
    alpha+=1.0;
    betta+=0.01;

    float x = gui_camera_radius*sin(gui_camera_rx/57.3)*sin(gui_camera_ry/57.3);
    float z = gui_camera_radius*sin(gui_camera_rx/57.3)*cos(gui_camera_ry/57.3);
    float y = gui_camera_radius*cos(gui_camera_rx/57.3);

    vec4 v = {10.0, 0.0, 0.0, 0.0};
   
    mat4x4_perspective(scene->p, scene->fov, ratio, scene->n, scene->f);
    // mat4x4_perspective(scene->p, 30, 1.0, 0.0001, 1000.0);
    mat4x4_look_at(scene->v, 
    	(vec3){x, y, z}, 
    	(vec3){0.0, 0.0, 0.0},
    	(vec3){0.0, 1.0, 0.0});

    shader_start(shader);
    for(size_t i=0; i<scene->objects.n; i++) {
        
        obj_p o = scene->objects.a[i];
        
        mat4x4_mul(scene->mvp, scene->p, scene->v);
        mat4x4_identity(scene->rot);
        mat4x4_rotate_U(scene->rot, scene->rot, gui_rot_u);
        // mat4x4_rotate_V(rot, rot, gui_rot_v);
        
        // mat4x4_mul(mvp, mvp, o->m);
        // print_mat("mvp", mvp);

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

