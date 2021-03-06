package org.mynanojava.exceptions;

public class BalanceException extends Throwable {
    private final int error;

    BalanceException(String message) {
        super(message);
        this.error= -1;
    }

    BalanceException(String message, int error) {
        super(message);
        this.error = error;
    }

    public int getError() {
        return error;
    }
}
