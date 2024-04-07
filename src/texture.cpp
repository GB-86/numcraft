#include "texture.h"


const texture textureTable[] = {
    {{15430, 17543, 13221, 17511, 13253, 15398, 21801, 15334, 17511, 15366, 19624}, {0, 18, 19, 69, 101, 87, 112, 55, 8, 73, 162, 24, 112, 112, 113, 89, 115, 144, 164, 5, 84, 115, 160, 116, 67, 84, 112, 145, 84, 80, 163, 71},0.9,100},
{{27173, 33511, 41897, 29614, 18820, 19623, 17511, 15398, 13286, 13253, 15366, 15366}, {0, 17, 34, 0, 16, 3, 0, 18, 66, 16, 17, 32, 0, 16, 33, 1, 32, 36, 0, 48, 68, 69, 68, 4, 70, 87, 120, 73, 122, 107, 121, 88},1,100},
{{23013, 29550, 31399, 35921, 39785, 48172}, {82, 69, 37, 68, 36, 2, 36, 48, 82, 32, 82, 34, 33, 82, 64, 37, 66, 69, 4, 36, 32, 68, 36, 0, 82, 34, 50, 37, 34, 85, 68, 34},0.9,100},
{{27469, 29614, 33808, 38034}, {51, 35, 33, 18, 34, 18, 18, 34, 33, 0, 17, 16, 34, 51, 35, 34, 18, 33, 17, 33, 35, 34, 34, 34, 18, 35, 51, 19, 16, 1, 16, 17},0.9,100},
{{20965, 25157, 29383, 33543, 39848, 42026, 44106, 46219, 48300}, {16, 18, 16, 17, 7, 135, 120, 112, 23, 84, 52, 129, 22, 71, 133, 113, 24, 56, 117, 96, 7, 69, 68, 113, 23, 135, 118, 113, 16, 17, 2, 17},1,100},
{{14691, 20965, 25157, 29383, 37768, 39881}, {51, 19, 51, 3, 49, 34, 48, 82, 48, 34, 48, 82, 64, 50, 49, 50, 65, 50, 48, 50, 67, 19, 65, 50, 35, 19, 66, 19, 35, 3, 67, 20},0.85,100}

};

//block texture indexj,
//0 grass_block_top.png
//1 grass_block_side.png
//2 dirt.png
//3 stone.png
//4 oak_log_top.png
//5 oak_log_side.png
//6 sand.png
//7 gravel.png
//8 glass.png
//9 oak_leave.png
//10 crafting_table_top.png
//11 crafting_table_side_n.png
//12 crafting_table_side_e.png
//13 crafting_table_side_s.png
//14 crafting_table_side_w.png
//15 crafting_table_side_w.png
//16 oak_plank.png
//17 water_1.png
//18 water_2.png
//19 water_3.png
//20 iron_ore.png
//21 iron_block.png
//22 gold_ore.png
//23 gold_block.png
//24 diamond_ore.png
//25 diamond_block.png
//26 coal_ore.png
//27 coal_block.png
//28 obsidian.png
//29 grass.png 
//30 lava.png
//31 glow_stone.png 
//...
//63 glitch.png
//items texture index
//64 stick.png
//65 

const unsigned char blockFaceTexture[][6]={
    {1,1,0,2,1,1},
    {3,3,3,3,3,3},
    {5,5,4,4,5,5},
    {2,2,2,2,2,2}
};
// block id
// 1 grass
// 2 stone
// 3 log
// 4 dirt
// 5
// 6
// 7
// 8
// 9
// 10
unsigned short texture::getPixel(unsigned char u,unsigned char v) const
{
    // return the short value of the color corresponding to the pixel of coordinates (u,v) :)
    return colorPalette[(u&1) ? (pixelData[((u>>1)+4*v)]&0b00001111): (pixelData[((u>>1)+4*v)]>>4)];    
}

unsigned short getPixelFromTexture(unsigned char id,unsigned char u,unsigned char v,unsigned char face)
{
    
    // return the short value of the color corresponding to the pixel of coordinates (u,v) of the texture corresponding to the id :)
    if (u==7 || u==0 || v==7 || v==0)
    {

        double outlineS=textureTable[blockFaceTexture[id-1][face]].outlineStrenght;
        unsigned short col = textureTable[blockFaceTexture[id-1][face]].getPixel(u,v);
        unsigned short r,g,b;
        r=(unsigned short)((col>>11)*outlineS);
        g=(unsigned short)(((col>>5)& 0b111111)*outlineS);
        b=(unsigned short)((col& 0b11111)*outlineS);
        return ((r<<11) | (g<<5) | (b))&0b1111111111111111;
    }
    return textureTable[blockFaceTexture[id-1][face]].getPixel(u,v);
}