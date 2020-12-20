package org.mynanojava.blockchain;

import org.mynanojava.enums.NanoAccountEnum;
import org.mynanojava.enums.NanoJavaEnumBalanceType;
import org.mynanojava.enums.NanoJavaEnumPrefix;

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

    public String getAccount(NanoAccountEnum accountType) {
        return "";
    }

    public String getPreviousBlock() {
        return "";
    }

    public String getRepresentative() {
        return "";
    }

    public String getBalance(NanoJavaEnumBalanceType balanceType) {
        return "";
    }

    public String getLink(NanoAccountEnum linkType) {
        return "";
    }

    public String getBlockSignature() {
        return "";
    }

    public long getWork() {
        return this.work;
    }

    public boolean hasXRBPrefix(NanoJavaEnumPrefix prefix) {
        return true;
    }
}
