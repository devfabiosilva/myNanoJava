package org.mynanojava;

import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import org.mynanojava.bitcoin.BitcoinWallet;
import org.mynanojava.bitcoin.Util;
import org.mynanojava.blockchain.NanoBlock;
import org.mynanojava.blockchain.P2PoWBlock;
import org.mynanojava.enums.BitcoinVersionBytesEnum;
import org.mynanojava.enums.EntropyTypeEnum;
import org.mynanojava.exceptions.BalanceException;
import org.mynanojava.exceptions.BitcoinUtilException;
import org.mynanojava.exceptions.NanoBlockException;
import org.mynanojava.exceptions.NanoKeyPairException;
import org.mynanojava.wallet.NanoKeyPair;

import static org.junit.jupiter.api.Assertions.*;
import static org.mynanojava.bitcoin.Util.*;
import static org.mynanojava.blockchain.NanoBlock.*;
import static org.mynanojava.enums.BitcoinVersionBytesEnum.*;
import static org.mynanojava.enums.BitcoinWIFTypeEnum.BITCOIN_WIF_MAINNET;
import static org.mynanojava.enums.BitcoinWIFTypeEnum.BITCOIN_WIF_TESTNET;
import static org.mynanojava.enums.EntropyTypeEnum.*;
import static org.mynanojava.enums.NanoJavaEnumBalanceType.*;
import static org.mynanojava.enums.NanoJavaEnumDirection.VALUE_TO_RECEIVE;
import static org.mynanojava.enums.NanoJavaEnumDirection.VALUE_TO_SEND;
import static org.mynanojava.enums.NanoAccountEnum.*;
import static org.mynanojava.enums.NanoJavaEnumAddSub.*;
import static org.mynanojava.enums.NanoJavaEnumPrefix.REP_XRB;
import static org.mynanojava.enums.NanoJavaEnumPrefix.SENDER_XRB;
import static org.mynanojava.enums.NanoJavaEnumValueToSendOrReceive.NANO_VALUE_TO_SEND_OR_RECEIVE_REAL;
import static org.mynanojava.enums.NanoJavaEnumWorkerFeeType.NANO_FEE_REAL;

/**
 * <p>MyNanoJavaTest class.</p>
 *
 * @author Fábio Pereira da Silva
 * @version $Id: $Id
 * @since 0.1.0
 */
public class MyNanoJavaTest {

    private MyNanoJava myNanoJava;

    /**
     * <p>initMyNanoJava.</p>
     */
    @Before
    public void initMyNanoJava() {
        myNanoJava = new MyNanoJava();
    }

    /**
     * <p>showingLicense.</p>
     */
    @Test
    public void showingLicense() {
        System.out.println("Testing license method ...");
        String licenseStr = myNanoJava.license();
        assertNotEquals(null, licenseStr);
        assertNotEquals("", licenseStr);
    }

    /**
     * <p>addTwoBigNumbers.</p>
     *
     * @throws java.lang.Exception if any.
     */
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

    /**
     * <p>creatingNanoBlock.</p>
     *
     * @throws java.lang.Exception if any.
     */
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

    /**
     * <p>parseNanoBlockToJSON.</p>
     *
     * @throws java.lang.Exception if any.
     */
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

    /**
     * <p>testNanoPoW.</p>
     *
     * @throws java.lang.Exception if any.
     */
    @Test
    public void testNanoPoW() throws Exception {
        System.out.println("\n"+myNanoJava.nanoPoW("83170d1119c054cefcd3d2f2192b2e6e93b0a7664b934ef599ab11ceae5b482b", 0xff00000000000000L, 4)+"\n");
    }

    /**
     * <p>testMyNanoBlockClass.</p>
     *
     * @throws java.lang.Throwable if any.
     */
    @Test
    public void testMyNanoBlockClass() throws Throwable {
        NanoBlock nanoBlock = myNanoJava.nanoCreateBlock(
                "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                "22E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "1.88",
                NANO_BALANCE_REAL.getValue(),
                "1.0",
                NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                VALUE_TO_SEND.getValue()
        );
        assertNotEquals(null, nanoBlock);
        assertNotEquals(null, nanoBlock.getByteAccount());
        assertNotEquals(null, nanoBlock.getBytePrevious());
        assertTrue(nanoBlock.hasXRBPrefix(SENDER_XRB));
        assertFalse(nanoBlock.hasXRBPrefix(REP_XRB));
        assertEquals("24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8", nanoBlock.getLink(HEX_ACCOUNT));
        assertEquals("xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54", nanoBlock.getAccount(PRE_DEFINED));
        assertEquals("nano_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54", nanoBlock.getAccount(NANO_PREFIX));
        assertEquals("40FB7380250F36790BF16747EC8DD46BE33F69AAB6F64A1C883D5A7128EAB80D", nanoBlock.getAccount(HEX_ACCOUNT));
        System.out.println(nanoBlock.toJson());
    }

    /**
     * <p>testGenesisBlockWithException.</p>
     */
    @Test
    public void testGenesisBlockWithException() {
        NanoBlock nanoBlock;
        try {
            // Genesis block should not have any amount
            nanoBlock = myNanoJava.nanoCreateBlock(
                    "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                    null,
                    "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                    "0.192",
                    NANO_BALANCE_REAL.getValue(),
                    "1.0",
                    NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                    "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                    VALUE_TO_RECEIVE.getValue()
            );
            fail("Failed pass 1");
        } catch (Throwable e) {
            assertEquals("nanoCreateBlock: myNanoEmbedded C library error in function \"nano_create_block_dynamic\" 8003",
            e.getMessage());
            System.out.println(e.getMessage()+"\n");
        }
//Can't send empty amount
        try {
            nanoBlock = myNanoJava.nanoCreateBlock(
                    "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                    null,
                    "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                    "0",
                    NANO_BALANCE_REAL.getValue(),
                    "1.0",
                    NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                    "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                    VALUE_TO_SEND.getValue()
            );
            fail("Failed pass 2");
        } catch (Throwable e) {
            assertEquals("nanoCreateBlock: myNanoEmbedded C library error in function \"nano_create_block_dynamic\" 8004",
                    e.getMessage());
            System.out.println(e.getMessage()+"\n");
        }

        //Does not make sense send 0.0 amount
        try {
            nanoBlock = myNanoJava.nanoCreateBlock(
                    "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                    null,
                    "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                    "0",
                    NANO_BALANCE_REAL.getValue(),
                    "0",
                    NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                    "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                    VALUE_TO_RECEIVE.getValue()
            );
            fail("Failed pass 3");
        } catch (Throwable e) {
            assertEquals("nanoCreateBlock: myNanoEmbedded C library error in function \"nano_create_block_dynamic\" 8016",
                    e.getMessage());
            System.out.println(e.getMessage()+"\n");
        }
    }

    /**
     * <p>testGenesisBlock.</p>
     */
    @Test
    public void testGenesisBlock() {
        NanoBlock nanoBlock;
        try {
            // Genesis block should not have any amount
            nanoBlock = myNanoJava.nanoCreateBlock(
                    "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                    null,
                    "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                    "0",
                    NANO_BALANCE_REAL.getValue(),
                    "1.2",
                    NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                    "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                    VALUE_TO_RECEIVE.getValue()
            );
            assertEquals(nanoBlock.getAccount(HEX_ACCOUNT), nanoBlock.getPreviousBlock());
            System.out.println(nanoBlock.getAccount(HEX_ACCOUNT));
            System.out.println("\n"+ nanoBlock.toJson() +"\n");

        } catch (Throwable e) {
            fail(e.getMessage());
        }
    }

    /**
     * <p>nanoBlockToByteTest.</p>
     *
     * @throws java.lang.Throwable if any.
     */
    @Test
    public void nanoBlockToByteTest() throws Throwable {
        NanoBlock nanoBlock = myNanoJava.nanoCreateBlock(
                "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                null,
                "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "0",
                NANO_BALANCE_REAL.getValue(),
                "1.0",
                NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                VALUE_TO_RECEIVE.getValue()
        );
        assertNotEquals(null, nanoBlock);
        byte[] nanoBlockByte = myNanoJava.nanoBlockToByte(nanoBlock);
        assertNotEquals(null, nanoBlockByte);
        assertEquals(249, nanoBlockByte.length);
    }

    /**
     * <p>creatingP2PoWBlockThroughNanoBlockClass.</p>
     *
     * @throws java.lang.Throwable if any.
     */
    @Test
    public void creatingP2PoWBlockThroughNanoBlockClass() throws Throwable {
        P2PoWBlock p2PoWBlock;
        NanoBlock nanoBlock = myNanoJava.nanoCreateBlock(
                "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                "22E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "1.88",
                NANO_BALANCE_REAL.getValue(),
                "1.0",
                NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                VALUE_TO_SEND.getValue()
        );
        assertNotEquals(null, nanoBlock);
        assertNotEquals(null, nanoBlock.getByteAccount());
        assertNotEquals(null, nanoBlock.getBytePrevious());
        assertTrue( nanoBlock.hasXRBPrefix(SENDER_XRB));
        assertFalse( nanoBlock.hasXRBPrefix(REP_XRB));

        p2PoWBlock = new P2PoWBlock(nanoBlock, "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "0.7", NANO_FEE_REAL.getValue(), null);
        assertNotEquals(null, p2PoWBlock);
        assertEquals(nanoBlock.getAccount(PRE_DEFINED), p2PoWBlock.getRewardBlock().getAccount(PRE_DEFINED));
        assertEquals(p2PoWBlock.getUserBlock().getAccount(PRE_DEFINED), p2PoWBlock.getRewardBlock().getAccount(PRE_DEFINED));
        assertEquals(p2PoWBlock.getUserBlock().getRepresentative(PRE_DEFINED), p2PoWBlock.getRewardBlock().getRepresentative(PRE_DEFINED));
        assertEquals(498, p2PoWBlock.getByteP2PoWBlock().length);
        System.out.println("\nJSON => \n"+p2PoWBlock.toJson());
    }

    /**
     * <p>creatingP2PoWBlockThroughNativeByte.</p>
     *
     * @throws java.lang.Exception if any.
     */
    @Test
    public void creatingP2PoWBlockThroughNativeByte() throws Exception {
        P2PoWBlock p2PoWBlock;
        byte[] nanoBlock = myNanoJava.nano_create_block(
                "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                "22E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "1.88",
                NANO_BALANCE_REAL.getValue(),
                "1.0",
                NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                VALUE_TO_SEND.getValue()
        );
        assertNotEquals(null, nanoBlock);
        assertEquals(249, nanoBlock.length);

        p2PoWBlock = new P2PoWBlock(nanoBlock, "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "0.7", NANO_FEE_REAL.getValue(), null);
        assertNotEquals(null, p2PoWBlock);
        assertEquals(p2PoWBlock.getUserBlock().getAccount(PRE_DEFINED), p2PoWBlock.getRewardBlock().getAccount(PRE_DEFINED));
        assertEquals(p2PoWBlock.getUserBlock().getRepresentative(PRE_DEFINED), p2PoWBlock.getRewardBlock().getRepresentative(PRE_DEFINED));
        assertEquals(498, p2PoWBlock.getByteP2PoWBlock().length);
        System.out.println("\nJSON => \n"+p2PoWBlock.toJson());
    }

    /**
     * <p>newTrowableMethod.</p>
     */
    @Test
    public void newTrowableMethod() {
        NanoBlock nanoBlock;
        try {
            nanoBlock = myNanoJava.nanoCreateBlock(
                    null,
                    null,
                    "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                    "0",
                    NANO_BALANCE_REAL.getValue(),
                    "1.0",
                    NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                    "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                    VALUE_TO_RECEIVE.getValue()
            );
            fail("Should occur an exception or throwable");
        } catch (Throwable e) {
            assertTrue(e instanceof NanoBlockException);
            assertEquals("nanoCreateBlock: Missing account", e.getMessage());
            assertEquals(20000, ((NanoBlockException)e).getError());
        }

        try {
            nanoBlock = myNanoJava.nanoCreateBlock(
                    "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                    "22E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                    "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                    "1.88",
                    NANO_BALANCE_REAL.getValue(),
                    "1.0",
                    NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                    "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                    VALUE_TO_SEND.getValue()-1000
            );
        } catch (Throwable e) {
            assertTrue(e instanceof NanoBlockException);
            assertEquals(8017, ((NanoBlockException) e).getError());
            assertEquals("nanoCreateBlock: myNanoEmbedded C library error in function \"nano_create_block_dynamic\" 8017", e.getMessage());
        }
    }

    @Test
    public void getBalanceTest() throws Throwable {
        NanoBlock nanoBlock;
        nanoBlock = myNanoJava.nanoCreateBlock(
                "xrb_1i9ugg14c5sph67z4st9xk8xatz59xntofqpbagaihctg6ngog1f45mwoa54",
                "22E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                    "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "1.88",
                NANO_BALANCE_REAL.getValue(),
                "1.0",
                NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                VALUE_TO_SEND.getValue()
        );

        assertNotEquals(null, nanoBlock);

        String balance = nanoBlock.getBalance(NANO_BALANCE_REAL);
        assertEquals("0.88", balance);

        balance = nanoBlock.getBalance(NANO_BALANCE_RAW);
        assertEquals("880000000000000000000000000000", balance);

        balance = nanoBlock.getBalance(NANO_BALANCE_HEX);
        assertEquals("0000000b1b6ef0655cb8d15d80000000", balance);

        try {
            NanoBlock.getBalanceFromByte(nanoBlock.getByteBalance(), NANO_BALANCE_REAL.getValue()+3);
            fail("Should fail");
        } catch (Throwable e) {
            assertTrue(e instanceof BalanceException);
            assertEquals(130, ((BalanceException) e).getError());
            assertEquals("getBalance_util @ getBalanceFromByte: Error 130. With byte size 16", e.getMessage());
        }

        try {
            NanoBlock.getBalanceFromByte(null, NANO_BALANCE_REAL.getValue());
            fail("Should fail here");
        } catch (Throwable e) {
            assertTrue(e instanceof Exception);
            assertEquals("getBalanceFromByte: Missing balance", e.getMessage());
        }

        assertEquals("0.88",NanoBlock.getBalanceFromByte(nanoBlock.getByteBalance(), NANO_BALANCE_REAL.getValue()));

        balance = NanoBlock.getBalanceFromNanoBlock(nanoBlock, NANO_BALANCE_REAL.getValue());
        assertEquals("0.88", balance);

        balance = null;

        try {
            balance = NanoBlock.getBalanceFromNanoBlock(null, NANO_BALANCE_RAW.getValue());
            fail("NanoBlock.getBalanceFromNanoBlock should fail");
        } catch (Throwable e) {
            assertTrue(e instanceof NanoBlockException);
            assertEquals(100, ((NanoBlockException) e).getError());
            assertEquals("getBalanceFromNanoBlock: Missing NanoBlock object", e.getMessage());
        } finally {
            assertNull(balance);
        }

        balance = null;

        try {
            balance = NanoBlock.getBalanceFromNanoBlock(nanoBlock, NANO_BALANCE_RAW.getValue()+1);
            fail("NanoBlock.getBalanceFromNanoBlock should fail 2");
        } catch (Throwable e) {
            assertTrue(e instanceof BalanceException);
            assertEquals(130, ((BalanceException) e).getError());
            assertEquals("getBalance_util @ getBalanceFromNanoBlock: Error 130", e.getMessage());
        } finally {
            assertNull(balance);
        }

        assertEquals("880000000000000000000000000000", NanoBlock.getBalanceFromNanoBlock(nanoBlock, NANO_BALANCE_RAW.getValue()));
        assertEquals("0000000b1b6ef0655cb8d15d80000000", NanoBlock.getBalanceFromNanoBlock(nanoBlock, NANO_BALANCE_HEX.getValue()));
    }

    @Ignore
    @Test
    public void generateRandomSeed() throws Throwable {
        final String str = "SEED Generating random ";

        try {
            generateNanoSeed(1726);
            fail("Nano seed should fail");
        } catch (Throwable e) {
            assertTrue(e instanceof NanoBlockException);
            assertEquals("generateNanoSeed: Invalid entropy number 1726", e.getMessage());
        }

        System.out.println(str + NOT_RECOMENDED.toString());
        System.out.println(generateNanoSeed(NOT_RECOMENDED.getValue()));

        System.out.println(str + NOT_ENOUGH.toString());
        System.out.println(generateNanoSeed(NOT_ENOUGH.getValue()));

        System.out.println(str + GOOD.toString());
        System.out.println(generateNanoSeed(GOOD.getValue()));

        System.out.println(str + " (slow) but secure " + EXCELENT.toString());
        System.out.println(generateNanoSeed(EXCELENT.getValue()));

        System.out.println(str + " (very slow) paranoic. It can take a little longer ..." + PARANOIC.toString());
        System.out.println("Please. Move your mouse, open programs, listen a music in your computer to increase system entropy");
        System.out.println("Don't forget. Pick a cup of coffe ;)");
        System.out.println(generateNanoSeed(PARANOIC.getValue()));

    }

    @Test
    public void extractFromNanoSeedTest() throws Throwable {
        final String NANO_SEED = "7C35B46F66B0923736E77650A9EEC3D6D31B89F649F27EE974BAA75C0ED04C77";

        NanoKeyPair keyPair;
        long accountNumber = 1981;

        keyPair = fromNanoSeed(NANO_SEED, accountNumber);
        assertEquals(accountNumber, keyPair.getAccountNumber());
        assertEquals("611BCE7922F5FB3A0A9C4ACFE7C9B53AC79DD902A2CE0A56830052B9F4A84FB1", keyPair.getPrivateKey());
        assertEquals("207BD39D3BF75850FB9F14F477C981593353EB501D39ED4BA06C79C21652BDBF", keyPair.getAccount(HEX_ACCOUNT));
        assertEquals("xrb_1a5utggmqxtrc5xsy79ngz6r4pbmchoo19bsxo7t1u5srad77hfzf6ugxwws", keyPair.getAccount(XRB_PREFIX));
        assertEquals("nano_1a5utggmqxtrc5xsy79ngz6r4pbmchoo19bsxo7t1u5srad77hfzf6ugxwws", keyPair.getAccount(NANO_PREFIX));
        System.out.println("Account " + keyPair.getAccountNumber());

        System.out.println("Private key " + keyPair.getPrivateKey());
        System.out.println("Public key " + keyPair.getAccount(HEX_ACCOUNT));
        System.out.println("Nano account " + keyPair.getAccount(NANO_PREFIX));
        System.out.println("Nano account (XRB) " + keyPair.getAccount(XRB_PREFIX));

        NanoKeyPair keyPair1 = null;
        final String NANO_SEED2 = "75E73258DB2A3DE79360A22A2D26B75576ADB58C8FDFF063FF5D95D16492ACFC";
        try {
            keyPair1 = fromNanoSeed(NANO_SEED2, (long) Math.pow(2, 32));
        } catch (Throwable e) {
            assertTrue(e instanceof NanoKeyPairException);
            assertEquals(291, ((NanoKeyPairException) e).getError());
            assertEquals("Invalid Nano account number 4294967296", e.getMessage());
        } finally {
            assertNull(keyPair1);
        }
        NanoKeyPair keyPair2 = null;
        try {
            keyPair2 = fromNanoSeed("", 0);
        } catch (Throwable e) {
            assertTrue(e instanceof NanoKeyPairException);
            assertEquals(-21, ((NanoKeyPairException) e).getError());
            assertEquals("array32bytes_str_to_hex_util @ fromNanoSeed: hex 2 bin error -21", e.getMessage());
        } finally {
            assertNull(keyPair2);
        }

        NanoKeyPair keyPair3 = null;
        try {
            keyPair3 = fromNanoSeed(null, 0);
        } catch (Throwable e) {
            assertTrue(e instanceof NanoKeyPairException);
            assertEquals(289, ((NanoKeyPairException) e).getError());
            assertEquals("Missing Nano SEED", e.getMessage());
        } finally {
            assertNull(keyPair3);
        }
    }

    @Test
    public void signBlockTest() throws Throwable {
        final String NANO_SEED = "7C34146F66B0923736E71650A9EEC3D6D31B39F649F27EE974BFA75C0ED04C72";

        NanoKeyPair keyPair;
        long accountNumber = 37000; // Bitcoin price in USD jan 5 2021

        keyPair = fromNanoSeed(NANO_SEED, accountNumber);
        assertEquals(accountNumber, keyPair.getAccountNumber());
        System.out.println("Public key: " + keyPair.getAccount(HEX_ACCOUNT));
        System.out.println("Account: " + keyPair.getAccount(NANO_PREFIX));
        System.out.println("Private key: " + keyPair.getPrivateKey());
        System.out.println("Creating Nano Block ...");
        byte[] block = myNanoJava.nano_create_block(
                keyPair.getAccount(NANO_PREFIX),
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
        signByteNanoBlock(block, keyPair.getPrivateKey());
        System.out.println("Block to json (signed)");
        System.out.println(myNanoJava.nanoBlockToJSON(block));
        assertTrue(verifySignatureByteNanoBlock(block));
    }

    @Test
    public void signNanoBlockTest() throws Throwable {
        final String NANO_SEED = "9B34126F69B0923736E71650A90EC3D6131B391649F27EA874BFA75C8ED04C30";

        NanoKeyPair keyPair;
        long accountNumber = 40500; // Bitcoin price in USD in jan 08 2021

        keyPair = fromNanoSeed(NANO_SEED, accountNumber);
        assertEquals(accountNumber, keyPair.getAccountNumber());
        System.out.println("Public key: " + keyPair.getAccount(HEX_ACCOUNT));
        System.out.println("Account: " + keyPair.getAccount(NANO_PREFIX));
        System.out.println("Private key: " + keyPair.getPrivateKey());
        System.out.println("Creating Nano Block ...");
        NanoBlock nanoBlock = new MyNanoJava().nanoCreateBlock(
                keyPair.getAccount(NANO_PREFIX),
                "21E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "56.18287166255143341",
                NANO_BALANCE_REAL.getValue(),
                "1.38716625534773",
                NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                VALUE_TO_SEND.getValue()
        );
        assertNull(nanoBlock.getBlockSignature());
        System.out.println(nanoBlock.toJson());
        nanoBlock.sign(keyPair.getPrivateKey());
        assertNotNull(nanoBlock.getBlockSignature());
        assertTrue(nanoBlock.isBlockSigned());
        System.out.println(nanoBlock.getBlockSignature());
        System.out.println(nanoBlock.toJson());
        byte[] sig = null;
        try {
            signatureByteToString(sig);
        } catch (Throwable e) {
            assertTrue(e instanceof NanoBlockException);
            assertEquals(170, ((NanoBlockException) e).getError());
            assertEquals("signatureByteToString: Missing ByteArray Signature", e.getMessage());
        } finally {
            assertNull(sig);
        }
    }

    @Test
    public void proofOfWorkTest() throws Throwable {
        final String NANO_SEED = "1D41F26F69B0923736E7F651A97EC3D6131B592649F27EA874BFA75A8ED04C30";

        NanoKeyPair keyPair;
        long accountNumber = 41179; // Bitcoin price in USD in jan 10 2021

        keyPair = fromNanoSeed(NANO_SEED, accountNumber);
        assertEquals(accountNumber, keyPair.getAccountNumber());
        System.out.println("Public key: " + keyPair.getAccount(HEX_ACCOUNT));
        System.out.println("Account: " + keyPair.getAccount(NANO_PREFIX));
        System.out.println("Private key: " + keyPair.getPrivateKey());
        System.out.println("Creating Nano Block ...");
        NanoBlock nanoBlock = new MyNanoJava().nanoCreateBlock(
                keyPair.getAccount(NANO_PREFIX),
                "21E0C2705A91D2DFB28F65D921E93A70CDF6599FEA232D9496FA759D9C2DE4C8",
                "nano_3jbj3kpt4jqpcb5f6npznxat3o3184r5ptsribhqy73muhxk3zsh7snznqfc",
                "56.1828716625514334199100199",
                NANO_BALANCE_REAL.getValue(),
                "1289.3871662553477319277266",
                NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                VALUE_TO_RECEIVE.getValue()
        );
        System.out.println("Hash: " +  nanoBlock.getBlockHash());
        System.out.println("Threshold: " + THRESHOLD_SEND);
        try {
            nanoBlock.calculateWork(THRESHOLD_SEND, 4);
            fail("Proof of Work should fail without signature");
        } catch (Throwable e) {
            assertTrue(e instanceof NanoBlockException);
            assertEquals(12621, ((NanoBlockException) e).getError());
            assertEquals("f_verify_signed_block @ verifySignatureNanoBlock: Error when verify Nano Block signature 12621",
                    e.getMessage());
        }
        System.out.println("Singning the block ...");
        nanoBlock.sign(keyPair.getPrivateKey());
        assertTrue(nanoBlock.isBlockSigned());
        System.out.println("Calculating Proof of Work in receive amount case...");
        nanoBlock.calculateWork(THRESHOLD_SEND, 4);
        assertTrue(nanoBlock.getWork() != 0);
        System.out.println("WORK:"+nanoBlock.getWork());
        System.out.println(nanoBlock.toJson());
    }

    @Test
    public void bitcoinUtilCreateMasterKey() throws Throwable {
        String masterKey = Util.generateMasterKey(TESTNET_PRIVATE.getValue(), GOOD.getValue());
        assertNotNull(masterKey);
        assertTrue(masterKey.contains("tprv"));
        System.out.println(masterKey);
        String masterKey2 = Util.generateMasterKey(MAINNET_PRIVATE.getValue(), GOOD.getValue());
        assertTrue(masterKey2.contains("xprv"));
        System.out.println(masterKey2);
    }

    @Test
    public void bitcoinWalletCreateTest() throws Throwable {
        BitcoinWallet bitcoinWallet = new BitcoinWallet(MAINNET_PRIVATE, GOOD);
        assertNotNull(bitcoinWallet);
        System.out.println(bitcoinWallet.xPrivateKey());
        System.out.println(
                privateKeyToWIF(
                        "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                        BITCOIN_WIF_MAINNET.getValue()
                )
        );
        System.out.println(
                privateKeyToWIF(
                        "24E0C2705A91D2DFB28A25D921E93A71CDF6599FEA232D8496FA759D9C2DE4C8",
                        BITCOIN_WIF_TESTNET.getValue()
                )
        );

        System.out.println(bitcoinWallet.toWIF(28717));
        bitcoinWallet = new BitcoinWallet(MAINNET_PRIVATE, GOOD);
        System.out.println("WIF extracted");
        System.out.println(bitcoinWallet.toWIF(1627));
//        System.out.println(bitcoinWallet.toBitcoinAddress(12));

        String wif = null;
        try {
            wif = byteMasterPrivateKeyToWIF(null, 0);
        } catch (Throwable e) {
            assertTrue(e instanceof BitcoinUtilException);
            assertEquals("byteMasterPrivateKeyToWIF: Missing master key", e.getMessage());
            assertEquals(5025, ((BitcoinUtilException) e).getError());
        } finally {
            assertNull(wif);
        }
    }

    @Test
    public void extractFromMasterKey() throws Throwable {
        int walletNumber = 100;
        BitcoinWallet bitcoinWallet = new BitcoinWallet(MAINNET_PRIVATE, GOOD);
        assertNotNull(bitcoinWallet);
        System.out.println("Private Key (master): "+bitcoinWallet.xPrivateKey());
        System.out.println("Public key (master):  "+bitcoinWallet.xPublicKey());
        System.out.println("Wallet number: " + walletNumber + " Wallet " + bitcoinWallet.toBitcoinAddress(walletNumber));

        bitcoinWallet = new BitcoinWallet(TESTNET_PRIVATE, GOOD);
        assertNotNull(bitcoinWallet);
        System.out.println("Private Key (master): "+bitcoinWallet.xPrivateKey());
        System.out.println("Public key (master):  "+bitcoinWallet.xPublicKey());
        System.out.println("Wallet number: " + walletNumber + " Wallet " + bitcoinWallet.toBitcoinAddress(walletNumber));
    }

    @Test
    public void generateMasterPrivateKeyAndExtractWalletFrom_C_Native_Test() throws Throwable {
        long walletNumber = 150;
        byte[] masterKey = generateByteMasterKey(MAINNET_PRIVATE.getValue(), GOOD.getValue());
        assertNotNull(masterKey);
        byte[] masterPublicKey = byteMasterPrivateKeyToMasterPublicKey(masterKey);
        assertNotNull(masterPublicKey);
        System.out.println("Master private key (Mainnet) = " + toBase58(masterKey));
        System.out.println("Master public key (Mainnet) = " + toBase58(masterPublicKey));
        String walletFromMasterPrivateKey = byteMasterPrivateKeyToBTC_Address(masterKey, walletNumber);
        String walletFromMasterPublicKey = byteMasterPublicKeyToBTC_Address(masterPublicKey, walletNumber);

        System.out.println("Wallet from master private key " + walletFromMasterPrivateKey + " number " + walletNumber);
        System.out.println("Wallet from master public key " + walletFromMasterPublicKey + " number " + walletNumber);

        assertEquals(walletFromMasterPrivateKey, walletFromMasterPublicKey);
        System.out.println("Private key WIF " + byteMasterPrivateKeyToWIF(masterKey, walletNumber));
    }
}
