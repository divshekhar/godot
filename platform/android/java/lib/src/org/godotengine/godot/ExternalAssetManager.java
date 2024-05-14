package org.godotengine.godot;

import android.util.Log;

public final class ExternalAssetManager implements AutoCloseable {
    private static final String TAG = "ExternalAssetManager";

    // Pointer to native implementation, stuffed inside a long.
    private long mObject;

    public ExternalAssetManager() {
        // mObject = nativeCreate();
        Log.d(TAG, "ExternalAssetManager created!");
    }

    /**
     * Close this asset manager.
     */
    @Override
    public void close() {
        // synchronized (this) {
        // if (!mOpen) {
        // return;
        // }

        // mOpen = false;
        // decRefsLocked(hashCode());
        // }
    }

    public void hello() {
        Log.d(TAG, "Hello from AExternalAssetManager, CPP Here!");
    }

    // ExternalAssetManager setup native methods.
    private static native long nativeCreate();

    private static native void nativeDestroy(long ptr);
}
