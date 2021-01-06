package org.mynanojava.wallet;

import org.mynanojava.enums.NanoAccountEnum;

import static org.mynanojava.blockchain.NanoBlock.byteToWallet;
import static org.mynanojava.enums.NanoAccountEnum.HEX_ACCOUNT;

public class NanoKeyPair {
    private byte[] privateKey = null;
    private byte[] publicKey = null;
    private long accountNumber = 0;

    private NanoKeyPair() {

    }

    public byte[] getBytePublicKey() {
        return this.publicKey;
    }

    public byte[] getBytePrivateKey() {
        return this.privateKey;
    }

    public String getPublicKey(NanoAccountEnum type) throws Throwable {
        return byteToWallet(this.publicKey, type.getValue());
    }

    public String getPrivateKey() throws Throwable {
        return byteToWallet(this.privateKey, HEX_ACCOUNT.getValue());
    }

    public long getAccountNumber() {
        return this.accountNumber;
    }
}
