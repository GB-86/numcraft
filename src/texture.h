#ifndef TEXTURE_H
#define TEXTURE_H


struct texture
{
    unsigned short colorPalette[16];
    unsigned char pixelData[32];
    double outlineStrenght;
    unsigned char transparency;
    unsigned long alphaMap;
    unsigned short getPixel(unsigned char u,unsigned char v) const;
};
//00000 000  00 000000

unsigned short getPixelFromTexture(unsigned char id,unsigned char u,unsigned char v,unsigned char face);
/*
0 air
1 grass
2 stone
3 dirt
4 log
5 glass
6 sand
*/

#endif
