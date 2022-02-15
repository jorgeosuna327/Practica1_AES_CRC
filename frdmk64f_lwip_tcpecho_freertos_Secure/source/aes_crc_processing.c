/*
 *
 */

#include <string.h> // CBC mode, for memset

#include "tcpecho.h"

#include "aes_crc_processing.h"
#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/api.h"

#include <stdio.h>

#include "aes.h"
#include "fsl_crc.h"

/*-----------------------------------------------------------------------------------*/

/*!
 * @brief Init for CRC-32.
 * @details Init CRC peripheral module for CRC-32 protocol.
 *          width=32 poly=0x04c11db7 init=0xffffffff refin=true refout=true xorout=0xffffffff check=0xcbf43926
 *          name="CRC-32"
 *          http://reveng.sourceforge.net/crc-catalogue/
 */
static void InitCrc32(CRC_Type *base, uint32_t seed)
{
    crc_config_t config;

    config.polynomial         = 0x04C11DB7U;
    config.seed               = seed;
    config.reflectIn          = true;
    config.reflectOut         = true;
    config.complementChecksum = true;
    config.crcBits            = kCrcBits32;
    config.crcResult          = kCrcFinalChecksum;

    CRC_Init(base, &config);
}

uint32_t split_received_data(struct netbuf **buf_ptr, char *body_msg_dataptr, u16_t *body_msg_len)
{
    void *data_tmp;
    u16_t len_tmp;
    uint32_t crc_received_tmp;

	/* Data received into an array */
	netbuf_data(*buf_ptr, &data_tmp, &len_tmp);
    char msg_received[len_tmp];
    memcpy(msg_received, data_tmp, len_tmp);

    printf("Received data: %.*s\n", (int)sizeof(msg_received), msg_received);
    printf("Size of Received Message (Including Padding and CRC): %u\n", (int)sizeof(msg_received));

    *body_msg_len = ((int)sizeof(msg_received) - 4);

    for(int i=0; i<*body_msg_len; i++)
	{
		body_msg_dataptr[i] = msg_received[i];
	}
    //printf("Received Body Message: %.*s\n", *body_msg_len, body_msg_dataptr);

    /* CRC Received into 4 bytes variable. Beware of the little-endian order: */
    crc_received_tmp = (0xFF & (msg_received[*body_msg_len]));
    crc_received_tmp += (0xFF00 & ((msg_received[*body_msg_len + 1]) << 8));
    crc_received_tmp += (0xFF0000 & ((msg_received[*body_msg_len + 2]) << 16));
    crc_received_tmp += (0xFF000000 & ((msg_received[*body_msg_len + 3]) << 24));
    //printf("Received CRC: 0x%08x, (%u) \n", crc_received_tmp);

    return crc_received_tmp;
}

uint32_t calculate_crc32_bodyMsg(char body_Msg[], u16_t bodyMsg_len)
{
	uint32_t crc_calculated_tmp = 0;
	CRC_Type *base = CRC0;

    InitCrc32(base, 0xFFFFFFFFU);
    CRC_WriteData(base, (uint8_t *)&body_Msg[0], bodyMsg_len);
    crc_calculated_tmp = CRC_Get32bitResult(base);

    return crc_calculated_tmp;
}

void Decrypt_received_bodyMsg(char body_Msg[], u16_t bodyMsg_len, u16_t *decrypted_msg_len)
{
	uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	struct AES_ctx ctx;
	uint8_t Decrypted_plain_len = 0;

	AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, body_Msg, bodyMsg_len);

	PRINTF("Decrypted Message in Hex: ");
	for(int i=0; i<bodyMsg_len; i++) {
		/* Removing extra zeros */
		if(body_Msg[i] != 0)
		{
			PRINTF("0x%02x, ", body_Msg[i]);
			Decrypted_plain_len++;
		}
	}
	PRINTF("\r\n");
	//printf("Decrypted Message in Base64: %.*s\n", Decrypted_plain_len, body_Msg);

	*decrypted_msg_len = Decrypted_plain_len;
}

void MsgToSend_Encrypt_CRC32(char plain_Msg[], u16_t plainMsg_len)
{
	uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	struct AES_ctx ctx;
	size_t padded_len;
	uint32_t crc_calculated_tmp = 0;
	CRC_Type *base = CRC0;

	/* Encrypting message back. Adding needed zeros */
	padded_len = plainMsg_len + (16 - (plainMsg_len%16) );
	char padded_msg[padded_len];
	for(int i=0; i<padded_len; i++) {
		padded_msg[i] = 0;
	}
	memcpy(padded_msg, plain_Msg, plainMsg_len);

	AES_init_ctx_iv(&ctx, key, iv);
	AES_CBC_encrypt_buffer(&ctx, padded_msg, padded_len);

	InitCrc32(base, 0xFFFFFFFFU);
    CRC_WriteData(base, (uint8_t *)&padded_msg[0], padded_len);
    crc_calculated_tmp = CRC_Get32bitResult(base);
    PRINTF("Calculated CRC-32 using Encryted data to send: 0x%08x\r\n", crc_calculated_tmp);

    uint8_t encrypted_msg_crc_tmp[padded_len+4];
	for(int i=0; i<padded_len; i++) {
		encrypted_msg_crc_tmp[i] = padded_msg[i];
	}

    /* CRC Received into 4 bytes variable. Beware of the little-endian order: */
	encrypted_msg_crc_tmp[padded_len] = (crc_calculated_tmp & 0x000000FF);
	encrypted_msg_crc_tmp[padded_len+1] = ((crc_calculated_tmp & 0x0000FF00) >> 8);
	encrypted_msg_crc_tmp[padded_len+2] = ((crc_calculated_tmp & 0x00FF0000) >> 16);
	encrypted_msg_crc_tmp[padded_len+3] = ((crc_calculated_tmp & 0xFF000000) >> 24);

	for(int i=0; i<(padded_len + 4); i++) {
		plain_Msg[i] = encrypted_msg_crc_tmp[i];
	}
}

void aes_crc_processing(struct netbuf **new_buf, void **dataptr, u16_t *len_ptr)
{

	struct netbuf **buf_ptr;
	buf_ptr = new_buf;
    char body_msg_tmp[512];
    u16_t body_msg_len;
    u16_t decrypted_msg_plain_len;
    uint32_t crc_received;
    uint32_t crc_calculated;

    crc_received = split_received_data(buf_ptr, body_msg_tmp, &body_msg_len);
    char body_msg[body_msg_len];
    memcpy(body_msg, body_msg_tmp, body_msg_len);
    //printf("Received Body Message: %.*s\n", body_msg_len, body_msg);
    //printf("Size of Received Body Message: %u\n", body_msg_len);
    //printf("Received CRC: 0x%08x, (%u) \n", crc_received);


    crc_calculated = calculate_crc32_bodyMsg(body_msg, body_msg_len);
    //PRINTF("CRC-32 Calculated from received data: 0x%08x\r\n", crc_calculated);

	  if (crc_calculated != crc_received)
	  {
		  printf("CRC Calculated: %u, did not match CRC Received: %u\n", crc_calculated, crc_received);
	  }
	  else
	  {
		  printf("CRC Calculated matched CRC Received: 0x%08x\r\n", crc_calculated);
	  }

	  Decrypt_received_bodyMsg(body_msg, body_msg_len, &decrypted_msg_plain_len);
	  char decrypted_msg_plain[decrypted_msg_plain_len];
	  memcpy(decrypted_msg_plain, body_msg, decrypted_msg_plain_len);
	  printf("Decrypted Message: %.*s\n", decrypted_msg_plain_len, decrypted_msg_plain);


	  *len_ptr = body_msg_len + 4;
	  char ready_ToSend_msg[*len_ptr];
	  for(int i=0; i<*len_ptr; i++) {
		  ready_ToSend_msg[i] = 0;
	  }
	  memcpy(ready_ToSend_msg, decrypted_msg_plain, decrypted_msg_plain_len);
	  MsgToSend_Encrypt_CRC32(ready_ToSend_msg, decrypted_msg_plain_len);
	  *dataptr = ready_ToSend_msg;
	  printf("Encrypted data to send: %.*s\n\n", *len_ptr, *dataptr);

    //vTaskDelete( NULL );

}

/* AES_CRC_PROCESSING */
