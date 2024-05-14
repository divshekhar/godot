#define LOG_TAG "ExternalAssetManagerJNI"

#include "external_asset_manager.h"

AExternalAssetManager *AExternalAssetManager_fromJava(JNIEnv *env, jobject extAssetManager) {
	jclass externalAssetManagerClass = env->FindClass("org/godotengine/godot/ExternalAssetManager");
	jfieldID res = env->GetFieldID(externalAssetManagerClass, "mObject", "J");

	return (AExternalAssetManager *)env->GetLongField(extAssetManager, res);
}