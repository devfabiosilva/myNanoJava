package org.mynanojava.exceptions;

public class NanoKeyPairException extends Throwable {
    private final int error;

    NanoKeyPairException(String message) {
        super(message);
        this.error= -1;
    }

    NanoKeyPairException(String message, int error) {
        super(message);
        this.error = error;
    }

    public int getError() {
        return error;
    }
}
