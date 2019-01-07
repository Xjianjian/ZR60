package com.zzwtec.wechat.util.security;

public class MyBase64Util {
    //	static Log logger = Log.getLog(MyBase64Util.class);

    private static final int BIT_LEN = 6;
    private static final char[] ENCODE_TABLE = {'A', 'b', 'C', 'd', 'I', 'f', 'G', 'h', 'E', 'j', 'K', 'l', 'M', 'a', 'B', 'c', 'D', 'e', 'F', 'g', 'H', 'i', 'J', 'k', 'L', 'm', 'P', 'o', 'N', 'q', 'R', 's', 'T', 'u', 'X', 'w', 'V', 'y', 'Z', 'n', 'O', 'p', 'Q', 'r', 'S', 't', 'U', 'v', 'W', 'x', 'Y', 'z', '9', '1', '2', '4', '3', '5', '6', '8', '7', '0', '-', '_'};

    /**
     * 编码原理
     * 1 将源字符串src转成UTF-8的字节数组bytes
     * 2 然后将字节数组bytes每次取6位并在前补0组成一个int类型的数字num，如果不足6位，则在前面补0补够6组成一个int类型的数字num
     * 3 将得到的多个数字num参照表转换成对应字符bChar
     * 4 将多个字符bChar连成字符串得到encode
     * <p>
     * 如 src是1234
     * 1 将源字符串src转成UTF-8的字节数组bytes是[49,50,51,52]
     * 2 bytes每次取6位并在前补0组成一个int类型的数字num是[12,19,8,51,13,0]
     * 3 num参照表转换成对应字符bChar是[M,g,E,z,a,A]
     * 4 bChar连成字符串得到encode是MgEzaA
     */
    public static String encode(String src) {
        //		logger.info("encode src:"+src);
        try {
            byte[] bytes = src.getBytes("utf-8");
            int bCharsLen = (int) Math.ceil(bytes.length * 8.0 / BIT_LEN);
            char[] bChars = new char[bCharsLen];
            for (int i = 0; i < bCharsLen; i++) {
                char bChar = getBChar(bytes, i);
                if (bChar == 0) {
                    return null;
                }
                bChars[i] = bChar;
            }
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < bCharsLen; i++) {
                sb.append(bChars[i]);
            }
            String encode = sb.toString();
            //			logger.info("encode encode:"+encode);
            return encode;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    private static char getBChar(byte[] bytes, int index) {
        char bChar = 0;
        int bytesIndex = 0, bitIndex = 0;
        int len = index * BIT_LEN;
        bytesIndex = len / 8;
        bitIndex = len % 8;
        int num = 0, getBits = 0;
        for (int i = 0; i < bytes.length; i++) {
            if (i < bytesIndex) {
                continue;
            } else {
                short B = (short) (bytes[bytesIndex] & 0x00ff);
                num = B & (0xff >>> bitIndex);
                getBits = 8 - bitIndex;
                if (getBits > BIT_LEN) {
                    int left = getBits - BIT_LEN;
                    num = num >>> left;
                } else if (getBits < BIT_LEN) {
                    bytesIndex++;
                    if (bytesIndex < bytes.length) {
                        int right = BIT_LEN - getBits;
                        num = num << right;
                        short nextB = (short) (bytes[bytesIndex] & 0x00ff);
                        num = num | (nextB >>> (8 - right));
                    }
                }
                break;
            }
        }
        bChar = ENCODE_TABLE[num];
        return bChar;
    }

    /**
     * 解码原理
     * 1 将已编码字符串encode逐个分离得到bChar
     * 2 将每个bChar参照表转换成对应的索引num
     * 3 将除最后一个num的每个索引num去掉前2位并组合成一个位串preBits
     * 4 位串preBits的长度模8得到值len
     * 5 再将位串preBits与最后一个num的后8减len的差位组合成新的位串bits
     * 6 将位串bits每次取8位得到字节数组bytes
     * 7 将字节数组bytes转成UTF-8的字符串
     * <p>
     * 如 encode是MgEzaA
     * 1 将已编码字符串encode逐个分离得到bChar是[M,g,E,z,a,A]
     * 2 将每个bChar参照表转换成对应的索引num是[12,19,8,51,13,0]
     * 3 将除最后一个num的每个索引num去掉前2位[001100,010011,001000,110011,001101]并组合成一个位串preBits是001100010011001000110011001101
     * 4 位串preBits的长度30模8得到值len是6
     * 5 再将位串preBits与最后一个num(000000)的后8-6=2位组合成新的位串bits是00110001001100100011001100110100
     * 6 将位串bits每次取8位得到字节数组bytes[00110001,00110010,00110011,00110100]即[49,50,51,52]
     * 7 将字节数组bytes转成UTF-8的字符串是1234
     */
    public static String decode(String str) {
        //		logger.info("decode str:"+str);
        byte[] nums = new byte[str.length()];
        for (int i = 0, j = 0; j < nums.length; i++, j++) {
            char bChar = str.charAt(i);
            for (int k = 0; k < ENCODE_TABLE.length; k++) {
                if (ENCODE_TABLE[k] == bChar) {
                    nums[j] = (byte) k;
                    break;
                }
            }
        }
        int bytesLen = (int) Math.floor(str.length() * 6.0 / 8);
        byte[] bytes = new byte[bytesLen];
        for (int i = 0; i < bytesLen; i++) {
            bytes[i] = getByte(nums, i);
        }
        try {
            String decode = new String(bytes, "utf-8");
            //			logger.info("decode decode:"+decode);
            return decode;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    private static byte getByte(byte[] nums, int index) {
        byte b = 0;
        int bytesIndex = (index * 8 / BIT_LEN);
        int bitIndex = (index * 8) % BIT_LEN + 2;
        int getBits = 0;
        for (int i = 0; i < nums.length; i++) {
            if (i < bytesIndex) {
                continue;
            } else {
                short B = (short) (nums[bytesIndex] & 0x00ff);
                b = (byte) (B & (0xff >>> bitIndex));
                getBits = 8 - bitIndex;
                int left = 8 - getBits;
                b = (byte) (b << left);
                bytesIndex++;
                short nextB = (short) (nums[bytesIndex] & 0x00ff);
                int right = 8 - (left + 2);
                b = (byte) (b | (nextB >>> right));
                break;
            }
        }
        return b;
    }

  /*  public static void main(String[] s) {
        long time = System.currentTimeMillis();
        String srcStr = "oNgRts1fTowKDQWFmbGLF4ox6FiI&df0077b1da1b4d9f9aa3bc589d933000&012de80cfdbb4af5b130f461ad53fc22&27&智之屋X2&" + time;
        String encodeStr = encode(srcStr);
        String decodeStr = decode(encodeStr);
    }*/
}
