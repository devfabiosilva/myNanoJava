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
            ) throws Exception;
    public static final int NANO_ADD_A_B = 1;
    public static final int NANO_SUB_A_B = 2;
    public static final int NANO_A_RAW = 8;
    public static final int NANO_B_RAW = 64;
    public static final int NANO_A_REAL = 16;
    public static final int NANO_B_REAL = 128;
    public static final int NANO_RES_RAW = 512;
    public static final int NANO_RES_RAW_128 = 256;
    public static final int NANO_RES_REAL = 1024;

    public static final int NANO_BALANCE_HEX = 4;
    public static final int NANO_BALANCE_RAW = 8;
    public static final int NANO_BALANCE_REAL = 16;

    public static final int NANO_VALUE_TO_SEND_OR_RECEIVE_HEX = 32;
    public static final int NANO_VALUE_TO_SEND_OR_RECEIVE_RAW = 64;
    public static final int NANO_VALUE_TO_SEND_OR_RECEIVE_REAL = 128;

    public static final int VALUE_TO_SEND = 1;
    public static final int VALUE_TO_RECEIVE = 2;
// Quick test JNI ...
    public static void main(String[] args) {
        MyNanoJava myNanoJava = new MyNanoJava();
        System.out.printf(myNanoJava.license());
        System.out.printf(myNanoJava.nano_add_sub("3.1", "4.009", NANO_ADD_A_B + NANO_A_REAL + NANO_B_REAL + NANO_RES_RAW_128)+"\n");
        try {
            System.out.println(myNanoJava.nano_create_block(
                    "nano_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                    "22E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                    "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                    "1.8",
                    NANO_BALANCE_REAL,
                    "61",
                    NANO_VALUE_TO_SEND_OR_RECEIVE_REAL,
                    "22E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                    VALUE_TO_SEND).length);
        } catch (Exception e) {
            //e.printStackTrace();
            System.out.printf(e.getMessage());
        }
    }
}
