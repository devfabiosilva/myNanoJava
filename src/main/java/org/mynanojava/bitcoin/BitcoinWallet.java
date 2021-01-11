package org.mynanojava.bitcoin;

import org.mynanojava.exceptions.BitcoinUtilException;

import static org.mynanojava.bitcoin.Util.toBase58;

public class BitcoinWallet {
    private byte[] masterPrivateKey = null;
    private byte[] masterPublicKey = null;
    private int index = 0;
    private byte[] wif = null;

    public BitcoinWallet(int versionBytes, int entropy) throws Throwable {
        this.masterPrivateKey = Util.generateByteMasterKey(versionBytes, entropy);
    }

    public BitcoinWallet(String privateKeyOrPublicKey) {
        //TODO to be implemented
    }

    public String toWIF(int index) throws BitcoinUtilException {
        if (index < 0)
            throw new BitcoinUtilException("WIF error. Invalid index", -182);

        if (this.wif != null) {
            if (index > 0)
                throw new BitcoinUtilException("WIF has no index greater than 0", -181);
        } else if (this.masterPrivateKey != null) {
            // TODO Implement
        } else
            throw new BitcoinUtilException("Wallet fatal.", -183);
        // TODO to be implemented.
        return null;
    }

    public String xPrivateKey() throws Throwable {
        if (this.masterPrivateKey == null)
            throw new BitcoinUtilException("No master private key", -180);

        return toBase58(this.masterPrivateKey);
    }

    public void setMasterPrivateKey(String masterPrivateKey) {
        //TODO to be implemented
    }


}
