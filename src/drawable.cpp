#include <iostream>
#include <GL/glut.h>
#include "textureManager.h"
#include "drawable.h"

void Drawable::draw_quad(std::string texture, float size, float repeat)
{
    /* Ukljucuje se tekstura ciji je kljuc prosledjen */
    TextureManager::enable_texture(texture);

    glBegin(GL_QUADS);
        /* Postavlja se normala */
        glNormal3f(0, 0, 1);

        /* 
         * Postavljaju se koordinate teksture i odgovarajuce tacke cetvorougla 
         * Parametar size predstavlja polovinu stranice kvadrata, a repeat broj
         * ponavljanja teksture
         */
        glTexCoord2f(0, 0);
        glVertex3f(-size, -size, -1);

        glTexCoord2f(repeat, 0);
        glVertex3f(size, -size, -1);

        glTexCoord2f(repeat, repeat);
        glVertex3f(size, size, -1);

        glTexCoord2f(0, repeat);
        glVertex3f(-size, size, -1);
    glEnd();

    /* Gasi se koriscena tekstura */
    TextureManager::disable_texture();
}

void Drawable::draw_cuboid(
        double x, double y, double z,
        double length, double height, double width)
{
    glPushMatrix();
        /* Translira se u tacku x, y, z */
        glTranslatef(x, y, z);

        /* Skalira se kocka tako da poprimi prosledjene dimenzije kvadra */
        glScalef(length, height, width);

        /* 
         * Iscrtava kocku jedinicne velicine u skaliranom prostoru, 
         * te se dobija kvadar 
         */
        glutSolidCube(1);
    glPopMatrix();
}

Drawable::Drawable(
    float tx, float ty, float tz,
    float ra, float rx, float ry, float rz,
    float sx, float sy, float sz)
         : trans(tx, ty, tz), 
           rot(rx,ry,rz), 
           scale(sx, sy, sz),
           rot_angle(ra) 
{
    /* Podrazumevano se ne koristi materijal */
    using_material = false;
}

void Drawable::draw(void) const
{
    glPushMatrix();

        /* Translira se u datu poziciju */
        glTranslatef(trans.get_x(), trans.get_y(), trans.get_z());

        /* Rotira se za dati ugao, podesenih osa */
        glRotatef(rot_angle, rot.get_x(), rot.get_y(), rot.get_z());

        /* Skalira se sa podesenim vrednostima */
        glScalef(scale.get_x(), scale.get_y(), scale.get_z());

        /* Ukoliko se koristi materijal, aktivira se podeseni materijal */
        if (using_material)
            material.enable_material();

        /* 
         * Iscrtava se objekat. Ovo je potpuno virtualna funkcija koju
         * implementiraju objekti (zapravo ja) koji nasledjuju ovu klasu
        */
        draw_object();
    glPopMatrix();
}

/* Geteri */
Point3D& Drawable::get_trans(void) { return trans; }

float Drawable::get_trans_x(void) const { return trans.get_x(); }

float Drawable::get_trans_y(void) const { return trans.get_y(); }

float Drawable::get_trans_z(void) const { return trans.get_z(); }

Material& Drawable::get_material(void) { return material; }

/* Seteri */
void Drawable::set_material(Material& mat)
{
    material.set(mat);
    using_material = true;
}

void Drawable::set_trans(const Point3D& p) { trans.set(p); }

void Drawable::set_trans(float x, float y, float z)
{
    trans.set(x, y, z);
}

void Drawable::set_rot(float angle, float x, float y, float z)
{
    rot_angle = angle;
    rot.set(x, y, z);
}

void Drawable::set_scale(float x, float y, float z) { scale.set(x, y, z); }

void Drawable::set_using_material(bool is_using) { using_material = is_using; }
