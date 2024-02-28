
package org.gpu.glload;

import java.io.*;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class ResultsActivity extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_results);
        TextView tv = (TextView) findViewById(R.id.results);

        File f = new File(getFilesDir(), "last_run.log");

        try {
            /* Read log from file */
            BufferedReader reader = new BufferedReader(new FileReader(f));
            String line = null;
            StringBuffer sb = new StringBuffer();

            while ((line = reader.readLine()) != null) {
                sb.append(line);
                sb.append('\n');
            }

            reader.close();
            tv.setText(sb.toString());
        }
        catch (Exception ex) {
        }
    }
}

