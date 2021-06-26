#ifndef __CLI_H
#define __CLI_H

/* includes ---------------------------------------------------------------- */
#include "USB.h"

/* defines ----------------------------------------------------------------- */
#define CLI_INIT 0
#define CLI_IDLE 1

#define CLI_BUFFER_SIZE 256

typedef struct
{
	uint8_t name[64];
	uint8_t (*f)(int argc, char **argv);
}T_COMMAND;

/* class ------------------------------------------------------------------- */
class CLI
{
	private:
		
		USB_VCP* m_serial;
		uint8_t m_state;
	
		uint8_t m_buffer[CLI_BUFFER_SIZE];
		uint16_t m_length;
	
		T_COMMAND* m_cmdTable;
		uint8_t m_cmdCount;
	
		uint8_t execute(uint8_t* buffer, uint8_t length);
	
	public:
		
		CLI(USB_VCP* serialx, T_COMMAND* table, uint8_t count);
		void update(void);
};

#endif
