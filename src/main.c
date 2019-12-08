#include <stdlib.h>
#include "num_recog.h"

int main()
{
   unsigned result;
   bool timeout;
   char charbuffer[124];
   int ret;
   unsigned * rd;

   T_numrecog_cotext context;
   T_numrecog_info buffer[10];

   do
   {
      ret = numrecog_start(&context,5000,1000,buffer,10,"recogfile",
                        charbuffer,sizeof(charbuffer));
   if(ret)
   {
      printf("\r\n Erro ao iniciar");
      return 0;
   }

   timeout =  false;
   while(!timeout)
   {
      rd = numrecog_read(&context,&result,&timeout);
      if(rd!=NULL)
      {
           printf("\r\n o valor reconhecido foi %d", result);
           printf("\r\n");
           break;
      }
      else
      {
         if(timeout)
         {
            printf("\r\n a crianca  nao respondeu");
         }
         else
         {
//                  printf("\r\n a crianca nao respondeu ainda");
         }
      }
   }
   }while(1);
   return 0;
}
