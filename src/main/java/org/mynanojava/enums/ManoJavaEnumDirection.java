package org.mynanojava.enums;

public enum ManoJavaEnumDirection {

    VALUE_TO_SEND(1), VALUE_TO_RECEIVE(2);
    private final int value;
    ManoJavaEnumDirection(int value) {
        this.value = value;
    }

    public int getValue(){
        return value;
    }
}
