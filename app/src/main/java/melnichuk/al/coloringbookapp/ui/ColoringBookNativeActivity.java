package melnichuk.al.coloringbookapp.ui;

import android.app.NativeActivity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.PopupWindow;

import melnichuk.al.coloringbookapp.R;
import yuku.ambilwarna.AmbilWarnaDialog;

import android.util.Log;
import android.widget.TextView;

import java.util.Locale;
import java.util.StringTokenizer;

/**
 * Created by al on 11.02.17.
 */

public class ColoringBookNativeActivity extends NativeActivity {

    static {
        System.loadLibrary("color_book");
    }

    ColoringBookNativeActivity activity;
    TextView fpsText;
    Button clear;
    View colorPicker;
    PopupWindow window;
    Handler handler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        activity = this;
        handler = new Handler(Looper.getMainLooper());
    }

    public void showUI() {
        if (window != null)
            return;
        handler.post(new Runnable() {
            public void run() {
                initLayout();
            }
        });
    }

    public void updateFPS(final float fps) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                if (fpsText != null) {
                    String nFps = String.format(Locale.US, "%.2f", fps);
                    fpsText.setText(String.format(Locale.US, "FPS: %s", nFps));
                }
            }
        });
    }

    private void initLayout() {
        View v = LayoutInflater.from(this).inflate(R.layout.activity_color_book, null);
        window = new PopupWindow(
                v,
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT);
        fpsText = (TextView) v.findViewById(R.id.fps);
        clear = (Button) v.findViewById(R.id.clear);
        colorPicker = v.findViewById(R.id.color_picker);

        colorPicker.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d("__CLICK", "color picker");
                AmbilWarnaDialog dialog = new AmbilWarnaDialog(activity, 0xffff0000, new AmbilWarnaDialog.OnAmbilWarnaListener() {
                    @Override
                    public void onOk(AmbilWarnaDialog dialog, int color) {
                        if (colorPicker != null) {
                            colorPicker.setBackgroundColor(color);
                            setPaint(color);
                        }
                    }
                    @Override public void onCancel(AmbilWarnaDialog dialog) {}
                });
                dialog.show();
            };
        });

        clear.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d("__CLICK", "clear");
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        clear();
                    }
                });

            }
        });
        
        LinearLayout mainLayout = new LinearLayout(activity);
        ViewGroup.MarginLayoutParams params = new ViewGroup.MarginLayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT);
        params.setMargins(0, 0, 0, 0);
        activity.setContentView(mainLayout, params);

        // Show our UI over NativeActivity window
        window.showAtLocation(mainLayout, Gravity.TOP | Gravity.START, 10, 10);
        window.update();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (window != null)
            window.dismiss();
        handler.removeCallbacksAndMessages(null);
    }

    private static native void clear();
    private static native void setPaint(int color);
}
