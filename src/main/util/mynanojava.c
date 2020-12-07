#include <jni.h>
#include <f_nano_crypto_util.h>
#include <org_mynanojava_MyNanoJava.h>
//java -Djava.library.path=. -jar
#define EMPTY_STR ""
#define MY_NANO_EMBEDDED_ERROR "myNanoEmbedded C library error in function \"%s\" %d"
#define JAVA_ERR_PARSE_UTF8_STRING "Error on parsing UTF-8 string. Maybe internal error or OutOfMemory"
static char msg[512];

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
/*
JNIEXPORT jbyteArray JNICALL Java_org_mynanojava_MyNanoJava_nano_1create_1block(
   JNIEnv *env, jobject thisObject, jstring account, jstring previous, jstring representative, jstring balance,
   jint balance_type, jstring value_to_send_or_receive, jint value_to_send_or_receive_type, jstring link, jint direction
)
{

   return NULL;
}
*/
jint throwError(JNIEnv *env, char *message)
{
   jclass exClass;
   char *className = "java/lang/Exception";

   exClass = (*env)->FindClass(env, className);
   if (exClass == NULL)
      return 1;

   return (*env)->ThrowNew(env, exClass, message);
}

