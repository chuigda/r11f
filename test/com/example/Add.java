package com.example;

import com.example.Add2;

public class Add {
    public static int add(int a, int b) {
        return Add2.add(a, b);
    }

    public static long add_mixed(long a, int b) {
        return Add2.add_mixed(a, b);
    }
}
