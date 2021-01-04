package org.mynanojava.enums;

public enum EntropyTypeEnum {
    PARANOIC(1), EXCELENT(2), GOOD(3), NOT_ENOUGH(4), NOT_RECOMENDED(5);

    private final int value;

    EntropyTypeEnum(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
