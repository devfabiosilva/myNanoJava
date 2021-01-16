package org.mynanojava.bitcoin;

import org.mynanojava.enums.BitcoinVersionBytesEnum;
import org.mynanojava.enums.EntropyTypeEnum;
import org.mynanojava.exceptions.BitcoinUtilException;

import static org.mynanojava.bitcoin.Util.*;

public class BitcoinWallet {
    private byte[] masterPrivateKey = null;
    private byte[] masterPublicKey = null;
    private int index = 0;
    private String wif = null;

    public BitcoinWallet(BitcoinVersionBytesEnum versionBytes, EntropyTypeEnum entropy) throws Throwable {
        this.masterPrivateKey = Util.generateByteMasterKey(versionBytes.getValue(), entropy.getValue());
    }

    public BitcoinWallet(String privateKeyOrPublicKeyOrWif) {
        //TODO to be implemented
    }

    public String toWIF(long index) throws Throwable {
        if (index < 0)
            throw new BitcoinUtilException("WIF error. Invalid index", -182);

        if (this.wif != null) {
            if (index > 0)
                throw new BitcoinUtilException("WIF has no index greater than 0", -181);
            return wif;
        }

        return byteMasterPrivateKeyToWIF(this.masterPrivateKey, index);
    }

    public String xPrivateKey() throws Throwable {
        if (this.masterPrivateKey == null)
            throw new BitcoinUtilException("No master private key", -180);

        return toBase58(this.masterPrivateKey);
    }

    public String xPublicKey() throws Throwable {
        if (this.masterPublicKey == null)
            this.masterPublicKey = byteMasterPrivateKeyToMasterPublicKey(this.masterPrivateKey);

        return toBase58(this.masterPublicKey);
    }

    public String toBitcoinAddress(long index) throws Throwable {
        if (index < 0)
            throw new BitcoinUtilException("Invalid Bitcoin address index", -200);

        if (wif != null) {
            if (index > 0)
                throw new BitcoinUtilException("Invalid index", -201);

            return "To be implemented";
        }

        if (this.masterPublicKey == null)
            this.masterPublicKey = byteMasterPrivateKeyToMasterPublicKey(this.masterPrivateKey);

        return byteMasterPublicKeyToBTC_Address(this.masterPublicKey, index);
    }

    public void setMasterPrivateKey(String masterPrivateKey) {
        //TODO to be implemented
    }

}
