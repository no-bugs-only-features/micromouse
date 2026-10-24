// this is the file that contains the udp packet structure both languages will use (C and C++)
//

#ifndef PACKETSTRUCTURE_H
#define PACKETSTRUCTURE_H

#include <stdint.h>

#pragma pack(push, 1)

// data we want to send to the display
typedef struct {
  float x_pos;
  float y_pos;
  uint8_t maze[256];

  //other data we want to send to display goes here

} DataPacket;


// data we want to send to the robot
typedef struct {
  uint8_t command_type; // possible commands: get/set/reset/stop/change mode: explore, solve, manual override
  float kp;
  float ki;
  float kd;
} CommandPacket;


#pragma pack(pop)


#endif

