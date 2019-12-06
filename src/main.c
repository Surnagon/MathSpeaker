#include <stdlib.h>
#include "num_recog.h"

int main()
{
   unsigned result;
   bool timeout;
   char charbuffer[124];
   int ret;

   T_numrecog_cotext context;
   T_numrecog_info buffer[10];

   ret = numrecog_start(&context,10,1,buffer,10,"/home/z/MathSpeaker/recogfile",
                        charbuffer,sizeof(charbuffer));
   if(ret)
   {
      printf("\r\n Erro ao iniciar");
   }

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
