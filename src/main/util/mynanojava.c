#include <jni.h>
#include <f_nano_crypto_util.h>
//java -Djava.library.path=. -jar
////https://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/functions.html
//https://developer.android.com/training/articles/perf-jni?hl=pt-br
#define EMPTY_STR ""
#define MY_NANO_EMBEDDED_ERROR "myNanoEmbedded C library error in function \"%s\" %d"
#define JAVA_ERR_PARSE_UTF8_STRING "Error on parsing UTF-8 string. Maybe internal error or OutOfMemory"
#define NANO_BLOCK_CLASS_PATH "org/mynanojava/blockchain/NanoBlock"
#define NANO_JAVA_GET_INIT_METHOD(class) (*env)->GetMethodID(env, class, "<init>", "()V")
#define NANO_JAVA_BYTE_ARRAY_TYPE "[B"
#define NANO_JAVA_LONG_TYPE "J"
#define NANO_JAVA_INT_TYPE "I"
#define CANT_FIND_NANO_BLOCK_ERROR "%s: Can't find class org.mynanojava.blockchain.NanoBlock"

static char msg[768];

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

jint throwError(JNIEnv *, char *);

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

   if (!previous) {
      throwError(env, "Missing previous");
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

   if (!(c_previous=(*env)->GetStringUTFChars(env, previous, NULL))) {
      throwError(env, "Get UTF previous error");
      goto Java_org_mynanojava_MyNanoJava_nano_1create_1block_EXIT1;
   }

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

   jfieldID field;
   jbyteArray byte_array;

   if (!(field=(*env)->GetFieldID(env, class, fieldId, NANO_JAVA_BYTE_ARRAY_TYPE)))
      return -40;

   if (!(byte_array=(*env)->NewByteArray(env, c_byte_array_sz)))
      return -41;

   (*env)->SetByteArrayRegion(env, byte_array, 0, c_byte_array_sz, (const jbyte *)c_byte_array);
   (*env)->SetObjectField(env, thisObject, field, byte_array);
   (*env)->DeleteLocalRef(env, byte_array);

   return 0;
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
//   *c_byte_array_sz=0;

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
   jfieldID fieldId;
   jbyte *c_byte_array;
   jbyteArray byte_array;
   uint32_t types;
   F_BLOCK_TRANSFER *nano_block;

   if (!account) {
      throwError(env, "nanoCreateBlock: Missing account");
      return NULL;
   }

   if (!previous) {
      throwError(env, "nanoCreateBlock: Missing previous");
      return NULL;
   }

   if (!representative) {
      throwError(env, "nanoCreateBlock: Missing representative");
      return NULL;
   }

   if (!balance) {
      throwError(env, "nanoCreateBlock: Missing balance");
      return NULL;
   }

   if (!value_to_send_or_receive) {
      throwError(env, "nanoCreateBlock: Missing value to send or receive");
      return NULL;
   }

   if (!link) {
      throwError(env, "nanoCreateBlock: Missing destination wallet or link to open block");
      return NULL;
   }

   if (!(c_account=(*env)->GetStringUTFChars(env, account, NULL))) {
      throwError(env, "nanoCreateBlock: Get UTF account error");
      return NULL;
   }

   jResult=NULL;

   if (!(c_previous=(*env)->GetStringUTFChars(env, previous, NULL))) {
      throwError(env, "nanoCreateBlock: Get UTF previous error");
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT1;
   }

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
         throwError(env, msg);
         goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT6;
      }

   p_val_send_rec=c_value_to_send_or_receive;

   if (types&F_VALUE_SEND_RECEIVE_RAW_128)
      if ((err=nano_java_raw128_str_to_bin_util((uint8_t *)(p_val_send_rec=((const char *)&msg[sizeof(f_uint128_t)])), c_value_to_send_or_receive))) {
         sprintf(msg, "nanoCreateBlock: Error when parse hex send or receive value to bin. "MY_NANO_EMBEDDED_ERROR, "nano_java_raw128_str_to_bin_util", err);
         throwError(env, msg);
         goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT6;
      }

   if ((err=nano_create_block_dynamic(&nano_block, c_account, 0, c_previous, 0, c_representative, 0, p_balance, p_val_send_rec,
      (uint32_t)types, c_link, 0, direction))) {
      sprintf(msg, "nanoCreateBlock: "MY_NANO_EMBEDDED_ERROR, "nano_create_block_dynamic", err);
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT6;
   }

   if (!(jNanoBlockClass=(*env)->FindClass(env, NANO_BLOCK_CLASS_PATH))) {
      sprintf(msg, CANT_FIND_NANO_BLOCK_ERROR, "nanoCreateBlock");
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT7;
   }

   if (!(methodId=NANO_JAVA_GET_INIT_METHOD(jNanoBlockClass))) {
      throwError(env, "nanoCreateBlock: Can't Init new class NanoBlock");
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT7;
   }

   if (!(jResult=(*env)->NewObject(env, jNanoBlockClass, methodId))) {
      throwError(env, "nanoCreateBlock: Can't create NanoBlock Object");
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT7;
   }

   if ((err=setNanoBlockToJVM_util(env, jResult, jNanoBlockClass, nano_block))) {
      throwError(env, msg);
      goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT8;
   }

   goto Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT7;

Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT8:
   (*env)->DeleteLocalRef(env, jResult);

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
   (*env)->ReleaseStringUTFChars(env, previous, c_previous);

Java_org_mynanojava_MyNanoJava_nanoCreateBlock_EXIT1:
   (*env)->ReleaseStringUTFChars(env, account, c_account);

   return jResult;

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
   jclass jNanoBlockClass;
   uint8_t *c_byte_array;
   size_t c_byte_array_sz;
   jobject fieldObj;
   NANO_BLK_TO_JVM *jvm_blk;
   F_BLOCK_TRANSFER *nano_block;
   const char *field;
   jfieldID fieldId;

   if (!nanoBlock) {
      throwError(env, "nanoBlockToJSON: Missing NanoBlock object");
      return NULL;
   }

   if (!(jNanoBlockClass=(*env)->FindClass(env, NANO_BLOCK_CLASS_PATH))) {
      sprintf(msg, CANT_FIND_NANO_BLOCK_ERROR, "nanoBlockToJSON");
      throwError(env, msg);
      return NULL;
   }

   if (!(nano_block=malloc(sizeof(F_BLOCK_TRANSFER)))) {
      throwError(env, "nanoBlockToJSON: Error when creating Nano Block");
      return NULL;
   }

   jvm_blk=blk_to_jvm;
   res=NULL;

   while (field=jvm_blk->key) {
      if (!(err=getByteArrayFieldId_util(&c_byte_array, &c_byte_array_sz, &fieldObj, env, nanoBlock, jNanoBlockClass, field)))
         (c_byte_array_sz==jvm_blk->size)?(void)(memcpy(((uint8_t *)nano_block)+jvm_blk->offset, c_byte_array, c_byte_array_sz)):(void)(err=31);

      if (c_byte_array)
         (*env)->ReleaseByteArrayElements(env, fieldObj, c_byte_array, JNI_ABORT);

      if (err) {

         (err>0)?
            sprintf(msg, "Wrong field[%s] size. Expected size = %lu and returned size = %lu", jvm_blk->key, jvm_blk->size, c_byte_array_sz):
            sprintf(msg, MY_NANO_EMBEDDED_ERROR, "getByteArrayFieldId_util", err);

         throwError(env, msg);

         goto Java_org_mynanojava_blockchain_NanoBlock_nanoBlockToJSON_EXIT1;
      }

      jvm_blk++;
   }

   if ((err=getLongFieldUtil((jlong *)&nano_block->work, env, nanoBlock, jNanoBlockClass, NANO_BLK_WORK))) {
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "getLongFieldUtil @ nanoBlockToJSON", err);
      throwError(env, msg);
      goto Java_org_mynanojava_blockchain_NanoBlock_nanoBlockToJSON_EXIT1;
   }

   if ((err=getIntFieldUtil((jint *)&nano_block->prefixes, env, nanoBlock, jNanoBlockClass, NANO_BLK_PREFIXES))) {
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "getIntFieldUtil @ nanoBlockToJSON", err);
      throwError(env, msg);
      goto Java_org_mynanojava_blockchain_NanoBlock_nanoBlockToJSON_EXIT1;
   }

   memset(nano_block->preamble, 0, 31);
   nano_block->preamble[31]=0x06;

   if ((err=f_nano_block_to_json(msg, NULL, sizeof(msg), nano_block))) {
      sprintf(msg, MY_NANO_EMBEDDED_ERROR, "f_nano_block_to_json @ nanoBlockToJSON", err);
      throwError(env, msg);
      goto Java_org_mynanojava_blockchain_NanoBlock_nanoBlockToJSON_EXIT1;
   }

   if (!(res=(*env)->NewStringUTF(env, msg)))
      throwError(env, "nanoBlockToJSON: Error when parse Nano block to JSON");

Java_org_mynanojava_blockchain_NanoBlock_nanoBlockToJSON_EXIT1:
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

jint throwError(JNIEnv *env, char *message)
{
   jclass exClass;
   char *className = "java/lang/Exception";

   exClass = (*env)->FindClass(env, className);
   if (exClass == NULL)
      return 1;

   return (*env)->ThrowNew(env, exClass, message);
}

