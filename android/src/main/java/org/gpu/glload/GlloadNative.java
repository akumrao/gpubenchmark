
package org.gpu.glload;

import android.content.res.AssetManager;

class GlloadNative {
    public static native void init(AssetManager assetManager, String args,
                                   String logFilePath);
    public static native void resize(int w, int h);
    public static native boolean render();
    public static native void done();
    public static native int scoreConfig(GLVisualConfig vc, GLVisualConfig target);
    public static native SceneInfo[] getSceneInfo(AssetManager assetManager);
}
