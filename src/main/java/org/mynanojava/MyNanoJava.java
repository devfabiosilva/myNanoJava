package org.mynanojava;

public class MyNanoJava {

    static {
        System.loadLibrary("mynanojava");
    }

    public native String license();
    public native String nano_add_sub(String a, String b, int mode) throws Exception;
    public native byte[] nano_create_block(
                String account,
                String previous,
                String representative,
                String balance,
                int balanceType,
                String valueToSendOrReceive,
                int valueToSendOrReceiveType,
                String link,
                int direction
            ) throws Exception;
    public native String nanoBlockToJSON(byte[] nanoBlock) throws Exception;

// Quick test JNI ...
    /*
    public static void main(String[] args) {

    }*/
}
