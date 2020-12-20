package org.mynanojava.blockchain;

public class NanoBlock {
    private byte[] account = null;
    private byte[] previous = null;
    private byte[] representative = null;
    private byte[] balance = null;
    private byte[] link = null;
    private byte[] signature = null;
    private long work = 0L;
    private int prefixes = 0;

    NanoBlock() {

    }

}
