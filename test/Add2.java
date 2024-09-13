package tech.icey.r11f.test;

public class Add2 {
    public long add(int a, int b) {
        return imp_add(a, b);
    }

    private long imp_add(int a, int b) {
        return native_add(a, b);
    }

    private static native long native_add(int a, int b);
}
