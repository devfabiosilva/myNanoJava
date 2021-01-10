#include <nano_java_common.h>

#define BITCOIN_UTIL_BLOCK_EXCEPTION_CLASS "org/mynanojava/exceptions/BitcoinUtilException"
#define THROW_BITCOIN_UTIL_EXCEPTION(msg, err) throwNewException(env, BITCOIN_UTIL_BLOCK_EXCEPTION_CLASS, msg, err)

/*
 * Class:     org_mynanojava_bitcoin_Util
 * Method:    generateMasterKey
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_mynanojava_bitcoin_Util_generateMasterKey(JNIEnv *env, jobject thisObj, jint versionBytes, jint entropy)
{
   int err;
   jstring res;
   BITCOIN_SERIALIZE *master_key;
   uint32_t entropy_level;
   char *base58_str;

   if ((versionBytes!=MAINNET_PRIVATE)&&(versionBytes!=TESTNET_PRIVATE)) {
      THROW_BITCOIN_UTIL_EXCEPTION("generateMasterKey: Invalid Bitcoin version bytes", 5000);
      return NULL;
   }

   if ((entropy_level=f_sel_to_entropy_level((int)entropy))==0) {
      sprintf(msg, "generateMasterKey: Invalid entropy number %d", entropy);
      THROW_BITCOIN_UTIL_EXCEPTION(msg, 5001);
      return NULL;
   }

   f_random_attach(gen_rand_no_entropy_util);

   if ((err=f_generate_master_key(master_key=(BITCOIN_SERIALIZE *)msg, (size_t)versionBytes, entropy_level))) {
      sprintf(msg, "f_generate_master_key @ generateMasterKey: Can't generate master key %d", err);
      THROW_BITCOIN_UTIL_EXCEPTION(msg, err);
   }

   f_random_detach();

   if (err)
      return NULL;

   if ((err=f_encode_b58(base58_str=(msg+sizeof(BITCOIN_SERIALIZE)), sizeof(msg)-sizeof(BITCOIN_SERIALIZE), NULL,
      (uint8_t *)master_key, sizeof(BITCOIN_SERIALIZE)))) {
      sprintf(msg, "f_encode_b58 @ generateMasterKey: Can't encode master key to Base58 %d", err);
      THROW_BITCOIN_UTIL_EXCEPTION(msg, err);
      return NULL;
   }

   if (!(res=(*env)->NewStringUTF(env, (const char *)base58_str)))
      throwError(env, JAVA_ERR_PARSE_UTF8_STRING);

   memset(msg, 0, sizeof(msg));

   return res;
}

