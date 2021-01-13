package org.mynanojava.bitcoin;

public class Util {
    public static native String generateMasterKey(int versionBytes, int entropy) throws Throwable;
    public static native byte[] generateByteMasterKey(int versionBytes, int entropy) throws Throwable;
    public static native String toBase58(byte[] val) throws Throwable;
    public static native String privateKeyToWIF(String privateKey, int wifType) throws Throwable;
    public static native String byteMasterPrivateKeyToWIF(byte[] masterKey, long index);
    public static native String byteMasterPrivateKeyToBTC_Address(byte[] masterKey, long index) throws Throwable;
}
