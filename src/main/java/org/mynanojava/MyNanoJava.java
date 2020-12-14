package org.mynanojava;

import org.mynanojava.enums.ManoJavaEnumDirection;
import org.mynanojava.enums.NanoJavaEnumBalanceType;
import org.mynanojava.enums.NanoJavaEnumValueToSendOrReceive;

import static org.mynanojava.enums.ManoJavaEnumDirection.VALUE_TO_SEND;
import static org.mynanojava.enums.NanoJavaEnumAddSub.*;
import static org.mynanojava.enums.NanoJavaEnumBalanceType.*;
import static org.mynanojava.enums.NanoJavaEnumValueToSendOrReceive.NANO_VALUE_TO_SEND_OR_RECEIVE_REAL;

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

// Quick test JNI ...
    /*
    public static void main(String[] args) {

    }*/
}
