
#include "Chunk.hpp"
#include <string>
#include <vector>
#include <sstream>
#include "UTime.hpp"

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#include "ProfilerConfig.hpp"

#include <lz4.h>

std::vector<std::string> split(std::string strToSplit, char delimeter);
std::vector<std::string> string_split(std::string s, const char delimiter);
std::vector<std::string>
split2(std::string const& original, char separator);

Chunk::Chunk(int x, int y, char* worldName) {
    this->x = x;
    this->y = y;

    this->fname = std::string(worldName) + "/chunks/chunk_" + std::to_string(x) + "_" + std::to_string(y);
}

Chunk::~Chunk() {
    if(tiles) delete[] tiles;
    if(layer2) delete[] layer2;
    if(background) delete[] background;
    if(biomes) delete[] biomes;
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

//MaterialInstanceData* Chunk::readBuf = (MaterialInstanceData*)malloc(CHUNK_W * CHUNK_H * 2 * sizeof(MaterialInstanceData));

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

        myfile.read((char*)&this->generationPhase, sizeof(int8_t));

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

        int src_size;
        myfile.read((char*)&src_size, sizeof(int));

        if(src_size != CHUNK_W * CHUNK_H * 2 * sizeof(MaterialInstanceData)) throw std::runtime_error("Chunk src_size was different from expected: " + std::to_string(src_size) + " vs " + std::to_string(CHUNK_W * CHUNK_H * 2 * sizeof(MaterialInstanceData)));

        int compressed_size;
        myfile.read((char*)&compressed_size, sizeof(int));

        int src_size2;
        myfile.read((char*)&src_size2, sizeof(int));
        int desSize = CHUNK_W * CHUNK_H * sizeof(unsigned int);

        if(src_size2 != desSize) throw std::runtime_error("Chunk src_size2 was different from expected: " + std::to_string(src_size2) + " vs " + std::to_string(desSize));

        int compressed_size2;
        myfile.read((char*)&compressed_size2, sizeof(int));

        MaterialInstanceData* readBuf = (MaterialInstanceData*)malloc(src_size);

        if(readBuf == NULL) throw std::runtime_error("Failed to allocate memory for Chunk readBuf.");

        char* compressed_data = (char*)malloc(compressed_size);

        EASY_BLOCK("read MaterialInstanceData");
        myfile.read((char*)compressed_data, compressed_size);
        EASY_END_BLOCK;

        const int decompressed_size = LZ4_decompress_safe(compressed_data, (char*)readBuf, compressed_size, src_size);

        free(compressed_data);

        // basically, if either of these checks trigger, the chunk is unreadable, either due to miswriting it or corruption
        // TODO: have the chunk regenerate on corruption (maybe save copies of corrupt chunks as well?)
        if(decompressed_size < 0) {
            logCritical("Error decompressing chunk tile data @ {},{} (err {}).", this->x, this->y, decompressed_size);
        } else if(decompressed_size != src_size) {
            logCritical("Decompressed chunk tile data is corrupt! @ {},{} (was {}, expected {}).", this->x, this->y, decompressed_size, src_size);
        }

        EASY_BLOCK("copy MaterialInstanceData");
        // copy everything but the material pointer
        //memcpy(tiles, readBuf, CHUNK_W * CHUNK_H * sizeof(MaterialInstance));
        //memcpy(layer2, &readBuf[CHUNK_W * CHUNK_H], CHUNK_W * CHUNK_H * sizeof(MaterialInstance));

        // copy the material pointer
        for(int i = 0; i < CHUNK_W * CHUNK_H; i++) {
            // twice as fast to set fields instead of making new ones
            tiles[i].color = readBuf[i].color;
            tiles[i].temperature = readBuf[i].temperature;
            tiles[i].mat = Materials::MATERIALS_ARRAY[readBuf[i].index];
            tiles[i].id = MaterialInstance::_curID++;
            //tiles[i].color = readBuf[i].color;
            //tiles[i].temperature = readBuf[i].temperature;
            //tiles[i] = MaterialInstance(Materials::MATERIALS_ARRAY[buf[i].index], buf[i].color, buf[i].temperature);

            layer2[i].color = readBuf[i + CHUNK_W * CHUNK_H].color;
            layer2[i].temperature = readBuf[i + CHUNK_W * CHUNK_H].temperature;
            layer2[i].mat = Materials::MATERIALS_ARRAY[readBuf[CHUNK_W * CHUNK_H + i].index];
            layer2[i].id = MaterialInstance::_curID++;
            //layer2[i].color = readBuf[CHUNK_W * CHUNK_H + i].color;
            //layer2[i].temperature = readBuf[CHUNK_W * CHUNK_H + i].temperature;
            //layer2[i] = MaterialInstance(Materials::MATERIALS_ARRAY[buf[CHUNK_W * CHUNK_H + i].index], buf[CHUNK_W * CHUNK_H + i].color, buf[CHUNK_W * CHUNK_H + i].temperature);
        }
        EASY_END_BLOCK;

        //delete readBuf;

        char* compressed_data2 = (char*)malloc(compressed_size2);

        EASY_BLOCK("read background data");
        myfile.read((char*)compressed_data2, compressed_size2);
        EASY_END_BLOCK;

        const int decompressed_size2 = LZ4_decompress_safe(compressed_data2, (char*)background, compressed_size2, src_size2);

        free(compressed_data2);

        if(decompressed_size2 < 0) {
            logCritical("Error decompressing chunk background data @ {},{} (err {}).", this->x, this->y, decompressed_size2);
        }else if(decompressed_size2 != src_size2) {
            logCritical("Decompressed chunk background data is corrupt! @ {},{} (was {}, expected {}).", this->x, this->y, decompressed_size2, src_size2);
        }

        free(readBuf);

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
    if(this->tiles == NULL || this->layer2 == NULL || this->background == NULL) return;
    hasTileCache = true;

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
        buf[i] = {(Uint16)tiles[i].mat->id, tiles[i].color, tiles[i].temperature};
        buf[CHUNK_W * CHUNK_H + i] = {(Uint16)layer2[i].mat->id, layer2[i].color, layer2[i].temperature};
    }

    const char* const src = (char*)buf;
    const int src_size = (int)(CHUNK_W * CHUNK_H * 2 * sizeof(MaterialInstanceData));
    const int max_dst_size = LZ4_compressBound(src_size);

    char* compressed_data = (char*)malloc((size_t)max_dst_size);

    EASY_BLOCK("compress");
    const int compressed_data_size = LZ4_compress_fast(src, compressed_data, src_size, max_dst_size, 10);
    EASY_END_BLOCK;

    if(compressed_data_size <= 0) {
        logCritical("Failed to compress chunk tile data @ {},{} (err {})", this->x, this->y, compressed_data_size);
    }

    /*if(compressed_data_size > 0){
        logDebug("Compression ratio: {}", (float)compressed_data_size / src_size * 100);
    }*/

    char* n_compressed_data = (char*)realloc(compressed_data, (size_t)compressed_data_size);
    if(n_compressed_data == NULL) throw std::runtime_error("Failed to realloc memory for Chunk::write compressed_data.");
    compressed_data = n_compressed_data;

    // bg compress

    const char* const src2 = (char*)background;
    const int src_size2 = (int)(CHUNK_W * CHUNK_H * sizeof(unsigned int));
    const int max_dst_size2 = LZ4_compressBound(src_size2);

    char* compressed_data2 = (char*)malloc((size_t)max_dst_size2);

    EASY_BLOCK("compress");
    const int compressed_data_size2 = LZ4_compress_fast(src2, compressed_data2, src_size2, max_dst_size2, 10);
    EASY_END_BLOCK;

    if(compressed_data_size2 <= 0) {
        logCritical("Failed to compress chunk tile data @ {},{} (err {})", this->x, this->y, compressed_data_size2);
    }

    /*if(compressed_data_size2 > 0){
        logDebug("Compression ratio: {}", (float)compressed_data_size2 / src_size2 * 100);
    }*/

    char* n_compressed_data2 = (char*)realloc(compressed_data2, (size_t)compressed_data_size2);
    if(n_compressed_data2 == NULL) throw std::runtime_error("Failed to realloc memory for Chunk::write compressed_data2.");
    compressed_data2 = n_compressed_data2;

    ofstream myfile;
    myfile.open(fname, std::ios::binary);
    myfile.write((char*)&generationPhase, sizeof(int8_t));

    myfile.write((char*)&src_size, sizeof(int));
    myfile.write((char*)&compressed_data_size, sizeof(int));
    myfile.write((char*)&src_size2, sizeof(int));
    myfile.write((char*)&compressed_data_size2, sizeof(int));

    myfile.write((char*)compressed_data, compressed_data_size);
    delete[] buf;
    myfile.write((char*)compressed_data2, compressed_data_size2);

    free(compressed_data);
    free(compressed_data2);

    myfile.close();
}

bool Chunk::hasFile() {
    EASY_FUNCTION();
    struct stat buffer;
    return (stat(fname.c_str(), &buffer) == 0);
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
