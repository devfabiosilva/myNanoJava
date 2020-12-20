package org.mynanojava;

import org.mynanojava.blockchain.NanoBlock;

public class MyNanoJava {

    static {
        System.loadLibrary("mynanojava");
    }

    public native String license();
    public native String nano_add_sub(String a, String b, int mode) throws Exception;
    public native byte[] nano_create_block(
                String account,
                String previous,
                String representative,
                String balance,
                int balanceType,
                String valueToSendOrReceive,
                int valueToSendOrReceiveType,
                String link,
                int direction
    ) throws Exception;

    public native NanoBlock nanoCreateBlock(
            String account,
            String previous,
            String representative,
            String balance,
            int balanceType,
            String valueToSendOrReceive,
            int valueToSendOrReceiveType,
            String link,
            int direction
    );

    public native String nanoBlockToJSON(byte[] nanoBlock) throws Exception;
    public native byte[] nanoP2PoWBlock(
            byte[] block,
            String workerAccount,
            String workerFee,
            int workerFeeType,
            String workerRepresentative
    ) throws Exception;
    public native String p2powToJson(byte[] p2powBlock) throws Exception;
    public native long nanoPoW(String hash, Long threshold, int numberOfThreads) throws Exception;
    /*static public void main(String args[]) throws Exception {
        MyNanoJava myNanoJava = new MyNanoJava();
        System.out.println(myNanoJava.nanoPoW("", Long.valueOf(1), 1)+"\n");
    }*/
}
