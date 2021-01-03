package org.mynanojava;
//https://stackoverflow.com/questions/32578965/how-to-create-jni-header-file-with-intellij-idea
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

    /**
     * Create a Nano block
     * @Author Fábio Pereira da Silva
     * @param account
     * @param previous
     * @param representative
     * @param balance
     * @param balanceType
     * @param valueToSendOrReceive
     * @param valueToSendOrReceiveType
     * @param link
     * @param direction
     * @return NanoBlock class instance
     * @throws org.mynanojava.exceptions.NanoBlockException
     * @throws Exception
     * @see NanoBlock
     */
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
    ) throws Throwable;

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
    public native byte[] nanoBlockToByte(NanoBlock nanoBlock) throws Exception;
    public native NanoBlock byteToNanoBlock(byte[] nanoBlock, int blockNumber) throws Exception;

}
