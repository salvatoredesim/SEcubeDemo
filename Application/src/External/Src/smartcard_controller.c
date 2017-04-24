/*
 * smartcard_controller.c
 *
 *  Created on: 08 feb 2017
 *      Author: raidenfox
 */

#include "smartcard_controller.h"

void select_auth_applet(){
	const uint8_t APPLET_AID_SIZE = 0x07;
	uint8_t AUTENTICATION_APPLET_AID[APPLET_AID_SIZE];
	AUTENTICATION_APPLET_AID[0] = 0x11;
	AUTENTICATION_APPLET_AID[1] = 0x33;
	AUTENTICATION_APPLET_AID[2] = 0x55;
	AUTENTICATION_APPLET_AID[3] = 0x77;
	AUTENTICATION_APPLET_AID[4] = 0x99;
	AUTENTICATION_APPLET_AID[5] = 0xAA;
	AUTENTICATION_APPLET_AID[6] = 0x00;

	select_apdu(AUTENTICATION_APPLET_AID,APPLET_AID_SIZE);
}

void get_public_key(uint8_t* data,uint8_t* size_data){

	SLJ52_APDU pubkey_apdu;
	SLJ52_APDU_Response response_apdu;
	memset(&pubkey_apdu,0x00,sizeof(pubkey_apdu));
	memset(&response_apdu,0x00,sizeof(response_apdu));

	pubkey_apdu.CLA = CLA_AUTH_APPLET;
	pubkey_apdu.INS = INS_GET_PUBLIC_KEY;
	pubkey_apdu.P1 = 0x00;
	pubkey_apdu.P2 = 0x00;
	pubkey_apdu.LC = 0x00;
	pubkey_apdu.LE = 0x00;

	uint8_t temp_size = 0;
	SLJ52_SendAPDU(pubkey_apdu,&temp_size,&response_apdu);
	*size_data = response_apdu.SW2;
	memset(&response_apdu,0x00,sizeof(response_apdu));
	response_apdu = get_response_apdu(*size_data);
	memcpy(data,response_apdu.Data,*size_data);
}

void decrypt_keytext(uint8_t* data,uint8_t size_data,uint8_t* decrypted_key,uint8_t* size){

	SLJ52_APDU decrypt_apdu;
	SLJ52_APDU_Response response_apdu;
	memset(&decrypt_apdu,0x00,sizeof(decrypt_apdu));
	memset(&response_apdu,0x00,sizeof(response_apdu));

	decrypt_apdu.CLA = CLA_AUTH_APPLET;
	decrypt_apdu.INS = INS_DECRYPT;
	decrypt_apdu.P1 = 0x00;
	decrypt_apdu.P2 = 0x00;
	decrypt_apdu.LC = size_data;
	memcpy(decrypt_apdu.Data,data,size_data);
	decrypt_apdu.LE = 0x00;

	uint8_t temp_size = 0;
	SLJ52_SendAPDU(decrypt_apdu,&temp_size,&response_apdu);
	*size = response_apdu.SW2;
	memset(&response_apdu,0x00,sizeof(response_apdu));
	response_apdu = get_response_apdu(*size);
	memcpy(decrypted_key,response_apdu.Data,*size);
}

SLJ52_APDU_Response select_apdu(uint8_t* id, uint8_t size){

	/* Defining APDU for selecting AID or IDs
	 */
	SLJ52_APDU select_applet_apdu;
	SLJ52_APDU_Response response_apdu;
	memset(&select_applet_apdu,0x00,sizeof(select_applet_apdu));
	memset(&response_apdu,0x00,sizeof(response_apdu));

	select_applet_apdu.CLA = 0x00;
	select_applet_apdu.INS = SELECT;
	select_applet_apdu.P1 = 0x04;
	select_applet_apdu.P2 = 0x00;
	select_applet_apdu.LC = size;
	memcpy(select_applet_apdu.Data,id,size);
	select_applet_apdu.LE = 0x00;

	uint8_t temp_size = 0;
	SLJ52_SendAPDU(select_applet_apdu,&temp_size,&response_apdu);
	return response_apdu;
}

SLJ52_APDU_Response get_response_apdu(uint8_t size){
	SLJ52_APDU_Response response_apdu;
	memset(&response_apdu,0x00,sizeof(response_apdu));

	SLJ52_APDU builtAPDU = Build_GetResponse(size);
	uint8_t temp_size = 0;
	SLJ52_SendAPDU(builtAPDU,&temp_size,&response_apdu);
	return response_apdu;
}

SLJ52_APDU Build_GetResponse(uint8_t size){

	SLJ52_APDU get_response;
	memset(&get_response,0x00,sizeof(get_response));
	get_response.CLA = 0x00;
	get_response.INS = GET_RESPONSE;
	get_response.P1 = 0x00;
	get_response.P2 = 0x00;
	get_response.LC = 0x00;
	get_response.LE = size;

	return get_response;
}
