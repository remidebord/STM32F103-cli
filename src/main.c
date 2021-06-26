#include "main.h"

uint8_t blink_1(int argc, char **argv);
uint8_t blink_2(int argc, char **argv);
uint8_t help(int argc, char **argv);

T_COMMAND commands[] = 
{
	{"blink_1", &blink_1},
	{"blink_2", &blink_2},
	{"help", &help},
};

DigitalOut led1(PC_13);

USB_VCP vcp(PA_12, PA_11);

CLI cli(&vcp, (T_COMMAND*)&commands, (sizeof(commands)/sizeof(T_COMMAND)));

int main(void)
{	
	while(1)
	{
		cli.update();
	}
}

uint8_t blink_1(int argc, char **argv)
{
	uint8_t i = 0;

	for(i = 0; i < 10; i++)
	{
		led1 = (led1 == 0) ? 1 : 0;
		Delay(500);
	}
	
	return 1;
}

uint8_t blink_2(int argc, char **argv)
{
	uint16_t duration = 1000;
	uint8_t loops = 10;
	uint8_t buffer[64] = {0};
	uint8_t i = 1;
	
	// Parse options
	while((i < argc) && (argv[i][0] == '-'))
	{
		switch (argv[i][1])
		{
			case 'l':			
				// Value present ?
				if(argv[++i][0] != '-')
				{
					sscanf(argv[i++], "%d", (int*)&loops);
				}
				break;

			case 'd':			
				// Value present ?
				if(argv[++i][0] != '-')
				{
					sscanf(argv[i++], "%d", (int*)&duration);
				}
				break;
		}
	}
	
	sprintf((char*)buffer, "\nloops: %d, duration: %d ms", loops, duration);
	
	vcp.write(buffer, strlen((const char*)buffer));
	
	for(i = 0; i < loops; i++)
	{
		led1 = (led1 == 0) ? 1 : 0;
		Delay(duration);
	}
	
	return 1;
}

uint8_t help(int argc, char **argv)
{
	uint8_t buffer[32] = {0};
	uint8_t i = 0;
	
	vcp.write((uint8_t*)"\nList of commands:\n", 18);
	
	for(i = 0; i < (sizeof(commands)/sizeof(T_COMMAND)); i++)
	{
		sprintf((char*)buffer, "\n%s", commands[i].name);
		
		vcp.write(buffer, strlen((const char*)buffer));
	}
	
	return 1;
}
