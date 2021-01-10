package org.mynanojava.exceptions;

public class NanoBlockException extends Throwable {
    private final int error;

    public NanoBlockException(String message) {
        super(message);
        this.error= -1;
    }

    public NanoBlockException(String message, int error) {
        super(message);
        this.error = error;
    }

    public int getError() {
        return error;
    }
}
