package org.mynanojava.enums;

public enum BitcoinVersionBytesEnum {
    MAINNET_PUBLIC(0),  MAINNET_PRIVATE(1), TESTNET_PUBLIC(2), TESTNET_PRIVATE(3);

    private final int value;

    BitcoinVersionBytesEnum(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
