package org.mynanojava;
// jar cvmf MANIFEST.MF example.jar classes/org/mynanojava/MyNanoJava.class
public class MyNanoJava {

    static {
        System.loadLibrary("mynanojava");
    }

    public native String license();
    public native String nano_add_sub(String a, String b, int mode);
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
            );
    public static final int NANO_ADD_A_B = 1;
    public static final int NANO_SUB_A_B = 2;
    public static final int NANO_A_RAW = 8;
    public static final int NANO_B_RAW = 64;
    public static final int NANO_A_REAL = 16;
    public static final int NANO_B_REAL = 128;
    public static final int NANO_RES_RAW = 512;
    public static final int NANO_RES_RAW_128 = 256;
    public static final int NANO_RES_REAL = 1024;
// Quick test JNI ...
    public static void main(String[] args) {
        MyNanoJava myNanoJava = new MyNanoJava();
        System.out.printf(myNanoJava.license());
        System.out.printf(myNanoJava.nano_add_sub("3.1", "4.009", NANO_ADD_A_B + NANO_A_REAL + NANO_B_REAL + NANO_RES_RAW_128));
    }
}
