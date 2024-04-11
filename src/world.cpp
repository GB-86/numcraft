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
    //updateSubChunk();
    
}
void chunk::updateSubChunk(){
    for(int y=0; y<8; y++){
        for(int z=0; z<4; z++){
            for(int x=0; x<4; x++){
                bool isSubChunkEmpty_=true;
                for(int y_=0;y_<2 && isSubChunkEmpty_; y_++){
                    for(int z_=0; z_<4 && isSubChunkEmpty_; z_++){
                        for(int x_=0; x_<4 && isSubChunkEmpty_; x_++){
                            signed char blockPos[3]={x*4+x_,y*2+y_,z*4+z_};
                            if(getBlock(&blockPos[0])!=0){
                                isSubChunkEmpty_=false;
                            }
                        }

                    }
                }
                if(isSubChunkEmpty_){
                    subchunk[y>4]=(~(1<<x+(y/2)*8+z*4))|subchunk[y>4];
                }
                else{
                    subchunk[y>4]=(1<<x+(y/2)*8+z*4)|subchunk[y>4];
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
bool chunk::isSubChunkEmpty(unsigned char n){
    return 1 == ( (subchunk[n>64] >> n) & 1);
}
bool isSubChunkEmptyFromWorld(signed char * subChunckPos){
    signed char chunkx=(*(subChunckPos)+8)/8;
    signed char chunkz=(*(subChunckPos+2)+8)/8;
    return world[chunkx+chunkz*3].isSubChunkEmpty(0);
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
            (world[chunkx+chunkz*3].changeBlock(&localBlockPos[0],blockId));
            world[chunkx+chunkz*3].updateSubChunk();
    }   
}

