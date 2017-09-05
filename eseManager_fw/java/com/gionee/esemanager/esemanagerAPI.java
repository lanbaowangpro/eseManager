//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager begin

package com.gionee.esemanager;

public class esemanagerAPI {

    static final public int START_BINDING = 1094;

    static final public int CLEAR_BINDING_INFO = 1095;

    static final public int GET_ESE_BINDDING_STATUS = 1096;

    static public native byte[] native_eseGetCplc();

    static public native byte[] native_eseSendCommand(byte[] cmd);


    /*
    * native_eseSendCommandEx(START_BINDING, null);
    *
    * native_eseSendCommandEx(CLEAR_BINDING_INFO, buffer);
    *
    * native_eseSendCommandEx(GET_ESE_BINDDING_STATUS, null);
    *
    * return value 0 is success, non 0 is failed.
    */
    static public native int native_eseSendCommandEx(int cmd_id, byte[] buffer);


    /*
     *  0  binding right
     *  1  no binding relations, new eSE
     *  2  communication error
     *  3  binding error
     */
    static public native int native_eseGetBindingStatus();
    /*
     * true is valid
     * false is invalid
     */
    static public native boolean native_eseValidateChipLegality();

    static {
        System.loadLibrary("eseManager_jni");
    }
}
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager end

