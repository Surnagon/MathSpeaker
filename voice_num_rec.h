/*********************************************************/


/**
 * @brief This type defines the context of the voice number recognizer
 */
typedef struct
{
	int no_answere_timeout; /*!< Timeout to consider the question not answered. */
	int completion_timeout; /*!< Timeout from the last number to consider the answer completed. */
}T_vnrecog_cotext;


/** 
 * @brief This function configures the voice-number recognizer.
 *
 * @param[out] arg_context_ptr is a pointer to the context that receives these settings. See #T_vnrecog_cotext.
 * @param[in] arg_timeout_ms  is the timeout in milliseconds to consider the question not answered.
 * @param[in] arg_num_timout_ms is the timeout after a number to consider the question answered. 
 *
 * @return This function returns 0 when the setting is successfully perfomed, otherwise it returns
 *         -1.
 */
int vnrecog_settings( T_vnrecog_cotext * arg_context_ptr, int arg_timeout_ms, int arg_num_timout_ms );

/**
 * @brief This function reads the number recognized.
 *
 * @param[in] arg_context_ptr is a pointer to the context that receives these settings. 
 *             This pointer must be previously configured by the function #vnrecog_settings. 
 *             See #T_vnrecog_cotext.
 * @param[out] arg_num_ptr is the address of the variable that receives the recognized number.
 *
 * @return This function returns the address arg_recog when a valid number has been recognized,
 * otherwise it returns NULL.
 *
 */
int * vnrecog_read(T_vnrecog_cotext * arg_context_ptr, int * arg_stop)

