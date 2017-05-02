#include	"Common.h"

void	Delay(volatile U16 count)
{
 while(count) {
 	count--;
 	}
}


