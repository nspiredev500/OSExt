#ifndef DISABLENAVNET_H
#define DISABLENAVNET_H
static bool NAVNETSTATUS = true;
void disablenavnet() // cannot be re-enabled currently
{
	if (! NAVNETSTATUS)
		return;
	NAVNETSTATUS = false;
	
	//TI_NN_StopService(0x00d3);
	//TI_NN_StopService(0x00fe);
	//TI_NN_StopService(0x4001);
	//TI_NN_StopService(0x4002);
	//TI_NN_StopService(0x4003);
	TI_NN_StopService(0x4004);
	//TI_NN_StopService(0x4020);
	TI_NN_StopService(0x4021);
	TI_NN_StopService(0x4022);
	TI_NN_StopService(0x4023);
	TI_NN_StopService(0x4041);
	TI_NN_StopService(0x4042);
	TI_NN_StopService(0x4043);
	TI_NN_StopService(0x4050);
	TI_NN_StopService(0x4051);
	TI_NN_StopService(0x4054);
	
	TI_NN_StopService(0x4060);
	
	TI_NN_StopService(0x4080);
	TI_NN_StopService(0x5000);
	disableInterrupts
}

void disableusb() // can be re-enabled, but os enables is quickly so it's not really usefull
{
	if (hwtype() == 1)
	{
		volatile uint32_t *peripheralacces = (uint32_t*) 0x900B0018;
		uint32_t access = *peripheralacces;
		access = access & 0b11111001111111111111111111111111;
		*peripheralacces = access;
	}
	else
	{
		
		
		
	}
}
void enableusb()
{
	if (hwtype() == 1)
	{
		volatile uint32_t *peripheralacces = (uint32_t*) 0x900B0018;
		uint32_t access = *peripheralacces;
		access = access | 0b00000110000000000000000000000000;
		*peripheralacces = access;
	}
	else
	{
		
		
		
		
	}
}
#endif