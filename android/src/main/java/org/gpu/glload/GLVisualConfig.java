
package org.gpu.glload;

/** 
 * Class that holds a configuration of a GL visual.
 */
class GLVisualConfig {
    public GLVisualConfig() {}
    public GLVisualConfig(int r, int g, int b, int a, int d, int s, int buf) {
        red = r;
        green = g;
        blue = b;
        alpha = a;
        depth = d;
        stencil = s;
        buffer = buf;
    }

    public int red;
    public int green;
    public int blue;
    public int alpha;
    public int depth;
    public int stencil;
    public int buffer;
}
