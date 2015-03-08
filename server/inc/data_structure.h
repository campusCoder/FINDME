#ifndef _datastructure
#define _datastructure

#define MAX_LINE_LENGTH 255
#define MAX_TYPE_LENGTH 10
#define MAX_MSG_LEN 1024
#define DELIMITER "$"
#define REPLY "REPLY"

/*Bit masks for encode and decoding*/
/*Fourth byte for actual value types*/
#define NAME         0x00000001
#define EMAIL        0x00000002
#define ADDRESS      0x00000004
#define LOCATION     0x00000008
#define TIMESTAMP    0x00000010
/*second byte for Error type*/
#define NOTYPE       0x00010000
#define NOREC        0x00020000
/*First byte for client/server identification*/
#define CLIENT       0x01000000
#define SERVER       0x02000000
#define UPDATE       0x04000000
#define NEW          0x08000000

typedef int int32;
typedef unsigned int uint32;
typedef char int8;
typedef unsigned char  uint8;
typedef short int int16;
typedef unsigned short int uint16;
typedef unsigned long int uint64;


/*Data structure to keep the contents of a
record*/
typedef struct _stRecord
{
uint64 ui64RecNum;
uint64 ui64LastUpdate;
uint32 ui32ContentFull;
int8 achName[MAX_LINE_LENGTH];
int8 achEmail[MAX_LINE_LENGTH];
int8 achAddr[MAX_LINE_LENGTH];
int8 achLastKnownLoc[MAX_LINE_LENGTH]; 
}stRecord;


/*Data structure to form message for the message queue*/

typedef struct _stRcvdMsg
{
struct sockaddr *strcvd_addr;
int8 *achBuffer;
}stRcvdMsg;

typedef enum _msgtypes
{
TNAME,
TEMAIL,
TADDR,
TLOCT,
TNOREC,
TNOTYPE
}eMsgType;
#endif