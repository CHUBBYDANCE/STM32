
#ifndef __SX126X_USER_H__
#define __SX126X_USER_H__


#define RF_FREQUENCY			435775000		// ���幤��Ƶ��
#define TX_OUTPUT_POWER			0				//SX1262/8: [-3..22]dBm SX1261: [-3 14] not [-18..+13]dBm

#define LORA_BANDWIDTH			LORA_BW_125		// �������

#define LORA_SPREADING_FACTOR	LORA_SF6		// [SF6..SF12] ������Ƶ����
#define LORA_CODINGRATE			LORA_CR_4_5		// ���������

#define LORA_LONG_PRE			16000			// ��ǰ����
#define LORA_SHORT_PRE			20				// ��ǰ����

//#define LORA_SYMBOL_TIMEOUT		1023		// Symbols   ��ʱ���Ÿ���
#define TX_TIMEOUT_VALUE			4000		// ms
#define RX_TIMEOUT_VALUE			4000		// ms

#define LORA_FIX_LENGTH_PAYLOAD		false
#define LORA_IQ_INVERSION			false


// ����
extern PacketParams_t PacketParams;
extern ModulationParams_t modulationParams;

extern uint32_t freqencyband[11];


// ����
void SX126xAppInit(void);


#endif