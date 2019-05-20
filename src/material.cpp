#include <GL/glut.h>
#include <sstream>
#include "material.h"

void Material::enable(
        GLfloat* ambient,
        GLfloat* diffuse,
        GLfloat* specular,
        GLfloat shininess)
{
    /* Ukljucuje materijal sa prosledjenim parametrima */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient); 
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse); 
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular); 
    glMaterialf(GL_FRONT, GL_SHININESS, shininess); 
}

Material::Material(
    float ambient_red, float ambient_green, float ambient_blue, float ambient_alpha,
    float diff_red, float diff_green, float diff_blue, float diff_alpha,
    float spec_red, float spec_green, float spec_blue, float spec_alpha,
    float shininess)
{
    /* Inicijalizacija parametara */
    set_ambient(ambient_red, ambient_green, ambient_blue, ambient_alpha);
    set_diffuse(diff_red, diff_green, diff_blue, diff_alpha);
    set_specular(spec_red, spec_green, spec_blue, spec_alpha);
    set_shininess(shininess);
    set_face(GL_FRONT);
}

void Material::enable_material(void) const
{
    /* Ukljucuje materijal sa trenutnim vrednostima */
    glMaterialfv(face, GL_AMBIENT, ambient); 
    glMaterialfv(face, GL_DIFFUSE, diffuse); 
    glMaterialfv(face, GL_SPECULAR, specular); 
    glMaterialf(face, GL_SHININESS, shininess); 
}

/* Seteri */
void Material::set_ambient(float red, float green, float blue, float alpha)
{
    ambient[0] = red; 
    ambient[1] = green; 
    ambient[2] = blue; 
    ambient[3] = alpha; 
}

void Material::set_diffuse(float red, float green, float blue, float alpha)
{
    diffuse[0] = red; 
    diffuse[1] = green; 
    diffuse[2] = blue; 
    diffuse[3] = alpha; 
}

void Material::set_specular(float red, float green, float blue, float alpha)
{
    specular[0] = red; 
    specular[1] = green; 
    specular[2] = blue; 
    specular[3] = alpha; 
}

void Material::set_ambient(GLfloat* params)
{
    for (int i = 0; i < 4; i++)
        ambient[i] = params[i];
}

void Material::set_diffuse(GLfloat* params)
{
    for (int i = 0; i < 4; i++)
        diffuse[i] = params[i];
}

void Material::set_specular(GLfloat* params)
{
    for (int i = 0; i < 4; i++)
        specular[i] = params[i];
}

void Material::set_shininess(float shininess)
{
    this->shininess = shininess;
}

void Material::set_face(GLenum face)
{
    this->face = face;
}

void Material::set(Material& m)
{
    set_ambient(m.ambient);
    set_diffuse(m.diffuse);
    set_specular(m.specular);
    set_shininess(m.shininess);
}

Material::operator const char*()
{
    std::ostringstream stream;
    stream << "ambient: ";
    for (GLfloat value: ambient)
        stream << value << " ";
    stream << std::endl;

    stream << "diffuse: ";
    for (GLfloat value: diffuse)
        stream << value << " ";
    stream << std::endl;

    stream << "specular: ";
    for (GLfloat value: specular)
        stream << value << " ";
    stream << std::endl;

    stream << "shininess: " << shininess << std::endl;
    to_string = stream.str();
    
    return to_string.c_str();
}
