#ifndef H_POINT3D
#define H_POINT3D

#include <string>

class Point3D
{
private:
    double x, y, z;        /* Koordinate u prostoru */
    std::string to_string; /* Ispis za debagovanje */
public:
    Point3D(double x = 0, double y = 0, double z = 0);
    Point3D(const Point3D& from, const Point3D& to);
    Point3D(const Point3D& p);

    void add(const Point3D& point);
    void add(double add_x, double add_y, double add_z);
    void sub(const Point3D& point);
    void sub(double sub_x, double sub_y, double sub_z);
    void mult(double mult);
    void div(double div);
    void normalize(void);
    void limit_intensity(double intensity);
    
    void set_cross(const Point3D& p1, const Point3D& p2);
    void change_coord_system(const Point3D& old_sys, const Point3D& new_sys);

    bool is_in_range(char axis, double min, double max);
    bool is_not_null(void) const;
    bool is_different_from(const Point3D p);

    void set_intensity(double intensity);
    double get_intensity(void) const;
    double get_distance_to(const Point3D& p) const;

    void limit_axis(char axis, double min, double max);
    double limit_value(double value, double min, double max);

    void reset(void);
    void flip_x(void);
    void flip_y(void);
    void flip_z(void);

    double get_x(void) const;
    double get_y(void) const;
    double get_z(void) const;

    /* Seteri */
    void set_x(double x);
    void set_y(double y);
    void set_z(double z);
    void set(double x, double y, double z);
    void set(const Point3D& p);
    void set(const Point3D& from, const Point3D& to);
    void set(const Point3D& from, const Point3D& to, double intensity);

    operator const char*();
};

#endif
