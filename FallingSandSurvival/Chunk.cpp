
#include "Chunk.h"
#include <string>
#include <vector>
#include <sstream>
#include "UTime.h"

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#include "ProfilerConfig.h"

using namespace std;

std::vector<std::string> split(std::string strToSplit, char delimeter);
std::vector<std::string> string_split(std::string s, const char delimiter);
std::vector<std::string>
split2(std::string const& original, char separator);

Chunk::Chunk(int x, int y, char* worldName) {
    this->x = x;
    this->y = y;

    char* buff = new char[255];
    snprintf(buff, 255, "%s/chunks/chunk_%d_%d.txt", worldName, x, y);

    this->fname = buff;
}

Chunk::~Chunk() {
    //if(tiles) delete tiles;
    //if(layer2) delete layer2;
    //if(background) delete background;
    //if(biomes) delete biomes;
}

void Chunk::loadMeta() {
    string line;
    ifstream myfile(fname);
    if(myfile.is_open()) {
        getline(myfile, line, '\n');

        int phase = stoi(line);
        generationPhase = phase;
        hasMeta = true;
    }
}

MaterialInstanceData* Chunk::readBuf = (MaterialInstanceData*)malloc(CHUNK_W * CHUNK_H * 2 * sizeof(MaterialInstanceData));

void Chunk::read() {
    EASY_FUNCTION();

    EASY_BLOCK("create arrays");
    // use malloc here instead of new so it doesn't call the constructor
    MaterialInstance* tiles = (MaterialInstance*)malloc(CHUNK_W * CHUNK_H * sizeof(MaterialInstance));
    if(tiles == NULL) throw std::runtime_error("Failed to allocate memory for Chunk tiles array.");
    MaterialInstance* layer2 = (MaterialInstance*)malloc(CHUNK_W * CHUNK_H * sizeof(MaterialInstance));
    if(layer2 == NULL) throw std::runtime_error("Failed to allocate memory for Chunk layer2 array.");
    //MaterialInstance* tiles = new MaterialInstance[CHUNK_W * CHUNK_H];
    //MaterialInstance* layer2 = new MaterialInstance[CHUNK_W * CHUNK_H];
    Uint32* background = new Uint32[CHUNK_W * CHUNK_H];
    EASY_END_BLOCK;

    EASY_BLOCK("open file");
    string line;
    ifstream myfile(fname, std::ios::binary);
    EASY_END_BLOCK;
    if(myfile.is_open()) {
        int state = 0;

        getline(myfile, line, '\n');
        int phase = stoi(line);
        this->generationPhase = phase;

        hasMeta = true;
        state = 1;

        /*unsigned int content;
        for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
            myfile.read((char*)&content, sizeof(unsigned int));
            int id = content;
            myfile.read((char*)&content, sizeof(unsigned int));
            Uint32 color = content;
            tiles[i] = MaterialInstance(Materials::MATERIALS[id], color);
        }
        for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
            myfile.read((char*)&content, sizeof(unsigned int));
            int id = content;
            myfile.read((char*)&content, sizeof(unsigned int));
            Uint32 color = content;
            layer2[i] = MaterialInstance(Materials::MATERIALS[id], color);
        }*/
        /*for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
            myfile.read((char*)&content, sizeof(unsigned int));
            background[i] = content;
        }*/

        EASY_BLOCK("read MaterialInstanceData");
        myfile.read((char*)readBuf, CHUNK_W * CHUNK_H * 2 * sizeof(MaterialInstanceData));
        EASY_END_BLOCK;

        EASY_BLOCK("copy MaterialInstanceData");
        // copy everything but the material pointer
        memcpy(tiles, readBuf, CHUNK_W * CHUNK_H * sizeof(MaterialInstance));
        memcpy(layer2, &readBuf[CHUNK_W * CHUNK_H], CHUNK_W * CHUNK_H * sizeof(MaterialInstance));

        // copy the material pointer
        for(int i = 0; i < CHUNK_W * CHUNK_H; i++) {
            // twice as fast to set fields instead of making new ones

            tiles[i].mat = Materials::MATERIALS_ARRAY[readBuf[i].index];
            //tiles[i].color = readBuf[i].color;
            //tiles[i].temperature = readBuf[i].temperature;
            //tiles[i] = MaterialInstance(Materials::MATERIALS_ARRAY[buf[i].index], buf[i].color, buf[i].temperature);

            layer2[i].mat = Materials::MATERIALS_ARRAY[readBuf[CHUNK_W * CHUNK_H + i].index];
            //layer2[i].color = readBuf[CHUNK_W * CHUNK_H + i].color;
            //layer2[i].temperature = readBuf[CHUNK_W * CHUNK_H + i].temperature;
            //layer2[i] = MaterialInstance(Materials::MATERIALS_ARRAY[buf[CHUNK_W * CHUNK_H + i].index], buf[CHUNK_W * CHUNK_H + i].color, buf[CHUNK_W * CHUNK_H + i].temperature);
        }
        EASY_END_BLOCK;

        //delete readBuf;

        EASY_BLOCK("read background data");
        myfile.read((char*)background, CHUNK_W * CHUNK_H * sizeof(unsigned int));
        EASY_END_BLOCK;

        myfile.close();
    }

    this->tiles = tiles;
    this->layer2 = layer2;
    this->background = background;
    hasTileCache = true;
}

void Chunk::write(MaterialInstance* tiles, MaterialInstance* layer2, Uint32* background) {
    EASY_FUNCTION();

    this->tiles = tiles;
    this->layer2 = layer2;
    this->background = background;
    hasTileCache = true;

    ofstream myfile;
    myfile.open(fname, std::ios::binary);
    myfile << generationPhase << "\n";

    // TODO: make these loops faster
    /*for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
        myfile.write((char*)&tiles[i].mat.id, sizeof(unsigned int));
        myfile.write((char*)&tiles[i].color, sizeof(unsigned int));
    }
    for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
        myfile.write((char*)&layer2[i].mat.id, sizeof(unsigned int));
        myfile.write((char*)&layer2[i].color, sizeof(unsigned int));
    }*/
    /*for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
        myfile.write((char*)&background[i], sizeof(unsigned int));
    }*/


    MaterialInstanceData* buf = new MaterialInstanceData[CHUNK_W * CHUNK_H * 2];
    for(int i = 0; i < CHUNK_W * CHUNK_H; i++) {
        buf[i] ={(Uint32)tiles[i].mat->id, tiles[i].color, tiles[i].temperature};
        buf[CHUNK_W * CHUNK_H + i] ={(Uint32)layer2[i].mat->id, layer2[i].color, layer2[i].temperature};
    }

    myfile.write((char*)buf, CHUNK_W * CHUNK_H * 2 * sizeof(MaterialInstanceData));
    delete[] buf;
    myfile.write((char*)background, CHUNK_W * CHUNK_H * sizeof(unsigned int));

    myfile.close();
}

bool Chunk::hasFile() {
    EASY_FUNCTION();
    struct stat buffer;
    return (stat(fname, &buffer) == 0);
}

std::vector<std::string> split(std::string strToSplit, char delimeter) {
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while(getline(ss, item, delimeter)) {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

std::vector<std::string> string_split(std::string s, const char delimiter) {
    size_t start = 0;
    size_t end = s.find_first_of(delimiter);

    std::vector<std::string> output;

    while(end <= std::string::npos) {
        output.emplace_back(s.substr(start, end - start));

        if(end == std::string::npos)
            break;

        start = end + 1;
        end = s.find_first_of(delimiter, start);
    }

    return output;
}

std::vector<std::string>
split2(std::string const& original, char separator) {
    std::vector<std::string> results;
    std::string::const_iterator start = original.begin();
    std::string::const_iterator end = original.end();
    std::string::const_iterator next = std::find(start, end, separator);
    while(next != end) {
        results.push_back(std::string(start, next));
        start = next + 1;
        next = std::find(start, end, separator);
    }
    results.push_back(std::string(start, next));
    return results;
}
