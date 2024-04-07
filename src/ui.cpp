#include "display.h"
#include "eadkpp.h"
#include "ui.h"
void draw_inventory(unsigned char * selectedSlotPos){
    EADK::Display::pushRectUniform(EADK::Rect(54,34,212,132), EADK::Color(0x555555));
    EADK::Display::pushRectUniform(EADK::Rect(56,36,208,128), EADK::Color(0xc6c6c6));
    EADK::Display::pushRectUniform(EADK::Rect(60,40,200,120), EADK::Color(0xFFFFFF));
    for(int y=2;y>=0; y--){
        for(int x=4;x>=0; x--){
            EADK::Display::pushRectUniform(EADK::Rect(64+x*40,44+y*40,32,32),EADK::Color(0x8b8b8b));
        }
    }
}
void draw_selectedInventorySlot(bool undraw,unsigned char * selectedSlotPos){
    if (undraw){
        EADK::Display::pushRectUniform(EADK::Rect(60+(*selectedSlotPos)*40,40+(*(selectedSlotPos+1))*40,40,40),EADK::Color(0x646464));
        EADK::Display::pushRectUniform(EADK::Rect(64+(*selectedSlotPos)*40,44+(*(selectedSlotPos+1))*40,32,32),EADK::Color(0x8b8b8b));
    }
    else
    {
        EADK::Display::pushRectUniform(EADK::Rect(60+(*selectedSlotPos)*40,40+(*(selectedSlotPos+1))*40,40,40),EADK::Color(0xFFFFFF));
        EADK::Display::pushRectUniform(EADK::Rect(64+(*selectedSlotPos)*40,44+(*(selectedSlotPos+1))*40,32,32),EADK::Color(0x8b8b8b));
    }
}
/*
def drawHotbar():
    fill_rect(60,180,200,40,(100,100,100))
    fill_rect(60+selected*40,180,40,40,(200,200,200))
    for i in range(5):
        fill_rect(64+i*40,184,32,32,(150,150,150))
        if inv[0][i][0]!=0:
            drawItem(64+i*40,184,inv[0][i][0])
    for i in range(5):
        if inv[0][i][1]>1 and inv[0][i][0]!=0:
            draw_string(str(inv[0][i][1]),84+i*40,204,(255,255,255),(100,100,100))
def drawInventory():

    for i in range(3):
        for j in range(5):
            if (j,i)==invSelect: 
                fill_rect(60+j*40,40+i*40,40,40,(100,100,100))
            fill_rect(64+j*40,44+i*40,32,32,(139,139,139))
            drawItem(64+j*40,44+i*40,getItem((j,2-i)))
*/