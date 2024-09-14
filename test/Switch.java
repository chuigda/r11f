package tech.icey.r11f.test;

public class Switch {
    public static String switch1(int a) {
        switch (a) {
            case 1: return "one";
            case 2: return "two";
            case 3: return "three";
            case -1: return "negative one";
            case -2: return "negative two";
            default: return "unknown";
        }
    }

    public static String switch2(int a) {
        switch (a) {
            case 114: return "逸一时"; // 如果追求一时的快乐
            case 514: return "误一世"; // 就会耽误一世的前途
            case 1919: return "逸久逸久"; // 就这样一直贪图享乐
            case 810: return "罢已龄"; // 回过头来已经老了

            case 7355608: return "三万里河入东海，五千仞岳上摩天";
            case 124875: return "遗民泪尽胡尘里，南望王师又一年";

            default: return "垂死病中惊坐起，笑问客从何处来";
        }
    }
}