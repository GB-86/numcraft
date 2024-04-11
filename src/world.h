#ifndef WORLD_H
#define WORLD_H


struct chunk
{
    //chunk 16x16x16
    //subChunk 4x4x2
    chunk();
    chunk(signed char x,signed char z);
    signed char x,z;
    bool isGenerated=false;
    unsigned char data[4096]=""; 
    void generate(int seed);
    unsigned char getBlock(signed char *xyz) const;
    bool changeBlock(signed char *xyz,unsigned char blockId) ;
    unsigned long long subchunk[2];
    void updateSubChunk();
    bool isSubChunkEmpty(unsigned char n);

};
unsigned char getBlockFromWorld(signed char *xyz);
bool changeBlockInWorld(signed char *xyz,unsigned char blockId);

bool isSubChunkEmptyFromWorld();

void initWorld();
#endif
