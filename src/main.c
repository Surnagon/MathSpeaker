#include <stdlib.h>
#include "num_recog.h"

int main()
{
   unsigned result;
        bool timeout;

   T_numrecog_cotext context;
   T_numrecog_info buffer[10];

   numrecog_start(&context,10,1,buffer,10);

   if( NULL != numrecog_read(&context,&result,&timeout))
   {
      printf("\r\n o valor reconhecido foi %d", result);
   }
   else
   {
      if(timeout)
      {
         printf("\r\n a crianca  nao respondeu");
      }
      else
      {
          printf("\r\n a crianca nao respondeu ainda");
      }
   }

   return 0;
}
