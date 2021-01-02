package org.mynanojava;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import static org.junit.jupiter.api.Assertions.*;

import java.util.Arrays;
import java.util.Collection;

import static org.mynanojava.enums.NanoJavaEnumAddSub.*;

/**
 * <p>MyNanoJavaParameterTest class.</p>
 *
 * @author fabiolinux
 * @version $Id: $Id
 * @since 0.1.0
 */
@RunWith(Parameterized.class)
public class MyNanoJavaParameterTest {

    private MyNanoJava myNanoJava;

    @Parameterized.Parameter
    /** Constant <code>aValue</code> */
    public static String aValue;

    @Parameterized.Parameter(value = 1)
    /** Constant <code>bValue</code> */
    public static String bValue;

    @Parameterized.Parameter(value = 2)
    /** Constant <code>resValue</code> */
    public static String resValue;

    @Parameterized.Parameter(value = 3)
    /** Constant <code>mode</code> */
    public static int mode;

    @Parameterized.Parameter(value = 4)
    /** Constant <code>desc</code> */
    public static String desc;


    /**
     * <p>setUp.</p>
     */
    @Before
    public void setUp() {
        System.out.printf("Initiating ADD/SUB Nano big number balance");
        myNanoJava = new MyNanoJava();
    }

    /** Constant <code>addRealRetReal=NANO_ADD_A_B.getValue() + NANO_A_REAL.getValue() + NANO_B_REAL.getValue() + NANO_RES_REAL.getValue()</code> */
    public static final int addRealRetReal = NANO_ADD_A_B.getValue() + NANO_A_REAL.getValue() + NANO_B_REAL.getValue() + NANO_RES_REAL.getValue();
    /** Constant <code>subRealRetReal=NANO_SUB_A_B.getValue() + NANO_A_REAL.getValue() + NANO_B_REAL.getValue() + NANO_RES_REAL.getValue()</code> */
    public static final int subRealRetReal = NANO_SUB_A_B.getValue() + NANO_A_REAL.getValue() + NANO_B_REAL.getValue() + NANO_RES_REAL.getValue();
    /** Constant <code>addHexRetRaw=NANO_ADD_A_B.getValue() + NANO_A_HEX.getValue() + NANO_B_HEX.getValue() + NANO_RES_RAW.getValue()</code> */
    public static final int addHexRetRaw = NANO_ADD_A_B.getValue() + NANO_A_HEX.getValue() + NANO_B_HEX.getValue() + NANO_RES_RAW.getValue();

    /**
     * <p>getAddSubObjParams.</p>
     *
     * @return a {@link java.util.Collection} object.
     */
    @Parameterized.Parameters(name = "Testing: {4}")
    public static Collection<Object[]> getAddSubObjParams() {
        return Arrays.asList(new Object[][] {
                {"00001.0000", "01.20", "2.2", addRealRetReal,"Adding two real numbers and returns real"},
                {"1810.41927182787617881772837", "1810.41927182787617881772837", "0.0", subRealRetReal, "Subtracting two real numbers and returns real"},
                {"00000000000000000000000000000010", "00000000000000000000000000000020", "48", addHexRetRaw, "Adding two hex values and returning raw value"}
        });
    }

    /**
     * <p>testAddSub.</p>
     *
     * @throws java.lang.Exception if any.
     */
    @Test
    public void testAddSub() throws Exception {
        assertEquals(resValue, myNanoJava.nano_add_sub(aValue, bValue, mode));
    }
}
