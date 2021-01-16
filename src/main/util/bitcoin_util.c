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

// WARNING: 0 if success
int is_valid_master_key_util(uint8_t *priv_key_type, const uint8_t *master_key)
{
   if (memcmp(master_key, F_VERSION_BYTES[(size_t)(*priv_key_type=MAINNET_PRIVATE)], 4))
      return memcmp(master_key, F_VERSION_BYTES[(size_t)(*priv_key_type=TESTNET_PRIVATE)], 4);

   return 0;
}

/*
 * Class:     org_mynanojava_bitcoin_Util
 * Method:    byteMasterPrivateKeyToWIF
 * Signature: ([BI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_mynanojava_bitcoin_Util_byteMasterPrivateKeyToWIF(JNIEnv *env, jobject thisObj, jbyteArray masterKey, jlong index)
{
   int err;
   char priv_key_type, *wif;
   jstring res;
   jbyte *c_master_key;
   jsize jSize;

   if (index<0) {
      THROW_BITCOIN_UTIL_EXCEPTION("byteMasterPrivateKeyToWIF: Invalid. Can't be negative index", 5029);
      return NULL;
   }

   if (index>((uint32_t)((int)-1))) {
      sprintf(msg, "byteMasterPrivateKeyToWIF: Invalid index range = %lu", (unsigned long int)index);
      THROW_BITCOIN_UTIL_EXCEPTION(msg, 5030);
      return NULL;
   }

   if (!masterKey) {
      THROW_BITCOIN_UTIL_EXCEPTION("byteMasterPrivateKeyToWIF: Missing master key", 5025);
      return NULL;
   }

   jSize=(*env)->GetArrayLength(env, masterKey);
   if ((*env)->ExceptionCheck(env)) {
      THROW_BITCOIN_UTIL_EXCEPTION("byteMasterPrivateKeyToWIF: Can't calculate 'masterKey' size", 5026);
      return NULL;
   }

   if ((jSize!=sizeof(BITCOIN_SERIALIZE))) {
      sprintf(msg, "byteMasterPrivateKeyToWIF: Wrong master key size = %d.", (int)jSize);
      THROW_BITCOIN_UTIL_EXCEPTION(msg, 5027);
      return NULL;
   }

   if (!(c_master_key=(*env)->GetByteArrayElements(env, masterKey, NULL))) {
      THROW_BITCOIN_UTIL_EXCEPTION("byteMasterPrivateKeyToWIF: Can't get 'masterKey' in ByteArray", 5028);
      return NULL;
   }

   res=NULL;

   if (is_valid_master_key_util(&priv_key_type, (const uint8_t *)c_master_key)) {
      THROW_BITCOIN_UTIL_EXCEPTION("is_valid_master_key_util @ byteMasterPrivateKeyToWIF: It is not a Master Key", 5031);
      goto Java_org_mynanojava_bitcoin_Util_byteMasterPrivateKeyToWIF_EXIT1;
   }

   if ((err=f_bip32_to_public_key_or_private_key((uint8_t *)msg, NULL, (uint32_t)index, (const void *)c_master_key, 0))) {
      sprintf(msg, "f_bip32_to_public_key_or_private_key @ byteMasterPrivateKeyToWIF: Can't extract private key from byte master key %d", err);
      THROW_BITCOIN_UTIL_EXCEPTION(msg, err);
      goto Java_org_mynanojava_bitcoin_Util_byteMasterPrivateKeyToWIF_EXIT2;
   }

   if ((err=f_private_key_to_wif(wif=(msg+(sizeof(msg)>>1)), sizeof(msg)>>1, NULL, 
      (priv_key_type==MAINNET_PRIVATE)?F_BITCOIN_WIF_MAINNET:F_BITCOIN_WIF_TESTNET, (uint8_t *)&msg[1]))) {
      sprintf(msg, "f_private_key_to_wif @ byteMasterPrivateKeyToWIF: Can't parse index %u from Master Key. Err = %d", (unsigned int)index, err);
      THROW_BITCOIN_UTIL_EXCEPTION(msg, err);
   } else if (!(res=(*env)->NewStringUTF(env, (const char *)wif)))
      throwError(env, JAVA_ERR_PARSE_UTF8_STRING);

Java_org_mynanojava_bitcoin_Util_byteMasterPrivateKeyToWIF_EXIT2:
   memset(msg, 0, sizeof(msg));

Java_org_mynanojava_bitcoin_Util_byteMasterPrivateKeyToWIF_EXIT1:
   (*env)->ReleaseByteArrayElements(env, masterKey, c_master_key, JNI_ABORT);

   return res;
}
/*
 * Class:     org_mynanojava_bitcoin_Util
 * Method:    byteMasterPrivateKeyToMasterPublicKey
 * Signature: ([B)[B
 */
JNIEXPORT jbyteArray JNICALL Java_org_mynanojava_bitcoin_Util_byteMasterPrivateKeyToMasterPublicKey(JNIEnv *env , jobject thisObj, jbyteArray masterKey)
{
   int err;
   jbyteArray outByteArray;
   jbyte *c_master_key;

   if (!masterKey) {
      THROW_BITCOIN_UTIL_EXCEPTION("byteMasterPrivateKeyToMasterPublicKey: Missing master key", 5040);
      return NULL;
   }

   if (!(c_master_key=(*env)->GetByteArrayElements(env, masterKey, NULL))) {
      THROW_BITCOIN_UTIL_EXCEPTION("byteMasterPrivateKeyToMasterPublicKey: Unable to get 'masterKey'", 5041);
      return NULL;
   }

   if ((err=f_xpriv2xpub((void *)msg, sizeof(msg), NULL, (void *)c_master_key, 0))) {
      outByteArray=NULL;
      sprintf(msg, "f_xpriv2xpub @ byteMasterPrivateKeyToMasterPublicKey: Can't extract master public key from master private key %d", err);
      THROW_BITCOIN_UTIL_EXCEPTION(msg, err);
      goto Java_org_mynanojava_bitcoin_Util_byteMasterPrivateKeyToMasterPublicKey_EXIT1;
   }

   if ((outByteArray=(*env)->NewByteArray(env, sizeof(BITCOIN_SERIALIZE))))
      (*env)->SetByteArrayRegion(env, outByteArray, 0, sizeof(BITCOIN_SERIALIZE), (const jbyte *)msg);
   else
      throwError(env, "byteMasterPrivateKeyToMasterPublicKey: Can't create JNI byte array to export Bitcoin Master Public Key");

Java_org_mynanojava_bitcoin_Util_byteMasterPrivateKeyToMasterPublicKey_EXIT1:
   (*env)->ReleaseByteArrayElements(env, masterKey, c_master_key, JNI_ABORT);

   return outByteArray;
}
/*
 * Class:     org_mynanojava_bitcoin_Util
 * Method:    byteMasterPublicKeyToBTC_Address
 * Signature: ([BJ)Ljava/lang/String;
 */
/*
JNIEXPORT jstring JNICALL Java_org_mynanojava_bitcoin_Util_byteMasterPublicKeyToBTC_1Address(JNIEnv *env, jobject thisObj, jbyteArray masterPublicKey, jlong index)
{
   int err;
   jstring res;
TODO
   return res;
}
*/

/*
 * Class:     org_mynanojava_bitcoin_Util
 * Method:    byteMasterPrivateKeyToBTC_Address
 * Signature: ([BJ)Ljava/lang/String;
 */
/*
JNIEXPORT jstring JNICALL Java_org_mynanojava_bitcoin_Util_byteMasterPrivateKeyToBTC_1Address(JNIEnv *env, jobject thisObj, jbyteArray masterKey, jlong index)
{
   int err;
   jstring res;
TODO
   return res;
}
*/
