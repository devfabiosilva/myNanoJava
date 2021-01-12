package org.mynanojava.enums;

public enum BitcoinWIFTypeEnum {
    BITCOIN_WIF_MAINNET(0x80), BITCOIN_WIF_TESTNET(0xEF);

    private final int value;

    BitcoinWIFTypeEnum(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
