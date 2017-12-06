#pragma once

//if the trade login success
extern bool ifTrade;

//token
#define TOKEN "VERSION1"
//const int avr=1;

//status
#define STATUS_SUCCESS	1
#define STATUS_FAILED	0

//error code and message
#define REQ_SUCCESS	200
#define TOKEN_ERR	1001
#define	SQL_ERR	1002
#define HET_ERR	1003
#define SUB_ERR	1004
#define	CON_ERR	1005
#define	MAR_ERR	1006
#define	EMR_ERR	1007
#define	KLE_ERR	1008

#define TIME_OUT 3