package tech.icey.r11f.test;

import tech.icey.r11f.test.Export;

public class Import {
    public static int complex_operation(int a, int b) {
        return Export.add(a, b);
    }
}