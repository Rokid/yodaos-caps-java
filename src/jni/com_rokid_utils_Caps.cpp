#include <functional>
#include "jni.h"
#include "caps.h"

#define ERROR_INVALID_PARAM -1
#define ERROR_INCORRECT_TYPE -6
#define ERROR_INSUFF_BUFFER -1000
#define ERROR_DESTROYED -1100

using namespace std;

class NativeCaps {
public:
  static NativeCaps* get(JNIEnv* env, jobject obj);

  static jobject createReadResult(JNIEnv* env);

  static jobject createReadBinaryResult(JNIEnv* env);

  static void setReadResultErr(JNIEnv* env, jobject obj, jint err);

  static void setReadResultInteger(JNIEnv* env, jobject obj, jlong v);

  static void setReadResultNumber(JNIEnv* env, jobject obj, jdouble v);

  static void setReadResultValue(JNIEnv* env, jobject obj, jobject v);

  static void setReadBinaryResultErr(JNIEnv* env, jobject obj, jint err);

  static void setReadBinaryResultByteArray(JNIEnv* env, jobject obj, jlong v);

  static void setReadBinaryResultSize(JNIEnv* env, jobject obj, jint v);

public:
  shared_ptr<Caps> caps;

public:
  static jfieldID capsHandleField;
  static jfieldID readResultErrField;
  static jfieldID readResultIntegerField;
  static jfieldID readResultNumberField;
  static jfieldID readResultValueField;
  static jfieldID readBinaryResultErrField;
  static jfieldID readBinaryResultByteArrayField;
  static jfieldID readBinaryResultSizeField;
  static jclass readResultClass;
  static jclass readBinaryResultClass;
  static function<void(JNIEnv*, jobject, NativeCaps*)> genReadResultFuncs[6];
};

jfieldID NativeCaps::capsHandleField;
jfieldID NativeCaps::readResultErrField;
jfieldID NativeCaps::readResultIntegerField;
jfieldID NativeCaps::readResultNumberField;
jfieldID NativeCaps::readResultValueField;
jfieldID NativeCaps::readBinaryResultErrField;
jfieldID NativeCaps::readBinaryResultByteArrayField;
jfieldID NativeCaps::readBinaryResultSizeField;
jclass NativeCaps::readResultClass;
jclass NativeCaps::readBinaryResultClass;
function<void(JNIEnv*, jobject, NativeCaps*)> NativeCaps::genReadResultFuncs[6];

NativeCaps* NativeCaps::get(JNIEnv* env, jobject obj) {
  jlong v = env->GetLongField(obj, capsHandleField);
  return reinterpret_cast<NativeCaps*>(v);
}

jobject NativeCaps::createReadResult(JNIEnv* env) {
  return env->AllocObject(readResultClass);
}

jobject NativeCaps::createReadBinaryResult(JNIEnv* env) {
  return env->AllocObject(readBinaryResultClass);
}

void NativeCaps::setReadResultErr(JNIEnv* env, jobject obj, jint err) {
  env->SetIntField(obj, readResultErrField, err);
}

void NativeCaps::setReadResultInteger(JNIEnv* env, jobject obj, jlong v) {
  env->SetLongField(obj, readResultIntegerField, v);
}

void NativeCaps::setReadResultNumber(JNIEnv* env, jobject obj, jdouble v) {
  env->SetDoubleField(obj, readResultNumberField, v);
}

void NativeCaps::setReadResultValue(JNIEnv* env, jobject obj, jobject v) {
  env->SetObjectField(obj, readResultValueField, v);
}

void NativeCaps::setReadBinaryResultErr(JNIEnv* env, jobject obj, jint err) {
  env->SetIntField(obj, readBinaryResultErrField, err);
}

void NativeCaps::setReadBinaryResultByteArray(JNIEnv* env, jobject obj, jlong v) {
  env->SetLongField(obj, readBinaryResultByteArrayField, v);
}

void NativeCaps::setReadBinaryResultSize(JNIEnv* env, jobject obj, jint v) {
  env->SetIntField(obj, readBinaryResultSizeField, v);
}

static jlong nativeCreate(JNIEnv* env, jclass clazz) {
  return (jlong)(new NativeCaps());
}

static jlong nativeParse(JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length) {
  if (data == nullptr || length == 0)
    return 0;
  jbyte* p = env->GetByteArrayElements(data, nullptr);
  NativeCaps* ncaps = new NativeCaps();
  if (Caps::parse(p + offset, length, ncaps->caps) != CAPS_SUCCESS) {
    delete ncaps;
    env->ReleaseByteArrayElements(data, p, JNI_ABORT);
    return 0;
  }
  env->ReleaseByteArrayElements(data, p, JNI_ABORT);
  return (jlong)ncaps;
}

static jobject nativeRead(JNIEnv* env, jobject thiz, jint type) {
  NativeCaps* ncaps = NativeCaps::get(env, thiz);
  jobject ret = NativeCaps::createReadResult(env);

  if (ncaps == nullptr) {
    NativeCaps::setReadResultErr(env, ret, ERROR_DESTROYED);
    return ret;
  }
  NativeCaps::genReadResultFuncs[type](env, ret, ncaps);
  return ret;
}

static jobject nativeReadBinary(JNIEnv* env, jobject thiz) {
  jobject obj = NativeCaps::createReadBinaryResult(env);
  NativeCaps* ncaps = NativeCaps::get(env, thiz);
  if (ncaps == nullptr) {
    NativeCaps::setReadBinaryResultErr(env, obj, ERROR_DESTROYED);
    return obj;
  }
  const void* p;
  uint32_t l;
  int32_t r = ncaps->caps->read(p, l);
  if (r != CAPS_SUCCESS) {
    NativeCaps::setReadBinaryResultErr(env, obj, r);
  } else {
    NativeCaps::setReadBinaryResultByteArray(env, obj, (jlong)p);
    NativeCaps::setReadBinaryResultSize(env, obj, l);
  }
  return obj;
}

static jint nativeWriteInt(JNIEnv* env, jobject thiz, jint v) {
  NativeCaps* ncaps = NativeCaps::get(env, thiz);
  if (ncaps == nullptr) {
    return ERROR_DESTROYED;
  }
  return ncaps->caps->write((int32_t)v);
}

static jint nativeWriteFloat(JNIEnv* env, jobject thiz, jfloat v) {
  NativeCaps* ncaps = NativeCaps::get(env, thiz);
  if (ncaps == nullptr) {
    return ERROR_DESTROYED;
  }
  return ncaps->caps->write((float)v);
}

static jint nativeWriteLong(JNIEnv* env, jobject thiz, jlong v) {
  NativeCaps* ncaps = NativeCaps::get(env, thiz);
  if (ncaps == nullptr) {
    return ERROR_DESTROYED;
  }
  return ncaps->caps->write((int64_t)v);
}

static jint nativeWriteDouble(JNIEnv* env, jobject thiz, jdouble v) {
  NativeCaps* ncaps = NativeCaps::get(env, thiz);
  if (ncaps == nullptr) {
    return ERROR_DESTROYED;
  }
  return ncaps->caps->write((double)v);
}

static jint nativeWriteString(JNIEnv* env, jobject thiz, jstring v) {
  NativeCaps* ncaps = NativeCaps::get(env, thiz);
  if (ncaps == nullptr) {
    return ERROR_DESTROYED;
  }
  const char* str = env->GetStringUTFChars(v, nullptr);
  int32_t r = ncaps->caps->write(str);
  env->ReleaseStringUTFChars(v, str);
  return r;
}

static jint nativeWriteBinary(JNIEnv* env, jobject thiz, jbyteArray data, jint offset, jint length) {
  NativeCaps* ncaps = NativeCaps::get(env, thiz);
  if (ncaps == nullptr) {
    return ERROR_DESTROYED;
  }
  jbyte* p = env->GetByteArrayElements(data, nullptr);
  int32_t r = ncaps->caps->write(p + offset, length);
  env->ReleaseByteArrayElements(data, p, JNI_ABORT);
  return r;
}

static jint nativeWriteCaps(JNIEnv* env, jobject thiz, jobject v) {
  NativeCaps* ncaps = NativeCaps::get(env, thiz);
  if (ncaps == nullptr)
    return ERROR_DESTROYED;
  if (v == nullptr)
    return ERROR_INVALID_PARAM;
  NativeCaps* wcaps = NativeCaps::get(env, v);
  if (wcaps == nullptr)
    return ERROR_INVALID_PARAM;
  return ncaps->caps->write(wcaps->caps);
}

static jint nativeSerialize(JNIEnv* env, jobject thiz, jbyteArray result, jint offset, jint length, jint flags) {
  NativeCaps* ncaps = NativeCaps::get(env, thiz);
  if (ncaps == nullptr)
    return ERROR_DESTROYED;
  jbyte* buf = env->GetByteArrayElements(result, nullptr);
  int32_t r = ncaps->caps->serialize(buf + offset, length, flags);
  if (r <= length)
    env->ReleaseByteArrayElements(result, buf, JNI_COMMIT);
  else
    env->ReleaseByteArrayElements(result, buf, JNI_ABORT);
  return r;
}

static jint nativeGetByIndex(JNIEnv* env, jobject thiz, jlong handle, jint idx) {
  const int8_t* p = reinterpret_cast<const int8_t*>(handle);
  return p[idx];
}

static void nativeGet(JNIEnv* env, jobject thiz, jlong handle, jint from, jbyteArray result, jint offset, jint length) {
  const jbyte* p = reinterpret_cast<const jbyte*>(handle);
  env->SetByteArrayRegion(result, offset, length, p + from);
}

static void nativeInit(JNIEnv* env, jclass clazz, jclass cls1, jclass cls2) {
  NativeCaps::capsHandleField = env->GetFieldID(clazz, "_handle", "J");
  NativeCaps::readResultErrField = env->GetFieldID(cls1, "err", "I");
  NativeCaps::readResultIntegerField = env->GetFieldID(cls1, "integer", "J");
  NativeCaps::readResultNumberField = env->GetFieldID(cls1, "number", "D");
  NativeCaps::readResultValueField = env->GetFieldID(cls1, "value", "Ljava/lang/Object;");
  NativeCaps::readResultClass = (jclass)env->NewGlobalRef(cls1);

  NativeCaps::readBinaryResultErrField = env->GetFieldID(cls2, "err", "I");
  NativeCaps::readBinaryResultByteArrayField = env->GetFieldID(cls2, "byteArray", "J");
  NativeCaps::readBinaryResultSizeField = env->GetFieldID(cls2, "size", "I");
  NativeCaps::readBinaryResultClass = (jclass)env->NewGlobalRef(cls2);

  NativeCaps::genReadResultFuncs[0] = [](JNIEnv* env, jobject obj, NativeCaps* ncaps) {
    int32_t v;
    int32_t r = ncaps->caps->read(v);
    if (r != CAPS_SUCCESS) {
      NativeCaps::setReadResultErr(env, obj, r);
    } else {
      NativeCaps::setReadResultInteger(env, obj, v);
    }
  };
  NativeCaps::genReadResultFuncs[1] = [](JNIEnv* env, jobject obj, NativeCaps* ncaps) {
    float v;
    int32_t r = ncaps->caps->read(v);
    if (r != CAPS_SUCCESS) {
      NativeCaps::setReadResultErr(env, obj, r);
    } else {
      NativeCaps::setReadResultNumber(env, obj, v);
    }
  };
  NativeCaps::genReadResultFuncs[2] = [](JNIEnv* env, jobject obj, NativeCaps* ncaps) {
    int64_t v;
    int32_t r = ncaps->caps->read(v);
    if (r != CAPS_SUCCESS) {
      NativeCaps::setReadResultErr(env, obj, r);
    } else {
      NativeCaps::setReadResultInteger(env, obj, v);
    }
  };
  NativeCaps::genReadResultFuncs[3] = [](JNIEnv* env, jobject obj, NativeCaps* ncaps) {
    double v;
    int32_t r = ncaps->caps->read(v);
    if (r != CAPS_SUCCESS) {
      NativeCaps::setReadResultErr(env, obj, r);
    } else {
      NativeCaps::setReadResultNumber(env, obj, v);
    }
  };
  NativeCaps::genReadResultFuncs[4] = [](JNIEnv* env, jobject obj, NativeCaps* ncaps) {
    const char* v;
    int32_t r = ncaps->caps->read(v);
    if (r != CAPS_SUCCESS) {
      NativeCaps::setReadResultErr(env, obj, r);
    } else {
      jstring str = env->NewStringUTF(v);
      NativeCaps::setReadResultValue(env, obj, str);
    }
  };
  NativeCaps::genReadResultFuncs[5] = [](JNIEnv* env, jobject obj, NativeCaps* ncaps) {
    shared_ptr<Caps> v;
    int32_t r = ncaps->caps->read(v);
    if (r != CAPS_SUCCESS) {
      NativeCaps::setReadResultErr(env, obj, r);
    } else {
      NativeCaps* rv = new NativeCaps();
      rv->caps = v;
      NativeCaps::setReadResultInteger(env, obj, (jlong)rv);
    }
  };
}

static JNINativeMethod capsMethods[] = {
  { "nativeCreate", "()J", (void*)nativeCreate },
  { "nativeParse", "([BII)J", (void*)nativeParse },
  { "nativeRead", "(I)Lcom/rokid/utils/Caps$ReadResult;", (void*)nativeRead },
  { "nativeReadBinary", "()Lcom/rokid/utils/Caps$ReadBinaryResult;", (void*)nativeReadBinary },
  { "nativeWrite", "(I)I", (void*)nativeWriteInt },
  { "nativeWrite", "(F)I", (void*)nativeWriteFloat },
  { "nativeWrite", "(J)I", (void*)nativeWriteLong },
  { "nativeWrite", "(D)I", (void*)nativeWriteDouble },
  { "nativeWrite", "(Ljava/lang/String;)I", (void*)nativeWriteString },
  { "nativeWrite", "([BII)I", (void*)nativeWriteBinary },
  { "nativeWrite", "(Lcom/rokid/utils/Caps;)I", (void*)nativeWriteCaps },
  { "nativeSerialize", "([BIII)I", (void*)nativeSerialize },
  { "nativeInit", "(Ljava/lang/Class;Ljava/lang/Class;)V", (void*)nativeInit }
};

static JNINativeMethod nativeInputStreamMethods[] = {
  { "nativeGetByIndex", "(JI)I", (void*)nativeGetByIndex },
  { "nativeGet", "(JI[BII)V", (void*)nativeGet }
};

static int registerCapsMethods(JNIEnv* env) {
  const char* klass = "com/rokid/utils/Caps";
  jclass cls = env->FindClass(klass);
  if (cls == nullptr)
    return -1;
  return env->RegisterNatives(cls, capsMethods,
      sizeof(capsMethods) / sizeof(JNINativeMethod));
}

static int registerNativeInputStreamMethods(JNIEnv* env) {
  const char* klass = "Lcom/rokid/utils/Caps$NativeInputStream;";
  jclass cls = env->FindClass(klass);
  if (cls == nullptr)
    return -1;
  return env->RegisterNatives(cls, nativeInputStreamMethods,
      sizeof(nativeInputStreamMethods) / sizeof(JNINativeMethod));
}

extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;

  if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
    return -1;
  }
  if (registerCapsMethods(env) < 0)
    return -1;
  if (registerNativeInputStreamMethods(env) < 0)
    return -1;
  return JNI_VERSION_1_4;
}
