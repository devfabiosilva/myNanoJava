#include <jni.h>
#include <f_nano_crypto_util.h>

jint throwNewException(JNIEnv *, const char *, const char *, int);
#define NANO_JAVA_LANG_EXCEPTION_CLASS "java/lang/Exception"
#define throwError(env, msg) throwNewException(env, NANO_JAVA_LANG_EXCEPTION_CLASS, msg, 0)
#define NANO_JAVA_GET_INIT_METHOD(class) (*env)->GetMethodID(env, class, "<init>", "()V")
#define JAVA_ERR_PARSE_UTF8_STRING "Error on parsing UTF-8 string. Maybe internal error or OutOfMemory"
#define NANO_JAVA_BYTE_ARRAY_TYPE "[B"
#define NANO_JAVA_LONG_TYPE "J"
#define NANO_JAVA_INT_TYPE "I"

static char msg[768];

void gen_rand_no_entropy_util(void *, size_t);
int array32bytes_str_to_hex_util(uint8_t *, const char *);

