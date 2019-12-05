#include "num_recog.h"
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief This function is the routine of the recognition thread
 */
static void * numrecog_task( void * arg)
{
	T_numrecog_cotext * context_ptr = (T_numrecog_cotext *)arg;
   /*
#1 - Esperar evento no arquivo
#2 - Le caracteres e detectar numeros 
#3 - Quando um numero for detectado utilize a funcao clock() para pegar o instante de deteccao
#4 - Preencher uma estrutura do tipo T_numrecog_info.
#5 - Escreve a estrutura no buffer cricular utilizando a funcao numrecog_rb_write
   */
}


/**
 * @brief This function reads the number of recognitions into the ring buffer
 *
 * @param[in] arg_rbuffer_ptr is the pointer to the ring buffer descriptor. See #T_numrecog_rbuffer.
 * 
 * @return This function returns the number of recognitions into the ring buffer.
 */
static unsigned numrecog_rb_count(T_numrecog_rbuffer * arg_rbuffer_ptr)
{
   int count;

   pthread_mutex_lock(arg_rbuffer_ptr->lock);
   count = arg_rbuffer_ptr->count;
   pthread_mutex_unlock(arg_rbuffer_ptr->lock);
   
   return count;
   
}

/**
 * @brief This function reads recognitions from the ring buffer
 *
 * @param[in] arg_rbuffer_ptr is the pointer to the ring buffer descriptor. See #T_numrecog_rbuffer.
 * @param[out] arg_recog_ptr is the pointer to the buffer that will receive the read symbols. See
	 #T_numrecog_info.
 * @param[in,out] arg_size_ptr points to the variable that informs the available size of the buffer
 *        arg_recog_ptr, and receives number of recognized read from the buffer to arg_recog_ptr.
 * 
 * @note This function may be called with arg_recog_ptr equal NULL to remove recognitions from the fifo. 
 */
static void numrecog_rb_read( T_numrecog_rbuffer * arg_rbuffer_ptr, 
                              T_numrecog_info * arg_recog_ptr,
                              unsigned * arg_size_ptr)
{
   int count;
   int i;

   count = (int)(*arg_size_ptr);
   pthread_mutex_lock(arg_rbuffer_ptr->lock);

   if(count > arg_rbuffer_ptr->count)
   {
	count = arg_rbuffer_ptr->count;
   }

   for(i=0;i<count;i++)
   {
    	if(arg_recog_ptr != NULL)
        {
	   memcpy((void *)&arg_recog_ptr[i],
	          (void *)&arg_rbuffer_ptr->buffer[arg_rbuffer_ptr->rd_pos],
	           sizeof(T_numrecog_info));
	}

	arg_rbuffer_ptr->rd_pos++;
        arg_rbuffer_ptr->count--;
 	
	if( arg_rbuffer_ptr->rd_pos = arg_rbuffer_ptr->size )
	{
	   arg_rbuffer_ptr->rd_pos = 0;
	 	
        }
   }   
	

   pthread_mutex_unlock(arg_rbuffer_ptr->lock);

   *arg_size_ptr = count; 
}

/**
 * @brief This function reads recognitions from the ring buffer
 *
 * @param[in] arg_rbuffer_ptr is the pointer to the ring buffer descriptor. See #T_numrecog_rbuffer.
 * @param[out] arg_recog_ptr is the pointer to the buffer of symbols to be written into ring buffer. 
	 See #T_numrecog_info.
 * @param[in,out] arg_size_ptr points to the variable that informs the size of arg_recog_ptr and 
 *       receives the number or recognitions actually writted.
 */
static void numrecog_rb_write( T_numrecog_rbuffer * arg_rbuffer_ptr,
			       T_numrecog_info * arg_recog_ptr,
			       unsigned * arg_size_ptr )
{
   int count;
   int available;
   int i;

   count = (int)(*arg_size_ptr);
   pthread_mutex_lock(arg_rbuffer_ptr->lock);
   available = arg_rbuffer_ptr->size - arg_rbuffer_ptr->count;
   
   if(count > available)
   {
      count = available;
   }

   for(i=0;i<count;i++)
   {
      memcpy((void *)&arg_rbuffer_ptr->buffer[arg_rbuffer_ptr->wd_pos],
             (void *)&arg_recog_ptr[i],
             sizeof(T_numrecog_info));
      
      arg_rbuffer_ptr->wd_pos++;
      arg_rbuffer_ptr->count++;	
      
      if( arg_rbuffer_ptr->wd_pos = arg_rbuffer_ptr->size )
      {
         arg_rbuffer_ptr->wd_pos = 0;	
      }
   }   
	
   pthread_mutex_unlock(arg_rbuffer_ptr->lock);

   *arg_size_ptr = count; 
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
   arg_context_ptr->rbuffer.count = 0;
   arg_context_ptr->rbuffer.size = 0;
   arg_context_ptr->rbuffer.buffer = NULL;

   arg_context_ptr->partial = 0;
   arg_context_ptr->last_clock = 0;
   arg_context_ptr->no_answere_timeout_clock = 0;
   arg_context_ptr->answere_timeout_clock = 0;

   ret= pthread_mutex_init(arg_context_ptr->rbuffer.lock,NULL);
   if(ret)
   {
      printf("\r\n ERROR INITIATING THE MUTEXT FOR RING BUFFER ACCESS");
      return ret;
   }

   arg_context_ptr->rbuffer.buffer = arg_buffer;
   arg_context_ptr->rbuffer.size = arg_size;

   arg_context_ptr->no_answere_timeout_clock = ceil(((double)arg_timeout_ms)/CLOCKS_PER_SEC);
   arg_context_ptr->answere_timeout_clock = ceil(((double)answere_timeout_ms)/CLOCKS_PER_SEC);
   arg_context_ptr->start_clock = clock();

   // Creating the thread that catches the recognized numbers
   ret = pthread_create(&arg_context_ptr->thread_id,
                        NULL,
                        numrecog_task,
                        (void*)arg_context_ptr);
   if(ret)
   {
      arg_context_ptr->rbuffer.buffer = NULL;
      arg_context_ptr->rbuffer.size = 0;
      arg_context_ptr->no_answere_timeout_clock = 0;
      arg_context_ptr->answere_timeout_clock = 0;
      printf("\r\n ERROR: THE THREAD CREATION FAILS : %d", ret);
      return ret;
   }

   return 0; 
}



/***************************************************************************************/
unsigned * numrecog_read( T_numrecog_cotext * arg_context_ptr, 
                          unsigned * arg_num_ptr,
                          bool * arg_timeout );
{
   unsigned count;
   int i;
   unsigned size;
   T_numrecog_info info;
   unsigned place;
   bool end;
   	
   if(arg_context_ptr == NULL)
	return EFAULT;

   if(arg_num_ptr == NULL)
	return EFAULT;

   if(arg_context_ptr->rbuffer.buffer == NULL)
	return EFAULT;

   end = false;

   // Get the counting of pending recognitions. 
   count = numrecog_rb_count(&arg_context_ptr->rbuffer);
   for(i=0;i++;i<count)
   {
      size = 1;
      // Get recognition from ring buffer
      numrecog_rb_read(&arg_context_ptr->rbuffer,&info,&size);
      if(size)
      {
         if(!arg_context_ptr->partial)
         {
            if((info.clock-arg_context_ptr->start_clock) <= arg_context_ptr->no_answere_timeout_clock)
            {
                arg_context_ptr->partial = info.number;
		arg_context_ptr->last_clock = info.clock;
		arg_context_ptr->last_place = floor(log10(info.number));
		if(info.number<20)
                { 
                   end = true;
                   break;
                }
            }
         }
         else
         {
	    if((info.clock-arg_context_ptr->last_clock) <= arg_context_ptr->answere_timeout_clock)
	    {
            	place = floor(log10(info.number));
		if(place<arg_context_ptr->last_place)
		{
		   arg_context_ptr->partial += info.number;
		   arg_context_ptr->last_clock = info.clock;
		   arg_context_ptr->last_place = place;
		}
		else
		{
                   end = true;
	           break;
                }

	        if(info.number<20)
		{
		   end =  true;
		   break;
		}    
            }
         }
      }
   }
   
   if(arg_context_ptr->partial)
   {
       if((clock()-arg_context_ptr->last_clock)>=arg_context_ptr->answere_timeout_clock)
       {
           end = true;
       }
   }
   else
   {
  	if(arg_timeout != NULL)
        {
           *arg_timeout = 
              (clock()-arg_context_ptr->start_clock) > arg_context_ptr->no_answere_timeout_clock 
        } 
   }
 
   if(end)
   {
      *arg_num_ptr = arg_context_ptr->partial;
      arg_context_ptr->partial = 0;
      return arg_num_ptr;
   }
   else
   {
      return NULL;
   }
}









  

   



}
