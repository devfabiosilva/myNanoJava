#include <stdio.h>
#include <jni.h>
#include <f_nano_crypto_util.h>
#include <org_mynanojava_MyNanoJava.h>
//java -Djava.library.path=. -jar
#define EMPTY_STR ""
#define MY_NANO_EMBEDDED_ERROR "myNanoEmbedded C library error in function \"%s\" %d"
static char msg[512];

jint throwError(JNIEnv *, char *);
JNIEXPORT void JNICALL Java_org_mynanojava_MyNanoJava_sayHello(JNIEnv *env, jobject thisObject)
{
   printf("Hello World!\n");
   return;
}

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

   ret = (*env)->NewStringUTF(env, c_ret);

Java_org_mynanojava_MyNanoJava_nano_1add_1sub_EXIT2:
   (*env)->ReleaseStringUTFChars(env, b, in_B_Str);

Java_org_mynanojava_MyNanoJava_nano_1add_1sub_EXIT1:
   (*env)->ReleaseStringUTFChars(env, a, in_A_Str);

   return ret;
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

