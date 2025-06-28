#include <stdio.h>
#include "PublicAPI/seqnet.h"
#include "PublicAPI/condsel.h"
#include <CustomAssert/customAssert.h>

int main ()
{

	CUSTOM_ASSERT_FALSE("This is a custom assert message");
	printf("Hello, World!\n");
	
}
