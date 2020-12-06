package org.mynanojava;
// jar cvmf MANIFEST.MF example.jar classes/org/mynanojava/MyNanoJava.class
public class MyNanoJava {
    static {
        System.loadLibrary("mynanojava");
    }

    private native void sayHello();
    private native String license();
// Quick test JNI ...
    public static void main(String[] args) {
        //new MyNanoJava().license();
        MyNanoJava myNanoJava = new MyNanoJava();
        System.out.printf(myNanoJava.license());
    }
}
