#include <jni.h>
#include <memory>
#include "Renderer.h"
#include <android/asset_manager_jni.h>

std::unique_ptr<Renderer> mRenderer;


extern "C" {
JNIEXPORT void JNICALL
Java_com_willitwork_javacppgles_MyCppRenderer__1init(JNIEnv *env, jobject instance,  jobject assetManager) {

    mRenderer->android_asset_manager  = AAssetManager_fromJava(env, assetManager);

    mRenderer->init();
}

JNIEXPORT void JNICALL
Java_com_willitwork_javacppgles_MyCppRenderer__1draw(JNIEnv *env, jobject instance ) {
    mRenderer->draw();
}


/**
 * Creates renderer instance
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    mRenderer = std::unique_ptr<Renderer> { new Renderer{}};

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }


//    if (!register_natives(env)) {
//
//      int x = 0;
//    }

    return JNI_VERSION_1_6;
}


#if 0

void
Java_org_gpu_glload_native_init(JNIEnv* env, jclass clazz,
                                jobject asset_manager,
                                jstring args,
                                jstring log_file)
{

}

static JNINativeMethod glmark2_native_methods[] = {
        {
                "init",
                "(Landroid/content/res/AssetManager;Ljava/lang/String;Ljava/lang/String;)V",
                reinterpret_cast<void *>(Java_org_gpu_glload_native_init)
        },
};



static int
register_native_methods(JNIEnv* env, const char* className,
                        JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;


    //pingThread->start();

    clazz = env->FindClass(className);
    if (clazz == NULL) {

        return JNI_FALSE;
    }
//    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
//
//        return JNI_FALSE;
//    }

    return JNI_TRUE;
}

static int
register_natives(JNIEnv *env)
{
    const char* const class_path_name = "org/gpu/glload/GlloadNative";
    return register_native_methods(env, class_path_name,
                                   glmark2_native_methods,
                                   sizeof(glmark2_native_methods) /
                                   sizeof(glmark2_native_methods[0]));
}


#endif










}