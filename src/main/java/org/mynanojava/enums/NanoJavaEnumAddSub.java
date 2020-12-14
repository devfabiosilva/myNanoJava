package org.mynanojava.enums;

public enum NanoJavaEnumAddSub {
    NANO_ADD_A_B(1), NANO_SUB_A_B(2), NANO_A_RAW(8),
    NANO_B_RAW(64), NANO_A_REAL(16), NANO_B_REAL(128),
    NANO_RES_RAW(512), NANO_RES_RAW_128(256), NANO_RES_REAL(1024);

    private final int value;

    NanoJavaEnumAddSub(int value){
        this.value = value;
    }

    public int getValue(){
        return value;
    }
}
