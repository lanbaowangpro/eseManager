package com.gionee.esemanagertest;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.gionee.esemanager.esemanagerAPI;

public class MainActivity extends AppCompatActivity {

    final private String TAG = "eseManager";
    TextView mTextView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTextView = (TextView) findViewById(R.id.textviewID);
    }

    @Override
    protected void onResume() {
        super.onResume();
        String cplc_str = "CPLC: ";
        int bindingStatus = esemanagerAPI.native_eseSendCommandEx(esemanagerAPI.GET_ESE_BINDDING_STATUS, null);

        byte[] cplc_byte = esemanagerAPI.native_eseGetCplc();
        boolean isValid = esemanagerAPI.native_eseValidateChipLegality();
        cplc_str += bytesToHexString(cplc_byte);
        cplc_str += "\n";
        cplc_str += "eSEBindingStatus " + bindingStatus;
		cplc_str += "\n";
		cplc_str += "eseValidateChipLegality " + isValid;
        cplc_str.replace("\\\n", "\n");
        mTextView.setText(cplc_str);
    }

    private static String bytesToHexString(byte[] bytes) {
        if (bytes == null || bytes.length  == 0)
            return null;
        StringBuffer sb = new StringBuffer();
        for (byte b : bytes)
            sb.append(String.format("%02X ", b & 0xFF));

        return sb.toString();
    }

    public void startBinding(View view) {
        int status = esemanagerAPI.native_eseSendCommandEx(esemanagerAPI.START_BINDING, null);
        Toast.makeText(this, "startBinding return " + status, Toast.LENGTH_SHORT).show();
    }

    public void clearBindingInfo(View view) {
        byte[] buffer = {0x10, 0x20, 0x30};
        int status = esemanagerAPI.native_eseSendCommandEx(esemanagerAPI.CLEAR_BINDING_INFO, buffer);
        Toast.makeText(this, "clearBindingInfo return " + status, Toast.LENGTH_SHORT).show();
    }
}
