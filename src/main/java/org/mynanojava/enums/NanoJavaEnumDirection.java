package org.mynanojava.enums;

public enum NanoJavaEnumDirection {

    VALUE_TO_SEND(1), VALUE_TO_RECEIVE(2);
    private final int value;
    NanoJavaEnumDirection(int value) {
        this.value = value;
    }

    public int getValue(){
        return value;
    }
}
