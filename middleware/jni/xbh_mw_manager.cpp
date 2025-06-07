#define LOG_TAG "xbh_mw@xbh_mw_manager"

#include <log/log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <utils/threads.h>
#include <utils/Mutex.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryBase.h>
#include <utils/String8.h>

#include "IXbhListener.h"

#include <hwbinder/Parcel.h>
#include <vendor/xbh/hardware/xbhhwmw/1.0/IXbhHwmw.h>
#include <vendor/xbh/hardware/xbhhwmw/1.0/IXbhHwmwCallback.h>

#include "jni.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_os_Parcel.h"
#include "XbhType.h"

using namespace android;
using ::vendor::xbh::hardware::xbhhwmw::V1_0::IXbhHwmw;
using ::vendor::xbh::hardware::xbhhwmw::V1_0::IXbhHwmwCallback;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

#define UNUSED(x) (void)x


typedef struct tarFIELD_S {
    jfieldID context;
    jmethodID post_event;
    JavaVM *pVM;
} FIELDS_S;

FIELDS_S g_fields;
static Mutex g_lock;

static struct {
    jclass clazz;
    jfieldID mNativePtr;
    jmethodID obtain;
    jmethodID recycle;
} gParcelOffsets;

jobject XbhCreateJavaParcelObject(JNIEnv* env)
{
    if (env == XBH_NULL) {
        ALOGE("null pointer.");
        return XBH_NULL;
    }

    return env->CallStaticObjectMethod(gParcelOffsets.clazz, gParcelOffsets.obtain);
}

Parcel* XbhParcelForJavaObject(JNIEnv* env, jobject obj)
{
    if (obj != XBH_NULL && env != XBH_NULL) {
        Parcel* p = reinterpret_cast<Parcel*>((uintptr_t)env->GetLongField(obj, gParcelOffsets.mNativePtr));
        if (p != XBH_NULL) {
            return p;
        }
    }
    return nullptr;
}

class JNIXbhListener : public IXbhListener, public vendor::xbh::hardware::xbhhwmw::V1_0::IXbhHwmwCallback {
public:
    JNIXbhListener(JNIEnv *env, jobject thiz, jobject weak_thiz);
    ~JNIXbhListener();
    virtual void notify(unsigned int type, const void *data, unsigned int len, const void *prev);
    virtual Return<void> hwNotify(unsigned int, const hidl_string &reply, const hidl_string &request, hwNotify_cb _hidl_cb);

private:
    int CallbackPostEvent(JNIEnv *env, unsigned int type, const void *data, unsigned int len);
    JNIXbhListener();
    jclass mClass;    // Reference to XbhMwManager class
    jobject mObject;  // Weak ref to XbhMwManager Java object to call on
};

JNIXbhListener::JNIXbhListener(JNIEnv *env, jobject thiz, jobject weak_thiz):mClass(XBH_NULL), mObject(XBH_NULL)
{
    // Hold onto the ATVClient class for use in calling the static method
    // that posts events to the application thread.
    if (env == XBH_NULL) {
        ALOGE("null pointer.");
        return;
    }

    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == XBH_NULL) {
        ALOGE("Can't find android/XBHManager");
        return;
    }
    mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the XbhMwManager object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject = env->NewGlobalRef(weak_thiz);
}

JNIXbhListener::~JNIXbhListener()
{
    JNIEnv *env = XBH_NULL;
    if (g_fields.pVM == XBH_NULL) {
        ALOGE("g_fields.pVM is XBH_NULL.");
        return;
    }
    jint attachResult = g_fields.pVM->AttachCurrentThread(&env, XBH_NULL);
    if (attachResult < 0) {
        ALOGE("ERROR: attach current thread failed,attach_result=%d", attachResult);
    }

    if (env != XBH_NULL) {
        env->DeleteGlobalRef(mObject);
        env->DeleteGlobalRef(mClass);
    }

    g_fields.pVM->DetachCurrentThread();
}

int JNIXbhListener::CallbackPostEvent(JNIEnv *env, unsigned int type, const void *data, unsigned int len)
{
    Parcel obj;
    obj.write(data, len);

    if (env == XBH_NULL) {
        ALOGE("env is XBH_NULL.");
        return -1;
    }

    ALOGI("CallbackPostEvent begin.");
    if (data != nullptr && len > 0) { /* tote extra data */
        jobject jParcel = XbhCreateJavaParcelObject(env);
        if (jParcel != XBH_NULL) {
            Parcel *nativeParcel = XbhParcelForJavaObject(env, jParcel);
            if (nativeParcel == XBH_NULL) {
                ALOGE("IparcelForJavaObject fail, nativeParcel=XBH_NULL");
                return -1;
            }
            const XBH_U8 *objData = obj.data();
            if (objData == XBH_NULL) {
                ALOGE("obj data fail, objData=XBH_NULL");
                return -1;
            }
            ALOGI("CallbackPostEvent step1. type:0x%x", type);
            nativeParcel->setData(objData, obj.dataSize());
            
            ALOGI("Have data and callback post event, type:0x%x", type);
            
            ALOGI("CallbackPostEvent step2. type:0x%x", type);
            env->CallStaticVoidMethod(mClass, g_fields.post_event, mObject, type, 0, 0, jParcel);
            ALOGI("CallbackPostEvent step3. type:0x%x", type);

            env->DeleteLocalRef(jParcel);
        }
    } else { /* no extra data */
        ALOGI("no data and callback post_event,type=0x%X (%d)", type, type);
        env->CallStaticVoidMethod(mClass, g_fields.post_event, mObject, type, 0, 0, XBH_NULL);
    }

    return 0;
}

void JNIXbhListener::notify(unsigned int type, const void *data, unsigned int len, const void *prev)
{
    ALOGI("JNIXbhListener::notify begin.");
    XBH_UNUSED(prev);

    JNIEnv *env = XBH_NULL;
    if (g_fields.pVM == XBH_NULL) {
        ALOGE("g_fields.pVM is XBH_NULL.");
        return;
    }
    jint attachResult = g_fields.pVM->AttachCurrentThread(&env, XBH_NULL);
    if (attachResult < 0) {
       ALOGE("ERROR: attach current thread failed,attach_result=%d", attachResult);
    }

    ALOGI("JNIXbhListener::notify step1.");
    if (env == XBH_NULL) {
        ALOGE("env is null,notify fail");
        return;
    }

    int ret = CallbackPostEvent(env, type, data, len);
    if (ret != 0) {
        ALOGE("ERROR: attach current thread failed,attach_result=%d", attachResult);
        return;
    }

    ALOGI("JNIXbhListener::notify step2.");

    if (env->ExceptionCheck()) {
        ALOGE("An exception occurred while notifying an event.");
        env->ExceptionClear();
    }
    g_fields.pVM->DetachCurrentThread();
    ALOGI("JNIXbhListener::notify end.");
}

Return<void> JNIXbhListener::hwNotify(unsigned int type, const hidl_string &reply, const hidl_string &request, hwNotify_cb _hidl_cb)
{
    void *data = (void *)reply.c_str();
    unsigned int length = reply.size();
    hidl_string replyStr = XBH_NULL;
    notify(type, data, length, XBH_NULL);
    _hidl_cb(replyStr);
    return hardware::Void();
}

static void android_xbhhwmanager_native_init(JNIEnv *env)
{
    ALOGI("INFO: native_init enter");
    jclass clazz;

    clazz = env->FindClass("com/xbh/eywa/api/XbhMwManager");
    if (clazz == XBH_NULL) {
        return;
    }
/*
    g_fields.context = env->GetFieldID(clazz, "mNativeContext", "J");
    if (g_fields.context == XBH_NULL) {
        return;
    }
*/
    g_fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
                                                 "(Ljava/lang/Object;IIILandroid/os/Parcel;)V");
    if (g_fields.post_event == XBH_NULL) {
        ALOGE("CAN NOT FIND postEventFromNative ~~~~~~");
        return;
    }
}


static void android_xbhhwmanager_native_setup(JNIEnv *env, jobject thiz, jobject weak_this)
{
    ALOGI("INFO: android_xbhhwmanager_native_setups enter!");
    sp<IXbhHwmw> service = IXbhHwmw::getService();
    if( service == nullptr ){
        ALOGE("Can't find IXbhHwmw service...");
        return;
    }

    sp<JNIXbhListener> listener = new JNIXbhListener(env, thiz, weak_this);
    service->hwRegistCallback(listener);
    return;
}

static jint android_xbhhwmanager_native_invoke(JNIEnv *env, jobject thiz, jobject java_request, jobject java_reply)
{
    jint ret = 0;
    sp<IXbhHwmw> service = IXbhHwmw::getService();
    if( service == nullptr ){
        ALOGE("Can't find IXbhHwmw service...");
        return -1;
    }
    //ALOGD("XbhMwManager::invoke");
    UNUSED(thiz);

    Parcel *request = android::parcelForJavaObject(env, java_request);
    Parcel *reply = android::parcelForJavaObject(env, java_reply);

    hardware::Parcel hwrequest,hwreply;
    hwrequest.setDataSize(request->dataSize());
    hwrequest.setDataPosition(0);
    hwrequest.write(request->data(),request->dataSize());
    hwrequest.setDataPosition(0);
    request->setDataPosition(0);
//    LOGW("CMD:0x%x, first data:%d", request->readInt32(), request->readInt32());
    hardware::hidl_string inputStr((char*)hwrequest.data(), hwrequest.dataSize());

    String8 retreply;
    auto cb = [&](hidl_string strreply){
        retreply = String8(strreply.c_str(), strreply.size());
    };
    service->hwInvoke(inputStr, cb);

    hwreply.setDataSize(retreply.size());
    hwreply.setDataPosition(0);
    hwreply.write(retreply.c_str(), retreply.size());
    hwreply.setDataPosition(0);
    ret = hwreply.readInt32();
    hwreply.setDataPosition(0);
    reply->write(hwreply.data() + sizeof(int), hwreply.dataSize() - sizeof(int));
    reply->setDataPosition(0);
    return ret;
}

static JNINativeMethod gMethods[] =
{
    { "native_init",              "()V",                       (void *)android_xbhhwmanager_native_init },
    { "native_setup",             "(Ljava/lang/Object;)V",     (void *)android_xbhhwmanager_native_setup },
    { "native_invoke", "(Landroid/os/Parcel;Landroid/os/Parcel;)I", (void *)android_xbhhwmanager_native_invoke },
};

static int register_jni_manager(JNIEnv* env)
{
    return AndroidRuntime::registerNativeMethods(env, "com/xbh/eywa/api/XbhMwManager", gMethods, NELEM(gMethods));
}

template <typename T>
static inline T MakeGlobalRefOrDie(JNIEnv* env, T in) {
    jobject res = env->NewGlobalRef(in);
    return static_cast<T>(res);
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UNUSED(reserved);
    JNIEnv* env = XBH_NULL;
    jint result = -1;

    //LOGD("JNI_OnLoad");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        ALOGE("ERROR: GetEnv failed");
        return -1;
    }
    assert(env != XBH_NULL);
    
    g_fields.pVM = vm;

    if (register_jni_manager(env) < 0)
    {
        ALOGE("ERROR: ATVClient native registration failed");
        return -1;
    }
    //LOGD("INFO: ATVClient native registration Ok");
    // for parcel
    jclass clazzParcel = env->FindClass("android/os/Parcel");
    gParcelOffsets.clazz = MakeGlobalRefOrDie(env, clazzParcel);
    gParcelOffsets.mNativePtr = env->GetFieldID(gParcelOffsets.clazz, "mNativePtr", "J");
    gParcelOffsets.obtain = env->GetStaticMethodID(gParcelOffsets.clazz, "obtain", "()Landroid/os/Parcel;");

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    return result;
}
