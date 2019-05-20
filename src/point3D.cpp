#include "point3D.h"
#include <iostream>
#include <math.h>
#include <sstream>

Point3D::Point3D(double x, double y, double z)
    : x(x), y(y), z(z) 
{}

Point3D::Point3D(const Point3D& p)
{
    x = p.x;
    y = p.y;
    z = p.z;
}

Point3D::Point3D(const Point3D& from, const Point3D& to)
{
    x = to.x -from.x;
    y = to.y -from.y;
    z = to.z -from.z;
}

void Point3D::add(const Point3D& p)
{
    x += p.x;
    y += p.y;
    z += p.z;
}

void Point3D::add(double add_x, double add_y, double add_z)
{
    x += add_x;
    y += add_y;
    z += add_z;
}

void Point3D::sub(const Point3D& p)
{
    x -= p.x;
    y -= p.y;
    z -= p.z;
}

void Point3D::sub(double sub_x, double sub_y, double sub_z)
{
    x -= sub_x;
    y -= sub_y;
    z -= sub_z;
}

void Point3D::mult(double mult)
{
    x *= mult;
    y *= mult;
    z *= mult;
}

void Point3D::div(double div)
{
    x /= div;
    y /= div;
    z /= div;
}

bool Point3D::is_in_range(char axis, double min, double max)
{
    /* 
     * Vraca TRUE ako je projekcija objekta na prosledjenu osu izmedju
     * prosledjene minimalne i maksimalne vrednosti
     */
    double value = 0.0;

    /* Podesava se projekcija */
    if (axis == 'x') 
        value = x;
    else if (axis == 'y') 
        value = y;
    else if (axis == 'z') 
        value = z;
    else
    {
        std::cerr << "Unknown axis: " << axis << std::endl;
        return false;
    }

    /* Porede se vrednosti */
    if (value >= min && value <= max)
        return true;
    else
        return false;
}

bool Point3D::is_not_null(void) const
{
    if (x != 0 || y != 0 || z != 0)
        return true;
    return false;
}

bool Point3D::is_different_from(const Point3D p)
{
    if (x != p.x || y != p.y || z != p.z)
        return true;
    return false;
}

void Point3D::normalize(void)
{
    /* Deli koordinate sa intenzitetom i vektor postaje normalizovan */
    div(get_intensity());
}

void Point3D::limit_intensity(double limit)
{
    double current_intensity = get_intensity();
    if (current_intensity > limit)
    {
        /* Ukoliko je intenzitet veci od granice, smanjuje ga na granicu */
        mult(limit / current_intensity);
    }
}

double Point3D::get_intensity() const 
{
    /* Vraca intenzitet vektora */
    return sqrt(x*x + y*y + z*z);
}

double Point3D::get_distance_to(const Point3D& p) const
{
    /* Vraca udaljenost od prosledjene tacke */
    return sqrt(
        (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y) + (p.z - z) * (p.z - z));
}

void Point3D::change_coord_system(const Point3D& old_sys, const Point3D& new_sys)
{
    /* 
     * Preslikava koordinate iz starog koordinatnog sistema u novi koordinatni
     * sistem sabiranjem i oduzimanjem.
     * "old_sys" i "new_sys" su koordinate koordinatnog pocetka starog i novog
     * koordinatnog sistema u globalnom koordinatnom sistemu.
     */
    add(old_sys);
    sub(new_sys);
}

void Point3D::set_cross(const Point3D& p1, const Point3D& p2)
{
    /* Postavlja koordinate na vektorski proizvod prosledjenih vektora */
    set_x(p1.z * p2.y - p1.y * p2.z);
    set_y(p1.x * p2.z - p1.z * p2.x);
    set_z(p1.y * p2.x - p1.x * p2.y);
}

void Point3D::limit_axis(char axis, double min, double max)
{
   /* Ogranicava prosledjenu komponentu na interval [min, max] */
   switch (axis)
   {
        case 'x': 
            x = limit_value(x, min, max);
            break;
        case 'y': 
            y = limit_value(y, min, max);
            break;
        case 'z': 
            z = limit_value(z, min, max);
            break;
        default:
            std::cout << "Point3D::limit_axis: Unknown axis" << std::endl;
   }
}

double Point3D::limit_value(double value, double min, double max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else 
        return value;
}

void Point3D::flip_x(void) { x *= -1; }
void Point3D::flip_y(void) { y *= -1; }
void Point3D::flip_z(void) { z *= -1; }

double Point3D::get_x(void) const { return x; }
double Point3D::get_y(void) const { return y; }
double Point3D::get_z(void) const { return z; }

void Point3D::reset(void) { x = 0; y = 0; z = 0; }

void Point3D::set_x(double x) { this->x = x; }
void Point3D::set_y(double y) { this->y = y; }
void Point3D::set_z(double z) { this->z = z; }

void Point3D::set(double x, double y, double z)
{
    set_x(x);
    set_y(y);
    set_z(z);
}

void Point3D::set(const Point3D& p)
{
    set(p.x, p.y, p.z);
}

void Point3D::set(const Point3D& from, const Point3D& to)
{
    /* Postavlja vrednosti na vektor od tacke "from" do tacke "to" :) */
    x = to.x -from.x;
    y = to.y -from.y;
    z = to.z -from.z;
}

void Point3D::set(const Point3D& from, const Point3D& to, double intensity)
{
    /* 
     * Postavlja vrednosti na vektor od tacke "from" do tacke "to" i postavlja
     * intenzitet na prosledjenu vrednost
     */
    set(from, to);
    set_intensity(intensity);
}

void Point3D::set_intensity(double intensity)
{
    /* Normalizuje vektor */
    normalize();

    /* Mnozi sa prosledjenim intenzitetom */
    mult(intensity);
}

/* Ispis za debagovanje */
Point3D::operator const char*()
{
    std::ostringstream stream;
    stream << "(" << x << ", " << y << ", " << z << ")" << std::endl;
    to_string = stream.str();
    return to_string.c_str();
}

