#include <stdio.h>
#include "commonHeader.h"
#include "PublicAPI/seqnet.h"
#include "PublicAPI/condsel.h"
#include <Utils/customAssert.h>
#include <Utils/instructionEncoder.h>

int main ()
{
	SeqNet_init();

	SeqNet_Out input = {0};
	input.jump_addr = 0x05;
	input.req_move_up = false;
	input.req_move_down = true;
	input.req_door_state = false;
	input.req_reset = false;
	input.cond_sel = CONDSEL_CALL_PENDING_SAME;
	input.cond_inv = false;
	uint16_t encoded = EncodeInstruction(&input);
	printf("Encoded instruction: 0x%04X\n", encoded);

	ProgMem[0] = encoded;
	SeqNet_Out output = SeqNet_loop(false);
	printf("Jump Address: 0x%02X\n", output.jump_addr);
	printf("Request Move Up: %s\n", output.req_move_up ? "true" : "false");
	printf("Request Move Down: %s\n", output.req_move_down ? "true" : "false");
	printf("Request Door State: %s\n", output.req_door_state ? "true" : "false");
	printf("Request Reset: %s\n", output.req_reset ? "true" : "false");
	printf("Condition Select: %u\n", output.cond_sel);
	printf("Condition Invert: %s\n", output.cond_inv ? "true" : "false");
}
