#include "num_recog.h"
#include <pthread.h>


/***************************************************************************************/
static void * numrecog_task( void * arg)
{
   printf("%s",(char *)arg);
}


/***************************************************************************************/
int numrecog_start( T_numrecog_cotext * arg_context_ptr,
                    int arg_timeout_ms,
                    int arg_num_timout_ms,
                    T_numrecog_info * arg_buffer,
                    unsigned arg_size )
{
   int ret;

   if(arg_context_ptr == NULL)
      return EFAULT;

   arg_context_ptr->rbuffer.wr_pos = 0;
   arg_context_ptr->rbuffer.rd_pos = 0;
   arg_context_ptr->rbuffer.buffer = NULL;
   arg_context_ptr->no_answere_timeout_ms = 0;
   arg_context_ptr->answere_timeout_ms = 0;

   ret= pthread_mutex_init(arg_context_ptr->rbuffer.lock,NULL);
   if(ret)
   {
      printf("\r\n ERROR INITIATING THE MUTEXT FOR RING BUFFER ACCESS");
      return ret;
   }
   arg_context_ptr->rbuffer.buffer = arg_buffer;
   arg_context_ptr->no_answere_timeout_ms = arg_timeout_ms;
   arg_context_ptr->answere_timeout_ms = answere_timeout_ms;

   // Creating the thread that catches the recognized numbers
   ret = pthread_create(&arg_context_ptr->thread_id,
                        NULL,
                        numrecog_task,
                        (void*)("\r\n NUMRECOG RUNNING HARD ALL NIGHT"));
   if(ret)
   {
      arg_context_ptr->rbuffer.buffer = NULL;
      arg_context_ptr->no_answere_timeout_ms = 0;
      arg_context_ptr->answere_timeout_ms = 0;
      printf("\r\n ERROR: THE THREAD CREATION FAILS : %d", ret);
      return ret;
   }

   return 0; 
}
