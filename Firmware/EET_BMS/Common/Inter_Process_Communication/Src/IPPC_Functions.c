#include "IPPC_Functions.h"
#include <string.h>

ippc_mailbox_register* mailbox_register = (ippc_mailbox_register*) SRAM4_BASE_ADDRESS;

#ifdef CORE_CM7

#endif

MailBoxStatus init_mailbox_system()
{
	for(uint32_t n = 0; n < MAX_MAILBOXES; n++)
	{
		mailbox_register->mailboxinuse[n] = 0;
		mailbox_register->mailboxes[n] = (ippc_mailbox*) (SRAM4_BASE_ADDRESS + 0x100 + n*((MAILBOXSIZE)+0x10));
	}

	for(uint32_t n = 0; n < MAX_MAILBOXES; n++)
	{
		ippc_mailbox* mailbox = mailbox_register->mailboxes[n];
		mailbox->datalen = 0;
		mailbox->mailboxlocked = 0;
		mailbox->new_data_available = 0;
		mailbox->payload_data = &mailbox->payload_data + 4;
		memset(mailbox->payload_data,0,MAILBOXSIZE);
	}

	return MAILBOX_OK;
}

MailBoxStatus write_mailbox(void* payload, uint16_t mailbox_datalen, uint8_t mailbox_number)
{
	if(mailbox_datalen > MAILBOXSIZE)
		return MAILBOX_WRITE_FAIL;

	if(mailbox_number >= MAX_MAILBOXES)
		return MAILBOX_WRITE_FAIL;

	while(mailbox_register->mailboxes[mailbox_number]->mailboxlocked == MAILBOX_LOCKED);

	mailbox_register->mailboxes[mailbox_number]->mailboxlocked = MAILBOX_LOCKED;
	memcpy(mailbox_register->mailboxes[mailbox_number]->payload_data, payload, mailbox_datalen);
	mailbox_register->mailboxes[mailbox_number]->datalen = mailbox_datalen;
	mailbox_register->mailboxes[mailbox_number]->new_data_available = 1;
	mailbox_register->mailboxes[mailbox_number]->mailboxlocked = MAILBOX_UNLOCKED;

	return MAILBOX_OK;
}

MailBoxStatus create_mailbox(uint8_t mailbox_number)
{
	return MAILBOX_OK;
}

MailBoxStatus read_mailbox(void* rx_buffer, uint16_t datalen, uint8_t mailbox_number)
{
	if(mailbox_number >= MAX_MAILBOXES)
		return MAILBOX_READ_FAIL;

	if(datalen > MAILBOXSIZE)
		return MAILBOX_READ_FAIL;

	while(mailbox_register->mailboxes[mailbox_number]->mailboxlocked == MAILBOX_LOCKED);

	mailbox_register->mailboxes[mailbox_number]->mailboxlocked = MAILBOX_LOCKED;

	memcpy(rx_buffer, mailbox_register->mailboxes[mailbox_number]->payload_data, datalen);

	mailbox_register->mailboxes[mailbox_number]->mailboxlocked = MAILBOX_UNLOCKED;

	return MAILBOX_OK;
}

MailBoxStatus check_mailbox(uint16_t* datalen_available, uint8_t mailbox_number)
{
	if(mailbox_number >= MAX_MAILBOXES)
		return MAILBOX_READ_FAIL;

	while(mailbox_register->mailboxes[mailbox_number]->mailboxlocked == MAILBOX_LOCKED);

	if(mailbox_register->mailboxes[mailbox_number]->new_data_available == 1)
	{
		return MAILBOX_NEWMESSAGE;
	}
	else
	{
		return MAILBOX_EMPTY;
	}

}

