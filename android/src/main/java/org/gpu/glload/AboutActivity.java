
package org.gpu.glload;

import android.app.Activity;
import android.content.pm.PackageInfo;
import android.os.Bundle;
import android.view.Window;
import android.widget.TextView;

public class AboutActivity extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_about);

        /* Get the application version */
        String versionName = "?";

        try {
            PackageInfo info = getPackageManager().getPackageInfo(getPackageName(), 0);
            versionName = info.versionName;
        }
        catch (Exception e) {
        }

        /* Display the application version */
        TextView tv = (TextView) findViewById(R.id.name_version);
        String formatString = getString(R.string.about_name_version_format);
        tv.setText(String.format(formatString, versionName));
    }
}
