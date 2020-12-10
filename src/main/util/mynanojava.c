#include <jni.h>
#include <f_nano_crypto_util.h>
//java -Djava.library.path=. -jar
////https://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/functions.html
#define EMPTY_STR ""
#define MY_NANO_EMBEDDED_ERROR "myNanoEmbedded C library error in function \"%s\" %d"
#define JAVA_ERR_PARSE_UTF8_STRING "Error on parsing UTF-8 string. Maybe internal error or OutOfMemory"
static char msg[512];
/*
int nano_java_parse_to_pk_util(uint32_t *pk, int *is_xrb_prefix, const char *pk_or_wallet_address)
{
   int err;
   uint8_t pk_tmp[MAX_STR_NANO_CHAR];

   if (is_xrb_prefix)
      *is_xrb_prefix=0;

   if (is_nano_prefix(pk_or_wallet_address, NANO_PREFIX))
      goto nano_java_parse_to_pk_util_PASS1;
   else if (is_nano_prefix(pk_or_wallet_address, XRB_PREFIX)) {
      if (is_xrb_prefix)
         *is_xrb_prefix=1;
nano_java_parse_to_pk_util_PASS1:
      err=nano_base_32_2_hex(pk_tmp, (char *)pk_or_wallet_address);
   } else if (!(err=f_str_to_hex(pk_tmp, (char *)pk_or_wallet_address)))
      memcpy(pk, pk_tmp, 32);

   return err;
}
*/
/*
int nano_java_is_pk_str_util(uint32_t *pk, const char *pk_or_wallet_address)
{
   int err=1;
   size_t sz_tmp;
   uint8_t pk_tmp[MAX_STR_NANO_CHAR];

   if ((is_nano_prefix(pk_or_wallet_address, NANO_PREFIX))||(is_nano_prefix(pk_or_wallet_address, XRB_PREFIX))) err=0;
   else if ((sz_tmp=strnlen(pk_or_wallet_address, MAX_STR_NANO_CHAR)==MAX_STR_NANO_CHAR)) err=-1;
   else if (sz_tmp!=64) err=-2;
   else if (f_str_to_hex(pk_tmp, (char *)pk_or_wallet_address)) err=-3;
   else memcpy(pk, pk_tmp, 32);

   return err;
}
*/

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
   const char *in_A_Str, *in_B_Str;
   char c_ret[F_RAW_STR_MAX_SZ];

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

   if ((err = f_nano_add_sub((void *)c_ret, (void *)in_A_Str, (void *)in_B_Str, (uint32_t)mode))) {
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

jint throwError(JNIEnv *env, char *message)
{
   jclass exClass;
   char *className = "java/lang/Exception";

   exClass = (*env)->FindClass(env, className);
   if (exClass == NULL)
      return 1;

   return (*env)->ThrowNew(env, exClass, message);
}

