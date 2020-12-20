package org.mynanojava.enums;

public enum NanoAccountEnum {
    NANO_PREFIX(1), XRB_PREFIX(2), HEX_ACCOUNT(3), PRE_DEFINED(4);

    private final int value;

    NanoAccountEnum(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
