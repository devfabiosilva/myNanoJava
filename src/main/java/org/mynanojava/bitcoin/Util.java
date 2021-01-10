package org.mynanojava.bitcoin;

public class Util {
    public static native String generateMasterKey(int versionBytes, int entropy) throws Throwable;
}
