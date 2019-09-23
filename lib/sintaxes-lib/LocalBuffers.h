#ifndef __SINTAX_LOCAL_BUFFERS_H_
#define __SINTAX_LOCAL_BUFFERS_H_

#include <sintaxes-framework-defines.h>

class LocalBuffers;

class LocalBuffers{
    public:
		LocalBuffers();

        /**
         * This is used by dtostrf() to convert float numbers into string
         * TODO: check if the string in PROGMEM accept %f so we dont need to use
         * we have 2 buffer for cases on which we do sprintf_P(str, buffer1, buffer2), so it's for when we have 2 sprintf_P() arguments
         * on the PROGMEM String variable;
         *
         * Remember this is for use bouth buffer in the same sprintf_P() statement, you will need to reuse it for others  write(FSH()
         * stament. Fof example, multiple sensors goes one by one so you only need 2 buffers at the same time. One for temperature, other
         * for humidity.
         *
         * FIXME: split sensors message segment into smal pieaces. As more  write(FSH() we do, as more little memory we'll need. One spit
         * at a time to client.
         *
         */
        static char float2char_buffer1[6];
        static char float2char_buffer2[6];

        /**
         * This is the buffer used on all sprintf_P for the PROGMEM String,
         * increase as the String is longer than this value or split into several other PROGMEM variables
         * this should keep low then 180 bytes
         */
        static char string_cpy_buffer[MAX_SIZE_ALLOWED_PROGMEM_STRING];


        static char client_request_buffer[MAX_SIZE_ALLOWED_REQUEST];



};


#endif
