#include <LocalBuffers.h>


LocalBuffers::LocalBuffers(){

}

static char LocalBuffers::float2char_buffer1[6];
static char LocalBuffers::float2char_buffer2[6];
static char LocalBuffers::string_cpy_buffer[MAX_SIZE_ALLOWED_PROGMEM_STRING];
static char LocalBuffers::client_request_buffer[MAX_SIZE_ALLOWED_REQUEST];
