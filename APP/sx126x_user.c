#include "sx126x-hal.h"
#include "sx126x_user.h"
#include "main.h"
#include "spi.h"
#include <stdio.h>
#include "tim.h"

#define CADTIME		10	// ɨ���Ƶ������

uint32_t freqencyband[11] = {
	433675000,
	433875000,
	434075000,
	434275000,
	434475000,
	434675000,
	434875000,
	435075000,
	435275000,			// SX1262_A�ŵ�
	433475000,			// ���������ŵ�
	435775000			// �����ŵ����ڵ������
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
	HAL_GPIO_WritePin(TXW_I_GPIO_Port, TXW_I_Pin, GPIO_PIN_RESET);				// ��������
	spTimerStart(TIMER_LED, 0, 40, 4);
	printf("������ɣ���ʱ:%f ms\r\n", (double)t_rec/20);
	
	
	// ���ö�ǰ����
	//PacketParams.Params.LoRa.PreambleLength = LORA_SHORT_PRE;
	//radio.SetPacketParams(&PacketParams);
	

	// �������մ���
	//radio.SetRx(0);
	
	//��������CAD����
//	t_rec=0;
//	TIM2_Start();
//	
//	cad_count = 0;
//	radio.SetRfFrequency(freqencyband[cad_count]);								// Ƶ��
//	spTimerStart(TIMER_CAD, 0, 123, 1);
}
void OnRxDone(void)
{
	spTimerStart(TIMER_LED, 0, 50, 4);
	printf("���յ�");

	radio.GetPacketStatus(&pktStatus);
	if(pktStatus.packetType == PACKET_TYPE_LORA)
	{
		printf("LoRa���ݰ�:\r\n");
		printf("FreqError=%d\r\n", pktStatus.Params.LoRa.FreqError);
		printf("SignalRssiPkt=%d\r\n", pktStatus.Params.LoRa.SignalRssiPkt);
		printf("SnrPkt=%d\r\n", pktStatus.Params.LoRa.SnrPkt);
		printf("RssiPkt=%d\r\n", pktStatus.Params.LoRa.RssiPkt);
	}
	else if(pktStatus.packetType == PACKET_TYPE_GFSK)
	{
		printf("GFSK���ݰ�:\r\n");
		printf("FreqError=%d\r\n", pktStatus.Params.Gfsk.FreqError);
		printf("RssiSync=%d\r\n", pktStatus.Params.Gfsk.RssiSync);
		printf("RssiAvg=%d\r\n", pktStatus.Params.Gfsk.RssiAvg);
		printf("RxStatus=%d\r\n", pktStatus.Params.Gfsk.RxStatus);
	}
	else
	{
		printf("δ֪���ݰ�\r\n");
	}
	printf("=============================\r\n");
	if(!radio.GetPayload(rxbuff, &rxlen, 255))
	{
		// ��ֹ��ӡ������Ϣ
		rxbuff[rxlen] = 0;
		printf("����:%d\r\n%s\r\n",rxlen,rxbuff);
	}
	else
	{
		printf("���ݸ��ض���ʧ��\r\n");
	}
	radio.SetRx(0);
}
void OnTxTimeout(void)
{
	TIM2_Stop();
	HAL_GPIO_WritePin(TXW_I_GPIO_Port, TXW_I_Pin, GPIO_PIN_RESET);				// ��������
	printf("���ͳ�ʱ����ʱ:%f ms\r\n", (double)t_rec/20);
	
	spTimerStart(TIMER_LED, 0, 100, 4);
	
	// t_rec = 0;
	//printf("���ͳ�ʱ\r\n");
	//radio.SetRx(0);
}
void OnRxTimeout(void)
{
	TIM2_Stop();
	printf("���ճ�ʱ����ʱ:%f ms\r\n", (double)t_rec/20);
	
	spTimerStart(TIMER_LED, 0, 100, 4);

	//radio.SetStandby(STDBY_XOSC);
	//radio.SetStandby(STDBY_RC);
	radio.SetRx(0);
}



void OnRxError(IrqErrorCode_t e)
{
	spTimerStart(TIMER_LED, 0, 100, 4);
	printf("���մ���\r\n");
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
		radio.SetRfFrequency(freqencyband[cad_count]);		// �л�Ƶ��
		spTimerStart(TIMER_CAD, 0, 123, 1);
	}
	else
	{
		TIM2_Stop();
		printf("CAD%d done:%s\t��ʱ:%lf ms\r\n", cad_count, cadstr, (double)t_rec/20);
		radio.SetStandby(STDBY_XOSC);
	}
	
	TIM2_Stop();
	//printf("CAD���:");
	if(cadFlag)
	{
		printf("detected");
	}
	else
	{
		printf("nothing detected");
	}
	printf("��ʱ:%lf ms\r\n", (double)t_rec/20);

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
		
	modulationParams.PacketType = PACKET_TYPE_LORA;								// LoRa������
	modulationParams.Params.LoRa.SpreadingFactor = LORA_SPREADING_FACTOR;		// SF
	modulationParams.Params.LoRa.Bandwidth = LORA_BANDWIDTH;					// BW
	modulationParams.Params.LoRa.CodingRate = LORA_CODINGRATE;					// CR

	PacketParams.PacketType = PACKET_TYPE_LORA;									// LoRa���ݰ�
	PacketParams.Params.LoRa.PreambleLength = LORA_SHORT_PRE;					// ǰ����
	PacketParams.Params.LoRa.HeaderType = LORA_PACKET_VARIABLE_LENGTH;			// ��ͷ
	PacketParams.Params.LoRa.PayloadLength = 80;								// ���͸��ش�С����Ӱ�����
	PacketParams.Params.LoRa.CrcMode = LORA_CRC_ON;								// CRCʹ��
	PacketParams.Params.LoRa.InvertIQ = LORA_IQ_NORMAL;							// IQ����ת
	
	SX126xHalInit(&hspi1, &callbacks);											// ��ʼ��IO���жϣ��ص���ͬ����
	// SX126xHalInit(&hspi1, NULL);											// ��ʼ��IO���жϣ��ص���ͬ����
	
	radio.SetStandby(STDBY_RC);
	radio.SetPacketType(modulationParams.PacketType);
	radio.SetModulationParams(&modulationParams);
	radio.SetPacketParams(&PacketParams);
	
	radio.SetRxTxFallbackMode(STDBY_XOSC);								// FS, STDBY_XOSC, STDBY_RC

	//radio.SetRfFrequency(freqencyband[0]);										// �����ŵ�
	radio.SetBufferBaseAddresses(0x00, 0x00);
	radio.SetTxParams(TX_OUTPUT_POWER, RADIO_RAMP_20_US);
	
	
	radio.SetDio3AsTcxoCtrl(TCXO_CTRL_3_0V, 0x100);
	delay_ms(1);
	radio.SetRfFrequency(freqencyband[9]);										// �����ŵ�
	
	radio.SetDioIrqParams(myIrqMask, myIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);// Gerneral IRQ, DIO1 IRQ, DIO2 IRQ, DIO3 IRQ.
	//radio.SetRx(RX_TIMEOUT_VALUE);

	radio.SetDio2AsRfSwitchCtrl(1);
	radio.SetPollingMode();
	
	// CAD����
	// ��2,3������������LoRa��������LoRa�������������������ȣ���SF��BW�йأ�Ҳ���1�������й�
	radio.SetCadParams(LORA_CAD_04_SYMBOL, 22, 10, LORA_CAD_ONLY, 50*64);
	
	//radio.SetRx(0);
}

