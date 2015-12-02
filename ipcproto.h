/* 
IPC protocol definiton for the the PTMain system and the QT touchscreen application
*/
/*
Data transmission
first 4 bytes BATT data
next 4 bytes SPEED data
*/
#ifndef IPCPROTO_H
#define IPCPROTO_H

#define PT_HANDSHAKE 0
#define PT_REQUEST_BATTERY_INFO 1
#define PT_REQUEST_SPEED 2
#define PT_REQUEST_ALL 3

#define PT_ACK 10
#define PT_NACK 11
#define PT_RESPONSE 12 //note: A response is followed by data and data is then ended with a 0 byte

#endif
