/*
 * Java apis for JNI layer
 *
 */


package org.gpu.glload;

import android.content.res.AssetManager;

class GlloadNative {
    public static native void init(AssetManager assetManager, String args,
                                   String logFilePath);
    public static native void resize(int w, int h);

    public static native boolean render(int battery,  boolean isPowSaveMode,  boolean isLowPowerStandbyEnabled, boolean isSustainedPerformanceModeSupported  );
    public static native void done();
    public static native int scoreConfig(GLVisualConfig vc, GLVisualConfig target);
    public static native SceneInfo[] getSceneInfo(AssetManager assetManager, String logFilePath);
}
