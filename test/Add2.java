package tech.icey.r11f.test;

public class Add2 {
    public static long add(long a, long b) {
        return imp_add(a, b);
    }

    private static long imp_add(long a, long b) {
        return native_add(a, b);
    }

    private static native long native_add(long a, long b);

    private static native String native_add_string(String a, String b);
}
