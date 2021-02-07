#include <nano_java_common.h>

//java -Djava.library.path=. -jar
////https://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/functions.html
//https://developer.android.com/training/articles/perf-jni?hl=pt-br

#define NANO_BLOCK_EXCEPTION_CLASS "org/mynanojava/exceptions/NanoBlockException"
#define BALANCE_EXCEPTION_CLASS "org/mynanojava/exceptions/BalanceException"
#define MY_NANO_EMBEDDED_ERROR "myNanoEmbedded C library error in function \"%s\" %d"
#define NANO_BLOCK_CLASS_PATH "org/mynanojava/blockchain/NanoBlock"
#define CANT_FIND_NANO_BLOCK_ERROR "%s: Can't find class org.mynanojava.blockchain.NanoBlock"
#define NANO_KEY_PAIR_EXCEPTION_CLASS "org/mynanojava/exceptions/NanoKeyPairException"
#define NANO_KEY_PAIR_CLASS_PATH "org/mynanojava/wallet/NanoKeyPair"
#define THROW_NANO_BLOCK_EXCEPTION(env, msg, err) throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, msg, err)

void gen_rand_no_entropy_util(void *output, size_t output_len)
{
   FILE *f;
   size_t rnd_sz, left;

   if (!(f=fopen("/dev/urandom", "r")))
      return;

   rnd_sz=0;
   left=output_len;

   while ((rnd_sz+=fread(output+rnd_sz, 1, left, f))<output_len)
      left-=rnd_sz;

   fclose(f);

   return;

}

inline int array32bytes_str_to_hex_util(uint8_t *out, const char *in)
{
   if (strnlen(in, 65)!=64)
      return -21;

   return f_str_to_hex(out, (char *)in);
}

int nano_java_raw128_str_to_bin_util(uint8_t *dest, const char *str)
{
   if (strnlen(str, 33)!=32)
      return -10;

   return f_str_to_hex(dest, (char *)str);
}

JNIEXPORT jstring JNICALL Java_org_mynanojava_MyNanoJava_license(JNIEnv *env, jobject thisObject)
{
   return (*env)->NewStringUTF(env, LICENSE);
}

JNIEXPORT jstring JNICALL Java_org_mynanojava_MyNanoJava_nano_1add_1sub(JNIEnv *env, jobject thisObject, jstring a, jstring b, jint mode)
{
   int err;
   jstring ret;
   const char *in_A_Str, *in_B_Str, *p_A_str, *p_B_str;
   char c_ret[2*F_RAW_STR_MAX_SZ]; // ALERT ! It MUST BE 2x MAX size due to MBEDTLS. Never CHANGE IT to lower

   if (!a) {
      throwError(env, "A value is NULL");
      return NULL;
   }

   if (!b) {
      throwError(env, "B value is NULL");
      return NULL;
   }

   if (!(in_A_Str = (*env)->GetStringUTFChars(env, a, NULL))) {
      throwError(env, "Get UTF A error");
      return NULL;
   }

   ret = NULL;

   if (!(in_B_Str = (*env)->GetStringUTFChars(env, b, NULL))) {
      throwError(env, "Get UTF B error");
      goto Java_org_mynanojava_MyNanoJava_nano_1add_1sub_EXIT1;
   }

   if ((uint32_t)mode&F_NANO_A_RAW_128) {
      if ((err=nano_java_raw128_str_to_bin_util((uint8_t *)(p_A_str=(const char *)msg), in_A_Str))) {
         sprintf(msg, "Hex A value: "MY_NANO_EMBEDDED_ERROR, "nano_java_raw128_str_to_bin_util", err);
         throwError(env, msg);
         goto Java_org_mynanojava_MyNanoJava_nano_1add_1sub_EXIT2;
      }
   } else
      p_A_str=in_A_Str;

   if ((uint32_t)mode&F_NANO_B_RAW_128) {
      if ((err=nano_java_raw128_str_to_bin_util((uint8_t *)(p_B_str=(const char *)(msg+sizeof(f_uint128_t))), in_B_Str))) {
         sprintf(msg, "Hex B value: "MY_NANO_EMBEDDED_ERROR, "nano_java_raw128_str_to_bin_util", err);
         throwError(env, msg);
         goto Java_org_mynanojava_MyNanoJava_nano_1add_1sub_EXIT2;
      }
   } else
      p_B_str=in_B_Str;

   if ((err = f_nano_add_sub((void *)c_ret, (void *)p_A_str, (void *)p_B_str, (uint32_t)mode))) {
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "f_nano_add_sub", err);
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_nano_1add_1sub_EXIT2;
   }

   if (!(ret = (*env)->NewStringUTF(env, 
      ((uint32_t)mode&F_NANO_RES_RAW_128)?((const char *)fhex2strv2(msg, (const void *)c_ret, sizeof(f_uint128_t), 0)):(const char *)c_ret)))
      throwError(env, JAVA_ERR_PARSE_UTF8_STRING);

Java_org_mynanojava_MyNanoJava_nano_1add_1sub_EXIT2:
   (*env)->ReleaseStringUTFChars(env, b, in_B_Str);

Java_org_mynanojava_MyNanoJava_nano_1add_1sub_EXIT1:
   (*env)->ReleaseStringUTFChars(env, a, in_A_Str);

   return ret;
}

JNIEXPORT jbyteArray JNICALL Java_org_mynanojava_MyNanoJava_nano_1create_1block(
   JNIEnv *env, jobject thisObject, jstring account, jstring previous, jstring representative, jstring balance,
   jint balance_type, jstring value_to_send_or_receive, jint value_to_send_or_receive_type, jstring link, jint direction
)
{
   int err;
   const char *c_account, *c_previous, *c_representative, *c_balance, *c_value_to_send_or_receive, *c_link, *p_balance, *p_val_send_rec;
   uint32_t types;
   F_BLOCK_TRANSFER *nano_block;
   jbyteArray outByteArray;

   if (!account) {
      throwError(env, "Missing account");
      return NULL;
   }

   if (!representative) {
      throwError(env, "Missing representative");
      return NULL;
   }

   if (!balance) {
      throwError(env, "Missing balance");
      return NULL;
   }

   if (!value_to_send_or_receive) {
      throwError(env, "Missing value to send or receive");
      return NULL;
   }

   if (!link) {
      throwError(env, "Missing destination wallet or link to open block");
      return NULL;
   }

   if (!(c_account=(*env)->GetStringUTFChars(env, account, NULL))) {
      throwError(env, "Get UTF account error");
      return NULL;
   }

   outByteArray=NULL;

   if (previous) {
      if (!(c_previous=(*env)->GetStringUTFChars(env, previous, NULL))) {
         throwError(env, "Get UTF previous error");
         goto Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT1;
      }
   } else
      c_previous=NULL;

   if (!(c_representative=(*env)->GetStringUTFChars(env, representative, NULL))) {
      throwError(env, "Get UTF representative error");
      goto Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT2;
   }

   if (!(c_balance=(*env)->GetStringUTFChars(env, balance, NULL))) {
      throwError(env, "Get UTF balance error");
      goto Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT3;
   }

   if (!(c_value_to_send_or_receive=(*env)->GetStringUTFChars(env, value_to_send_or_receive, NULL))) {
      throwError(env, "Get UTF value to send/receive error");
      goto Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT4;
   }

   if (!(c_link=(*env)->GetStringUTFChars(env, link, NULL))) {
      throwError(env, "Get UTF send wallet address or link error");
      goto Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT5;
   }

   p_balance=c_balance;

   if ((types=(uint32_t)(balance_type|value_to_send_or_receive_type))&F_BALANCE_RAW_128)
      if ((err=nano_java_raw128_str_to_bin_util((uint8_t *)(p_balance=(const char *)msg), c_balance))) {
         sprintf(msg, "Error when parse hex balance to bin. "MY_NANO_EMBEDDED_ERROR, "nano_java_raw128_str_to_bin_util", err);
         throwError(env, msg);
         goto Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT6;
      }

   p_val_send_rec=c_value_to_send_or_receive;

   if (types&F_VALUE_SEND_RECEIVE_RAW_128)
      if ((err=nano_java_raw128_str_to_bin_util((uint8_t *)(p_val_send_rec=((const char *)&msg[sizeof(f_uint128_t)])), c_value_to_send_or_receive))) {
         sprintf(msg, "Error when parse hex send or receive value to bin. "MY_NANO_EMBEDDED_ERROR, "nano_java_raw128_str_to_bin_util", err);
         throwError(env, msg);
         goto Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT6;
      }

   if ((err=nano_create_block_dynamic(&nano_block, c_account, 0, c_previous, 0, c_representative, 0, p_balance, p_val_send_rec,
      (uint32_t)types, c_link, 0, direction))) {
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "nano_create_block_dynamic", err);
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT6;
   }

   if (!(outByteArray=(*env)->NewByteArray(env, sizeof(F_BLOCK_TRANSFER)))) {
      throwError(env, "Can't create JNI byte array");
      goto Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT7;
   }

   (*env)->SetByteArrayRegion(env, outByteArray, 0, sizeof(F_BLOCK_TRANSFER), (const jbyte *)nano_block);

Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT7:
   memset(nano_block, 0, sizeof(F_BLOCK_TRANSFER));
   free(nano_block);

Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT6:
   (*env)->ReleaseStringUTFChars(env, link, c_link);

Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT5:
   (*env)->ReleaseStringUTFChars(env, value_to_send_or_receive, c_value_to_send_or_receive);

Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT4:
   (*env)->ReleaseStringUTFChars(env, balance, c_balance);

Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT3:
   (*env)->ReleaseStringUTFChars(env, representative, c_representative);

Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT2:
   if (c_previous)
      (*env)->ReleaseStringUTFChars(env, previous, c_previous);

Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT1:
   (*env)->ReleaseStringUTFChars(env, account, c_account);

   return outByteArray;
}

JNIEXPORT jstring JNICALL Java_org_mynanojava_MyNanoJava_nanoBlockToJSON(JNIEnv *env, jobject thisObject, jbyteArray nanoBlock)
{
   int err;
   jbyte *c_byte_array;
   jsize jSize;
   jstring ret;

   if (!nanoBlock) {
      throwError(env, "nanoBlock can NOT be NULL");
      return NULL;
   }

   if (!(c_byte_array=(*env)->GetByteArrayElements(env, nanoBlock, JNI_FALSE))) {
      throwError(env, "Unable to get Nano Block");
      return NULL;
   }

   ret=NULL;

   if ((jSize=(*env)->GetArrayLength(env, nanoBlock))!=F_BLOCK_TRANSFER_SIZE) {
      sprintf(msg, "Wrong Nano Block size %d", (int)jSize);
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_nanoBlockToJSON_EXIT1;
   }

   if ((err=f_nano_block_to_json(msg, NULL, sizeof(msg), (F_BLOCK_TRANSFER *)c_byte_array))) {
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "f_nano_block_to_json", err);
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_nanoBlockToJSON_EXIT1;
   }

   if (!(ret=(*env)->NewStringUTF(env, msg)))
      throwError(env, "Error when parse Nano block to JSON");

Java_org_mynanojava_MyNanoJava_nanoBlockToJSON_EXIT1:
   (*env)->ReleaseByteArrayElements(env, nanoBlock, c_byte_array, JNI_ABORT);
   return ret;
}

JNIEXPORT jbyteArray JNICALL Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock(JNIEnv *env, jobject thisObj, jbyteArray block, jstring workerAccount,
   jstring workerFee, jint workerFeeType, jstring workerRepresentative)
{
   int err;
   F_BLOCK_TRANSFER *p2pow_block;
   jbyte *c_byte_array;
   const char *p_worker_rep, *c_worker_account, *c_worker_fee;
   jbyteArray outByteArray;

   if (!block) {
      throwError(env, "Missing user block");
      return NULL;
   }

   if (!(c_byte_array=(*env)->GetByteArrayElements(env, block, JNI_FALSE))) {
      throwError(env, "P2PoW error when parse Nano block");
      return NULL;
   }

   outByteArray=NULL;

   if ((err=(int)(*env)->GetArrayLength(env, block))!=F_BLOCK_TRANSFER_SIZE) {
      sprintf(msg, "P2PoW: Wrong Nano Block size %d", err);
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT1;
   }

   if (!workerAccount) {
      throwError(env, "Missing worker account");
      goto Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT1;
   }

   if (!(c_worker_account=(*env)->GetStringUTFChars(env, workerAccount, NULL))) {
      throwError(env, "P2PoW error when parse worker account");
      goto Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT1;
   }

   if (!workerFee) {
      throwError(env, "Missing worker fee");
      goto Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT2;
   }

   if (!(c_worker_fee=(*env)->GetStringUTFChars(env, workerFee, NULL))) {
      throwError(env, "P2PoW error when parse worker fee");
      goto Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT2;
   }

   if (!workerRepresentative)
      p_worker_rep=NULL;
   else if (!(p_worker_rep=(*env)->GetStringUTFChars(env, workerRepresentative, NULL))) {
      throwError(env, "Error when parse worker representative");
      goto Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT3;
   }

   if ((err=nano_create_p2pow_block_dynamic(&p2pow_block, (F_BLOCK_TRANSFER *)c_byte_array, c_worker_account, 0, c_worker_fee, (uint32_t)workerFeeType,
      p_worker_rep, 0))) {
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "nano_create_p2pow_block_dynamic", err);
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT4;
   }

   if (!(outByteArray=(*env)->NewByteArray(env, F_P2POW_BLOCK_TRANSFER_SIZE))) {
      throwError(env, "P2PoW error: Can't create JNI byte array");
      goto Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT5;
   }

   (*env)->SetByteArrayRegion(env, outByteArray, 0, F_P2POW_BLOCK_TRANSFER_SIZE, (const jbyte *)p2pow_block);

Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT5:
   memset(p2pow_block, 0, 2*sizeof(F_BLOCK_TRANSFER));
   free(p2pow_block);

Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT4:
   if (p_worker_rep)
      (*env)->ReleaseStringUTFChars(env, workerRepresentative, p_worker_rep);

Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT3:
   (*env)->ReleaseStringUTFChars(env, workerFee, c_worker_fee);

Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT2:
   (*env)->ReleaseStringUTFChars(env, workerAccount, c_worker_account);

Java_org_mynanojava_MyNanoJava_nanoP2PoWBlock_EXIT1:
   (*env)->ReleaseByteArrayElements(env, block, c_byte_array, JNI_ABORT);

   return outByteArray;
}

#define NANO_P2POW_BUFFER_SZ (size_t)2048
JNIEXPORT jstring JNICALL Java_org_mynanojava_MyNanoJava_p2powToJson(JNIEnv *env, jobject thisObj, jbyteArray nanoBlock)
{
   int err;
   jbyte *c_byte_array;
   char *buffer;
   jstring ret;

   if (!nanoBlock) {
      throwError(env, "Missing Nano Block");
      return NULL;
   }

   if ((err=(int)(*env)->GetArrayLength(env, nanoBlock))!=F_P2POW_BLOCK_TRANSFER_SIZE) {
      sprintf(msg, "P2PoW to JSON: Wrong Nano Block size %d", err);
      throwError(env, msg);
      return NULL;
   }

   if (!(c_byte_array=(*env)->GetByteArrayElements(env, nanoBlock, JNI_FALSE))) {
      throwError(env, "P2PoW to JSON error. Can't parse byte array");
      return NULL;
   }

   ret=NULL;

   if (!(buffer=malloc(NANO_P2POW_BUFFER_SZ))) {
      throwError(env, "P2PoW Malloc error");
      goto Java_org_mynanojava_MyNanoJava_p2powToJson_EXIT1;
   }

   if ((err=f_nano_p2pow_to_JSON(buffer, NULL, NANO_P2POW_BUFFER_SZ, (F_BLOCK_TRANSFER *)c_byte_array))) {
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "f_nano_p2pow_to_JSON", err);
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_p2powToJson_EXIT2;
   }

   if (!(ret=(*env)->NewStringUTF(env, buffer)))
      throwError(env, "Error when parse P2PoW block to JSON");;

Java_org_mynanojava_MyNanoJava_p2powToJson_EXIT2:
   memset(buffer, 0, NANO_P2POW_BUFFER_SZ);
   free(buffer);

Java_org_mynanojava_MyNanoJava_p2powToJson_EXIT1:
   (*env)->ReleaseByteArrayElements(env, nanoBlock, c_byte_array, JNI_ABORT);

   return ret;
}

int setByteArrayFieldId_util(
   JNIEnv *env,
   jobject thisObject,
   jclass class,
   const char *fieldId,
   uint8_t *c_byte_array,
   size_t c_byte_array_sz
)
{

   int err;
   jfieldID field;
   jbyteArray byte_array;

   if (!(field=(*env)->GetFieldID(env, class, fieldId, NANO_JAVA_BYTE_ARRAY_TYPE)))
      return -45;

   if (!(byte_array=(*env)->NewByteArray(env, c_byte_array_sz)))
      return -46;

   err=0;
   (*env)->SetByteArrayRegion(env, byte_array, 0, c_byte_array_sz, (const jbyte *)c_byte_array);
   if ((*env)->ExceptionCheck(env)) {
      err=-47;
      goto setByteArrayFieldId_util_EXIT1;
   }

   (*env)->SetObjectField(env, thisObject, field, byte_array);
   if ((*env)->ExceptionCheck(env))
      err=-48;

setByteArrayFieldId_util_EXIT1:
   (*env)->DeleteLocalRef(env, byte_array);
   if ((*env)->ExceptionCheck(env))
      err=-49;

   return err;
}

int getByteArrayFieldId_util(
   uint8_t **c_byte_array,
   size_t *c_byte_array_sz,
   jobject *fieldObj,
   JNIEnv *env,
   jobject thisObject,
   jclass class,
   const char *fieldId
)
{

   jfieldID field;

   *c_byte_array=NULL;

   if (!(field=(*env)->GetFieldID(env, class, fieldId, NANO_JAVA_BYTE_ARRAY_TYPE)))
      return -80;

   if (!(*fieldObj=(*env)->GetObjectField(env, thisObject, field)))
      return -81;

   if (!(*c_byte_array=(*env)->GetByteArrayElements(env, *fieldObj, JNI_FALSE)))
      return -82;

   *c_byte_array_sz=(size_t)(*env)->GetArrayLength(env, *fieldObj);

   if ((*env)->ExceptionCheck(env))
      return -83;

   return 0;
}

int getLongFieldUtil(
   jlong *long_value,
   JNIEnv *env,
   jobject thisObject,
   jclass class,
   const char *fieldId
)
{

   jfieldID field;

   if (!(field=(*env)->GetFieldID(env, class, fieldId, NANO_JAVA_LONG_TYPE)))
      return -90;

   *long_value=(*env)->GetLongField(env, thisObject, field);

   if ((*env)->ExceptionCheck(env))
      return -91;

   return 0;
}

int setLongField_util(
   JNIEnv *env,
   jobject thisObject,
   jclass class,
   const char *fieldId,
   long long int value
)
{

   jfieldID field;

   if (!(field=(*env)->GetFieldID(env, class, fieldId, NANO_JAVA_LONG_TYPE)))
      return -50;

   (*env)->SetLongField(env, thisObject, field, (jlong)value);

   if ((*env)->ExceptionCheck(env))
      return -51;

   return 0;
}

int getIntFieldUtil(
   jint *int_value,
   JNIEnv *env,
   jobject thisObject,
   jclass class,
   const char *fieldId
)
{

   jfieldID field;

   if (!(field=(*env)->GetFieldID(env, class, fieldId, NANO_JAVA_INT_TYPE)))
      return -95;

   *int_value=(*env)->GetIntField(env, thisObject, field);

   if ((*env)->ExceptionCheck(env))
      return -96;

   return 0;
}

#define NANO_BLK_ACCOUNT "account"
#define NANO_BLK_PREVIOUS "previous"
#define NANO_BLK_REPRESENTATIVE "representative"
#define NANO_BLK_BALANCE "balance"
#define NANO_BLK_LINK "link"
#define NANO_BLK_SIGNATURE "signature"
#define NANO_BLK_WORK "work"
#define NANO_BLK_PREFIXES "prefixes"

typedef struct set_nano_blk_to_jvm_t {
   const char *key;
   const size_t offset;
   const size_t size;
} NANO_BLK_TO_JVM;

static NANO_BLK_TO_JVM blk_to_jvm[] = {
   {NANO_BLK_ACCOUNT, offsetof(F_BLOCK_TRANSFER, account), sizeof(((F_BLOCK_TRANSFER *)0)->account)},
   {NANO_BLK_PREVIOUS, offsetof(F_BLOCK_TRANSFER, previous), sizeof(((F_BLOCK_TRANSFER *)0)->previous)},
   {NANO_BLK_REPRESENTATIVE, offsetof(F_BLOCK_TRANSFER, representative), sizeof(((F_BLOCK_TRANSFER *)0)->representative)},
   {NANO_BLK_BALANCE, offsetof(F_BLOCK_TRANSFER, balance), sizeof(((F_BLOCK_TRANSFER *)0)->balance)},
   {NANO_BLK_LINK, offsetof(F_BLOCK_TRANSFER, link), sizeof(((F_BLOCK_TRANSFER *)0)->link)},
   {NANO_BLK_SIGNATURE, offsetof(F_BLOCK_TRANSFER, signature), sizeof(((F_BLOCK_TRANSFER *)0)->signature)},
   {0}
};

#define NANO_CREATE_BLOCK_ERROR_MSG "setNanoBlockToJVM_util: Error when set byte array field in %s %d"

int setNanoBlockToJVM_util(
   JNIEnv *env,
   jobject thisObject,
   jclass thisClass,
   F_BLOCK_TRANSFER *nano_block
)
{
   int err;
   NANO_BLK_TO_JVM *blk;
   jfieldID field;
   jbyteArray byte_array;

   if (!(field=(*env)->GetFieldID(env, thisClass, NANO_BLK_WORK, NANO_JAVA_LONG_TYPE))) {
      sprintf(msg, NANO_CREATE_BLOCK_ERROR_MSG, NANO_BLK_WORK, err=-42);
      return err;
   }

   (*env)->SetLongField(env, thisObject, field, (jlong)nano_block->work);

   if (!(field=(*env)->GetFieldID(env, thisClass, NANO_BLK_PREFIXES, NANO_JAVA_INT_TYPE))) {
      sprintf(msg, NANO_CREATE_BLOCK_ERROR_MSG, NANO_BLK_PREFIXES, err=-43);
      return err;
   }

   (*env)->SetIntField(env, thisObject, field, (jint)nano_block->prefixes);

   blk=blk_to_jvm;

   while (blk->key) {
      if ((err=setByteArrayFieldId_util(env, thisObject, thisClass, blk->key, (((uint8_t *)nano_block)+(size_t)blk->offset), (size_t)blk->size))) {
         sprintf(msg, NANO_CREATE_BLOCK_ERROR_MSG, blk->key, err);
         return err;
      }
      blk++;
   }

   return 0;
}

/*
 * Class:     org_mynanojava_MyNanoJava
 * Method:    nanoCreateBlock
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;ILjava/lang/String;I)Lorg/mynanojava/blockchain/NanoBlock;
 */
#define NANO_BLOCK_EXCEPTION_MISSING_BLOCK_ERR 20000
#define NANO_BLOCK_EXCEPTION_MISSING_REPRESENTATIVE_ERR 20001
#define NANO_BLOCK_EXCEPTION_MISSING_BALANCE_ERR 20002
#define NANO_BLOCK_EXCEPTION_VAL_SEND_REC_ERR 20003
#define NANO_BLOCK_EXCEPTION_MISSING_LINK_ERR 20004
#define NANO_BLOCK_EXCEPTION_CANT_FIND_NANO_BLOCK_CLS_ERR 20005
#define NANO_BLOCK_EXCEPTION_CANT_INIT_NANO_BLK_METHOD_ERR 20006
#define NANO_BLOCK_EXCEPTION_CANT_CREATE_NANO_BLK_OBJ_ERR 20007

JNIEXPORT jobject JNICALL Java_org_mynanojava_MyNanoJava_nanoCreateBlock(
   JNIEnv *env, jobject thisObject, jstring account, jstring previous, jstring representative, jstring balance,
   jint balance_type, jstring value_to_send_or_receive, jint value_to_send_or_receive_type, jstring link, jint direction
)
{

   int err;
   const char *c_account, *c_previous, *c_representative, *c_balance, *c_value_to_send_or_receive, *c_link, *p_balance, *p_val_send_rec;
   jobject jResult;
   jclass jNanoBlockClass;
   jmethodID methodId;
   jbyte *c_byte_array;
   jbyteArray byte_array;
   uint32_t types;
   F_BLOCK_TRANSFER *nano_block;

   if (!account) {
      throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, "nanoCreateBlock: Missing account", NANO_BLOCK_EXCEPTION_MISSING_BLOCK_ERR);
      return NULL;
   }

   if (!representative) {
      throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, "nanoCreateBlock: Missing representative", NANO_BLOCK_EXCEPTION_MISSING_REPRESENTATIVE_ERR);
      return NULL;
   }

   if (!balance) {
      throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, "nanoCreateBlock: Missing balance", NANO_BLOCK_EXCEPTION_MISSING_BALANCE_ERR);
      return NULL;
   }

   if (!value_to_send_or_receive) {
      throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, "nanoCreateBlock: Missing value to send or receive", NANO_BLOCK_EXCEPTION_VAL_SEND_REC_ERR);
      return NULL;
   }

   if (!link) {
      throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, "nanoCreateBlock: Missing destination wallet or link to open block",
         NANO_BLOCK_EXCEPTION_MISSING_LINK_ERR);
      return NULL;
   }

   if (!(c_account=(*env)->GetStringUTFChars(env, account, NULL))) {
      throwError(env, "nanoCreateBlock: Get UTF account error");
      return NULL;
   }

   jResult=NULL;

   if (previous) {
      if (!(c_previous=(*env)->GetStringUTFChars(env, previous, NULL))) {
         throwError(env, "nanoCreateBlock: Get UTF previous error");
         goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT1;
      }
   } else
      c_previous=NULL;

   if (!(c_representative=(*env)->GetStringUTFChars(env, representative, NULL))) {
      throwError(env, "nanoCreateBlock: Get UTF representative error");
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT2;
   }

   if (!(c_balance=(*env)->GetStringUTFChars(env, balance, NULL))) {
      throwError(env, "nanoCreateBlock: Get UTF balance error");
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT3;
   }

   if (!(c_value_to_send_or_receive=(*env)->GetStringUTFChars(env, value_to_send_or_receive, NULL))) {
      throwError(env, "nanoCreateBlock: Get UTF value to send/receive error");
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT4;
   }

   if (!(c_link=(*env)->GetStringUTFChars(env, link, NULL))) {
      throwError(env, "nanoCreateBlock: Get UTF send wallet address or link error");
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT5;
   }

   p_balance=c_balance;

   if ((types=(uint32_t)(balance_type|value_to_send_or_receive_type))&F_BALANCE_RAW_128)
      if ((err=nano_java_raw128_str_to_bin_util((uint8_t *)(p_balance=(const char *)msg), c_balance))) {
         sprintf(msg, "nanoCreateBlock: Error when parse hex balance to bin. "MY_NANO_EMBEDDED_ERROR, "nano_java_raw128_str_to_bin_util", err);
         throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, msg, err);
         goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT6;
      }

   p_val_send_rec=c_value_to_send_or_receive;

   if (types&F_VALUE_SEND_RECEIVE_RAW_128)
      if ((err=nano_java_raw128_str_to_bin_util((uint8_t *)(p_val_send_rec=((const char *)&msg[sizeof(f_uint128_t)])), c_value_to_send_or_receive))) {
         sprintf(msg, "nanoCreateBlock: Error when parse hex send or receive value to bin. "MY_NANO_EMBEDDED_ERROR, "nano_java_raw128_str_to_bin_util", err);
         throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, msg, err);
         goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT6;
      }

   if ((err=nano_create_block_dynamic(&nano_block, c_account, 0, c_previous, 0, c_representative, 0, p_balance, p_val_send_rec,
      (uint32_t)types, c_link, 0, direction))) {
      sprintf(msg, "nanoCreateBlock: "MY_NANO_EMBEDDED_ERROR, "nano_create_block_dynamic", err);
      throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, msg, err);
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT6;
   }

   if (!(jNanoBlockClass=(*env)->FindClass(env, NANO_BLOCK_CLASS_PATH))) {
      sprintf(msg, CANT_FIND_NANO_BLOCK_ERROR, "nanoCreateBlock");
      throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, msg, NANO_BLOCK_EXCEPTION_CANT_FIND_NANO_BLOCK_CLS_ERR);
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT7;
   }

   if (!(methodId=NANO_JAVA_GET_INIT_METHOD(jNanoBlockClass))) {
      throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, "nanoCreateBlock: Can't Init new class NanoBlock", NANO_BLOCK_EXCEPTION_CANT_INIT_NANO_BLK_METHOD_ERR);
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT7;
   }

   if (!(jResult=(*env)->NewObject(env, jNanoBlockClass, methodId))) {
      throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, "nanoCreateBlock: Can't create NanoBlock Object", NANO_BLOCK_EXCEPTION_CANT_CREATE_NANO_BLK_OBJ_ERR);
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT7;
   }

   if ((err=setNanoBlockToJVM_util(env, jResult, jNanoBlockClass, nano_block))) {
      throwNewException(env, NANO_BLOCK_EXCEPTION_CLASS, msg, err);
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT8;
   }

   goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT7;

Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT8:
   (*env)->DeleteLocalRef(env, jResult);
   jResult=NULL;

Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT7:
   memset(nano_block, 0, sizeof(F_BLOCK_TRANSFER));
   free(nano_block);

Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT6:
   (*env)->ReleaseStringUTFChars(env, link, c_link);

Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT5:
   (*env)->ReleaseStringUTFChars(env, value_to_send_or_receive, c_value_to_send_or_receive);

Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT4:
   (*env)->ReleaseStringUTFChars(env, balance, c_balance);

Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT3:
   (*env)->ReleaseStringUTFChars(env, representative, c_representative);

Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT2:
   if (c_previous)
      (*env)->ReleaseStringUTFChars(env, previous, c_previous);

Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT1:
   (*env)->ReleaseStringUTFChars(env, account, c_account);

   return jResult;

}

int java_NanoBlock_2_C_NanoBlock_util(F_BLOCK_TRANSFER **nano_block, JNIEnv *env, jobject nanoBlock, const char *function_name)
{
   int err;
   jclass jNanoBlockClass;
   uint8_t *c_byte_array;
   size_t c_byte_array_sz;
   jobject fieldObj;
   NANO_BLK_TO_JVM *jvm_blk;
   const char *field;
   char *msg2;
   jfieldID fieldId;

   if (!nanoBlock) {
      sprintf(msg, "%s: Missing NanoBlock object", function_name);
      return 100;
   }

   if (!(jNanoBlockClass=(*env)->FindClass(env, NANO_BLOCK_CLASS_PATH))) {
      sprintf(msg, CANT_FIND_NANO_BLOCK_ERROR, function_name);
      return 101;
   }

   if (!(*nano_block=malloc(sizeof(F_BLOCK_TRANSFER)))) {
      sprintf(msg, "%s: Error when creating Nano Block", function_name);
      return 102;
   }

   jvm_blk=blk_to_jvm;

   while (field=jvm_blk->key) {
      if (!(err=getByteArrayFieldId_util(&c_byte_array, &c_byte_array_sz, &fieldObj, env, nanoBlock, jNanoBlockClass, field)))
         (c_byte_array_sz==jvm_blk->size)?(void)(memcpy(((uint8_t *)*nano_block)+jvm_blk->offset, c_byte_array, c_byte_array_sz)):(void)(err=31);

      if (c_byte_array)
         (*env)->ReleaseByteArrayElements(env, fieldObj, c_byte_array, JNI_ABORT);

      if (err) {

         (err>0)?
            sprintf(msg, "Wrong field[%s] size. Expected size = %lu and returned size = %lu", jvm_blk->key, jvm_blk->size, c_byte_array_sz):
            sprintf(msg, MY_NANO_EMBEDDED_ERROR, "getByteArrayFieldId_util", err);

         goto java_NanoBlock_2_C_NanoBlock_util_EXIT1;
      }

      jvm_blk++;
   }

   msg2=(msg+(sizeof(msg)>>1));

   if ((err=getLongFieldUtil((jlong *)&(*nano_block)->work, env, nanoBlock, jNanoBlockClass, NANO_BLK_WORK))) {
      sprintf(msg2, "getLongFieldUtil @ %s", function_name);
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, msg2, err);
      goto java_NanoBlock_2_C_NanoBlock_util_EXIT1;
   }

   if ((err=getIntFieldUtil((jint *)msg2, env, nanoBlock, jNanoBlockClass, NANO_BLK_PREFIXES))) {
      sprintf(msg2, "getIntFieldUtil @ %s", function_name);
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, msg2, err);
      goto java_NanoBlock_2_C_NanoBlock_util_EXIT1;
   }

   (*nano_block)->prefixes=(uint8_t)*((jint *)msg2);
   memset((*nano_block)->preamble, 0, 31);
   (*nano_block)->preamble[31]=0x06;

   return 0;

java_NanoBlock_2_C_NanoBlock_util_EXIT1:
   memset(*nano_block, 0, sizeof(F_BLOCK_TRANSFER));
   free(*nano_block);

   return err;
}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    nanoBlockToJSON
 * Signature: (Lorg/mynanojava/blockchain/NanoBlock;)Ljava/lang/String;
 */

JNIEXPORT jstring JNICALL Java_org_mynanojava_blockchain_NanoBlock_nanoBlockToJSON(JNIEnv *env, jobject thisObj, jobject nanoBlock)
{
   int err;
   jstring res;
   F_BLOCK_TRANSFER *nano_block;

   if (java_NanoBlock_2_C_NanoBlock_util(&nano_block, env, nanoBlock, "nanoBlockToJSON")) {
      throwError(env, msg);
      return NULL;
   }

   if ((err=f_nano_block_to_json(msg, NULL, sizeof(msg), nano_block))) {
      res=NULL;
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "f_nano_block_to_json @ nanoBlockToJSON", err);
      throwError(env, msg);
   } else if (!(res=(*env)->NewStringUTF(env, msg)))
      throwError(env, "nanoBlockToJSON: Error when parse Nano block to JSON");

   memset(nano_block, 0, sizeof(F_BLOCK_TRANSFER));
   free(nano_block);

   return res;
}

JNIEXPORT jbyteArray JNICALL Java_org_mynanojava_MyNanoJava_nanoBlockToByte(JNIEnv *env, jobject thisObject, jobject nanoBlock)
{

   jbyteArray res;
   F_BLOCK_TRANSFER *nano_block;

   if (java_NanoBlock_2_C_NanoBlock_util(&nano_block, env, nanoBlock, "nanoBlockToByte")) {
      throwError(env, msg);
      return NULL;
   }

   ((res=(*env)->NewByteArray(env, sizeof(F_BLOCK_TRANSFER))))?
      (void)(*env)->SetByteArrayRegion(env, res, 0, sizeof(F_BLOCK_TRANSFER), (const jbyte *)nano_block):
      (void)throwError(env, "nanoBlockToByte: Can't create JNI byte array");

   memset(nano_block, 0, sizeof(F_BLOCK_TRANSFER));
   free(nano_block);

   return res;
}

JNIEXPORT jlong JNICALL Java_org_mynanojava_MyNanoJava_nanoPoW(JNIEnv *env, jobject thisObj, jstring jHash, jobject jThreshold, jint jNumberOfThreads)
{
   int err;
   const char *c_hash;
   uint64_t c_threshold, result;
   jmethodID methodId;
   jclass userDataClass;

   if (!jHash) {
      throwError(env, "Missing hash");
      return -1;
   }

   if (jThreshold) {

      if (!(userDataClass=(*env)->FindClass(env, "java/lang/Long"))) {
         throwError(env, "Error when finding JVM Long class in Nano PoW method");
         return -2;
      }

      if (!(methodId=(*env)->GetMethodID(env, userDataClass, "longValue", "()J"))) {
         throwError(env, "Error when get JVM method in threshold long value");
         return -3;
      }

      c_threshold=(uint64_t)(*env)->CallLongMethod(env, jThreshold, methodId);

      if ((*env)->ExceptionCheck(env)) {
         throwError(env, "Error when calling long method in JVM for Nano PoW method");
         return -4;
      }

   } else
      c_threshold=F_DEFAULT_THRESHOLD;

   if (!(c_hash=(*env)->GetStringUTFChars(env, jHash, NULL))) {
      throwError(env, "Error when get Nano Hash value");
      return -5;
   }

   if ((err=array32bytes_str_to_hex_util((uint8_t *)msg, (const char *)c_hash))) {
      result=-6;
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "array32bytes_str_to_hex_util", err);
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_nanoPoW_EXIT1;
   }

   f_random_attach(gen_rand_no_entropy_util);

   if ((err=f_nano_pow(&result, (unsigned char *)msg, c_threshold, (int)jNumberOfThreads))) {
      result=-7;
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "f_nano_pow", err);
      throwError(env, msg);
   }

   f_random_detach();

Java_org_mynanojava_MyNanoJava_nanoPoW_EXIT1:
   (*env)->ReleaseStringUTFChars(env, jHash, c_hash);

   return (jlong)result;
}

/*
 * Class:     org_mynanojava_MyNanoJava
 * Method:    nanoBytePoW
 * Signature: ([BJI)J
 */
JNIEXPORT jlong JNICALL Java_org_mynanojava_MyNanoJava_nanoBytePoW(JNIEnv *env, jobject thisObject, jbyteArray hash, jlong threshold, jint numberOfThreads)
{
   int err;
   jbyte *c_hash;
   uint64_t result;
   jsize jSize;

   jSize=(*env)->GetArrayLength(env, hash);
   if ((*env)->ExceptionCheck(env)) {
      THROW_NANO_BLOCK_EXCEPTION(env, "nanoBytePoW: Can't calculate hash size in ByteArray", 236);
      return -8;
   }

   if (jSize!=32) {
      sprintf(msg, "nanoBytePoW: Wrong Hash Block size %u", (int)jSize);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, 239);
      return -11;
   }

   if (!(c_hash=(*env)->GetByteArrayElements(env, hash, NULL))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "getByteNanoBlockHash: Can't get hash in ByteArray", 237);
      return -9;
   }

   f_random_attach(gen_rand_no_entropy_util);

   if ((err=f_nano_pow(&result, (unsigned char *)c_hash, (uint64_t)threshold, (int)numberOfThreads))) {
      result=-10;
      sprintf(msg, "f_nano_pow @ nanoBytePoW: Can't calculate PoW %d", err);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, err);
   }

   f_random_detach();

   (*env)->ReleaseByteArrayElements(env, hash, c_hash, JNI_ABORT);

   return (jlong)result;
}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    byteToWallet
 * Signature: ([BI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_mynanojava_blockchain_NanoBlock_byteToWallet(JNIEnv *env, jobject thisObj, jbyteArray wallet, jint type)
{
   int err;
   jstring res;
   const char *p, *prefix;
   jbyte *c_byte_array;
   jsize jSize;

   if (!wallet) {
      throwError(env, "byteToWallet: Missing wallet");
      return NULL;
   }

   if (type==3)
      prefix=NULL;
   else if (type==1)
      prefix=NANO_PREFIX;
   else if (type==2)
      prefix=XRB_PREFIX;
   else {
      throwError(env, "byteToWallet: Wrong wallet type");
      return NULL;
   }

   if (!(c_byte_array=(*env)->GetByteArrayElements(env, wallet, JNI_FALSE))) {
      throwError(env, "byteToWallet: Unable to get wallet");
      return NULL;
   }

   res=NULL;

   if ((jSize=(*env)->GetArrayLength(env, wallet))!=32) {
      sprintf(msg, "byteToWallet: Wrong wallet size %d", (int)jSize);
      throwError(env, msg);
      goto Java_org_mynanojava_blockchain_NanoBlock_byteToWallet_EXIT1;
   }

   p=memcpy(msg+128, c_byte_array, 32);

   if (prefix) {
      if ((err=pk_to_wallet(msg, (char *)prefix, (uint8_t *)p))) {
         sprintf(msg, MY_NANO_EMBEDDED_ERROR, "pk_to_wallet", err);
         throwError(env, msg);
         goto Java_org_mynanojava_blockchain_NanoBlock_byteToWallet_EXIT1;
      }
   } else
      fhex2strv2(msg, (const void *)p, 32, 1);

   if (!(res = (*env)->NewStringUTF(env, msg)))
      throwError(env, JAVA_ERR_PARSE_UTF8_STRING);

Java_org_mynanojava_blockchain_NanoBlock_byteToWallet_EXIT1:
   (*env)->ReleaseByteArrayElements(env, wallet, c_byte_array, JNI_ABORT);

   return res;
}

JNIEXPORT jobject JNICALL Java_org_mynanojava_MyNanoJava_byteToNanoBlock(JNIEnv *env, jobject thisObj, jbyteArray nanoBlock, jint blockNumber)
{
   int err;
   jbyte *c_byte_array, *p;
   size_t block_offset;
   jobject jResult;
   jclass jNanoBlockClass;
   jmethodID methodId;

   if (!nanoBlock) {
      throwError(env, "byteToNanoBlock: Missing Java byte array Nano block");
      return NULL;
   }

   if (blockNumber<0) {
      throwError(env, "byteToNanoBlock: Invalid block number");
      return NULL;
   }

   if (((block_offset=blockNumber*sizeof(F_BLOCK_TRANSFER))+sizeof(F_BLOCK_TRANSFER))>(*env)->GetArrayLength(env, nanoBlock)) {
      throwError(env, "byteToNanoBlock: Wrong Java Byte array size or invalid block number");
      return NULL;
   }

   if (!(c_byte_array=(*env)->GetByteArrayElements(env, nanoBlock, JNI_FALSE))) {
      throwError(env, "byteToNanoBlock: Unable to get Nano block in Java Byte Array");
      return NULL;
   }

   jResult=NULL;

   if (!f_nano_is_valid_block((F_BLOCK_TRANSFER *)(p=c_byte_array+block_offset))) {
      sprintf(msg, "byteToNanoBlock: Block number %d is invalid in Java Byte Array", (int)blockNumber);
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_byteToNanoBlock_EXIT1;
   }

   if (!(jNanoBlockClass=(*env)->FindClass(env, NANO_BLOCK_CLASS_PATH))) {
      sprintf(msg, CANT_FIND_NANO_BLOCK_ERROR, "byteToNanoBlock");
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_byteToNanoBlock_EXIT1;
   }

   if (!(methodId=NANO_JAVA_GET_INIT_METHOD(jNanoBlockClass))) {
      throwError(env, "byteToNanoBlock: Can't Init new class NanoBlock");
      goto Java_org_mynanojava_MyNanoJava_byteToNanoBlock_EXIT1;
   }

   if (!(jResult=(*env)->NewObject(env, jNanoBlockClass, methodId))) {
      throwError(env, "byteToNanoBlock: Can't create NanoBlock Object");
      goto Java_org_mynanojava_MyNanoJava_byteToNanoBlock_EXIT1;
   }

   if ((err=setNanoBlockToJVM_util(env, jResult, jNanoBlockClass, (F_BLOCK_TRANSFER *)p))) {
      sprintf(msg, "byteToNanoBlock: error when set C Nano Block to Java NanoBlock class %d", err);
      throwError(env, msg);
      (*env)->DeleteLocalRef(env, jResult);
      jResult=NULL;
   }

Java_org_mynanojava_MyNanoJava_byteToNanoBlock_EXIT1:
   (*env)->ReleaseByteArrayElements(env, nanoBlock, c_byte_array, JNI_ABORT);
   return jResult;
}

#define NANO_BALANCE_TYPE_REAL 16
#define NANO_BALANCE_TYPE_RAW 8
#define NANO_BALANCE_TYPE_HEX 4
int getBalance_util(char *ret_val, size_t ret_val_sz, uint8_t *val, size_t val_sz, int type)
{

   if (val_sz!=sizeof(((F_BLOCK_TRANSFER *)0)->balance))
      return 129;

   switch (type) {
      case NANO_BALANCE_TYPE_REAL:
         return f_nano_raw_to_string(ret_val, NULL, ret_val_sz, (void *)val, F_RAW_TO_STR_UINT128);
      case NANO_BALANCE_TYPE_RAW:
         return f_nano_balance_to_str(ret_val, ret_val_sz, NULL, val);
      case NANO_BALANCE_TYPE_HEX:
         fhex2strv2(ret_val, (const void *)val, val_sz, 0);
         return 0;
   }

   return 130;

}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    getBalanceFromByte
 * Signature: ([BI)Ljava/lang/String;
 */
#define THROW_NANO_BALANCE_EXCEPTION(env, msg, err) throwNewException(env, BALANCE_EXCEPTION_CLASS, msg, err)
JNIEXPORT jstring JNICALL Java_org_mynanojava_blockchain_NanoBlock_getBalanceFromByte(JNIEnv *env, jobject thisObj, jbyteArray balance, jint balanceType)
{
   int err;
   jstring res;
   jbyte *c_byte_array;
   jsize jSize;

   if (!balance) {
      throwError(env, "getBalanceFromByte: Missing balance");
      return NULL;
   }

   if (!(c_byte_array=(*env)->GetByteArrayElements(env, balance, JNI_FALSE))) {
      throwError(env, "getBalanceFromByte: Unable to get balance");
      return NULL;
   }

   res=NULL;

   jSize=(*env)->GetArrayLength(env, balance);
   if ((*env)->ExceptionCheck(env)) {
      throwError(env, "getBalanceFromByte: Unable to get balance length");
      goto Java_org_mynanojava_blockchain_NanoBlock_getBalanceFromByte_EXIT1;
   }

   if ((err=getBalance_util(msg, sizeof(msg), (uint8_t *)c_byte_array, jSize, balanceType))) {
      sprintf(msg, "getBalance_util @ getBalanceFromByte: Error %d. With byte size %d", err, jSize);
      THROW_NANO_BALANCE_EXCEPTION(env, msg, err);
      goto Java_org_mynanojava_blockchain_NanoBlock_getBalanceFromByte_EXIT1;
   }

   if (!(res=(*env)->NewStringUTF(env, msg)))
      throwError(env, JAVA_ERR_PARSE_UTF8_STRING);

Java_org_mynanojava_blockchain_NanoBlock_getBalanceFromByte_EXIT1:
   (*env)->ReleaseByteArrayElements(env, balance, c_byte_array, JNI_ABORT);

   return res;
}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    getBalanceFromNanoBlock
 * Signature: (Lorg/mynanojava/blockchain/NanoBlock;I)Ljava/lang/String;
 */

JNIEXPORT jstring JNICALL Java_org_mynanojava_blockchain_NanoBlock_getBalanceFromNanoBlock(JNIEnv *env, jobject thisObj, jobject nanoBlock, jint balanceType)
{
   int err;
   jstring res;
   F_BLOCK_TRANSFER *nano_block;

   if ((err=java_NanoBlock_2_C_NanoBlock_util(&nano_block, env, nanoBlock, "getBalanceFromNanoBlock"))) {
      THROW_NANO_BLOCK_EXCEPTION(env, msg, err);
      return NULL;
   }

   res=NULL;

   if ((err=getBalance_util(msg, sizeof(msg), nano_block->balance, sizeof(((F_BLOCK_TRANSFER *)0)->balance), balanceType))) {
      sprintf(msg, "getBalance_util @ getBalanceFromNanoBlock: Error %d", err);
      THROW_NANO_BALANCE_EXCEPTION(env, msg, err);
      goto Java_org_mynanojava_blockchain_NanoBlock_getBalanceFromNanoBlock_EXIT1;
   }

   if (!(res=(*env)->NewStringUTF(env, msg)))
      throwError(env, JAVA_ERR_PARSE_UTF8_STRING);

Java_org_mynanojava_blockchain_NanoBlock_getBalanceFromNanoBlock_EXIT1:
   memset(nano_block, 0, sizeof(F_BLOCK_TRANSFER));
   free(nano_block);

   return res;
}

int signBlock_util(
   F_BLOCK_TRANSFER *user_block,
   F_BLOCK_TRANSFER *fee_block,
   uint8_t *private_key
)
{

   if (fee_block)
      if (!f_nano_is_valid_block(fee_block))
         return 60;

   if (!f_nano_is_valid_block(user_block))
      return 61;

   return f_nano_sign_block(user_block, fee_block, private_key);

}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    signByteNanoBlock
 * Signature: ([BLjava/lang/String;)V
 */

JNIEXPORT void JNICALL Java_org_mynanojava_blockchain_NanoBlock_signByteNanoBlock(JNIEnv *env, jobject thisObject, jbyteArray nanoBlock, jstring privateKey)
{

   int err;
   jint mode;
   jbyte *c_byte_array;
   const char *c_private_key;
   jsize jSize;

   if (!nanoBlock) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signByteNanoBlock: Missing Nano block (bytes)", 301);
      return;
   }

   if (!privateKey) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signByteNanoBlock: Missing private key", 302);
      return;
   }

   if (!(c_byte_array=(*env)->GetByteArrayElements(env, nanoBlock, NULL))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signByteNanoBlock: Unable to get Nano Block", 303);
      return;
   }

   mode=JNI_ABORT;

   jSize=(*env)->GetArrayLength(env, nanoBlock);
   if ((*env)->ExceptionCheck(env)) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signByteNanoBlock: Can't determine size of Nano block", 304);
      goto Java_org_mynanojava_blockchain_NanoBlock_signByteNanoBlock_EXIT1;
   }

   if (jSize!=sizeof(F_BLOCK_TRANSFER)) {
      sprintf(msg, "signByteNanoBlock: Invalid Nano block size %d", jSize);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, 305);
      goto Java_org_mynanojava_blockchain_NanoBlock_signByteNanoBlock_EXIT1;
   }

   if (!(c_private_key=(*env)->GetStringUTFChars(env, privateKey, NULL))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signByteNanoBlock: Can't parse private key to C string", 306);
      goto Java_org_mynanojava_blockchain_NanoBlock_signByteNanoBlock_EXIT1;
   }

   if ((err=array32bytes_str_to_hex_util((uint8_t *)msg, c_private_key))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "array32bytes_str_to_hex_util @ signByteNanoBlock: Can't parse C string private key to bin", err);
      goto Java_org_mynanojava_blockchain_NanoBlock_signByteNanoBlock_EXIT2;
   }

   memcpy(msg+32, ((F_BLOCK_TRANSFER *)c_byte_array)->account, sizeof(((F_BLOCK_TRANSFER *)c_byte_array)->account));

   if ((err=signBlock_util((F_BLOCK_TRANSFER *)c_byte_array, NULL, (uint8_t *)msg))) {
      sprintf(msg, "signBlock_util @ signByteNanoBlock: Can't sign Nano block. Error: %d", err);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, err);
   } else
      mode=0;

Java_org_mynanojava_blockchain_NanoBlock_signByteNanoBlock_EXIT2:
   memset(msg, 0, sizeof(msg));
   (*env)->ReleaseStringUTFChars(env, privateKey, c_private_key);

Java_org_mynanojava_blockchain_NanoBlock_signByteNanoBlock_EXIT1:
   (*env)->ReleaseByteArrayElements(env, nanoBlock, c_byte_array, mode);

}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    signNanoBlock
 * Signature: (Lorg/mynanojava/blockchain/NanoBlock;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_mynanojava_blockchain_NanoBlock_signNanoBlock(JNIEnv *env, jobject thisObj, jobject nanoBlock, jstring privateKey)
{

   int err;
   F_BLOCK_TRANSFER *nano_block;
   const char *c_private_key;
   jclass jNanoBlockClass;

   if (!nanoBlock) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signNanoBlock: Missing Nano Block", 167);
      return;
   }

   if (!privateKey) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signNanoBlock: Missing private key", 168);
      return;
   }

   if (!(c_private_key=(*env)->GetStringUTFChars(env, privateKey, NULL))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signNanoBlock: Can't parse private key to C string", 169);
      return;
   }

   if ((err=array32bytes_str_to_hex_util((uint8_t *)msg, c_private_key))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "array32bytes_str_to_hex_util @ signNanoBlock: Can't parse C string private key to bin", err);
      goto Java_org_mynanojava_blockchain_NanoBlock_signNanoBlock_EXIT1;
   }

   if ((err=java_NanoBlock_2_C_NanoBlock_util(&nano_block, env, nanoBlock, "signNanoBlock"))) {
      THROW_NANO_BLOCK_EXCEPTION(env, msg, err);
      goto Java_org_mynanojava_blockchain_NanoBlock_signNanoBlock_EXIT1;
   }

   memcpy(msg+32, nano_block->account, sizeof(nano_block->account));

   if ((err=signBlock_util(nano_block, NULL, (uint8_t *)msg))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signBlock_util @ signNanoBlock: Can't sign Nano block", err);
      goto Java_org_mynanojava_blockchain_NanoBlock_signNanoBlock_EXIT2;
   }

   if (!(jNanoBlockClass=(*env)->FindClass(env, NANO_BLOCK_CLASS_PATH))) {
      sprintf(msg, CANT_FIND_NANO_BLOCK_ERROR, "signNanoBlock");
      THROW_NANO_BLOCK_EXCEPTION(env, msg, 170);
      goto Java_org_mynanojava_blockchain_NanoBlock_signNanoBlock_EXIT2;
   }

   if ((err=setByteArrayFieldId_util(env, nanoBlock, jNanoBlockClass, NANO_BLK_SIGNATURE, nano_block->signature, sizeof(nano_block->signature))))
      THROW_NANO_BLOCK_EXCEPTION(env, "setByteArrayFieldId_util @ signNanoBlock: Can't parse signature to Java byte array", err);

Java_org_mynanojava_blockchain_NanoBlock_signNanoBlock_EXIT2:
   memset(nano_block, 0, sizeof(F_BLOCK_TRANSFER));
   free(nano_block);

Java_org_mynanojava_blockchain_NanoBlock_signNanoBlock_EXIT1:
   memset(msg, 0, sizeof(msg));
   (*env)->ReleaseStringUTFChars(env, privateKey, c_private_key);

}

#define NANO_BLOCK_SIGNATURE_SZ (size_t)sizeof(((F_BLOCK_TRANSFER *)0)->signature)
JNIEXPORT jstring JNICALL Java_org_mynanojava_blockchain_NanoBlock_signatureByteToString(JNIEnv *env, jobject thisObj, jbyteArray signature)
{
   int err;
   jbyte *c_byte_array;
   jstring res;
   jsize jSize;

   if (!signature) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signatureByteToString: Missing ByteArray Signature", 170);
      return NULL;
   }

   if (!(c_byte_array=(*env)->GetByteArrayElements(env, signature, NULL))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signatureByteToString: Unable to get ByteArray Signature", 171);
      return NULL;
   }

   res=NULL;

   jSize=(*env)->GetArrayLength(env, signature);
   if ((*env)->ExceptionCheck(env)) {
      THROW_NANO_BLOCK_EXCEPTION(env, "signatureByteToString: Can't determine size of ByteArray Signature", 172);
      goto Java_org_mynanojava_blockchain_NanoBlock_signatureByteToString_EXIT1;
   }

   if (jSize!=NANO_BLOCK_SIGNATURE_SZ) {
      sprintf(msg, "signatureByteToString: Wrong signature size = %d. Error = %d", (int)jSize, 173);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, err);
      goto Java_org_mynanojava_blockchain_NanoBlock_signatureByteToString_EXIT1;
   }

   if (!is_filled_with_value((uint8_t *)c_byte_array, NANO_BLOCK_SIGNATURE_SZ, 0))
      if (!(res=(*env)->NewStringUTF(env, (const char *)fhex2strv2(msg, (const void *)c_byte_array, NANO_BLOCK_SIGNATURE_SZ, 1))))
         throwError(env, JAVA_ERR_PARSE_UTF8_STRING);

Java_org_mynanojava_blockchain_NanoBlock_signatureByteToString_EXIT1:
   (*env)->ReleaseByteArrayElements(env, signature, c_byte_array, JNI_ABORT);

   return res;

}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    verifySignatureByteNanoBlock
 * Signature: ([B)Z
 */

JNIEXPORT jboolean JNICALL Java_org_mynanojava_blockchain_NanoBlock_verifySignatureByteNanoBlock(JNIEnv *env, jobject thisObject, jbyteArray nanoBlock)
{
   int err;
   jboolean res;
   jbyte *c_byte_array;
   jsize jSize;

   if (!nanoBlock) {
      THROW_NANO_BLOCK_EXCEPTION(env, "verifySignatureByteNanoBlock: Missing Nano Block in ByteArray", 191);
      return JNI_FALSE;
   }

   if (!(c_byte_array=(*env)->GetByteArrayElements(env, nanoBlock, NULL))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "verifySignatureByteNanoBlock: Unable to get Nano Block ByteArray", 192);
      return JNI_FALSE;
   }

   res=JNI_FALSE;

   jSize=(*env)->GetArrayLength(env, nanoBlock);
   if ((*env)->ExceptionCheck(env)) {
      THROW_NANO_BLOCK_EXCEPTION(env, "verifySignatureByteNanoBlock: Can't determine size of Nano Block ByteArray", 193);
      goto Java_org_mynanojava_blockchain_NanoBlock_verifySignatureByteNanoBlock_EXIT1;
   }

   if (jSize!=sizeof(F_BLOCK_TRANSFER)) {
      sprintf(msg, "verifySignatureByteNanoBlock: Wrong Nano Block size = %d", jSize);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, 194);
      goto Java_org_mynanojava_blockchain_NanoBlock_verifySignatureByteNanoBlock_EXIT1;
   }

   if ((err=f_verify_signed_block((F_BLOCK_TRANSFER *)c_byte_array))>0) {
      sprintf(msg, "f_verify_signed_block @ verifySignatureByteNanoBlock: Error when verify Nano Block signature %d", err);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, err);
   } else if (err==0)
      res=JNI_TRUE;

Java_org_mynanojava_blockchain_NanoBlock_verifySignatureByteNanoBlock_EXIT1:
   (*env)->ReleaseByteArrayElements(env, nanoBlock, c_byte_array, JNI_ABORT);

   return res;
}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    verifySignatureNanoBlock
 * Signature: (Lorg/mynanojava/blockchain/NanoBlock;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_mynanojava_blockchain_NanoBlock_verifySignatureNanoBlock(JNIEnv *env, jobject thisObject, jobject nanoBlock)
{
   int err;
   jboolean res;
   F_BLOCK_TRANSFER *nano_block;

   if (!nanoBlock) {
      THROW_NANO_BLOCK_EXCEPTION(env, "verifySignatureNanoBlock: Missing Nano Block", 190);
      return JNI_FALSE;
   }

   if ((err=java_NanoBlock_2_C_NanoBlock_util(&nano_block, env, nanoBlock, "verifySignatureNanoBlock"))) {
      THROW_NANO_BLOCK_EXCEPTION(env, msg, err);
      return JNI_FALSE;
   }

   res=JNI_FALSE;

   if ((err=f_verify_signed_block(nano_block))>0) {
      sprintf(msg, "f_verify_signed_block @ verifySignatureNanoBlock: Error when verify Nano Block signature %d", err);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, err);
   } else if (err==0)
      res=JNI_TRUE;

   memset(nano_block, 0, sizeof(F_BLOCK_TRANSFER));
   free(nano_block);

   return res;
}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    generateNanoSeed
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_mynanojava_blockchain_NanoBlock_generateNanoSeed(JNIEnv *env, jobject thisObj, jint entropy)
{
   int err;
   jstring res;
   uint32_t entropy_level;
   uint8_t *p;

   if ((entropy_level=f_sel_to_entropy_level(entropy))==0) {
      sprintf(msg, "generateNanoSeed: Invalid entropy number %d", entropy);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, 177);
      return NULL;
   }

   f_random_attach(gen_rand_no_entropy_util);

   if ((err=f_generate_nano_seed(p=(uint8_t *)&msg[(sizeof(msg)>>1)], entropy_level))) {
      sprintf(msg, "f_generate_nano_seed @ generateNanoSeed: Can't generate SEED with entropy level %u", entropy_level);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, err);
   }

   f_random_detach();

   if (err)
      return NULL;

   if (!(res=(*env)->NewStringUTF(env, (const char *)fhex2strv2(msg, (const void *)p, 32, 1))))
      throwError(env, JAVA_ERR_PARSE_UTF8_STRING);

   memset(msg, 0, sizeof(msg));

   return res;
}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    fromNanoSeed
 * Signature: (Ljava/lang/String;I)Lorg/mynanojava/wallet/NanoKeyPair;
 */
#define NANO_KEY_PAIR_CLASS_ACCOUNT_NUMBER "accountNumber"
#define NANO_KEY_PAIR_CLASS_PUBLIC_KEY "publicKey"
#define NANO_KEY_PAIR_CLASS_PRIVATE_KEY "privateKey"
#define THROW_NANO_KEY_PAIR_EXCEPTION(env, msg, err) throwNewException(env, NANO_KEY_PAIR_EXCEPTION_CLASS, msg, err)
JNIEXPORT jobject JNICALL Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed(JNIEnv *env, jobject thisObj, jstring nanoSeed, jlong number)
{
   int err;
   jobject res;
   jclass jNanoKeyPairClass;
   jmethodID methodId;
   const char *c_nano_seed;
   uint8_t *c_private_key, *c_public_key;

   if (!nanoSeed) {
      THROW_NANO_KEY_PAIR_EXCEPTION(env, "Missing Nano SEED", 289);
      return NULL;
   }

   if (number<0) {
      THROW_NANO_KEY_PAIR_EXCEPTION(env, "Negative number for wallet is forbidden", 290);
      return NULL;
   }

   if ((uint64_t)number>(uint64_t)((uint32_t)-1)) {
      sprintf(msg, "Invalid Nano account number %ld", (long int)number);
      THROW_NANO_KEY_PAIR_EXCEPTION(env, msg, 291);
      return NULL;
   }

   if (!(c_nano_seed=(*env)->GetStringUTFChars(env, nanoSeed, NULL))) {
      THROW_NANO_KEY_PAIR_EXCEPTION(env, "fromNanoSeed: Error when parse Nano SEED", 292);
      return NULL;
   }

   res=NULL;

   if ((err=array32bytes_str_to_hex_util((uint8_t *)msg, c_nano_seed))) {
      sprintf(msg, "array32bytes_str_to_hex_util @ fromNanoSeed: hex 2 bin error %d", err);
      THROW_NANO_KEY_PAIR_EXCEPTION(env, msg, err);
      goto Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT2;
   }

   if ((err=f_seed_to_nano_wallet(c_private_key=((uint8_t *)msg+128), c_public_key=((uint8_t *)msg+256), (uint8_t *)msg, (uint32_t)number))) {
      sprintf(msg, "f_seed_to_nano_wallet @ fromNanoSeed: Key pair parse error %d", err);
      THROW_NANO_KEY_PAIR_EXCEPTION(env, msg, err);
      goto Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT2;
   }

   if (!(jNanoKeyPairClass=(*env)->FindClass(env, NANO_KEY_PAIR_CLASS_PATH))) {;
      THROW_NANO_KEY_PAIR_EXCEPTION(env, "fromNanoSeed error: "NANO_KEY_PAIR_CLASS_PATH, 294);
      goto Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT2;
   }

   if (!(methodId=NANO_JAVA_GET_INIT_METHOD(jNanoKeyPairClass))) {
      THROW_NANO_KEY_PAIR_EXCEPTION(env, "Can't init method: "NANO_KEY_PAIR_CLASS_PATH, 295);
      goto Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT2;
   }

   if (!(res=(*env)->NewObject(env, jNanoKeyPairClass, methodId))) {
      THROW_NANO_KEY_PAIR_EXCEPTION(env, "Can't create object at class: "NANO_KEY_PAIR_CLASS_PATH, 296);
      goto Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT2;
   }

   if ((err=setLongField_util(env, res, jNanoKeyPairClass, NANO_KEY_PAIR_CLASS_ACCOUNT_NUMBER, (long long int)number))) {
      sprintf(msg, "setLongField_util @ fromNanoSeed. Error when setting Nano account number. Err = %d", err);
      THROW_NANO_KEY_PAIR_EXCEPTION(env, msg, err);
      goto Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT3;
   }

   if ((err=setByteArrayFieldId_util(env, res, jNanoKeyPairClass, NANO_KEY_PAIR_CLASS_PUBLIC_KEY, c_public_key, 32))) {
      THROW_NANO_KEY_PAIR_EXCEPTION(env, "Error when set public key", err);
      goto Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT3;
   }

   if ((err=setByteArrayFieldId_util(env, res, jNanoKeyPairClass, NANO_KEY_PAIR_CLASS_PRIVATE_KEY, c_private_key, 32))) {
      THROW_NANO_KEY_PAIR_EXCEPTION(env, "Error when set private key", err);
      goto Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT3;
   }

   goto Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT2;

Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT3:
   (*env)->DeleteLocalRef(env, res);
   res=NULL;

Java_org_mynanojava_blockchain_NanoBlock_fromNanoSeed_EXIT2:
   memset(msg, 0, sizeof(msg));

   (*env)->ReleaseStringUTFChars(env, nanoSeed, c_nano_seed);

   return res;
}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    getByteNanoBlockHash
 * Signature: ([B)[B
 */
JNIEXPORT jbyteArray JNICALL Java_org_mynanojava_blockchain_NanoBlock_getByteNanoBlockHash(JNIEnv *env, jobject thisObj, jbyteArray nanoBlock)
{
   int err;
   jbyteArray outByteArray;
   jbyte *c_byte_array;
   jsize jSize;
   uint8_t *hash;

   if (!nanoBlock) {
      THROW_NANO_BLOCK_EXCEPTION(env, "getByteNanoBlockHash: Missing Nano Block", 226);
      return NULL;
   }

   if (!(c_byte_array=(*env)->GetByteArrayElements(env, nanoBlock, NULL))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "getByteNanoBlockHash: Can't get Nano Block in Byte Array", 227);
      return NULL;
   }

   outByteArray=NULL;

   jSize=(*env)->GetArrayLength(env, nanoBlock);
   if ((*env)->ExceptionCheck(env)) {
      THROW_NANO_BLOCK_EXCEPTION(env, "getByteNanoBlockHash: Can't calculate size of the Nano block", 228);
      goto Java_org_mynanojava_blockchain_NanoBlock_getByteNanoBlockHash_EXIT1;
   }

   if (jSize!=sizeof(F_BLOCK_TRANSFER)) {
      sprintf(msg, "getByteNanoBlockHash: Invalid Nano block size %d", jSize);
      THROW_NANO_BLOCK_EXCEPTION(env, msg, 229);
      goto Java_org_mynanojava_blockchain_NanoBlock_getByteNanoBlockHash_EXIT1;
   }

   if ((err=f_nano_get_block_hash(hash=((uint8_t *)msg+128), (F_BLOCK_TRANSFER *)c_byte_array))) {
      THROW_NANO_BLOCK_EXCEPTION(env, "f_nano_get_block_hash @ getByteNanoBlockHash: Unable to get Nano Block Hash", err);
      goto Java_org_mynanojava_blockchain_NanoBlock_getByteNanoBlockHash_EXIT1;
   }

   if (!(outByteArray=(*env)->NewByteArray(env, 32))) {
      throwError(env, "getByteNanoBlockHash: Can't create JNI byte array");
      goto Java_org_mynanojava_blockchain_NanoBlock_getByteNanoBlockHash_EXIT1;
   }

   (*env)->SetByteArrayRegion(env, outByteArray, 0, 32, (const jbyte *)hash);

Java_org_mynanojava_blockchain_NanoBlock_getByteNanoBlockHash_EXIT1:
   (*env)->ReleaseByteArrayElements(env, nanoBlock, c_byte_array, JNI_ABORT);

   return outByteArray;
}

/*
 * Class:     org_mynanojava_blockchain_NanoBlock
 * Method:    getNanoBlockHash
 * Signature: (Lorg/mynanojava/blockchain/NanoBlock;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_org_mynanojava_blockchain_NanoBlock_getNanoBlockHash(JNIEnv *env, jobject thisObj, jobject nanoBlock)
{
   int err;
   jbyteArray outByteArray;
   uint8_t *hash;
   F_BLOCK_TRANSFER *nano_block;

   if (!nanoBlock) {
      THROW_NANO_BLOCK_EXCEPTION(env, "getNanoBlockHash: Missing Nano Block", 225);
      return NULL;
   }

   if ((err=java_NanoBlock_2_C_NanoBlock_util(&nano_block, env, nanoBlock, "getNanoBlockHash"))) {
      THROW_NANO_BLOCK_EXCEPTION(env, msg, err);
      return NULL;
   }

   if ((err=f_nano_get_block_hash(hash=((uint8_t *)msg+128), nano_block))) {
      outByteArray=NULL;
      THROW_NANO_BLOCK_EXCEPTION(env, "f_nano_get_block_hash @ getNanoBlockHash: Unable to get Nano Block Hash", err);
      goto Java_org_mynanojava_blockchain_NanoBlock_getNanoBlockHash_EXIT1;
   }

   if (!(outByteArray=(*env)->NewByteArray(env, 32))) {
      throwError(env, "getNanoBlockHash: Can't create JNI byte array");
      goto Java_org_mynanojava_blockchain_NanoBlock_getNanoBlockHash_EXIT1;
   }

   (*env)->SetByteArrayRegion(env, outByteArray, 0, 32, (const jbyte *)hash);

Java_org_mynanojava_blockchain_NanoBlock_getNanoBlockHash_EXIT1:
   memset(nano_block, 0, sizeof(F_BLOCK_TRANSFER));
   free(nano_block);

   return outByteArray;
}

#define NANO_JAVA_INIT_THROWABLE_WITH_CODE(class) (*env)->GetMethodID(env, class, "<init>", "(Ljava/lang/String;I)V")
jint throwNewException(JNIEnv *env, const char *class, const char *message, int error)
{
   jint err;
   jclass exClass;
   jfieldID field;
   jmethodID methodId;
   jobject jErrObj;
   jstring errMsg;
   static char err_msg[1024];

   if (!(exClass=(*env)->FindClass(env, class))) {
      if (class==NANO_JAVA_LANG_EXCEPTION_CLASS)
         return 1;

      if (!(exClass=(*env)->FindClass(env, NANO_JAVA_LANG_EXCEPTION_CLASS)))
         return 2;

      sprintf(err_msg, "throwNewException: Class not found '%s' with related message = '%s'", class, message);

      message=err_msg;
   } else if (error) {

      if (!(errMsg=(*env)->NewStringUTF(env, message))) {
         sprintf(err_msg, "throwNewException: Cannot set message '%s' into throwable class '%s' and errcode %d", message, class, error);
         throwError(env, err_msg);
         return 3;
      }

      if (!(methodId=NANO_JAVA_INIT_THROWABLE_WITH_CODE(exClass))) {
         sprintf(err_msg, "throwNewException: Cannot initialize throwable class '%s' with message '%s' and errcode %d", class, message, error);
         throwError(env, err_msg);
         err=4;
         goto throwNewException_EXIT1;
      }

      if (!(jErrObj=(*env)->NewObject(env, exClass, methodId, errMsg, error))) {
         sprintf(err_msg, "throwNewException: Cannot create throwable class '%s' with message '%s' and errcode %d", class, message, error);
         throwError(env, err_msg);
         err=5;
      } else if ((err=(*env)->Throw(env, jErrObj))) {
         (*env)->DeleteLocalRef(env, jErrObj);
         sprintf(err_msg, "throwNewException: Can't throw '%s' with message '%s' and errcode %d", class, message, error);
         throwError(env, err_msg);
      }

throwNewException_EXIT1:
      (*env)->DeleteLocalRef(env, errMsg);

      return err;

   }

   return (*env)->ThrowNew(env, exClass, message);
}

