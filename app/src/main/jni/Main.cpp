#include <list>
#include <vector>
#include <codecvt>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "Menu/Setup.h"
#include "Includes/Unity.hpp"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"

void (*SceneManager$LoadScene) (monoString* scene);

void (*MainMenu$SetActive) (void* ref, bool value);

void* (*old_MainMenu$Start) (void* ref);
void* (MainMenu$Start) (void* ref) {
    if (ref != NULL) {
        MainMenu$SetActive(ref, true);
    }
    old_MainMenu$Start(ref);
}

// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    //Anti-lib rename
    /*
    do {
        sleep(1);
    } while (!isLibraryLoaded("libYOURNAME.so"));*/

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

    sleep(2);

    //PATCH("0x2868B1C", "01 00 A0 E3 1E FF 2F E1"); // BX.App.Sys.NetManager.get_IsOnline
    PATCH("0x2A9EA1C", "01 00 A0 E3 1E FF 2F E1"); // BX.App.Sys.NetUtil.get_IsOfflineMode
    PATCH("0x29536BC", "01 00 A0 E3 1E FF 2F E1"); // BX.App.Field.MainMenu.GetActive <--- FAILED ATTEMPT, WILL KEEP
    PATCH("0x2AF7E0C", "01 00 A0 E3 1E FF 2F E1"); // BX.App.Title.TitleProc.serverDataCheckValidData
    PATCH("0x2CAC908", "01 00 A0 E3 1E FF 2F E1"); // Woncomz.Modules.LineSDK.LineSDKListener.get_SupportLanguaged
    //PATCH("0x2868A84", "01 00 A0 E3 1E FF 2F E1"); // BX.App.Sys.NetManager.get_IsLogin
    //PATCH("0x2A9EA2C", "01 00 A0 E3 1E FF 2F E1"); // BX.App.Sys.NetUtil.get_WasLogin
    //PATCH("0x2868B38", "01 00 A0 E3 1E FF 2F E1"); // BX.App.Sys.NetManager.get_InternetReachability -> ReachableViaCarrierDataNetwork, will change!
    //PATCH("0x286928C", "01 00 A0 E3 1E FF 2F E1"); // BX.App.Sys.NetManager.get_IsDeveloper // DELETE THIS IF THERE ARE ANY SIDE EFFECTS

    // AddMoneyExample = (void (*)(void *, int)) getAbsoluteAddress(targetLibName, 0x123456);
    SceneManager$LoadScene = (void (*)(monoString*)) getAbsoluteAddress(targetLibName, 0x1B87EF8);
    MainMenu$SetActive = (void (*)(void*, bool)) getAbsoluteAddress(targetLibName, 0x29532A4);

    PATCH("0x2AF73F4", "1E FF 2F E1"); // BX.App.Title.TitleProc.LanguageSetting
    PATCH("0x2AA2024", "1E FF 2F E1"); // BX.App.Sys.PurchaseManager.Init
    // PATCH("0x2BD5FF4", "01 00 A0 E3 1E FF 2F E1"); // BI.RequestGameLanguageList

    sleep(1);

    HOOK("0x29532A4", MainMenu$Start, old_MainMenu$Start);

    LOGI(OBFUSCATE("Done"));

    // SceneManager$LoadScene(CreateIl2cppString("!System"));

    //Anti-leech
    /*if (!iconValid || !initValid || !settingsValid) {
        //Bad function to make it crash
        sleep(5);
        int *p = 0;
        *p = 0;
    }*/

    return NULL;
}

// Do not change or translate the first text unless you know what you are doing
// Assigning feature numbers is optional. Without it, it will automatically count for you, starting from 0
// Assigned feature numbers can be like any numbers 1,3,200,10... instead in order 0,1,2,3,4,5...
// ButtonLink, Category, RichTextView and RichWebView is not counted. They can't have feature number assigned
// Toggle, ButtonOnOff and Checkbox can be switched on by default, if you add True_. Example: CheckBox_True_The Check Box
// To learn HTML, go to this page: https://www.w3schools.com/

jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    const char *features[] = {
    };

    //Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    switch (featNum) {
    }
}

__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

int RegisterMenu(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
            {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
            {OBFUSCATE("IsGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
            {OBFUSCATE("Init"),  OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)V"), reinterpret_cast<void *>(Init)},
            {OBFUSCATE("SettingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(SettingsList)},
            {OBFUSCATE("GetFeatureList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(GetFeatureList)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Menu"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterPreferences(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Preferences"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterMain(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("CheckOverlayPermission"), OBFUSCATE("(Landroid/content/Context;)V"), reinterpret_cast<void *>(CheckOverlayPermission)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Main"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;

    return JNI_OK;
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (RegisterMenu(env) != 0)
        return JNI_ERR;
    if (RegisterPreferences(env) != 0)
        return JNI_ERR;
    if (RegisterMain(env) != 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}
