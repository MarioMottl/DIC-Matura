#include "DS18B20.hpp"

int main(void)
{
	uint8_t reset_value;
	uint64_t result;
	float temp;
	char buf[32];

	DS18B20::init();

	while (true)
	{
		DS18B20::resetBus();
		DS18B20::sendByte(0xCC);
		DS18B20::sendByte(0x44);
		DS18B20::resetBus();
		DS18B20::sendByte(0xCC);
		DS18B20::sendByte(0xBE); // get temperature
		
		// read data
		result = DS18B20::readBytes();
		// interpret result
		temp = DS18B20::interpret(result);
		snprintf(buf, sizeof(buf), "%f°C\r\n", temp);
		USART1_put_bytes(buf, strlen(buf));

		for(size_t i=0; i<200000; i++)
		{
			wait_10us();
		}
	}	

	return 0;
}
