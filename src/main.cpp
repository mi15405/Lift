#include <GL/glut.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#include "point3D.h"
#include "ball.h"
#include "elevator.h"
#include "floor.h"
#include "gameController.h"
#include "camera.h"
#include "image.h"
#include "textureManager.h"

//#define DEBUG 0

/* Podesavanja prozora */
int window_x = 100, window_y = 100;
int window_width = 2000, window_height = 800;

/* Animacija */
unsigned int INTERVAL = 30;
const int TIMER_ID = 0;
bool animation_on = false;

/* Kamera */
Camera camera;
Point3D coord_center(0,0,0);

/* Mis */
int mouse_x = 0, mouse_y = 0;

/* Objekti */
GameController game_controller;

/* Callback funkcije */
void on_reshape(int width, int height);
void on_display(void);
void on_keyboard(unsigned char key, int x, int y);
void on_keyboard_up(unsigned char key, int x, int y);
void on_motion(int x, int y);
void on_timer(int id);

/* Funkcije za inicijalizaciju */
void init(void);
void init_camera(void);
void init_light(void);
void init_textures(void);
void init_balls(int ball_num);

int main(int argc, char** argv)
{
    /* Inicijalizacija gluta */
    glutInit(&argc, argv);
    glutInitDisplayMode(
        GLUT_RGB |
        GLUT_DOUBLE |
        GLUT_DEPTH |
        GLUT_MULTISAMPLE);

    /* Inicijalizacija prozora*/
    glutInitWindowPosition(window_x, window_y);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("LIFT");

    /* Prosledjivanje callback funkcija */
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    glutKeyboardUpFunc(on_keyboard_up);
    glutPassiveMotionFunc(on_motion);

    /* Pocetno podesavanje */
    init();

    /* Ulazak u beskonacnu petlju */
    glutMainLoop();

    return 0;
}

void init(void)
{
    /* Podesavanje boje pozadine */
    glClearColor(0.25, 0.25, 0.25, 1);

    /* 
     * Aktiviranje Depth bafera.
     * Omogucava prikaz sve tri dimenzije 
     */
    glEnable(GL_DEPTH_TEST);

    /* 
     * Aktiviranje GL_MULTISAMPLE-a iscrtava glatke linije umesto nazubljenih
     */
    glEnable(GL_MULTISAMPLE);

    /*
     * Inicijalizacija tekstura, svetla i kamere 
     */
    init_textures();
    init_light();
    init_camera();

    /*
     * Generise se broj(seed) koji ce sluziti za dalje generisanje
     * pseudo-slucajnih brojeva. Kao ulaz koristim trenutno vreme
     * kako bi svako novo pokretanje programa davalo drugacije 
     * "random" brojeve
     */
    srand(std::time(nullptr));

    /* 
     * Postavljam fleg da je glavna animacija (igra) pocela.
     * Ova promenljiva se koristi iskljucivo za debagovanje
     */
    animation_on = true;;

    /*
     * Postavljam funkciju "on_timer" da bude pozvana nakon 
     * vremenskog INTERVAL-a 
     */
    glutTimerFunc(INTERVAL, on_timer, TIMER_ID);
}

void init_textures(void)
{
    /* Ukljucujem opciju za koriscenje 2D tekstura */
    glEnable(GL_TEXTURE_2D);

    /* 
     * "Color::get_colors()" vraca niz boja, koje koristim u igri, u obliku
     * stringova. Svaka boja ima svoje teksture, a ime boje je deo imena
     * fajla u kome je tekstura sacuvana.
     */
    std::vector<std::string> colors = Color::get_colors();

    for (unsigned int i = 0; i < colors.size(); i++)
    {
        /* Konstruisem imena slika koje cu da koristim za teksture */
        std::string image_wall = "wall_" + colors[i];
        std::string image_portal = "portal_" + colors[i];

        /* Konstruisem putanje do fajlova, za svaku boju (njenu sliku) */
        std::string filename_wall = "../textures/" + colors[i] + "_circles.bmp";
        std::string filename_portal = "../textures/" + colors[i] + "_portal.bmp";

        /* 
         * Ubacujem sliku iz putanje "filename_..." u objekat klase Image
         * koji cuvam u klasi TextureManager u mapi (kao kljuc koristim
         * ime slike koje sam konstruisao)
         */
        TextureManager::insert_image(image_wall, filename_wall.c_str());
        TextureManager::insert_image(image_portal, filename_portal.c_str());

        /* 
         * Podesavam teksture i njihove generisane identifikacije cuvam
         * u drugoj mapi klase TextureManager.
         * U ovom slucaju ce kljucevi mapa biti isti te odatle ponovljeni
         * argumenti u pozivu funkcije.
         * U opstem slucaju, kada koristim jednu sliku za vise tekstura, prvi
         * argument je kljuc za mapu sa teksturama, a drugi kljuc za mapu sa
         * slikama. 
         * Detaljnije u okviru klase TextureManager.
         * ------------------------------------------------------------------
         * PS. Za one koji bi odmah da znaju sta su treci i cetvrti argumenti
         * funkcije:
         * Trecim argumentom se bira opcija da li ce tekstura da se ponavlja
         * ili ce da se razvuce preko povrsine koju pokriva.
         * Cetvrtim argumentom se bira metod odredjivanja piksela cija se
         * boja izgubila razvlacenjem ili suzavanjem slike.
         */
        TextureManager::set_texture(image_wall, image_wall, GL_REPEAT, GL_LINEAR);
        TextureManager::set_texture(image_portal, image_portal, GL_CLAMP, GL_LINEAR);
    }

    /*
     * Ponavljam isti proces ucitavanja slika i tekstura.
     * Razmisljao sam da napisem metodu koja bi radila ovaj posao, ali sam
     * shvatio da bih verovatno previse zakomplikovao stvar i dobio bih
     * funkciju koja nije citljiva i koristi se samo u ovom delu koda.
     */
    for (int i = 0; i < 10; i++)
    {
        /* Da, crtao sam teksture za svaku cifru u GIMP-u i uzasne su, ali rade
         * posao. Koristim ih za prikaz rezultata na kraju igre.
         */
        std::string image_name = "digit_" + std::to_string(i);
        std::string file_name = "../textures/" + image_name + ".bmp";

        /*
         * GL_REPEAT : Ponavlja sliku
         * GL_CLAMP: Razvlaci sliku
         * GL_LINEAR: Piksel cija se boja utvrdjuje uzima prosecnu vrednost
         * piksela iz okoline
         * GL_NEAREST: Piksel cija se boja utvrdjuje uzima vrednost najblizeg
         * piksela
         */
        TextureManager::insert_image(image_name, file_name.c_str());
        TextureManager::set_texture(image_name, image_name, GL_CLAMP, GL_NEAREST);
    }

    /* 
     * U ovom delu ucitavam slike ciji nazivi nisu povezani kao oni iznad.
     * Dakle, prvi argument je kljuc koji cu koristiti da referisem sliku
     * cija je putanja drugi argument 
     */
    TextureManager::insert_image("metal", "../textures/rails.bmp");
    TextureManager::insert_image("red_blue", "../textures/red_blue.bmp");
    TextureManager::insert_image("blue_net", "../textures/blue_fishnet.bmp");
    TextureManager::insert_image("hex", "../textures/hexagram.bmp");
    TextureManager::insert_image("bg_floor", "../textures/lava.bmp");
    TextureManager::insert_image("bg", "../textures/mountain.bmp");
    TextureManager::insert_image("lava_mountain", "../textures/lava_mountain.bmp");
    TextureManager::insert_image("lava_mountain_01", "../textures/lava_mountain_01.bmp");

    /*
     * Generisem teksture i cuvam ih u mapi pod kljucevima (prvi argument),
     * drugi argument je kljuc slike
     */

    /* Pozadine */
    TextureManager::set_texture("wall", "bg", GL_REPEAT, GL_LINEAR);
    TextureManager::set_texture("lava", "bg_floor", GL_REPEAT, GL_LINEAR);
    
    /* Lift */
    TextureManager::set_texture("elevator", "blue_net", GL_REPEAT, GL_LINEAR);
    TextureManager::set_texture("elevator_rails", "metal", GL_REPEAT, GL_NEAREST);
    TextureManager::set_texture("bridge", "red_blue", GL_REPEAT, GL_LINEAR);

    /* Spratovi */
    TextureManager::set_texture("rails", "metal", GL_REPEAT, GL_LINEAR);
    TextureManager::set_texture("floor", "hex", GL_REPEAT, GL_LINEAR);
    
    /* Portal: spoljasni deo */
    TextureManager::set_texture("portal_rings", "red_blue", GL_REPEAT, GL_LINEAR);

    /* Pocetni ekran i restart ekran */
    TextureManager::set_texture("start_game", "lava_mountain_01", GL_CLAMP, GL_LINEAR);
    TextureManager::set_texture("restart_game", "lava_mountain", GL_CLAMP, GL_LINEAR);
    
    /* 
     * Brisem slike iz mape (memorije), jer sam ih iskoristio za ucitavanje
     * tekstura.
     */
    TextureManager::clear_images();
}

void init_light(void)
{
    /* Ukljucujem svetlo */
    glEnable(GL_LIGHTING);

    /* Podesavam mod za glatko sencenje */
    glShadeModel(GL_SMOOTH);

    /* Postavljam parametre glavnog svetla */
    GLfloat main_position[] = {15.0, 8.0, 5.0, 0.0};
    GLfloat main_ambient[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat main_diffuse[] = {0.2, 0.2, 0.1, 1.0};
    GLfloat main_specular[] = {0.6, 0.6, 0.1, 1.0};

    /* Ukljucujem glavno svetlo */
    glEnable(GL_LIGHT0);

    /* Podesavam glavnom svetlo pomocu parametara */
    glLightfv(GL_LIGHT0, GL_POSITION, main_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, main_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, main_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, main_specular);

    /* 
     * Postavljam parametre svetla lave.
     * U pitanju je poziciono svetlo, jer je cetvrti argument pozicije 1.
     */
    GLfloat lava_position[] = {0.0, -50.0, 5.0, 1.0};
    GLfloat lava_ambient[] = {0.3, 0.2, 0.0, 1.0};
    GLfloat lava_diffuse[] = {0.9, 0.9, 0.2, 1.0};
    GLfloat lava_specular[] = {0.9, 0.9, 0.2, 1.0};

    /* Ukljucujem svetlo lave */
    glEnable(GL_LIGHT1);

    /* Podesavam svetlo lave pomocu parametara */
    glLightfv(GL_LIGHT1, GL_POSITION, lava_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, lava_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lava_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lava_specular);
}

void init_camera(void)
{
    /* 
     * Podesavanja za debagovanje, ovaj deo koda koristim da kontrolisem
     * kameru i gledam modele. Kontrola kamere nije deo igre. 
     * Postavljam kameru u tacku (1, 1, 1) i okrece je da gleda tacku
     * (0, 0, 0). 
     * Funkcija "set_free_look(true)" omogucava rotiranje pomocu misa
     *
     * Ako rezim za debagovanje nije ukljucen, kamera se postavlja da prati
     * lift, na rastojanju (0, 3, 4) po osama (x, y, z).
     * Dakle, koordinate kamere se postavljaju u odnosu na lift.
     */

#ifdef DEBUG
    camera.set_pos(1, 1, 1);
    camera.get_center().set(0, 0, 0);
    camera.set_free_look(true);
#else
    camera.set_follow_target(game_controller.get_elevator_movable(), 0, 3, 4);
#endif
}

void on_reshape(int width, int height)
{
    /* Podesavam se sirina i visina prozora */
    window_width = width;
    window_height = height;
}

void on_motion(int x, int y)
{
    /* 
     * Racunam pomeranje misa u odnosu na poziciju na kojoj se nalazio pre
     * toga (mouse_x, mouse_y).
     * Razliku mnozim sa brzinom rotacije kamere i dobijam ugao za koji
     * treba da rotiram kameru.
     */
    double rotate_x = (mouse_x - x) * camera.get_rotation_speed();
    double rotate_y = (mouse_y - y) * camera.get_rotation_speed();

    /*
     * Izracunate vrednosti prosledjujem kao argument funkcijama "add_theta" 
     * i "add_phi" koje odradjuju ostatak posla.
     * Detaljnije u klasi Camera.
     */ 
    camera.add_theta(rotate_x);
    camera.add_phi(rotate_y);

    /* Postavljam koordinate misa na nove vrednosti */
    mouse_x = x;
    mouse_y = y;
}

void on_keyboard(unsigned char key, int x, int y)
{
    /*
     * Ukoliko je pritisnuto dugme neko od dugmadi koje koristi igrac,
     * obrada ulaza se prebacuje na funkciju "on_keyboard" klase
     * GameController.
     * Ukoliko dugme nije predvidjeno za igraca, obrada se vrsi u ovoj
     * funkciji.
     */
    if (game_controller.on_keyboard(key))
        return;

    /* Dugme ESCAPE ima vrednost 27 */
    const unsigned char ESCAPE = 27;

    switch (key)
    {
        /* Ukoliko se pritisne ESCAPE, izlazi se iz igre */
        case ESCAPE:
            exit(0);
            break;

        /*
         * Ovaj deo sluzi za pomeranje kamere kada je ukljucen mod za
         * debagovanje
         */
#ifdef DEBUG
        case 'g':
            if (!animation_on)
            {
                /* Pokrece animaciju, ukoliko je zaustavljena */
                animation_on = true;
                glutTimerFunc(INTERVAL, on_timer, TIMER_ID);
            }
            break;
        case 'f':
            /* Zaustavlja animaciju, ukoliko je pokrenuta */
            if (animation_on)
                animation_on = false;
            break;
        case 'j':
            /* Pomera kameru u pravcu i smeru tacke koja se posmatra */
            camera.move_forward();
            break;
        case 'k':
            /* 
             * Pomera kameru u istom pravcu kao i za 'j', ali u suprotnom
             * smeru.
             */
            camera.move_backward();
            break;
        case 'h':
            /* Pomera kameru u levo */
            camera.move_left();
            break;
        case 'l':
            /* Pomera kameru u desno */
            camera.move_right();
            break;
        case 'u':
            /* Pomera kameru ka gore */
            camera.move_up();
            break;
        case 'n':
            /* Pomera kameru ka dole */
            camera.move_down();
            break;
        case 'c':
            /* Rotira kameru tako da gleda u lift */
            camera.fixate_look(game_controller.get_elevator_reference().get_pos());
            break;
        case 'C':
            /* 
             * Ukljucuje slobodno rotiranje kamere. Ovo je ubaceno jer funkcija
             * "fixate_look()" gasi slobodno rotiranje kamere
             */
            camera.set_free_look(true);
            break;
#endif
    }
}

void on_keyboard_up(unsigned char key, int x, int y)
{
    /* 
     * Isto kao i za "on_keyboard", s'tim sto ova funkcija
     * obradjuje dogadjaje kad se dugme pusti
     */
    if (game_controller.on_keyboard_up(key))
        return;

#ifdef DEBUG
    switch (key)
    {
        case 'j':
        case 'k':
        case 'h':
        case 'l':
        case 'u':
        case 'n':
            /* Zaustavlja kretanje kamere */
            camera.stop_moving();
            break;
    }
#endif
}

void on_timer(int id)
{
    /* Proverava da li je tajmer od funkcije koja nam treba */
    if (id != TIMER_ID)
        return;

    /* Osvezava polozaj i rotaciju kamere */
    camera.update();

    /* 
     * Osvezava celu igru. 
     * Detaljnije u klasi GameController
     */
    game_controller.update();

    /* 
     * Javlja glutu da se sadrzaj scene promenio i da je vreme za novo
     * iscrtavanje
     */
    glutPostRedisplay(); 

    /* 
     * Ukoliko animacije jos uvek traje, prosledjuje se funkcija "on_timer"
     * da bude ponovo pozvana posle vremenskog INTERVALA.
     * Rekurzija sa pauzom.
     */
    if (animation_on)
       glutTimerFunc(INTERVAL, on_timer, id); 
}

void on_display(void)
{
    /* 
     * Ciste se baferi za boju i dubinu, kako bi bili spremni za sledece
     * iscrtavanje.
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* 
     * Podesavaju se dimenzije viewporta.
     * To je kao prozor iz kog se posmatra scena.
     */
    glViewport(0, 0, window_width, window_height);
    
    /* Podesavanje projekcije*/
    glMatrixMode(GL_PROJECTION);

    /* Postavlja se jedinicna matrica */
    glLoadIdentity();

    /* Postavlja se perspektiva */
    gluPerspective(
            60,     /* Ugao sirine pogleda */
            window_width / (float) window_height,  /* Odnos sirine i visine */
            1,      /* Udaljenost ravni odsecanja koja je bliza kameri */
            100     /* Udaljenost ravni odsecanja koja je dalja od kamere */
        );
    
    /* 
     * Postavlja poziciju i rotaciju kamere.
     * Sadrzi u sebi poziv funkcije gluLookAt()
     */
    camera.turn_on();

    /* Podesavanje matrice modela */
    glMatrixMode(GL_MODELVIEW);

    /* Postavlja se jedinicna matrica */
    glLoadIdentity();

    /* Iscrtavaju se svi delovi scene */
    game_controller.draw_scene();
       
    /* Zamenjuju se baferi i igracu se prikazuje nofi frejm */
    glutSwapBuffers();

    /* Salje se komanda da se izvrse prethodno zadata iscrtavanja */
    glFlush();
}
