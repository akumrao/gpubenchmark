package com.willitwork.javacppgles;

import android.content.res.AssetManager;

//java interface for rendering using c++
class MyCppRenderer {

    void draw() {
        _draw();
    }

    void init(AssetManager asset){


        _init(asset);
    }


    private native void _init (AssetManager asset);
    private native void _draw ();
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("crenderer");
    }

}
