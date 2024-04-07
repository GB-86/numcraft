#ifndef WORLD_H
#define WORLD_H


struct chunk
{
    chunk();
    chunk(signed char x,signed char z);
    signed char x,z;
    bool isGenerated=false;
    unsigned char data[4096]=""; 
    void generate(int seed);
    unsigned char getBlock(signed char *xyz) const;
    bool changeBlock(signed char *xyz,unsigned char blockId) ;

};
unsigned char getBlockFromWorld(signed char *xyz);
bool changeBlockInWorld(signed char *xyz,unsigned char blockId);
void initWorld();
#endif
