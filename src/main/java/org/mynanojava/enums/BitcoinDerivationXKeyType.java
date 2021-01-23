package org.mynanojava.enums;

public enum BitcoinDerivationXKeyType {
    OUT_XPRIV(16),  OUT_XPUB(32);

    private final int value;

    BitcoinDerivationXKeyType(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
