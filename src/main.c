#include <stdlib.h>
#include <time.h>
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

   result = 0
   /*result = calc_gen()*/
   srand(time(NULL));   // Initialization, should only be called once.
   
   int oper_select, a, b;
   
   oper_select = ceil(rand()*4)

   select = 0

   switch(oper_select)
   {
      case 1:
         a = ceil(rand()*10)
         b = ceil(rand()*10)
         select = a+b
         break;
      case 2:
         do
         {
         a = ceil(rand()*10)
         b = ceil(rand()*10)
         }while(a < b)
         select = a-b
         break;
      case 3:
         a = ceil(rand()*10)
         b = ceil(rand()*10)
         select = a*b
         break;
      case 4:
         do
         {
         a = ceil(rand()*10)
         b = ceil(rand()*10)
         }while(a%b != 0)
         select = a/b
         break;
      default:
         printf("\r\n opcao invalida %d", result);
         printf("\r\n");
         break;
   }

   timeout = false;
   while(!timeout && select == 0)
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
