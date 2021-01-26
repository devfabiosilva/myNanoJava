package org.mynanojava.blockchain;

import org.mynanojava.MyNanoJava;
import org.mynanojava.enums.NanoJavaEnumWorkerFeeType;

public class P2PoWBlock {
    private byte[] p2PoWBlock;

    public P2PoWBlock(NanoBlock nanoBlock, String workerAccount, String workerFee, NanoJavaEnumWorkerFeeType workerFeeType, String workerRepresentative) throws Exception {
        byte[] blkTmp;
        MyNanoJava myNanoJava = new MyNanoJava();

        blkTmp = myNanoJava.nanoBlockToByte(nanoBlock);
        this.p2PoWBlock = new MyNanoJava().nanoP2PoWBlock(blkTmp, workerAccount, workerFee, workerFeeType.getValue(), workerRepresentative);
    }

    public P2PoWBlock (byte[] nanoBlock, String workerAccount, String workerFee, int workerFeeType, String workerRepresentative) throws Exception {
        this.p2PoWBlock = new MyNanoJava().nanoP2PoWBlock(nanoBlock, workerAccount, workerFee, workerFeeType, workerRepresentative);
    }

    public NanoBlock getUserBlock() throws Exception {
        return new MyNanoJava().byteToNanoBlock(p2PoWBlock, 0);
    }

    public NanoBlock getRewardBlock() throws Exception {
        return new MyNanoJava().byteToNanoBlock(p2PoWBlock, 1);
    }

    public String toJson() throws Exception {
        return new MyNanoJava().p2powToJson(p2PoWBlock);
    }

    public byte[] getByteP2PoWBlock() {
        return p2PoWBlock;
    }
}
