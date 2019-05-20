#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "gameController.h"
#include "floor.h"
#include "point3D.h"
#include "textureManager.h"

Floor::Floor(float len, unsigned int capacity, Color col)  
    : length(len),
      color(col), 
      ball_row(Point3D(-length/2, 0, 0), Point3D(length/2, 0, 0), capacity)
{
    /* 
     * Pocetak i kraj reda loptica se postavljaju na levi i desni kraj sprata
     * i postavlja se kapacitet reda unutar liste za inicijalizaciju
     */

    /* Ucitavam rgb vrednosti iz boje sprata */
    float rgb[3];     
    color.set_rgb_array(rgb, 0.2);
    
    /* Podesavam parametre materijala */
    Material mat(
        rgb[0], rgb[1], rgb[2], 1,
        0.0, 0.0, 0.2, 1.0,
        0.0, 0.0, 0.0, 1.0, 
        1.5
    );

    /* Postavljam podeseni materijal */
    set_material(mat);

    /* Pseudo-slucajan broj u intervalu [0, 1] */
    float random = std::rand() / (float) RAND_MAX;

    /* 
     * Inicijalizujem pseudo-slucajan broj u intervalu [-1,1] od koga ce
     * zavisiti izgled i animacija portala na kraju sprata
     */
    random_num = -1 + 2 * random;
}

Floor::~Floor()
{}

void Floor::draw_object(void) const
{
    /* Debljina poda */
    float height = 0.1f;

    /* Sirina poda */
    float width = 0.5f;

    glPushMatrix();
        /* Ukljucujem generisanje koordinata za teksture */
        TextureManager::enable_texture_generation();

        /* Ukljucujem teksturu */
        TextureManager::enable_texture("floor");

        /* Transliram ka dole, kako bi povrsina poda imala koordinatu Y = 0 */
        glTranslatef(0, -height/2, 0);

        /* Crtam pod */
        Drawable::draw_cuboid(0, 0, 0, length, height, width);

        /* 
         * Ukljucujem teksturu u zavisnosti od boje sprata.
         * Trenutno su sve teksture zida slicne (razlikuju se po boji), te sam
         * mogao da ucitam samo jednu sliku za teksturu i da spratovi poprime
         * boju od materijala. Medjutim, na ovaj nacin mogu lako da promenim
         * teksturu sprata za svaku pojedinacnu boju, ako budem zeleo da
         * spratovi imaju razlicite teksture (samo promenim fajlove u folderu
         * za teksture).
         */
        TextureManager::enable_texture("wall_" + color.get_color());

        /* 
         * Crtam zid, pomeren po Y i Z osi i obrnutih dimenzija od poda (umesto
         * rotiranja oko X ose).
         */
        Drawable::draw_cuboid(
            0, width/2 - height/2, -width/2, 
            length, width, height);

        /* 
         * Ukljucujem teksturu za bocne pregrade, koje daju utisak da loptice
         * ne mogu da ispadnu sa sprata u lavu
        */
        TextureManager::enable_texture("rails");

        /* Crtam bocne pregrade na datim pozicijama i datih dimenzija */
        Drawable::draw_cuboid(0, height/2, -width/3, length, 0.05, 0.05);
        Drawable::draw_cuboid(0, height/2, width/3, length, 0.05, 0.05);

        /* Gasim teksturu */
        TextureManager::disable_texture();

        /* Gasim generisanje koordinata za teksture */
        TextureManager::disable_texture_generation();
    glPopMatrix();

    /* Iscrtavam portal na kraju sprata */
    draw_portal();

    /* Iscrtavam loptice unutar reda loptica */
    ball_row.draw();
}

void Floor::draw_portal() const
{
    /* 
     * X komponentu pozicije sprata postavljam na desni kraj sprata.
     * Portal pomeram malo ka sredini sprata kako bi se stvorio utisak 
     * da loptice porlaze kroz portal.
     */
    float pos_x = length/2 - 0.2;

    /* Postavljam Y komponentu malo iznad nule */
    float pos_y = 0.3f;

    /* 
     * Ukoliko je boja siva (spratovi sa leve strane), portale iscrtavam sa
     * suprotne strane
     */
    if (color.get_color() == "gray")
        pos_x *= -1;

    /* Broj temena */
    int sides = 8;

    /* Menjam broj temena kako bi portali bili malo raznovrsniji */
    if (random_num < -0.4)
        sides = 6;
    else if (random_num > 0.4)
        sides = 12;

    /* Radijus - udaljenost temena od centra */
    float radius = 0.8f;

    /* Trenutni ugao iscrtanog temena */
    float angle = 0.0f;

    /* Ugao koji dodajem na trenutni ugao nakon iscrtavanja temena */
    float delta_angle = M_PI * 2 / sides;

    /* Trenutno vreme */
    float time = GameController::get_current_time();

    /* 
     * Posto cu za animaciju portala uglavnom koristiti sinus i cosinus,
     * izracunacu ih sada pa cu te vrednosti koristiti u ostatku funkcije
     */
    float sin_time = sin(time);
    float cos_time = cos(time);

    /* 
     * Ove vrednosti dodajem kako bih pomerio faze animacija portala, te ce se
     * animirati asinhrono u odnosu na ostale portale
     */
    float sin_random = sin(time + random_num);
    float cos_random = cos(time + random_num);

    /* Brzina rotacije portala */
    float rotation_speed = 10.0f;

    /* Pomeranje po Y osi */
    float delta_pos_y = 0.1 * sin_random;

    glPushMatrix();

        /* Pomeram portal po Y osi u zavisnosti od "delta_pos_y" */
        glTranslatef(0.0f, pos_y + delta_pos_y, 0.0f);

        /* Rotiram portal oko X ose u zavisnosti od proteklog vremena */
        glRotatef(rotation_speed * time, 1.0f, 0.0f, 0.0f);

        /* Ukljucujem teksturu */
        TextureManager::enable_texture("portal_" + color.get_color());

        /* Crtam portal */
        glBegin(GL_POLYGON);
            for (int i = 0; i < sides; i++)
            {
                /* 
                 * Postavljam centar kruga u tacku (0.5, 0.5) u centar
                 * testure, i koristim tacke sa kruznice ciji je 
                 * radijus 0.5, koristeci trenutni ugao.
                 * Na taj nacin dobijam koordinatu teksture.
                 */
                glTexCoord2f(0.5 + 0.5 * cos(angle),0.5 + 0.5 * sin(angle));

                /* Crtam tacku portala korsiteci jednacinu kruznice ciji sam
                 * radijus podesio (opet se koriste polarne koordinate za
                 * kruznicu: Y = radius * cos(t), Z = radius * sin(t)
                 */
                glVertex3f(pos_x, radius * cos(angle), radius * sin(angle));

                /* Pomeram ugao na sledecu vrednost u krugu */
                angle += delta_angle;
            }
        glEnd();

        /* 
         * Sledi crtanje ivica portala, koje implementiram tako sto iscrtam
         * jednu ivicu, a zatim rotiram oko X ose kako bih iscrtao sledecu.
         * Rotaciju izmedju dva crtanja dobijam deljenjem punog kruga na broj
         * ivica.
         */
        float rotation = 360 / sides;

        /* 
         * Posto je broj stranica promenljiv, kao i radijus portala,
         * promenljiva je i duzina ivice oko portala.
         * Ivica koja spaja dva temena, zajedno sa dve duzi koje krecu iz tih
         * temena i spajaju se u centru portala, cini jednakokraki trougao.
         *
         * Ako iz centra povucemo simetralu ugla (ili ivice, iste su)
         * dobijamo pravougli trougao cija jedna stranica radijus, druga
         * stranica je polovina ivice, a treca stranica spaja centar ivice sa
         * centrom portala.
         * U tom trouglu nam je poznat ugao kod centra, delta_angle/2 izracunat iznad,
         * nije rotation/2 jer nam trebaju radijani za sinus i kosinus).
         * Kako znamo ugao i hipotenuzu (radijus) dobijamo duzinu ivice iz
         * sledece formule:
         */
        float edge = 2 * radius * sin(delta_angle/2);

        /* Povecavam ivicu malo kako bi se coskovi lepo preklopili */
        edge += 0.03;

        /* 
         * Ostaje jos da izracunamo rastojanje centra ivice od centra portala.
         * Koristimo isti pravougli trougao i iz Pitagorine teoreme sledi:
         */
        float distance = sqrt(radius * radius - edge/2 * edge/2);

        /* Ukljucujem generaciju koordinata tekstura */
        TextureManager::enable_texture_generation();

        /* Ukljucujem teksturu */
        TextureManager::enable_texture("portal_rings");

        /* 
         * Crtanje zapocinjem od gornje tacke kruga, u kojoj se nalazi teme,
         * i rotiram oko X ose za izracunati ugao.
         * Ivice crtam u odnosu na njihov centar, pa je potrebno da prvi put
         * rotiram za polovinu rotacije kako bih pomerio pocetnu tacku sa
         * temena na centar ivice.
         */
        glRotatef(rotation/2, 1, 0, 0);

        for (int i = 0; i < sides; i++)
        {
            /* Staticne ivice */
            Drawable::draw_cuboid(
                pos_x, distance, 0, 
                0.1, 0.1, edge 
            );

            /* 
             * Spoljasnji efekti:
             * Crtam ivicu cija Z koordinata zavisi od vremena 
             */
            Drawable::draw_cuboid(
                pos_x, distance, sin_random * distance * 0.6, 
                0.1, 0.1, edge
            );

            /* 
             * Unutrasnji efekti: 
             * Crtam ivica cija Y koordinata zavisi od vremena
             * i cija dimenzija takodje zavisi od vremena 
             */
            Drawable::draw_cuboid(
                pos_x, cos_random * distance, 0,
                0.05 * sin_time, 0.05 * cos_time, edge * sin_time
            );

            /* Rotiram oko X ose i prelazim na crtanje sledece ivice */
            glRotatef(rotation, 1, 0, 0);
        }

        /* Gasim teksturu */
        TextureManager::disable_texture();

        /* Gasim generisanje teksturnih koordinata */
        TextureManager::disable_texture_generation();

    glPopMatrix();
}

void Floor::spawn_ball(void)
{
    /* Stvaranje loptice odradjuje red loptica */
    ball_row.spawn_ball();
}

void Floor::update(void)
{
    /* Azurira se red i njegova referentna tacka */
    ball_row.update();
    ball_row.set_parent(get_trans());
}

bool Floor::is_empty(void) const
{
    /* Sprat je prazan ukoliko je njegov red loptica prazan */
    return ball_row.is_empty();
}

bool Floor::is_full(void) const
{
    /* Sprat je pun ukoliko je njegov red loptica pun */
    return ball_row.is_full();
}

bool Floor::all_balls_inside(void) const
{
    /* 
     * Vraca TRUE ukoliko su sve loptice usle na sprat.
     * Pocinjem da razmisljam da li gresim, jer ove tri funkcije
     * samo pozivaju funkcije reda loptica 
     */
    return ball_row.all_balls_in_row();
}

void Floor::reset(void)
{
    /* Resetuje red loptica na pocetno stanje */
    ball_row.reset();
}

/* Geteri */
BallRow& Floor::get_ball_row(void) { return ball_row; }

Color Floor::get_color(void) const { return color; }
