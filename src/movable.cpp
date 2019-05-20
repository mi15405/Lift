#include "movable.h"
#include <iostream>
#include <sstream>

Movable::Movable(
    double pos_x, double pos_y, double pos_z,
    double vel_x, double vel_y, double vel_z)
        : pos(pos_x, pos_y, pos_z),
          vel(vel_x, vel_y, vel_z)
{
    /* Postavljam pocetno vreme */
    time = Clock::now();
}
          
void Movable::update()
{
    /* Trenutno vreme */
    auto new_time = Clock::now();

    /* Proteklo vreme izmedju dva poziva ove funkcije, u sekudama */
    delta_time =
        std::chrono::duration<double, std::ratio<1>> (new_time - time).count();

    /* Cuvam novo vreme da bih ga iskoristio u sledecem pozivu ove funkcije */
    time = new_time;

    Point3D delta_vel = vel;

    /* 
     * Skaliram brzinu u zavisnosti od proteklog vremena izmedju dva poziva za
     * azuriranje
     */
    delta_vel.mult(delta_time);

    /* Pomeram objekat za vrednost skalirane brzine. Na ovaj nacin kretanje 
     * zavisi samo od proteklog vremena, a ne od brzine izvrsavanja
     * instrukcija
     */
    pos.add(delta_vel);    
}

void Movable::move_to(const Point3D& target, double speed)
{
    if (pos.get_distance_to(target) < 0.05)
    {
        /* 
         * Ukoliko je objekat na maloj razdaljini od cilja, postavljam ga na
         * poziciju cilja i resetujem brzinu na 0.
         */
        pos.set(target);
        reset_vel();
    }
    else 
    {
        /* 
         * Ukoliko nije blizu cilja, postavljam vektor brzine tako da je
         * usmeren ka cilju, a intenzitet na prosledjenu vrednost "speed".
         * Detaljnije u klasi Point3D
         */
        vel.set(pos, target, speed);
    }
}

void Movable::bounce_in_box(
    double x_min, double x_max,
    double y_min, double y_max,
    double z_min, double z_max) { /* 
     * Funkcija se trenutno ne koristi, sluzila je za testiranje.
     * Ogranicava poziciju objekta unutar kvadra prosledjenih dimenzija,
     * i odbija objekat kada stigne do granice
     */

    /* Trenutna pozicija objekta */
    double x = pos.get_x();
    double y = pos.get_y();
    double z = pos.get_z();
    
    if (x < x_min || x > x_max)
    {
        if (x < x_min)
            pos.set_x(x_min);
        else
            pos.set_x(x_max);

        /* Obrce x komponentu vektora brzine, mnozeci je sa -1 */
        vel.flip_x();
    }

    if (y < y_min || y > y_max)
    {
        if (y < y_min)
            pos.set_y(y_min);
        else
            pos.set_y(y_max);

        /* Obrce y komponentu vektora brzine, mnozeci je sa -1 */
        vel.flip_y();
    }

    if (z < z_min || z > z_max)
    {
        if (z < z_min) 
            pos.set_z(z_min);
        else 
            pos.set_z(z_max);

        /* Obrce z komponentu vektora brzine, mnozeci je sa -1 */
        vel.flip_z();
    }
}

/* Ispis za debagovanje */
Movable::operator const char*()
{
    std::ostringstream stream;
    stream << "pos: " << pos << "vel: " << vel << std::endl;
    to_string = stream.str();
    return to_string.c_str();
}

void Movable::reset_pos(void) { pos.reset(); }
void Movable::reset_vel(void) { vel.reset(); }

/* Seteri */
void Movable::set_pos_x(double px) { pos.set_x(px); }
void Movable::set_pos_y(double py) { pos.set_y(py); }
void Movable::set_pos_z(double pz) { pos.set_z(pz); }
void Movable::set_pos(double px, double py, double pz) { pos.set(px, py, pz); }
void Movable::set_vel(double px, double py, double pz) { vel.set(px, py, pz); }
void Movable::set_pos(const Point3D& p) { pos.set(p); }
void Movable::set_vel(const Point3D& p) { vel.set(p); }

/* Geteri */
double Movable::get_delta_time(void) { return delta_time; }
Point3D Movable::get_pos(void) const { return Point3D(pos); }
Point3D Movable::get_vel(void) const { return Point3D(vel); }
Point3D& Movable::get_pos_reference(void) { return pos; }
Point3D& Movable::get_vel_reference(void) { return vel; }
Movable& Movable::get_movable(void) { return *this; }

