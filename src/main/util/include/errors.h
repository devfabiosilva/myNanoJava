//seg abr 26 20:56:00 -03 2021 

#define ERROR_SUCCESS 0
#define ERROR_GEN_TOKEN_NO_RAND_NUM_GEN 3858

//nano_base_32_2_hex
#define ERROR_INVALID_NANO_ADDRESS_VERIFY_CHKSUM 23

enum f_nano_account_or_pk_string_to_pk_util_err_t {
   NANO_ACCOUNT_TO_PK_OK = 0,
   NANO_ACCOUNT_TO_PK_OVFL = 8100,
   NANO_ACCOUNT_TO_PK_NULL_STRING,
   NANO_ACCOUNT_WRONG_PK_STR_SZ,
   NANO_ACCOUNT_WRONG_HEX_STRING,
   NANO_ACCOUNT_BASE32_CONVERT_ERROR,
   NANO_ACCOUNT_TO_PK_WRONG_ACCOUNT_LEN
};

//valid_raw_balance
#define INVALID_RAW_BALANCE 8893

//f_nano_seed_to_bip39
#define CANT_OPEN_DICTIONARY_FILE 2580

