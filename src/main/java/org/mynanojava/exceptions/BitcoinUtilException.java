package org.mynanojava.exceptions;

public class BitcoinUtilException extends Throwable{
    private final int error;

    BitcoinUtilException(String message) {
        super(message);
        this.error= -1;
    }

    BitcoinUtilException(String message, int error) {
        super(message);
        this.error = error;
    }

    public int getError() {
        return error;
    }
}

