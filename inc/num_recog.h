#ifndef numrecog
#define numrecog

#include <errno.h>
#include <pthread.h>
#include <time.h>


/** @file num_recog.h
 *  This file provides a interface for number recognition from voice.
 */


/**
 * @brief This type defines the information set of each number recognition.
 */
typedef struct
{
   int number; /*!< Detected number */
   int detection_time; /*!< Monotonic time at the detection instant */
}T_numrecog_info;

/**
 * @brief This type defines the ring buffer of recognized numbers
 */
typedef struct
{
   T_numrecog_info * buffer; /*!< The buffer of recognizied */
   unsigned size; /*!< The buffer size in number of recognitions */
   unsigned rd_pos; /*!< Position of next reading */
   unsigned wr_pos; /*!< Position of next writing */
   pthread_mutex_t lock; /*!< Mutex for safe access */
}T_numrecog_rbuffer;


/**
 * @brief This type defines the context of the number recognition
 */
typedef struct
{
   int no_answere_timeout_ms; /*!< Timeout to consider the question not answered. */
   int answere_timeout_ms; /*!< Timeout to consider the question answered */
   T_numrecog_rbuffer rbuffer; /*!< Ring buffer of recognized numbers */
}T_numrecog_cotext;


/** 
 * @brief This function configures the number recognition
 *
 * @param[out] arg_context_ptr is a pointer to the context that receives these settings. 
 *       See #T_numrecog_cotext.
 * @param[in] arg_timeout_ms  is the timeout in milliseconds to consider "no recognition"
 * @param[in] arg_num_timout_ms is the timeout the end the recognition after the recognition
 *       of a number that may be a complement, i.e "vinte" e "cinco".
 * @param[in] arg_buffer is the buffer available to store the recognized numbers. 
 *       See #T_numrecog_info.
 * @param[in] arg_size is the size of arg_bufer in number of elements #T_numrecog_info.
 * 
 * @return This function returns 0 when the setting is successfully perfomed, otherwise it 
 *         returns:
 *         - EFAULT, if arg_context_ptr is a invalid address;
 *         - The error code returned by pthread_mutex_init, if it fails initializing 
 *           ring buffer mutex.
 *         - The error code returned by pthread_create, if it fails creating the thread
 *           for regonition.
 */
int numrecog_start( T_numrecog_cotext * arg_context_ptr,
                    int arg_timeout_ms,
                    int arg_num_timout_ms,
                    T_numrecog_info * arg_buffer,
                    unsigned arg_size );

/**
 * @brief This function reads the recognized numbers.
 *
 * @param[in] arg_context_ptr is a pointer to the context that receives these settings. 
 *       This pointer must be previously configured by the function #numrecog_start. 
 *       See #T_numrecog_cotext.
 * @param[out] arg_num_ptr is the address of the variable that receives the recognized number.
 *
 * @return This function returns the address arg_recog when a valid number has been 
 *         recognized, otherwise it returns NULL.
 *
 */
int * numrecog_read( T_numrecog_cotext * arg_context_ptr, 
                     int * arg_stop);

/**
 * @brief This function stops the voice-number recognition.
 * 
 * @param[in] arg_context_ptr is a pointer to the context of the number recognition
 *
 * @return This function returns 0 when the stopping is successfully performed, otherwise
 *         it returns -1.
 */
int numrecog_stop( T_numrecog_cotext * arg_context_ptr );


#endif
