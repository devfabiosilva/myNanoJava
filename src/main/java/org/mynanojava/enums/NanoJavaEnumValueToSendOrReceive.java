package org.mynanojava.enums;

public enum NanoJavaEnumValueToSendOrReceive {
    NANO_VALUE_TO_SEND_OR_RECEIVE_HEX(32), NANO_VALUE_TO_SEND_OR_RECEIVE_RAW(64),
    NANO_VALUE_TO_SEND_OR_RECEIVE_REAL(128);
    private final int value;

    NanoJavaEnumValueToSendOrReceive(int value) {
        this.value = value;
    }

    public int getValue(){
        return value;
    }
}
