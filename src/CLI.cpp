#include "CLI.h"

#include <stdio.h>
#include <string.h>

const uint8_t welcome[64] = "\rCommand line interface 1.0\n\n> ";
const uint8_t unknown[64] = "\n\rCommand unknown!";

CLI :: CLI(USB_VCP* serialx, T_COMMAND* table, uint8_t count)
{
	m_serial = serialx;
	m_state = CLI_INIT;
	
	m_cmdTable = table;
	m_cmdCount = count;
	
	memset(&m_buffer, 0x00, CLI_BUFFER_SIZE);
	m_length = 0;
}

void CLI :: update(void)
{
	uint8_t buffer[CLI_BUFFER_SIZE] = {};
	uint16_t length = 0;
	uint8_t i = 0;
	uint8_t result = 0;
		
	switch(m_state)
	{
		case CLI_INIT:
			
			length = m_serial->read(buffer);
		
			if(length > 0)
			{
				m_serial->write((uint8_t*)welcome, strlen((const char*)welcome));
		
				m_state = CLI_IDLE;
			}
			break;
		
		case CLI_IDLE:
			
			length = m_serial->read(buffer);
		
			if(length > 0)
			{
				switch(buffer[0])
				{
					// Carrier return or new line
					case '\r':
					case '\n':
						if(m_length > 0)
						{
							// Process the command
							result = this->execute(m_buffer, m_length);
					
							if(result != 0)
							{
								m_serial->write((uint8_t*)"\n\rOK", 4);
							}
							else
							{
								m_serial->write((uint8_t*)"\n\rNOK", 5);
							}
						}
						
						m_serial->write((uint8_t*)"\n\r> ", 4);
					
						// Clear variables
						memset(&m_buffer, 0x00, CLI_BUFFER_SIZE);
						m_length = 0;
						break;
					
					// Backspace
					case '\b':
						if(m_length > 0)
						{
							m_length--;
							m_buffer[m_length] = 0x00;
						}
						break;
					
					default:
						// Append data received
						for(i = 0; ((i < length) && (m_length < CLI_BUFFER_SIZE)); i++)
						{
							m_buffer[m_length++] = buffer[i];
						}
						break;
				}
			}
			
		default:
			break;
	}
}

uint8_t CLI :: execute(uint8_t* buffer, uint8_t length)
{
	uint8_t argc = 0;
  uint8_t *argv[30];
	uint8_t i = 0;
	
	// Get the first token (cmd name)
	argv[argc] = (uint8_t*)strtok((char*)buffer, " ");

	// Walk through the other tokens (parameters)
	while((argv[argc] != NULL) && (argc < 30))
	{
		argv[++argc] = (uint8_t*)strtok((char*)NULL, " ");
	}
	
	// Search the command table for a matching command, using argv[0]
	for(i = 0 ; i < m_cmdCount; i++)
	{
		if(strcmp((const char*)argv[0], (const char*)m_cmdTable[i].name) == 0)
		{
			/* Found a match, execute the associated function. */
			return m_cmdTable[i].f(argc, (char**)&argv);
		}
	}
	
	m_serial->write((uint8_t*)unknown, strlen((const char*)unknown));
	
	return 0;
}
