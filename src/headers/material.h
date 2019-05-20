#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL/glut.h>
#include <string>

class Material
{
private:
    /* Parametri materijala */
    GLfloat ambient[4];       
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat shininess;

    /* Strane poligona na koje materijal utice */
    GLenum face;

    /* Ispis za debagovanje */
    std::string to_string;     
public:
    Material(
        float ambient_red = 0.2, 
        float ambient_green = 0.2, 
        float ambient_blue = 0.2, 
        float ambient_alpha = 1.0,
        float diff_red = 0.8, 
        float diff_green = 0.8, 
        float diff_blue = 0.8, 
        float diff_alpha = 1.0,
        float spec_red = 0.0, 
        float spec_green = 0.0, 
        float spec_blue = 0.0, 
        float spec_alpha = 1.0,
        float shininess = 0);

    /* Ukljucuje materijal sa prosledjenim parametrima */
    static void enable(
            GLfloat* ambient,
            GLfloat* diffuse, 
            GLfloat* specular, 
            GLfloat shininess);

    /* Ukljucuje materijal */
    void enable_material(void) const;

    /* Seteri */
    void set_ambient(float red, float green, float blue, float alpha);
    void set_diffuse(float red, float green, float blue, float alpha);
    void set_specular(float red, float green, float blue, float alpha);
    
    void set_ambient(GLfloat* params);
    void set_diffuse(GLfloat* params);
    void set_specular(GLfloat* params);

    void set_shininess(float shininess);
    void set_face(GLenum face);
    void set(Material& m);

    operator const char*();
};

#endif
