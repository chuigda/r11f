package tech.icey.r11f.test;

public class Add2 {
    public static long add(int a, int b) {
        return imp_add(a, b);
    }

    private static long imp_add(int a, int b) {
        return native_add(a, b);
    }

    private static native long native_add(int a, int b);
}
