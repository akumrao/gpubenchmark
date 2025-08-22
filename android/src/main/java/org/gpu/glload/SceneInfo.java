/*
 * Class for Scene Info
 */


package org.gpu.glload;

import android.os.Parcelable;
import android.os.Parcel;
import java.util.ArrayList;

class SceneInfo implements Parcelable {
    static class Option {
        String name;
        String description;
        String defaultValue;
        String[] acceptableValues;
    }

    public SceneInfo(String name) {
        this.name = name;
        this.options = new ArrayList<Option>();
    }

    public void addOption(String name, String description, String defaultValue,
                          String[] acceptableValues)
    {
        Option opt = new Option();
        opt.name = name;
        opt.description = description;
        opt.defaultValue = defaultValue;
        opt.acceptableValues = acceptableValues;
        this.options.add(opt);
    }

    public String name;
    public ArrayList<Option> options;

    /* Parcelable interface */
    public static final Parcelable.Creator<SceneInfo> CREATOR =
        new Parcelable.Creator<SceneInfo>() {
            public SceneInfo createFromParcel(Parcel in) {
                return new SceneInfo(in);
            }

            public SceneInfo[] newArray(int size) {
                return new SceneInfo[size];
            }
        };

    public int describeContents() {
        return 0;
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeString(name);
        out.writeInt(options.size());
        for (Option opt: options) {
            out.writeString(opt.name);
            out.writeString(opt.description);
            out.writeString(opt.defaultValue);
            out.writeStringArray(opt.acceptableValues);
        }
    }

    private SceneInfo(Parcel in) {
        name = in.readString();
        options = new ArrayList<Option>();

        int size = in.readInt();
        for (int i = 0; i < size; i++) {
            Option opt = new Option();
            opt.name = in.readString();
            opt.description = in.readString();
            opt.defaultValue = in.readString();
            opt.acceptableValues = in.createStringArray();
            options.add(opt);
        }
    }
}
