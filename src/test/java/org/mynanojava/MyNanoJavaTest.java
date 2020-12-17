package org.mynanojava;

import org.junit.Before;
import org.junit.Test;

import static org.junit.jupiter.api.Assertions.*;
import static org.mynanojava.enums.ManoJavaEnumDirection.VALUE_TO_SEND;
import static org.mynanojava.enums.NanoJavaEnumAddSub.*;
import static org.mynanojava.enums.NanoJavaEnumBalanceType.NANO_BALANCE_REAL;
import static org.mynanojava.enums.NanoJavaEnumValueToSendOrReceive.NANO_VALUE_TO_SEND_OR_RECEIVE_REAL;
import static org.mynanojava.enums.NanoJavaEnumWorkerFeeType.NANO_FEE_REAL;

public class MyNanoJavaTest {

    private MyNanoJava myNanoJava;

    @Before
    public void initMyNanoJava() {
        myNanoJava = new MyNanoJava();
    }

    @Test
    public void showingLicense() {
        System.out.println("Testing license method ...");
        String licenseStr = myNanoJava.license();
        assertNotEquals(null, licenseStr);
        assertNotEquals("", licenseStr);
    }

    @Test
    public void addTwoBigNumbers() throws Exception {
        System.out.println("Add two big numbers ...");
        assertEquals("7.0",  myNanoJava.nano_add_sub(
                "00002.000000",
                "5.000000000000000",
                NANO_ADD_A_B.getValue() +
                        NANO_A_REAL.getValue() +
                        NANO_B_REAL.getValue() +
                        NANO_RES_REAL.getValue()));
    }

    @Test
    public void creatingNanoBlock() throws Exception {
        System.out.println("Creating Nano Block ...");
        byte[] block = myNanoJava.nano_create_block(
                "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                "22E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "1.8",
                NANO_BALANCE_REAL.getValue(),
                "1.0",
                NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                VALUE_TO_SEND.getValue());
        assertEquals(249, block.length);
    }

    @Test
    public void parseNanoBlockToJSON() throws Exception {
        System.out.println("Creating Nano Block ...");
        byte[] block = myNanoJava.nano_create_block(
                "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                "22E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "1.8",
                NANO_BALANCE_REAL.getValue(),
                "1.0",
                NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                VALUE_TO_SEND.getValue());
        System.out.println("Block to json");
        System.out.println(myNanoJava.nanoBlockToJSON(block));
        byte[] p2pow = myNanoJava.nanoP2PoWBlock(block,
                "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "0.7", NANO_FEE_REAL.getValue(), null);
        System.out.println(p2pow.length);
        System.out.println(myNanoJava.p2powToJson(p2pow));
    }
}