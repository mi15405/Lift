#include <iostream>
#include <vector>
#include "textureManager.h"

std::map<std::string, GLuint> TextureManager::textures;
std::map<std::string, Image*> TextureManager::images;

void TextureManager::insert_image(std::string image_name, const char* filename)
{
    /* Kreira se slika u dinamicki alociranoj memoriji */
    Image* image = new Image;

    /* Ucitavaju se podaci iz fajla */
    image->read(filename);

    /* Ubacuje se slika u mapu svih slika. Kljuc je prvi argument */
    images.insert(std::pair<std::string, Image*> (image_name, image));
}

Image* TextureManager::find_image(std::string image_name) 
{
    /* Trazi sliku sa prosledjenim kljucem */
    auto search = images.find(image_name);

    /* Ukoliko je pronadje vraca je kao povratnu vrednost */
    if (search != images.end())
        return search->second;
    else
    {
        /* Inace vraca nullptr i ispisuje gresku */
        std::cerr << "Image " << image_name << " not found." << std::endl;
        return nullptr;
    }
}

void TextureManager::clear_images(void)
{
    /* Brise sve slike iz mape */
    auto it = images.begin();

    while (it != images.end())
    {
        Image* img = it->second;

        /* Oslobadja se alocirana memorija */
        delete img;

        /* izbacuje se slika iz mape */
        it = images.erase(it);
    }
}

bool TextureManager::has_texture(std::string name) 
{
    /* Proverava postoji li tekstura sa datim kljucem */
    return textures.find(name) != textures.end();
}

void TextureManager::set_mode(GLenum mode)
{
    /* Ukljucuje prosledjeni mod za nacin odredjivanja vrednosti piksela */
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
}

void TextureManager::enable_texture(std::string name) 
{
    /* Ukljucuje teksturu ukoliko je pronadje u mapi, inace javlja gresku */
    auto search = textures.find(name);
    if (search != textures.end())
    {
        glBindTexture(GL_TEXTURE_2D, search->second);
    }
    else
    {
        std::cerr << "Texture " << name << " not found.";
        return;
    }
}

void TextureManager::disable_texture(void) 
{
    /* Iskljucuje teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureManager::generate_texture(std::string name)
{
    GLuint id;
    /* Generise jednu teksturu */
    glGenTextures(1, &id);

    /* Ucitava generisanu teksturu u mapu */
    textures.insert(std::pair<std::string, GLuint> (name, id));
}

void TextureManager::generate_textures(std::vector<std::string> names)
{
    unsigned int size = names.size();

    /* Alocira se memorija za ucitavanje tekstura */
    GLuint* id = new GLuint[size];

    /* Ucitavaju se teksture */
    glGenTextures(size, id);

    /* Ucitane teksture se upisuju u mapu sa prosledjenim kljucevima */
    for (unsigned int i = 0; i < size; i++)
        textures.insert(std::pair<std::string, GLuint> (names[i], id[i]));

    /* Oslobadja se alocirana memorija */
    delete[] id;
}

void TextureManager::set_texture(
    std::string name, 
    std::string image_name, 
    GLenum repeat, 
    GLenum filter) 
{
    /* ukoliko tekstura ne postoji u mapi, generise se i ubacuje u mapu */
    if (!has_texture(name))
        generate_texture(name);

    /* Ukljucuje se tekstura */
    enable_texture(name);

    /* 
     * Postavljaju se prosledjeni parametri.
     * "repeat" oznacava da li se tekstura ponavlja ili razvlaci
     * "filter" odredjuje nacin izracunavanja vrednosti piksela cija se
     * informacija izgubi nakon skaliranja slike
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

    /* Trazi se slika u mapi svih slika, sa prosledjenim kljucem */
    Image* image = find_image(image_name);

    /* Ukoliko je slika pronadjena, ucitava se u teksturu */
    if (image != nullptr)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                image->get_width(), image->get_height(), 0,
                GL_RGB, GL_UNSIGNED_BYTE, image->get_pixels());
    }

    /* Gasi se koriscena tekstura */
    disable_texture();
}

void TextureManager::enable_texture_generation(void)
{
    /* 
     * Ukljucuje se mod generisanja teksturnih koordinata, koji ubrzava proces
     * mapiranja i dodaje zanimljive efekte objektima koji se krecu, jer
     * mapiranje na ovaj nacin zavisi od pozicije objekta (nagadjam).
     * Svakako deluje zanimljivo.
     */
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
}

void TextureManager::disable_texture_generation(void)
{
    /* Gasi se generacija teksturnih koordinata */
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
}

