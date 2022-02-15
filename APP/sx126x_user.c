#include "sx126x-hal.h"
#include "sx126x_user.h"
#include "main.h"
#include "spi.h"
#include <stdio.h>
#include "tim.h"

#define CADTIME		10	// 扫描的频带个数

uint32_t freqencyband[11] = {
	433675000,
	433875000,
	434075000,
	434275000,
	434475000,
	434675000,
	434875000,
	435075000,
	435275000,			// SX1262_A信道
	433475000,			// 高速上行信道
	435775000			// 下行信道，节点接收用
};


PacketParams_t PacketParams;
ModulationParams_t modulationParams;
PacketStatus_t pktStatus;

uint16_t myIrqMask = IRQ_TX_DONE | IRQ_RX_DONE | IRQ_CRC_ERROR | IRQ_RX_TX_TIMEOUT | IRQ_CAD_DONE | IRQ_CAD_ACTIVITY_DETECTED | IRQ_HEADER_ERROR;;


uint8_t rxbuff[256];
uint8_t rxlen;
uint8_t cadstr[12];
uint8_t cad_count = 0;


void OnTxDone(void)
{
	TIM2_Stop();
	HAL_GPIO_WritePin(TXW_I_GPIO_Port, TXW_I_Pin, GPIO_PIN_RESET);				// 引脚拉低
	spTimerStart(TIMER_LED, 0, 40, 4);
	printf("发送完成，计时:%f ms\r\n", (double)t_rec/20);
	
	
	// 设置短前导码
	//PacketParams.Params.LoRa.PreambleLength = LORA_SHORT_PRE;
	//radio.SetPacketParams(&PacketParams);
	

	// 开启接收窗口
	//radio.SetRx(0);
	
	//立即启动CAD任务
//	t_rec=0;
//	TIM2_Start();
//	
//	cad_count = 0;
//	radio.SetRfFrequency(freqencyband[cad_count]);								// 频率
//	spTimerStart(TIMER_CAD, 0, 123, 1);
}
void OnRxDone(void)
{
	spTimerStart(TIMER_LED, 0, 50, 4);
	printf("接收到");

	radio.GetPacketStatus(&pktStatus);
	if(pktStatus.packetType == PACKET_TYPE_LORA)
	{
		printf("LoRa数据包:\r\n");
		printf("FreqError=%d\r\n", pktStatus.Params.LoRa.FreqError);
		printf("SignalRssiPkt=%d\r\n", pktStatus.Params.LoRa.SignalRssiPkt);
		printf("SnrPkt=%d\r\n", pktStatus.Params.LoRa.SnrPkt);
		printf("RssiPkt=%d\r\n", pktStatus.Params.LoRa.RssiPkt);
	}
	else if(pktStatus.packetType == PACKET_TYPE_GFSK)
	{
		printf("GFSK数据包:\r\n");
		printf("FreqError=%d\r\n", pktStatus.Params.Gfsk.FreqError);
		printf("RssiSync=%d\r\n", pktStatus.Params.Gfsk.RssiSync);
		printf("RssiAvg=%d\r\n", pktStatus.Params.Gfsk.RssiAvg);
		printf("RxStatus=%d\r\n", pktStatus.Params.Gfsk.RxStatus);
	}
	else
	{
		printf("未知数据包\r\n");
	}
	printf("=============================\r\n");
	if(!radio.GetPayload(rxbuff, &rxlen, 255))
	{
		// 防止打印无用信息
		rxbuff[rxlen] = 0;
		printf("长度:%d\r\n%s\r\n",rxlen,rxbuff);
	}
	else
	{
		printf("数据负载读出失败\r\n");
	}
	radio.SetRx(0);
}
void OnTxTimeout(void)
{
	TIM2_Stop();
	HAL_GPIO_WritePin(TXW_I_GPIO_Port, TXW_I_Pin, GPIO_PIN_RESET);				// 引脚拉低
	printf("发送超时，计时:%f ms\r\n", (double)t_rec/20);
	
	spTimerStart(TIMER_LED, 0, 100, 4);
	
	// t_rec = 0;
	//printf("发送超时\r\n");
	//radio.SetRx(0);
}
void OnRxTimeout(void)
{
	TIM2_Stop();
	printf("接收超时，计时:%f ms\r\n", (double)t_rec/20);
	
	spTimerStart(TIMER_LED, 0, 100, 4);

	//radio.SetStandby(STDBY_XOSC);
	//radio.SetStandby(STDBY_RC);
	radio.SetRx(0);
}



void OnRxError(IrqErrorCode_t e)
{
	spTimerStart(TIMER_LED, 0, 100, 4);
	printf("接收错误\r\n");
	radio.SetRx(0);
	radio.SetStandby(STDBY_XOSC);
}
void OncadDone(uint8_t cadFlag)
{

	if(cadFlag)
		cadstr[cad_count]='1';
	else
		cadstr[cad_count]='0';
	cad_count++;
	
	if(cad_count<CADTIME)
	{
		radio.SetRfFrequency(freqencyband[cad_count]);		// 切换频段
		spTimerStart(TIMER_CAD, 0, 123, 1);
	}
	else
	{
		TIM2_Stop();
		printf("CAD%d done:%s\t计时:%lf ms\r\n", cad_count, cadstr, (double)t_rec/20);
		radio.SetStandby(STDBY_XOSC);
	}
	
	TIM2_Stop();
	//printf("CAD完成:");
	if(cadFlag)
	{
		printf("detected");
	}
	else
	{
		printf("nothing detected");
	}
	printf("计时:%lf ms\r\n", (double)t_rec/20);

}

RadioCallbacks_t callbacks = {
	&OnTxDone,        // txDone
	&OnRxDone,        // rxDone
	NULL,             // rxPreambleDetect
	NULL,             // rxSyncWordDone
	NULL,             // rxHeaderDone
	&OnTxTimeout,     // txTimeout
	&OnRxTimeout,     // rxTimeout
	&OnRxError,       // rxError
	&OncadDone,       // cadDone
};



void SX126xAppInit(void)
{
		
	modulationParams.PacketType = PACKET_TYPE_LORA;								// LoRa调制器
	modulationParams.Params.LoRa.SpreadingFactor = LORA_SPREADING_FACTOR;		// SF
	modulationParams.Params.LoRa.Bandwidth = LORA_BANDWIDTH;					// BW
	modulationParams.Params.LoRa.CodingRate = LORA_CODINGRATE;					// CR

	PacketParams.PacketType = PACKET_TYPE_LORA;									// LoRa数据包
	PacketParams.Params.LoRa.PreambleLength = LORA_SHORT_PRE;					// 前导码
	PacketParams.Params.LoRa.HeaderType = LORA_PACKET_VARIABLE_LENGTH;			// 有头
	PacketParams.Params.LoRa.PayloadLength = 80;								// 发送负载大小，不影响接收
	PacketParams.Params.LoRa.CrcMode = LORA_CRC_ON;								// CRC使能
	PacketParams.Params.LoRa.InvertIQ = LORA_IQ_NORMAL;							// IQ不反转
	
	SX126xHalInit(&hspi1, &callbacks);											// 初始化IO，中断，回调，同步字
	// SX126xHalInit(&hspi1, NULL);											// 初始化IO，中断，回调，同步字
	
	radio.SetStandby(STDBY_RC);
	radio.SetPacketType(modulationParams.PacketType);
	radio.SetModulationParams(&modulationParams);
	radio.SetPacketParams(&PacketParams);
	
	radio.SetRxTxFallbackMode(STDBY_XOSC);								// FS, STDBY_XOSC, STDBY_RC

	//radio.SetRfFrequency(freqencyband[0]);										// 下行信道
	radio.SetBufferBaseAddresses(0x00, 0x00);
	radio.SetTxParams(TX_OUTPUT_POWER, RADIO_RAMP_20_US);
	
	
	radio.SetDio3AsTcxoCtrl(TCXO_CTRL_3_0V, 0x100);
	delay_ms(1);
	radio.SetRfFrequency(freqencyband[9]);										// 下行信道
	
	radio.SetDioIrqParams(myIrqMask, myIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);// Gerneral IRQ, DIO1 IRQ, DIO2 IRQ, DIO3 IRQ.
	//radio.SetRx(RX_TIMEOUT_VALUE);

	radio.SetDio2AsRfSwitchCtrl(1);
	radio.SetPollingMode();
	
	// CAD设置
	// 第2,3个参数决定了LoRa调制器与LoRa符号做相关运算的灵敏度，与SF和BW有关，也与第1个参数有关
	radio.SetCadParams(LORA_CAD_04_SYMBOL, 22, 10, LORA_CAD_ONLY, 50*64);
	
	//radio.SetRx(0);
}

