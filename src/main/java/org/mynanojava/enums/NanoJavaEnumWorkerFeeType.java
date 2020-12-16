package org.mynanojava.enums;

public enum NanoJavaEnumWorkerFeeType {

    NANO_FEE_HEX(32), NANO_FEE_RAW(64), NANO_FEE_REAL(128);

    private final int value;

    NanoJavaEnumWorkerFeeType(int value) {
        this.value = value;
    }
    public int getValue() {
        return this.value;
    }
}
