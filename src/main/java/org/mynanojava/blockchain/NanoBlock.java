package org.mynanojava.blockchain;

import org.mynanojava.enums.NanoAccountEnum;
import org.mynanojava.enums.NanoJavaEnumBalanceType;
import org.mynanojava.enums.NanoJavaEnumPrefix;
import org.mynanojava.wallet.NanoKeyPair;

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

    public static native String byteToWallet(byte[] wallet, int type) throws Exception;
    public static native String nanoBlockToJSON(NanoBlock nanoBlock) throws Exception;
    public static native String signatureByteToString(byte[] signature) throws Throwable;
    public static native boolean verifySignatureByteNanoBlock(byte[] nanoBlock) throws Throwable;
    public static native boolean verifySignatureNanoBlock(NanoBlock nanoBlock) throws Throwable;
    public static native String getBalanceFromByte(byte[] balance, int balanceType) throws Throwable;
    public static native String getBalanceFromNanoBlock(NanoBlock balance, int balanceType) throws Throwable;
    public static native void signByteNanoBlock(byte[] nanoBlock, String privateKey) throws Throwable;
    public static native void signNanoBlock(NanoBlock nanoBlock, String privateKey) throws Throwable;
    public static native String generateNanoSeed(int entropy) throws Throwable;
    public static native NanoKeyPair fromNanoSeed(String seed, long number) throws Throwable;

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

    public String getBalance(NanoJavaEnumBalanceType balanceType) throws Throwable {
        return getBalanceFromByte(this.balance, balanceType.getValue());
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

    public String getBlockSignature() throws Throwable {
        return signatureByteToString(this.signature);
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

    public void sign(String privateKey) throws Throwable {
        signNanoBlock(this, privateKey);
    }

    public boolean isBlockSigned() throws Throwable {
        if (this.signature == null)
            return false;

        return verifySignatureNanoBlock(this);
    }

}
