package org.mynanojava.exceptions;

public class NanoBlockException extends Throwable {
    private final int error;

    NanoBlockException(String message) {
        super(message);
        this.error= -1;
    }

    NanoBlockException(String message, int error) {
        super(message);
        this.error = error;
    }

    public int getError() {
        return error;
    }
}
