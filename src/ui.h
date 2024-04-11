
#ifndef UI_H
#define UI_H

void draw_inventory(unsigned char * selectedSlotPos,unsigned char * inventoryData);
void drawCase(unsigned char slotx, unsigned char sloty, unsigned char id,unsigned char count,bool selected);
void drawItem(unsigned char slotx,unsigned char sloty,unsigned char id);
#endif 