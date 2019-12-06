#include "num_recog.h"
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/**
 * @brief This type defines the objecte that represents a number that
 * may be recognized from a file.
 *
 * Only numbers writted with one word are detected from the file.
 * The other numbers are composed by the read function #numrecog_read,
 * by adding compatible numbers whose the time interval between their 
 * detections are smaller than the confgured timeout.
 */
typedef struct
{
   unsigned value; /*!< Numeric value */
   char * word; /*!< Number writted in one word. Number writted*/
}T_numrecog;


/** Array of supported numbers that may be recognized from the file */
const T_numrecog numrecog_table[] =
{
   {.value = 100,	.word = "cem"},
   {.value = 90,	.word = "noventa"},
   {.value = 80,	.word = "oitenta"},
   {.value = 70,	.word = "setenta"},
   {.value = 60,	.word = "sessenta"},
   {.value = 50,	.word = "cinquenta"},
   {.value = 40,	.word = "quarenta"},
   {.value = 30,	.word = "trinta"},
   {.value = 20,	.word = "vinte"},
   {.value = 19,	.word = "dozenove"},
   {.value = 18,	.word = "dezoito"},
   {.value = 17,	.word = "dezesete"},
   {.value = 16,	.word = "dezeseis"},
   {.value = 15,	.word = "quinze"},
   {.value = 14,	.word = "quartoze"},
   {.value = 13,	.word = "treze"},
   {.value = 12,	.word = "doze"},
   {.value = 11,	.word = "onze"},
   {.value = 10,	.word = "dez"},
   {.value = 9,	.word = "nove"},
   {.value = 8,	.word = "oito"},
   {.value = 7,	.word = "sete"},
   {.value = 6,	.word = "seis"},
   {.value = 5,	.word = "cinco"},
   {.value = 4,	.word = "quatro"},
   {.value = 3,	.word = "três"},
   {.value = 2,	.word = "dois"},
   {.value = 1,	.word = "um" }
};

/**
 * @brief This function searches for a number in words in a string.
 *
 * @param[in] arg_str is the string where the wrods are searched.
 * @param[out] arg_value_ptr points to the variable that receives the
 *       recognized number. If no number is recognized it receives
 *       #UINT_MAX
 *
 * @return[in] This function returns the pointer to the first
 *        charecter after the detected number. 
 *
 * 
 * @note The numwords inside others words are ignored,
 *       e.g the numword "sete" inside "setenta" is ignored.
 */
static char * numrecog_strscan(char * arg_str, unsigned * arg_value_ptr )
{
   char * scan;
   unsigned scanlen;
   char * winchar;
   unsigned winlen;
   unsigned winvalue;
   int i;

   winchar = NULL;

   for(i=0;i<(sizeof(numrecog_table)/sizeof(numrecog_table[0]));i++)
   {
      scan = strstr(arg_str,numrecog_table[i].word);
      if(scan!=NULL)
      {
         scanlen = strlen(numrecog_table[i].word);
         //The current detection wins the scan if:
         //-It is the first number detected in the scan.
         //-It comes first in the scanned string than the winner detection.
         //-It starts in the same character that the winner detection and it has a larger length.
         if((winchar!=NULL)||(scan<winchar)||((scan==winchar)&&(scanlen>winlen)))
         {
            winchar = scan;
            winlen = scanlen;
            winvalue = numrecog_table[i].value;
         }
      }
   }
   
   if(winchar!=NULL)
   {
      *arg_value_ptr = winvalue;
      return (winchar + winlen);
   }
   else
   {
      *arg_value_ptr = UINT_MAX;
      return NULL;
   }
}

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
   while(1)
      printf("running");
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

   pthread_mutex_lock(&arg_rbuffer_ptr->lock);
   count = arg_rbuffer_ptr->count;
   pthread_mutex_unlock(&arg_rbuffer_ptr->lock);
   
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
   pthread_mutex_lock(&arg_rbuffer_ptr->lock);

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
   

   pthread_mutex_unlock(&arg_rbuffer_ptr->lock);

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
   pthread_mutex_lock(&arg_rbuffer_ptr->lock);
   available = arg_rbuffer_ptr->size - arg_rbuffer_ptr->count;
   
   if(count > available)
   {
      count = available;
   }

   for(i=0;i<count;i++)
   {
      memcpy((void *)&arg_rbuffer_ptr->buffer[arg_rbuffer_ptr->wr_pos],
             (void *)&arg_recog_ptr[i],
             sizeof(T_numrecog_info));
      
      arg_rbuffer_ptr->wr_pos++;
      arg_rbuffer_ptr->count++;   
      
      if( arg_rbuffer_ptr->wr_pos = arg_rbuffer_ptr->size )
      {
         arg_rbuffer_ptr->wr_pos = 0;   
      }
   }   
   
   pthread_mutex_unlock(&arg_rbuffer_ptr->lock);

   *arg_size_ptr = count; 
}

/***************************************************************************************/
int numrecog_start( T_numrecog_cotext * arg_context_ptr,
                    int arg_timeout_ms,
                    int arg_num_timout_ms,
                    T_numrecog_info * arg_buffer,
                    unsigned arg_size,
                    const char * arg_file_str,
                    char * arg_charbuffer_ptr,
                    unsigned arg_charlen )

{
   int ret;

   if((arg_context_ptr == NULL)||(arg_buffer==NULL)||(arg_charbuffer_ptr==NULL))
      return EFAULT;

   arg_context_ptr->rbuffer.wr_pos = 0;
   arg_context_ptr->rbuffer.rd_pos = 0;
   arg_context_ptr->rbuffer.count = 0;
   arg_context_ptr->rbuffer.size = 0;
   arg_context_ptr->rbuffer.buffer = NULL;
   arg_context_ptr->scan_file = NULL;
   arg_context_ptr->charbuffer = NULL;
   arg_context_ptr->charlen = 0;
   arg_context_ptr->partial = 0;
   arg_context_ptr->last_clock = 0;
   arg_context_ptr->no_answere_timeout_clock = 0;
   arg_context_ptr->answere_timeout_clock = 0;

   arg_context_ptr->scan_file = fopen(arg_file_str,"r");
   if(arg_context_ptr->scan_file==NULL)
      return errno;

   ret= pthread_mutex_init(&arg_context_ptr->rbuffer.lock,NULL);
   if(ret)
   {
      printf("\r\n ERROR INITIATING THE MUTEXT FOR RING BUFFER ACCESS");
      return ret;
   }

   arg_context_ptr->rbuffer.buffer = arg_buffer;
   arg_context_ptr->rbuffer.size = arg_size;
   arg_context_ptr->no_answere_timeout_clock = 
      ceil(((double)arg_timeout_ms)/CLOCKS_PER_SEC);
   arg_context_ptr->answere_timeout_clock = 
      ceil(((double)arg_num_timout_ms)/CLOCKS_PER_SEC);
   arg_context_ptr->start_clock = clock();
   arg_context_ptr->charbuffer = arg_charbuffer_ptr;
   arg_context_ptr->charlen = arg_charlen;


   // Creating the thread that catches the recognized numbers
   ret = pthread_create(&arg_context_ptr->thread_id,
                        NULL,
                        numrecog_task,
                        (void*)arg_context_ptr);
   if(ret)
   {
      arg_context_ptr->rbuffer.wr_pos = 0;
      arg_context_ptr->rbuffer.rd_pos = 0;
      arg_context_ptr->rbuffer.count = 0;
      arg_context_ptr->rbuffer.size = 0;
      arg_context_ptr->rbuffer.buffer = NULL;
      arg_context_ptr->scan_file = NULL;
      arg_context_ptr->charbuffer = NULL;
      arg_context_ptr->charlen = 0;
      arg_context_ptr->partial = 0;
      arg_context_ptr->last_clock = 0;
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
                          bool * arg_timeout )
{
   unsigned count;
   int i;
   unsigned size;
   T_numrecog_info info;
   unsigned place;
   bool end;
      
   if(arg_context_ptr == NULL)
   return NULL;

   if(arg_num_ptr == NULL)
   return NULL;

   if(arg_context_ptr->rbuffer.buffer == NULL)
   return NULL;

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
              (clock()-arg_context_ptr->start_clock) > 
              arg_context_ptr->no_answere_timeout_clock;
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









  

   



