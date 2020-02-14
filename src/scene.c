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
    
    mat4x4_perspective(scene->p, 30, 1.0, 0.0001, 1000.0);
    mat4x4_look_at(scene->v, 
    	(vec3){1.0,1.0,1.0}, 
    	(vec3){0.0,0.0,0.0},
    	(vec3){0.0,1.0,0.0});

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

/*
static void print_vec(vec4 v) {
    for(int i=0;i<4;i++) printf("%f ",v[i]);
    printf("\n");
}
static void print_mat(char* n, mat4x4 m){
    printf("-%s-\n",n);
    for(int i=0;i<4;i++) print_vec(m[i]);
}
*/

static inline void mat4x4_rotate_U(mat4x4 Q, mat4x4 M, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	mat4x4 R = {
		{ 1.f, 0.f, 0.f, 0.f},
		{ 0.f, 1.f, 0.f, 0.f},
		{ 0.f, 0.f,   c,   s},
		{ 0.f, 0.f,  -s,   c}
	};
	mat4x4_mul(Q, M, R);
}

void 
scene_render(scene_p scene) {

    static float alpha = 0.0, betta=0.0;
    alpha+=1.0;
    betta+=0.01;

    float x = gui_camera_r*sin(gui_camera_rx/57.3)*sin(gui_camera_ry/57.3);
    float z = gui_camera_r*sin(gui_camera_rx/57.3)*cos(gui_camera_ry/57.3);
    float y = gui_camera_r*cos(gui_camera_rx/57.3);

    mat4x4_perspective(scene->p, 30, (float)width/(float)height, 0.0001, 1000.0);
    
    mat4x4_look_at(scene->v, 
    	(vec3){x,y,z}, 
    	(vec3){0.0,0.0,0.0},
    	(vec3){0.0,1.0,0.0});
    shader_start(shader);
    for(size_t i=0; i<scene->objects.n; i++) {
        obj_p o = scene->objects.a[i];
        mat4x4 mvp;
        mat4x4_mul(mvp, scene->p, scene->v);

        // print_mat("p", scene->p);
        // print_mat("v", scene->v);
        // print_mat("m", o->m);
        mat4x4 m;
        mat4x4_identity(m);
        // mat4x4_rotate_U(o->m,m,betta);
        mat4x4_mul(mvp, mvp, o->m);
        // print_mat("mvp", mvp);
        // printf("%d\n", shader->mvp);

        glUniformMatrix4fv(shader->mvp, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform1f(shader->min, (const GLfloat) gui_min);
        glUniform1f(shader->max, (const GLfloat) gui_max);
        glUniform1f(shader->alpha_1, (const GLfloat) gui_alpha_1);
        glUniform1f(shader->alpha_2, (const GLfloat) gui_alpha_2);
    
        
        obj_render(o);
    }
    shader_stop(shader);
}
