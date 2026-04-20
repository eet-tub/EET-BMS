#ifndef _IPPC_COM
#define _IPPC_COM

#include <stdint.h>

#define MAX_MAILBOXES 10
#define MAILBOXSIZE 1024

#define BMS_MEASUREMENTS_MAILBOX_NR 1
#define BMS_STATE_ESTIMATION_MAILBOX_NR 2

#define SRAM4_BASE_ADDRESS 0x38000000

typedef struct
{
	uint8_t new_data_available;
	uint8_t mailboxlocked;
	uint16_t datalen;
	void* payload_data;
}ippc_mailbox;


typedef struct
{
	ippc_mailbox* mailboxes[MAX_MAILBOXES];
	uint8_t mailboxinuse[MAX_MAILBOXES];
	uint8_t mailboxes_initialized;
}ippc_mailbox_register;

typedef enum
{
	MAILBOX_OK,
	MAILBOX_WRITE_FAIL,
	MAILBOX_READ_FAIL,
	MAILBOX_IN_USE,
	MAILBOX_EMPTY,
	MAILBOX_NEWMESSAGE,
}MailBoxStatus;

typedef enum
{
	MAILBOX_UNLOCKED,
	MAILBOX_LOCKED
}MailBoxMutex;




MailBoxStatus write_mailbox(void* payload, uint16_t mailbox_datalen, uint8_t mailbox_number);
MailBoxStatus create_mailbox(uint8_t mailbox_number);
MailBoxStatus read_mailbox(void* rx_buffer, uint16_t datalen, uint8_t mailbox_number);
MailBoxStatus check_mailbox(uint16_t* datalen_available, uint8_t mailbox_number);
MailBoxStatus init_mailbox_system();



#endif
