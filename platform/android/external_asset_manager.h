/**
 * @file external_asset_manager.h
 */

#ifndef EXTERNAL_ASSET_MANAGER_H
#define EXTERNAL_ASSET_MANAGER_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

struct AExternalAssetManager;

typedef struct AExternalAssetManager AExternalAssetManager;

AExternalAssetManager *AExternalAssetManager_fromJava(JNIEnv *env, jobject assetManager);

#ifdef __cplusplus
};
#endif

#endif // EXTERNAL_ASSET_MANAGER_H