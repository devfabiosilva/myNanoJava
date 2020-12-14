package org.mynanojava.enums;

public enum NanoJavaEnumBalanceType {

    NANO_BALANCE_HEX(4), NANO_BALANCE_RAW(8), NANO_BALANCE_REAL(16);

    private final int value;

    NanoJavaEnumBalanceType(int value){
        this.value = value;
    }

    public int getValue(){
        return value;
    }
}
