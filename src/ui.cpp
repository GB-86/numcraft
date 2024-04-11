#include "display.h"
#include "eadkpp.h"
#include "ui.h"
#include "texture.h"
#include "vector3.h"
void draw_inventory(unsigned char *selectedSlotPos, unsigned char *inventoryData)
{
    EADK::Display::pushRectUniform(EADK::Rect(54, 34, 212, 132), EADK::Color(0x555555));
    EADK::Display::pushRectUniform(EADK::Rect(56, 36, 208, 128), EADK::Color(0xc6c6c6));
    EADK::Display::pushRectUniform(EADK::Rect(60, 40, 200, 120), EADK::Color(0xFFFFFF));
    for (int y = 2; y >= 0; y--)
    {
        for (int x = 4; x >= 0; x--)
        {
            drawCase(x, y, *((inventoryData + (x + 5 * y) * 2)),*((inventoryData + (x + 5 * y) * 2+1)),false);
        }
    }
}
void drawCase(unsigned char slotx, unsigned char sloty, unsigned char id,unsigned char count,bool selected){
    if(selected){
        EADK::Display::pushRectUniform(EADK::Rect(60 + slotx * 40, 40 + sloty * 40, 32, 32), EADK::Color(0xFFFFFF));
    }
    else{
        EADK::Display::pushRectUniform(EADK::Rect(60 + slotx * 40, 40 + sloty * 40, 32, 32), EADK::Color(0x646464));
    }

    EADK::Display::pushRectUniform(EADK::Rect(64 + slotx * 40, 44 + sloty * 40, 32, 32), EADK::Color(0x8b8b8b));
    
    if(id!=0){
        drawItem(slotx,sloty,id);
    }
    
    if(count<10){
        char countChar[2]="1";
        EADK::Point p=EADK::Point(94+slotx*40,94+sloty*40);
        EADK::Display::drawString("a",
        p,true,EADK::Color(0b0),
        EADK::Color(0x00FFFFFF));
    }
    else if (count<=64)
    {
        //char countChar[3]={'0'+count/10,'0'+(count%10), 0};
        //EADK::Display::drawString(countChar,EADK::Point(70+slotx*40,94+sloty*40),false,EADK::Color(0b0),EADK::Color(0xFFFFFFFF));
    }
    
}
void drawItem(unsigned char slotx, unsigned char sloty, unsigned char id)
{
    if (id != 0)
    {
        for (unsigned char y = 0; y < 8; y++)
        {
            for (unsigned char x = 0; x < 8; x++)
            {
                unsigned short col = getPixelFromTexture(id, x, 7-y, 4);
                Vector3 color = {(double)((col >> 11) * 8), (double)(((col >> 5) & 0b0000000000111111) * 4), (double)((col & 0b0000000000011111) * 8)};
                EADK::Display::pushRectUniform(EADK::Rect(64 + (slotx) * 40+x * 4, 44 + (sloty) * 40+ y * 4, 4, 4), EADK::Color(color.toColor()));
            }
        }
    }
}