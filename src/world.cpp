#include "world.h"
#include "eadkpp.h"

chunk world[16];

chunk::chunk(): x(0), z(0) {}

chunk::chunk(signed char x,signed char z): x(x), z(z) {}

void initWorld() {
    for(signed char x=-2;x<2;x++){
        for(signed char y=-2;y<2;y++){
            world[(x+2)*+3*(y+2)]=chunk(x,y);
        }
    }
}

void chunk::generate(int seed){
    isGenerated=true;
    for(int y=0; y<16; y++){
        for(int z=0; z<16; z++){
            for(int x=0; x<16; x++){
                if(y<2){
                    data[x+z*16+y*256]=1+((x+z)&1);
                }
                else{
                    data[x+z*16+y*256]=0;
                }
            }
        }
        
    }
    
}

unsigned char chunk::getBlock( signed char *xyz) const{
    return data[(*xyz)+(*(xyz+2))*16+(*(xyz+1))*256];
}

bool chunk::changeBlock(signed char *xyz,unsigned char blockId) {
    data[(*xyz)+(*(xyz+2))*16+(*(xyz+1))*256]=blockId;
    return true;
}
unsigned char getBlockFromWorld(signed char *xyz){
    signed char chunkx=(*(xyz)+32)/16;
    signed char chunkz=(*(xyz+2)+32)/16;
    signed char localBlockPos[3]={(*xyz+32)-16*((*xyz+32)/16),(*(xyz+1)+32)-16*((*(xyz+1)+32)/16),(*(xyz+2)+32)-16*((*(xyz+2)+32)/16)};
    if (world[chunkx+chunkz*3].isGenerated){
        return (world[chunkx+chunkz*3].getBlock(&localBlockPos[0]));
    }
    else{
            world[chunkx+chunkz*3].generate(0);
            return (world[chunkx+chunkz*3].getBlock(&localBlockPos[0]));
    }   
}
bool changeBlockInWorld(signed char *xyz,unsigned char blockId){
    signed char chunkx=(*(xyz)+32)/16;
    signed char chunkz=(*(xyz+2)+32)/16;
    signed char localBlockPos[3]={(*xyz+32)-16*((*xyz+32)/16),(*(xyz+1)+32)-16*((*(xyz+1)+32)/16),(*(xyz+2)+32)-16*((*(xyz+2)+32)/16)};
    if (world[chunkx+chunkz*3].isGenerated){
        return (world[chunkx+chunkz*3].changeBlock(&localBlockPos[0],blockId));
    }
    else{   
            world[chunkx+chunkz*3].generate(0);
            return (world[chunkx+chunkz*3].changeBlock(&localBlockPos[0],blockId));
    }   
}

