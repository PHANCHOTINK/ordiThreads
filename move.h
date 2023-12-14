#ifndef __MOVE_H__
#define __MOVE_H__

#define X_AXIS_NONE  0x0
#define X_AXIS_LEFT  0x1
#define X_AXIS_RIGHT 0x2


#define Y_AXIS_NONE  0x00
#define Y_AXIS_LEFT  0x10
#define Y_AXIS_RIGHT 0x20
#include<stdint.h>

typedef struct cam_move{
    char left_move;
    char up_move;
} cam_move;
/*
cam_move u16_to_cam_move(uint16_t num){
    cam_move m;
    m.left_move=num & 0xF;
    m.up_move=(num & 0xF0)>>8;
    switch (m.left_move)
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        m.left_move=-1;
        break;
    
    default:
        break;
    } 
    switch (m.up_move)
    {
    case 0:
   
    case -1:
        val|=20;
        break;
    
    default:
        break;
    } 
    return val;
}
uint16_t cam_move_to_u16(cam_move move){
    uint16_t val=0;
    switch (move.left_move)
    {
    case 0:
        val|=0;
        break;
    case 1:
        val|=1;
        break;
    case -1:
        val|=2;
        break;
    
    default:
        break;
    } 
    switch (move.up_move)
    {
    case 0:
        val|=00;
        break;
    case 1:
        val|=10;
        break;
    case -1:
        val|=20;
        break;
    
    default:
        break;
    } 
    return val;
}
**/
#endif // __MOVE_H__