#ifndef THERMGLOBALS_H

#define THERMGLOBALS_H

#define LOG_ENABLED true

#define LORA_RFM9X_CS 10
#define LORA_RFM9X_INT 2
#define LORA_RFM9X_RST 4
#define LORA_RFM9X_FREQ 915.0

//#define LORA_RFM9X_MAX_MESSAGE_LEN 64
#define LORA_RFM9X_MAX_MESSAGE_LEN 128
#define LORA_MAX_CHUNK_SIZE 100 // leave 28 bytes for metadata
#define LORA_MAX_CHUNK_IN_BUFFER_SIZE 1024 // max size allowed for any chunk bunch
#define LORA_MAX_CHUNK_TIME_MILLIS 8000 // max time to wait for all chunks to arrive

#define LORA_CHUNK_START_KEY "CHUNKSTART"
#define LORA_CHUNK_END_KEY "CHUNKEND:"
#define LORA_ACK_TIMEOUT 1000

#define LORA_ADDR_SELF 1
#define LORA_ADDR_REMOTE 0
#define LORA_ADDR_SPOTTER 1
#define LORA_ADDR_ACTOR 2
#define LORA_ADDR_BROADCAST 255

#define THERMAL_INTERPOLATE_BUFFER_SIZE 192
#define THERMAL_ENCODE_BUFFER_SIZE 768

// 1331 pins: sckl (SCL); mosi (SDA); cs(CS); rst (RES); dc (DC)
#define Display_1331_SCLK 6
#define Display_1331_MOSI 5
#define Display_1331_CS   8
#define Display_1331_RST  9
#define Display_1331_DC   7

#define THERMAL_WIDTH 32
#define THERMAL_HEIGHT 24

#define THERMAL_INTERPOLATED_WIDTH 64
#define THERMAL_INTERPOLATED_HEIGHT 48

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

#endif