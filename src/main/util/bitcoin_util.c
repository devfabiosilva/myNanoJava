#include <nano_java_common.h>

#define BITCOIN_UTIL_BLOCK_EXCEPTION_CLASS "org/mynanojava/exceptions/BitcoinUtilException"
#define THROW_BITCOIN_UTIL_EXCEPTION(msg, err) throwNewException(env, BITCOIN_UTIL_BLOCK_EXCEPTION_CLASS, msg, err)

int generateMasterKey_util(BITCOIN_SERIALIZE *master_key, int versionBytes, int entropy, const char *function_name)
{
   int err;
   uint32_t entropy_level;

   if ((versionBytes!=MAINNET_PRIVATE)&&(versionBytes!=TESTNET_PRIVATE)) {
      sprintf(msg, "%s: Invalid Bitcoin version bytes %d", function_name, err=5000);
      return err;
   }

   if ((entropy_level=f_sel_to_entropy_level((int)entropy))==0) {
      sprintf(msg, "%s: Invalid entropy number %d. Err %d", function_name, entropy, err=5001);
      return err;
   }

   f_random_attach(gen_rand_no_entropy_util);

   if ((err=f_generate_master_key(master_key, (size_t)versionBytes, entropy_level)))
      sprintf(msg, "f_generate_master_key @ %s: Can't generate master key %d", function_name, err);

   f_random_detach();

   return err;

}

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
   char *base58_str;

   if ((err=generateMasterKey_util(master_key=(BITCOIN_SERIALIZE *)msg, (int)versionBytes, (int)entropy, "generateMasterKey"))) {
      THROW_BITCOIN_UTIL_EXCEPTION(msg, err);
      return NULL;
   }

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

/*
 * Class:     org_mynanojava_bitcoin_Util
 * Method:    generateByteMasterKey
 * Signature: (II)[B
 */
JNIEXPORT jbyteArray JNICALL Java_org_mynanojava_bitcoin_Util_generateByteMasterKey(JNIEnv *env, jobject thisObj, jint versionBytes, jint entropy)
{
   int err;
   jbyteArray outByteArray;

   if ((err=generateMasterKey_util((BITCOIN_SERIALIZE *)msg, (int)versionBytes, (int)entropy, "generateByteMasterKey"))) {
      THROW_BITCOIN_UTIL_EXCEPTION(msg, err);
      return NULL;
   }

   if ((outByteArray=(*env)->NewByteArray(env, sizeof(BITCOIN_SERIALIZE))))
      (*env)->SetByteArrayRegion(env, outByteArray, 0, sizeof(BITCOIN_SERIALIZE), (const jbyte *)msg);
   else
      throwError(env, "generateByteMasterKey: Can't create JNI byte array to export Bitcoin Master Private Key");

   memset(msg, 0, sizeof(msg));
   return outByteArray;

}

/*
 * Class:     org_mynanojava_bitcoin_Util
 * Method:    toBase58
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_mynanojava_bitcoin_Util_toBase58(JNIEnv *env, jobject thisObj, jbyteArray val)
{
   int err;
   jsize jSize;
   jbyte *c_val;
   jstring res;

   if (!val) {
      THROW_BITCOIN_UTIL_EXCEPTION("toBase5: Missing val", 5010);
      return NULL;
   }

   jSize=(*env)->GetArrayLength(env, val);
   if ((*env)->ExceptionCheck(env)) {
      THROW_BITCOIN_UTIL_EXCEPTION("toBase5: Can't calculate 'val' size", 5011);
      return NULL;
   }

   if (!(c_val=(*env)->GetByteArrayElements(env, val, NULL))) {
      THROW_BITCOIN_UTIL_EXCEPTION("toBase5: Can't get 'val' in ByteArray", 5012);
      return NULL;
   }

   if ((err=f_encode_b58(msg, sizeof(msg), NULL, (uint8_t *)c_val, (size_t)jSize))) {
      sprintf(msg, "f_encode_b58 @ toBase58: Can't encode 'val' ByteArray to Base58 %d", err);
      THROW_BITCOIN_UTIL_EXCEPTION(msg, err);
   } else if (!(res=(*env)->NewStringUTF(env, (const char *)msg)))
      throwError(env, JAVA_ERR_PARSE_UTF8_STRING);

   memset(msg, 0, sizeof(msg));
   (*env)->ReleaseByteArrayElements(env, val, c_val, JNI_ABORT);
   return res;
}

/*
 * Class:     org_mynanojava_bitcoin_Util
 * Method:    privateKeyToWIF
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_mynanojava_bitcoin_Util_privateKeyToWIF(JNIEnv *env, jobject thisObj, jstring privateKey, jint wifType)
{
   int err;
   const char *c_private_key;
   char *dest;
   jbyteArray res;

   if (!privateKey) {
      THROW_BITCOIN_UTIL_EXCEPTION("privateKeyByteToWIF: Missing private key", 5013);
      return NULL;
   }

   if ((F_BITCOIN_WIF_MAINNET!=wifType)&&(F_BITCOIN_WIF_TESTNET!=wifType)) {
      THROW_BITCOIN_UTIL_EXCEPTION("privateKeyByteToWIF: Unknown WIF type", 5014);
      return NULL;
   }

   if (!(c_private_key=(*env)->GetStringUTFChars(env, privateKey, NULL))) {
      THROW_BITCOIN_UTIL_EXCEPTION("privateKeyByteToWIF: Can't parse Private Key to C char", 5015);
      return NULL;
   }

   res=NULL;

   if ((err=array32bytes_str_to_hex_util((uint8_t *)msg, c_private_key))) {
      THROW_BITCOIN_UTIL_EXCEPTION("privateKeyByteToWIF: Invalid private key size", err);
      goto Java_org_mynanojava_bitcoin_Util_privateKeyByteToWIF_EXIT2;
   }

   if ((err=f_private_key_to_wif(dest=(msg+(sizeof(msg)>>1)), sizeof(msg)>>1, NULL, (uint8_t)wifType, (uint8_t *)msg))) {
      sprintf(msg, "f_private_key_to_wif @ privateKeyByteToWIF: Can't parse private key to Byte Array WIF %d", err);
      THROW_BITCOIN_UTIL_EXCEPTION(msg, err);
   } else if (!(res=(*env)->NewStringUTF(env, (const char *)dest)))
      throwError(env, JAVA_ERR_PARSE_UTF8_STRING);

Java_org_mynanojava_bitcoin_Util_privateKeyByteToWIF_EXIT2:
   memset(msg, 0, sizeof(msg));

Java_org_mynanojava_bitcoin_Util_privateKeyByteToWIF_EXIT1:
   (*env)->ReleaseStringUTFChars(env, privateKey, c_private_key);

   return res;
}

