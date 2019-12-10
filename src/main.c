#include <stdlib.h>
#include "num_recog.h"
#include "calc_gen.h"


#define NO_ANS_TIMEOUT_MS (10000)
#define ANS_COMPLETION_TIMEOUT (3000)

int main()
{
   unsigned result = 0, select = 0;
   bool timeout;
   char charbuffer[124];
   int ret;
   unsigned * rd;

   T_numrecog_cotext context;
   T_numrecog_info buffer[10];

   do
   {
      ret = numrecog_start(&context,NO_ANS_TIMEOUT_MS,ANS_COMPLETION_TIMEOUT,buffer,
                           sizeof(charbuffer)/sizeof(charbuffer[0]),"recogfile",
                           charbuffer,sizeof(charbuffer));
      if(ret)
      {
         printf("\r\n Erro ao iniciar");
         return 0;
      }

      select = calc_gen();

      timeout = false;
      do
      {
         rd = numrecog_read(&context,&result,&timeout);
         if(rd!=NULL)
         {
            printf("\r\n %d, certo?", result);
            printf("\r\n");
            if(result == select)
            {
               printf("\r\n PARABENS!!! \n Voce acertou!");
               printf("\r\n");
            }
            else
            {
               printf("\r\n Que pena. Voce errou. A resposta correta era %d", select);
               printf("\r\n");
            }
            break;
         }
         else
         {
            if(timeout)
            {
               printf("\r\n a crianca  nao respondeu");
            }
         }
      }
      while((!timeout) && (select));

      printf("\r\n timeout %d select %d",timeout,select);
   }
   while(1);

   return 0;
}
