#include <list>
#include <vector>
#include <string.h>
#include <codecvt>
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

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"
#include "Includes/Unity.hpp"

/*bool feature1, feature2, featureHookToggle, Health;
int sliderValue = 1, level = 0;
void *instanceBtn;*/

//void (*AddMoneyExample)(void *instance, int amount);
monoString* (*string$$Replace)(monoString* instance, monoString* oldValue, monoString* newValue);

monoString* (*old_CommonAPI$$GetDomainURL)(monoString* keyName);
monoString* CommonAPI$$GetDomainURL(monoString* keyName) {
    LOGE("CommonAPI.GetDomainURL edited from %s, %s was supposed to be returned", keyName->getChars(), old_CommonAPI$$GetDomainURL(keyName)->getChars());
    return CreateIl2cppString("http://192.168.178.41:5000");
}

monoString* (*old_CryptPlayerPrefs$$Decrypt)(monoString* src);
monoString* CryptPlayerPrefs$$Decrypt(monoString* src) {
    LOGE("Bro is decrypting %s", src->getChars());
    LOGE("Would end up to be %s", old_CryptPlayerPrefs$$Decrypt(src)->getChars());
    return old_CryptPlayerPrefs$$Decrypt(src);
}

monoString* (*old_Crypt$$Encrypt)(monoString* uid);
monoString* Crypt$$Encrypt(monoString* uid) {
    LOGE("Bro is encrypting %s", uid->getChars());
    return uid;
}

void* (*old_NTJson$$ParseJson)(monoString* json);
void* NTJson$$ParseJson(monoString* json) {
    LOGE("Tried to parse json: %s", json->getChars());
    return old_NTJson$$ParseJson(json);
}

void* (*old___Internal$$OnRegisterToken)(bool success, monoString* pushToken);
void* __Internal$$OnRegisterToken(bool success, monoString* pushToken) {
    LOGE("OnRegisterToken success: (i'm assuming true), pushToken: %s", pushToken->getChars());
    return old___Internal$$OnRegisterToken(success, pushToken);
}

monoString* sexmanip(monoString* inst) {
    monoString* manipstr = inst;
    manipstr = string$$Replace(manipstr, CreateIl2cppString("PiG"), CreateIl2cppString("SEWORKS")); // joke
    manipstr = string$$Replace(manipstr, CreateIl2cppString("Ver: 1.2.1"), CreateIl2cppString("Lost3 - Mod menu (and figuring out appsolid)\nsynzr - Servers and hosting them\nToro's Friend Dungeon (discord server) - Archiving the files\n\nCirrus5075 - Greatest nuke supporter (halal)\nDSUser - Being extremely cool (facts)\nleggi - Simply being too good at ULTRAKILL\nrandomacman - Kuro's sake provider\nscePixel - Simply too cool\nKURO AND SORA - KISSING\n"));
    return manipstr;
}

monoString* (*old_TMP_Text$get_text)(void* instance);
monoString* TMP_Text$get_text(void* instance) {
    return sexmanip(old_TMP_Text$get_text(instance));
}

monoString* (*old_Text$get_text)(void* instance);
monoString* Text$get_text(void* instance) {
    return sexmanip(old_Text$get_text(instance));
}

bool (*old_ReturnTrueIJustDontCare)();
bool ReturnTrueIJustDontCare() {
   return true;
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

    sleep(3);

    // Hook example. Comment out if you don't use hook
    // Strings in macros are automatically obfuscated. No need to obfuscate!
    HOOK("0xC1C7C0", CommonAPI$$GetDomainURL, old_CommonAPI$$GetDomainURL);
    HOOK("0x298FC98", CryptPlayerPrefs$$Decrypt, old_CryptPlayerPrefs$$Decrypt);
    HOOK("0x2ABF7D0", Crypt$$Encrypt, old_Crypt$$Encrypt);
    HOOK("0x26C74A8", NTJson$$ParseJson, old_NTJson$$ParseJson);
    HOOK("0x270EAF0", __Internal$$OnRegisterToken, old___Internal$$OnRegisterToken);
    PATCH("0x270C8F8", "01 00 A0 E3 1E FF 2F E1");
    PATCH("0x270CA10", "01 00 A0 E3 1E FF 2F E1");
    PATCH("0xC1F3E4", "01 00 A0 E3 1E FF 2F E1");
    PATCH("0x19D2FC0", "01 00 A0 E3 1E FF 2F E1");
    HOOK("0xA0DC70", TMP_Text$get_text, old_TMP_Text$get_text);
    HOOK("0x15C0CCC", Text$get_text, old_Text$get_text);

    //PATCH("0x20D3A8", "0100A0E31EFF2FE1");

    //RESTORE("0x20D3A8");

    //AddMoneyExample = (void (*)(void *, int)) getAbsoluteAddress(targetLibName, 0x123456);
    string$$Replace = (monoString* (*)(monoString*, monoString*, monoString*)) getAbsoluteAddress(targetLibName, 0x17A9450);

    LOGI(OBFUSCATE("Done"));

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
            OBFUSCATE("Category_This is all you need"),
            OBFUSCATE("ButtonLink_Community discord_https://discord.gg/EPxCmpBABN"),
            OBFUSCATE("Button_Credits"),
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

    //BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
        case 0: {
            Toast(env,obj,OBFUSCATE("--^ lost3 ^--"),ToastLength::LENGTH_LONG);
            Toast(env,obj,OBFUSCATE("synzr"),ToastLength::LENGTH_LONG);
            break;
        }
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
