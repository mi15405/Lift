#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <GL/glut.h>
#include <string>
#include <map>
#include <vector>
#include "image.h"

class TextureManager
{
private:
    /* Mapa tekstura */
    static std::map<std::string, GLuint> textures;

    /* Mapa slika */
    static std::map<std::string, Image*> images;

public:
    static void insert_image(std::string image_name, const char* filename);
    static void clear_images(void);

    static void set_mode(GLenum mode);
    static void enable_texture(std::string name);
    static void disable_texture(void);

    static void set_texture(
        std::string name, 
        std::string image_name, 
        GLenum repeat, 
        GLenum filter);

    static void enable_texture_generation(void);
    static void disable_texture_generation(void);

private:
    static Image* find_image(std::string image_name);
    static bool has_texture(std::string name);

    static void generate_texture(std::string name);
    static void generate_textures(std::vector<std::string> names);
};

#endif
