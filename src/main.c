#include <stdio.h>
#include "PublicAPI/seqnet.h"
#include "PublicAPI/condsel.h"
#include <CustomAssert/customAssert.h>

int main ()
{
	ASSERT_FATAL_ERROR("TEST: This is a custom assert test that should always fail.");
	
	
}
