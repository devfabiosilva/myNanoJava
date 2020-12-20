package org.mynanojava.enums;

public enum NanoJavaEnumPrefix {
    DEST_XRB(1), SENDER_XRB(2), REP_XRB(4);

    private final int value;

    NanoJavaEnumPrefix(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
