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
    scene->n = 0.001;
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
    
    
    float x = gui_camera_radius * sin(gui_camera_rx/57.3)*sin(gui_camera_ry/57.3);
    float z = gui_camera_radius * sin(gui_camera_rx/57.3)*cos(gui_camera_ry/57.3);
    float y = gui_camera_radius * cos(gui_camera_rx/57.3);
        
    glm_perspective(scene->fov, ratio, scene->n, scene->f, scene->p);
    glm_lookat(
    	(vec3){  x,   y,   z}, 
    	(vec3){0.0, 0.0, 0.0},
    	(vec3){0.0, 1.0, 0.0},
        scene->v);
    // glm_translate_make(scene->v, (vec3){0,0,-40});
    // vec3 eye = {  x,   y,   z}; 
    // vec3  to = {0.0, 0.0, 0.0};
    // versor q = {-x, -y, -z, 0.0};
    // glm_quat_normalize(q);
    // glm_quat_mat4t(q, scene->v);
    // glm_mat4_mulv3(scene->v, eye, 1.0f, scene->v[3]);
    // glm_vec3_negate(scene->v[3]);

    //glm_quat_look(eye, q, scene->v);

    shader_start(shader);
    for(size_t i=0; i<scene->objects.n; i++) {
        
        obj_p o = scene->objects.a[i];
        
        glm_mat4_mul(scene->p, scene->v, scene->mvp);
        glm_mat4_identity(scene->rot);

        glUniformMatrix4fv(shader->mvp, 1, GL_FALSE, (const GLfloat*) scene->mvp);
        // glUniformMatrix4fv(shader->rot, 1, GL_FALSE, (const GLfloat*) scene->rot);
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
