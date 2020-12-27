package org.mynanojava.blockchain;

import org.mynanojava.enums.NanoAccountEnum;
import org.mynanojava.enums.NanoJavaEnumBalanceType;
import org.mynanojava.enums.NanoJavaEnumPrefix;

import static org.mynanojava.enums.NanoAccountEnum.*;
import static org.mynanojava.enums.NanoJavaEnumPrefix.*;

public class NanoBlock {
    private byte[] account = null;
    private byte[] previous = null;
    private byte[] representative = null;
    private byte[] balance = null;
    private byte[] link = null;
    private byte[] signature = null;
    private long work = 0L;
    private int prefixes = 0;

    private NanoBlock() {

    }

    public static native String byteToWallet(byte[] wallet, int type) throws Exception;
    public static native String nanoBlockToJSON(NanoBlock nanoBlock) throws Exception;

    public String getAccount(NanoAccountEnum accountType) throws Exception {
        int type = accountType.getValue();

        if (type == PRE_DEFINED.getValue()) {
            if (((this.prefixes & SENDER_XRB.getValue()) != 0)) {
                type = XRB_PREFIX.getValue();
            } else {
                type = NANO_PREFIX.getValue();
            }
        }

        return byteToWallet(this.account, type);
    }

    public String getPreviousBlock() throws Exception {
        return byteToWallet(this.previous, HEX_ACCOUNT.getValue());
    }

    public String getRepresentative(NanoAccountEnum representativeType) throws Exception {
        int type = representativeType.getValue();

        if (type == PRE_DEFINED.getValue()) {
            if (((this.prefixes & REP_XRB.getValue()) != 0)) {
                type = XRB_PREFIX.getValue();
            } else {
                type = NANO_PREFIX.getValue();
            }
        }

        return byteToWallet(this.representative, type);
    }

    public String getBalance(NanoJavaEnumBalanceType balanceType) {
        return "";
    }

    public String getLink(NanoAccountEnum linkType) throws Exception {
        int type = linkType.getValue();

        if (type == PRE_DEFINED.getValue()) {
            if (((this.prefixes & DEST_XRB.getValue()) != 0)) {
                type = XRB_PREFIX.getValue();
            } else {
                type = NANO_PREFIX.getValue();
            }
        }

        return byteToWallet(this.link, type);
    }

    public String getBlockSignature() {
        return this.signature.toString();
    }

    public long getWork() {
        return this.work;
    }

    public boolean hasXRBPrefix(NanoJavaEnumPrefix prefix) {
        return ((prefix.getValue()&this.prefixes)!=0);
    }

    public byte[] getByteAccount() {
        return this.account;
    }

    public byte[] getBytePrevious() {
        return this.previous;
    }

    public byte[] getByteRepresentative() {
        return this.representative;
    }

    public byte[] getByteBalance() {
        return this.balance;
    }

    public byte[] getByteLink() {
        return this.link;
    }

    public byte[] getByteSignature() {
        return this.signature;
    }

    public String toJson() throws Exception {
        return nanoBlockToJSON(this);
    }
}
