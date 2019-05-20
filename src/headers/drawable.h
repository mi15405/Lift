#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <string>
#include "textureManager.h"
#include "point3D.h"
#include "material.h"

class Drawable
{
private:
    Point3D trans;        /* Pozicija objekta */
    Point3D rot;          /* Rotacija objekta */
    Point3D scale;        /* Razmera objekta */

    float rot_angle;      /* Ugao za koji se objekat rotira */

    bool using_material;  /* Fleg za koriscenje materijala pri crtanju */

    Material material;    /* Materijal */

public:
    Drawable(
        float tx = 0, float ty = 0, float tz = 0,
        float rot_angle = 0, float rx = 0, float ry = 0, float rz = 0,
        float sx = 1, float sy = 1, float sz = 1);

    /* Crtanje */
    static void draw_quad(std::string texture, float size, float repeat);
    static void draw_cuboid(
            double x, double y, double z, 
            double length, double height, double width);

    void draw(void) const; 
    virtual void draw_object(void) const = 0;

    /* Geteri */
    Point3D& get_trans(void);
    float get_trans_x(void) const;
    float get_trans_y(void) const;
    float get_trans_z(void) const;
    Material& get_material(void);

    /* Seteri */
    void set_trans(float x, float y, float z);
    void set_rot(float angle, float x, float y, float z);
    void set_scale(float x, float y, float z);
    void set_material(Material& mat);
    void set_trans(const Point3D& p);
    void set_using_material(bool is_using);
};

#endif
