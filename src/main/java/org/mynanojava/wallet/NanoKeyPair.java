package org.mynanojava.wallet;

import org.mynanojava.enums.NanoAccountEnum;

import static org.mynanojava.blockchain.NanoBlock.byteToWallet;
import static org.mynanojava.enums.NanoAccountEnum.HEX_ACCOUNT;

public class NanoKeyPair {
    private byte[] privateKey = null;
    private byte[] publicKey = null;
    private long keyNumber = 0;

    private NanoKeyPair() {

    }

    public byte[] getBytePublicKey() {
        return this.publicKey;
    }

    public byte[] getBytePrivateKey() {
        return this.privateKey;
    }

    public long getKeyNumber() {
        return this.keyNumber;
    }

    public String getPublicKey(NanoAccountEnum type) throws Exception {
        return byteToWallet(this.publicKey, type.getValue());
    }

    public String getPrivateKey() throws Exception {
        return byteToWallet(this.privateKey, HEX_ACCOUNT.getValue());
    }
}
