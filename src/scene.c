#include "scene.h"
#include "obj.h"
#include "shader.h"
#include "interactive.h"

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
    obj_p o = obj_plane();
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

void 
scene_render(scene_p scene) {

    static float alpha = 0.0, betta=0.0;
    alpha+=1.0;
    betta+=0.1;

    float x = 10.0*sin(alpha/57.0)*sin(betta/57.0);
    float y = 10.0*sin(alpha/57.0)*cos(betta/57.0);
    float z = 10.0*cos(alpha/57.0);
    
    mat4x4_look_at(scene->v, 
    	(vec3){  x,  y,  z}, 
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
        mat4x4_mul(mvp, mvp, o->m);
        // print_mat("mvp", mvp);
        // printf("%d\n", shader->mvp); 
        glUniformMatrix4fv(shader->mvp, 1, GL_FALSE, (const GLfloat*) mvp);

        obj_render(o);
    }
    shader_stop(shader);
}
