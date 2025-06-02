#ifndef CORE_CONFIG_HEADER
#define CORE_CONFIG_HEADER


// blitter
#define FRAME_RESOLUTION_X 1600
#define FRAME_RESOLUTION_Y 900
#define FRAME_TIME_BUDGET_MS 15.

// renderer
#define RENDERER_SPRITE_MEMORY_WIDTH 1500
#define RENDERER_SPRITE_MEMORY_HEIGHT 1500
#define RENDERER_FONT_MEMORY_WIDTH 1500
#define RENDERER_FONT_MEMORY_HEIGHT 1500
#define RENDERER_MAXIMUM_FONT_COUNT 2

// network
#define NETWORK_HOST "ec2-3-66-164-207.eu-central-1.compute.amazonaws.com"
#define NETWORK_PORT_ADAPTER "8080"
#define NETWORK_PORT_WEBSOCKET "8083"

//#define FEAT_MULTIPLAYER 1

// TODO define in extern config file and store constants at runtime


#endif
