#include "PublicAPI/seqnet.h"
#include <stdio.h>

void SeqNet_init(void)
{
    printf("Sequential Network initialized.\n");
}

SeqNet_Out SeqNet_loop(const bool condition_active)
{
    return (SeqNet_Out){
        .cond_inv = false,
        .cond_sel = 0,
        .req_reset = false,
        .req_door_state = false,
        .req_move_down = false,
        .req_move_up = false,
        .jump_addr = 0
    };
}
