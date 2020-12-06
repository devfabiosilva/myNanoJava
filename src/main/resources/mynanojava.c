#include <stdio.h>
#include <jni.h>
#include <f_nano_crypto_util.h>
#include <org_mynanojava_MyNanoJava.h>
//java -Djava.library.path=. -jar
JNIEXPORT void JNICALL Java_org_mynanojava_MyNanoJava_sayHello(JNIEnv *env, jobject thisObject)
{
   printf("Hello World!\n");
   return;
}

JNIEXPORT jstring JNICALL Java_org_mynanojava_MyNanoJava_license(JNIEnv *env, jobject thisObject)
{
   return (*env)->NewStringUTF(env, LICENSE);
}

