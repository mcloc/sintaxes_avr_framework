#include <LocalBuffers.h>


LocalBuffers::LocalBuffers(){

}

char LocalBuffers::float2char_buffer1[6];
char LocalBuffers::float2char_buffer2[6];
char LocalBuffers::string_cpy_buffer[MAX_SIZE_ALLOWED_PROGMEM_STRING];
char LocalBuffers::client_request_buffer[MAX_SIZE_ALLOWED_REQUEST];
