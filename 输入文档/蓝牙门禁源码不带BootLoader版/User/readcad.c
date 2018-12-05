package com.pointercn.doorbell.service;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.annotation.Nullable;
import android.util.Log;

import com.pointercn.doorbell.util.OpenDoorManager;

import java.io.File;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.Arrays;

import android_serialport_api.SerialPortClient;

/**
 * <pre>
 *     @author : 袁光芯
 *     time   : 2017/04/26
 *     desc   : mifare读卡模块服务, 使用于BW13V机型上, 读卡模块是TX522B
 *     version: 1.0
 * </pre>
 */

public class MifareReadService extends Service implements SerialPortClient.OnErrorListener, SerialPortClient.OnMsgListener {
    private static final String TAG = "MifareReadService";
    public static final boolean DEBUG = true;
    /**
     * 验证卡的秘钥
     */
    private static final char[] myKey = {0xF5, 0x79, 0xED, 0xD8, 0x9F, 0x77};

    /**
     * 打开模块需要的参数
     */
    private File device = new File("/dev/ttyS3");   // 设备
    private int baudrate = 9600;                    // 模块的波特率
    private int flag = 0;                           // 标签

    private boolean isSectedCard = false;

    /**
     * 操作模块的代理类对象
     */
    private SerialPortClient serialPortClient;      // 串口代理类对象


    private ReadMifareHandler handler;


    /**
     * 一个CardInfo对象, 表示IC卡的ID号, 小区ID, 楼栋编号1, 楼栋编号2等信息
     */
    private CardInfo cardInfo = new CardInfo();

    /**
     * <pre>
     *     @author 袁光芯
     *     @date 2017-04-28
     *     @desc SEQNR是读卡的时候, 主机自定义的传递给模块, 然后模块原样返回的一个十六进制字符;
     *     用于将主机发起的指令和模块反馈的数据一一对应, 例如: 主机发起一条seqnr为0x01的命令A后,
     *     模块返回一条seqnr也为0x01的数据B, 那么可以认为数据B是模块对主机命令A的反馈
     * </pre>
     */
    interface SEQNR {
        /**
         * 读到卡的时候模块会主动向主机反馈0x00
         */
        char On_Card = 0x00;
        /**
         * 加载key
         */
        char Load_Key = 0x01;

        /**
         * 激活卡片并获取卡片号
         */
        char Get_CardSnr = 0x02;

        /**
         * 设置为主动检查卡片模式 IDLE模式
         */
        char SetDetectCard_ALL = 0x03;

        /**
         * 扇区0x14 存储了小区ID信息communityId
         */
        char Block_0x14 = 0x14;

        /**
         * 扇区0x15 存储了楼栋编号信息buildNum1
         */
        char Block_0x15 = 0x15;

        /**
         * 扇区0x16 村粗了楼栋编号信息buildNum2
         */
        char Block_0x16 = 0x16;

        /**
         * 数据块10, 旧IC卡存储用户ID块
         */
        char Block_0x10 = 0x10;

        /**
         * 数据块10, 旧IC卡存储手机号码的块
         */
        char Block_0x11 = 0x11;

    }

    /**
     * <pre>
     *     @author 袁光芯
     *     @date 2017-04-28
     *     @desc 主机发给模块的命令. 这些命令是按照模块的厂商提供的协议拼接而成的, 发送这些指令前, 需要计算
     *     指令的效验码, 再将指令转为byte[]数组发给模块;
     *     已经封装好两个方法供直接调用
     *     {@link #sendCommand(char[])} 发送指令, 内部实现是:
     *                                  1. 首先计算效验码并赋值给命令数组(倒数第二个元素);
     *                                  2. 接着将char数组转为byte数组;
     *                                  3. 将byte数组通过{@link #serialPortClient} 传递给模块
     *     {@link #readBlock(char[], char)} 读扇区, 参数一是读扇区的指令{@link #Read_Block},
     *                                             参数二是要读的扇区; 内部实现是:
     *                                  1. 将要读的扇区地址赋值给命令数组, 并将扇区地址作为本次命令的
     *                                     seqnr赋值给命令数组;
     *                                  2. 调用sendCommand(char[])方法发送命令
     * </pre>
     */
    interface COMMANDS {


        /**
         * 装载秘钥
         */
        char[] Load_Key = {0x20, 0x01, 0x20, 0x06, 0xF5, 0x79, 0xED, 0xD8, 0x9F, 0x77, 0x00, 0x03};

        /**
         * 请求天线范围内的所有卡
         */
        char[] SetDetectCard_ALL = {0x20, 0x03, 0x25, 0x01, 0x01, 0x00, 0x03};

        /**
         * 读取模块的数据:
         * 0x20表示开始符
         * 0x00表示包名(其实就是主机写给模块, 模块原样返回)
         * 0x22表示是读卡操作命令
         * 0x01表示本次携带的数据只有一位
         * 0x02表示本次读取的是扇区02中的数据,
         * 0x00表示的是验证码, 需要计算
         * 0x03表示的是命令的结束符
         */
        char[] Read_Block = {0x20, 0x00, 0x22, 0x01, 0x02, 0x00, 0x03};
    }

    /**
     * <pre>
     *     @author 袁光芯
     *     @date 2017-04-28
     *     @desc 一个RebackInfo对象表示一次模块给主机的反馈信息
     *
     * </pre>
     */
    private static class RebackInfo {
        /**
         * 标示符
         */
        char seqnr;

        /**
         * 状态码
         */
        char status;

        /**
         * 数据数组
         */
        char[] data;

        /**
         * <pre>
         *     @author 袁光芯
         *     @desc 状态码字段
         *     @date 2017-04-28
         * </pre>
         */
        interface Status {

            char OK = 0x00;             // 模块的函数调用成功
            char QUIT = 0x1e;           // 上一次发送命令时被打断
            char COMM_ERR = 0xff;       // 串行通信错误
            char NO_TAG_ERR = 0x01;     // 在有效期内没有卡
            char CRC_ERR = 2;           // 从卡中接收到了错误的CRC校验和
            char NOT_AUTH_ERR = 0x0a;   // 卡没有验证
            char PARITY_ERR = 5;        // 从卡中接收到了错误的效验位
            char BIT_COUNT_ERR = 0x0b;  // 从卡中接收到了错误数量的位
        }
    }

    /**
     * <pre>
     *     @author 袁光芯
     *     @date 2017-04-28
     *     @desc 卡片信息
     * </pre>
     */
    private static class CardInfo {
        static final int OldIc_Key = 0b110001;
        static final int NewIc_Key = 0b001111;
        static final int ReadIc_Key = 0b000001;
        static final int Key = 0b111111;

        String cardId;          // 卡片ID
        String communityId;     // 小区ID
        String buildNum1;       // 楼栋信息1
        String buildNum2;       // 楼栋信息2
        String phone;
        String uuid;

        char[] chipIdBytes = new char[4];     // 表示chipid的字节数组


        static int calcFlag(CardInfo info) {
            if (info == null) {
                return 0;
            }

            int cardIdFlag = info.cardId == null ? 0 : 1;
            int communityIdFlag = (info.communityId == null ? 0 : 1) << 1;
            int buildNum1Flag = (info.buildNum1 == null ? 0 : 1) << 2;
            int buildNum2Flag = (info.buildNum2 == null ? 0 : 1) << 3;
            int phoneFlag = (info.phone == null ? 0 : 1) << 4;
            int uuidFlag = (info.uuid == null ? 0 : 1) << 5;

            int result =
                    (cardIdFlag | communityIdFlag | buildNum1Flag | buildNum2Flag | phoneFlag | uuidFlag);
            return result;
        }

        @Override
        public String toString() {
            return "cardId: " + cardId + ",\n"
                    + "communityId: " + communityId + ", \n "
                    + "buildNum1: " + buildNum1 + ", \n"
                    + "buildNum2: " + buildNum2;
        }

        public void releaseNull() {
            this.cardId = null;
            this.communityId = null;
            this.buildNum1 = null;
            this.buildNum2 = null;
            this.phone = null;
            this.uuid = null;
        }
    }

    /**
     * 处理Msg的handler
     */
    private static class ReadMifareHandler extends Handler {

        static final int LOAD_KEY = 1;

        static final int READ_DELAY = 2;

        private WeakReference<MifareReadService> ref;

        ReadMifareHandler(MifareReadService ms) {
            ref = new WeakReference<>(ms);
        }

        @Override
        public void handleMessage(Message msg) {
            MifareReadService service = ref.get();
            if (service == null) {
                return;
            }

            switch (msg.what) {
                case LOAD_KEY:
                    service.initSerialPortClient();
                    break;
                case READ_DELAY:
                    service.releaseCardInfo();
                    service.sendCommand(COMMANDS.SetDetectCard_ALL);
                    break;

            }
        }
    }


    /**
     * 计算command数组的bbc值;
     * bbc, 效验值: 从包名(数组中第二位元素)到data数据的最后一位元素的异或值 取反
     * char bbc = ~(src[1] ^ src[2] ^ ... ^ src[src.length-3])
     *
     * @param src
     * @return
     */
    private char[] calcCommandsBBC(char[] src) {

        int length = src.length;

        char c = src[1];
        for (int i = 2; i < length - 2; i++) {
            c ^= src[i];
        }

        src[length - 2] = (char) ~c;

        return src;
    }

    /**
     * @param chs
     * @return
     */
    private byte[] charArr2byteArr(char[] chs) {
        int length = chs.length;
        byte[] bytes = new byte[length];
        for (int i = 0; i < length; i++) {
            bytes[i] = (byte) chs[i];
        }

        return bytes;
    }

    /**
     * byte数组转换为char数组
     *
     * @param arr
     * @return
     */
    private char[] byteArr2charArr(byte[] arr) {
        int length = arr.length;
        char[] chars = new char[length];
        byte b = 0;
        for (int i = 0; i < length; i++) {
            b = arr[i];
            if (b < 0) {
                chars[i] = recoverByte2Char(b);
            } else {
                chars[i] = (char) b;
            }
        }

        return chars;
    }

    /**
     * 解析反馈的信息
     *
     * @param buffer 读取到的buffer数组
     * @param size   读取到的信息的个数
     * @return null为解析错误
     */
    private RebackInfo decodeMsg(byte[] buffer, int size) {

        try {
            // 筛选掉无用的数据
            byte[] datas = trans2Valid(buffer, size);

            // byte转换为char数组
            char[] charDatas = byteArr2charArr(datas);

            // 转成Reback对象
            RebackInfo rebackInfo = new RebackInfo();
            rebackInfo.seqnr = charDatas[1];
            rebackInfo.status = charDatas[2];

            int dataLength = charDatas[3];
            rebackInfo.data = new char[dataLength];
            System.arraycopy(charDatas, 4, rebackInfo.data, 0, dataLength);
            return rebackInfo;
        } catch (Exception e) {

        }
        return null;
    }

    /**
     * 将byte类型的负数转换为char类型的正数
     *
     * @param b
     * @return
     */
    private char recoverByte2Char(byte b) {
        return (char) (b & 0xFF);
    }

    private String toHexString(char[] args, boolean toUpperCase) {
        StringBuilder sBuilder = new StringBuilder();

        for (int i = 0; i < args.length; i++) {
            String hexString = Integer.toHexString(args[i] & 0xFF);
            sBuilder.append(hexString.length() < 2 ? "0" + hexString
                    : hexString);
        }

        return toUpperCase ? sBuilder.toString().toUpperCase() : sBuilder.toString();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        Log.d(TAG,"oncreate");

        handler = new ReadMifareHandler(this);

        handler.sendEmptyMessage(ReadMifareHandler.LOAD_KEY);
    }


    /**
     * 初始化串口参数
     */
    private void initSerialPortClient() {
        serialPortClient = SerialPortClient.getSerialPortClient();

        try {
            serialPortClient.start(device, baudrate, flag);
            serialPortClient.regiestSerialPortErrorListener(this);
            serialPortClient.regiestSerialPortListener(this);
            sendCommand(COMMANDS.SetDetectCard_ALL);
        } catch (IOException e) {
            restartMifareRead();
        }
    }


    @Override
    public void onError() {
        restartMifareRead();

    }

    @Override
    public void onMsgReceived(byte[] buffer, int size) {
        Log.d(TAG, Arrays.toString(buffer));
        handleMsg(buffer, size);
    }

    /**
     * 处理模块反馈的数据
     *
     * @param buffer
     * @param size
     */
    private void handleMsg(byte[] buffer, int size) {
        RebackInfo rebackInfo = decodeMsg(buffer, size);
        if (rebackInfo == null) {
            return;
        }

        switch (rebackInfo.seqnr) {
            case SEQNR.On_Card:            // 标签位为0表示读到卡了
                decodeOnCard(rebackInfo);
                break;
            case SEQNR.Load_Key:           // 标签位为Load_Key表示这是从机给主机的装载秘钥的反馈
                decodeLoadKeyResult(rebackInfo);
                break;
            case SEQNR.SetDetectCard_ALL: // 表示这是从机给主机的设置自动读卡的反馈
                decodeSetDetectCard(rebackInfo);
                break;


            case SEQNR.Block_0x10:         // 表示这是从机读取扇区0x10的反馈
            case SEQNR.Block_0x11:         // 表示这是从机读取扇区0x11的反馈
            case SEQNR.Block_0x14:         // 表示这是从机读取扇区0x14的反馈
            case SEQNR.Block_0x15:         // 表示这是从机读取扇区0x15的反馈
            case SEQNR.Block_0x16:         // 表示这是从机读取扇区0x16的反馈
                decodeReadBlock(rebackInfo);
                break;
        }
    }


    private void decodeReadBlock(RebackInfo rebackInfo) {
        char status = rebackInfo.status;
        char seqnr = rebackInfo.seqnr;

        switch (seqnr) {
            case SEQNR.Block_0x10:
                if (status != RebackInfo.Status.OK) {
                    cardInfo.uuid = null;
                    debug(TAG, "decodeReadBlock: 获取 0x10 fail");
                } else {
                    cardInfo.uuid = toHexString(rebackInfo.data, false);
                    debug(TAG, "decodeReadBlock: 0x10 success");
                }

                readBlock(COMMANDS.Read_Block, (char) 0x11);
                break;
            case SEQNR.Block_0x11:

                if (status != RebackInfo.Status.OK) {
                    debug(TAG, "decodeReadBlock: 0x11 fail");
                    cardInfo.phone = null;
                } else {
                    cardInfo.phone = toHexString(rebackInfo.data, false);
                    debug(TAG, "decodeReadBlock: 0x11 success");
                }

                /** 旧卡需要将chipId转换为大写, 新卡不需要 */
                cardInfo.cardId = cardInfo.cardId.toUpperCase();
                handleOldIc(cardInfo);

                break;
            case SEQNR.Block_0x14:
                // 如果数据块0x14读取数据失败, 就读取0x10数据块的数据
                // 如果读取成功就读取数据块0x15的数据
                if (status != RebackInfo.Status.OK) {
                    readBlock(COMMANDS.Read_Block, (char) 0x10);
                    cardInfo.communityId = null;
                    debug(TAG, "decodeReadBlock: 0x14 fail");
                } else {
                    cardInfo.communityId = toHexString(rebackInfo.data, false);
                    readBlock(COMMANDS.Read_Block, (char) 0x15);
                    debug(TAG, "decodeReadBlock: 0x14 success");
                }
                break;
            case SEQNR.Block_0x15:
                if (status != RebackInfo.Status.OK) {
                    cardInfo.buildNum1 = null;
                    debug(TAG, "decodeReadBlock: Block_0x15 fail");
                } else {
                    cardInfo.buildNum1 = toHexString(rebackInfo.data, false);
                    debug(TAG, "decodeReadBlock: Block_0x15 success");
                }
                readBlock(COMMANDS.Read_Block, (char) 0x16);
                break;
            case SEQNR.Block_0x16:
                if (status != RebackInfo.Status.OK) {
                    cardInfo.buildNum2 = null;
                    debug(TAG, "decodeReadBlock: Block_0x16 fail");
                } else {
                    cardInfo.buildNum2 = toHexString(rebackInfo.data, false);
                    debug(TAG, "decodeReadBlock: Block_0x16 success");
                }

                cardInfo.cardId = cardInfo.cardId.toUpperCase();

                handleNewIc(cardInfo);
//                handler.sendEmptyMessageDelayed(ReadMifareHandler.READ_DELAY, 1000);
                break;
        }
    }

    /**
     * 处理老IC卡
     *
     * @param cardInfo
     */
    private void handleOldIc(CardInfo cardInfo) {
        long delayMillis = 1000;
        int flag = CardInfo.calcFlag(cardInfo);
        debug(TAG, "handleOldIc: 0b" + Integer.toBinaryString(flag));
        /** 表示已经获取所有的老ic卡数据*/
        if ((flag & CardInfo.OldIc_Key) == CardInfo.OldIc_Key) {
            OpenDoorManager.checkOldIcCard(cardInfo.cardId);
            debug(TAG, "handleOldIc: success");
            delayMillis = 1500;
        } else if ((flag & CardInfo.Key) == CardInfo.ReadIc_Key) {
            debug(TAG, "handleOldIc: 获取到了IC卡ID号, 但是所有数据块都获取不到数据, 无效卡"+cardInfo.toString());
            // OpenDoorManager.playUselessVoice();
            OpenDoorManager.checkByResidencePermit(cardInfo.chipIdBytes);
        } else {
            debug(TAG, "handleOldIc: 获取到了IC卡ID号, 获取了其中某个数据块的数据"+cardInfo.toString());
            OpenDoorManager.playTryAgainVoice();
        }

        handler.sendEmptyMessageDelayed(ReadMifareHandler.READ_DELAY, delayMillis);
    }

    private void releaseCardInfo() {
        if (cardInfo != null) {
            cardInfo.releaseNull();
        }
    }


    /**
     * 读数据块失败的时候的处理方法
     * <p>
     * <p>
     * 如果是读取块0x14的时候错误, 则尝试读0x10块(兼容旧卡)
     * 否则一律提示无效卡, 并结束读卡
     *
     * @param rebackInfo
     */
    private void dealReadBlockFail(RebackInfo rebackInfo) {
        char seqnr = rebackInfo.seqnr;
        if (seqnr == SEQNR.Block_0x14) {
            readBlock(COMMANDS.Read_Block, (char) 0x10);
        } else {
            handler.sendEmptyMessageDelayed(ReadMifareHandler.READ_DELAY, 1000);
            OpenDoorManager.playUselessVoice();
        }
    }

    /**
     * 处理有卡的时候模块向主机传递的数据
     *
     * @param rebackInfo
     */
    private void decodeOnCard(RebackInfo rebackInfo) {
        char status = rebackInfo.status;

        switch (status) {
            case RebackInfo.Status.OK:
                debug(TAG, "decodeOnCard: 检测到IC卡, 并且获取到了ic卡ID号");
                // char[] validData = new char[rebackInfo.data[3]];
                System.arraycopy(rebackInfo.data, 4, cardInfo.chipIdBytes, 0, cardInfo.chipIdBytes.length);
                // System.arraycopy(rebackInfo.data, 4, validData, 0, validData.length);
                cardInfo.cardId = toHexString(cardInfo.chipIdBytes, false);
                readBlock(COMMANDS.Read_Block, (char) 0x14);
                break;
            default:
                debug(TAG, "decodeOnCard: 检测到IC卡, 获取IC卡号失败");
                cardInfo.cardId = null;
                handler.sendEmptyMessageDelayed(ReadMifareHandler.READ_DELAY, 1000);
                OpenDoorManager.playUselessVoice();
                break;
        }
    }

    /**
     * DB
     * 处理设置自动读卡
     *
     * @param rebackInfo
     */
    private void decodeSetDetectCard(RebackInfo rebackInfo) {
        char status = rebackInfo.status;

        switch (status) {
            case RebackInfo.Status.OK:
                // 设置自动读卡成功
                isSectedCard = true;
                break;
            default:
                // 设置自动读卡失败
                restartMifareRead();
                break;
        }
    }

    /**
     * 处理装载Key的结果
     *
     * @param rebackInfo
     */
    private void decodeLoadKeyResult(RebackInfo rebackInfo) {
        char status = rebackInfo.status;

        switch (status) {
            case RebackInfo.Status.OK:
                // 装载秘钥成功, 接下来就是设置模块自动读卡
                sendCommand(COMMANDS.SetDetectCard_ALL);
                break;
            default:

                // 装载秘钥不成功
                restartMifareRead();
                break;
        }
    }


    /**
     * 5秒后 重启读卡流程
     */
    private void restartMifareRead() {

        if (serialPortClient != null) {
            serialPortClient.stop();
        }

        handler.sendEmptyMessageDelayed(ReadMifareHandler.LOAD_KEY, 1000 * 5);
    }

    /**
     * 发送指令
     *
     * @param command 指令数组
     * @return false, 发送指令不成功; true 发送指令成功
     */
    private boolean sendCommand(char[] command) {
        isSectedCard = false;
        if (serialPortClient == null) {
            restartMifareRead();
            return false;
        }
        // 先计算正确的校验值
        calcCommandsBBC(command);

        // 再将char数组转换为byte数组
        byte[] byteCommand = charArr2byteArr(command);

        try {
            // 再将字节数组发给模块
            serialPortClient.sendByteCommand(byteCommand);
        } catch (Exception e) {
            /** 发送的时候报错了, 需要重启模块 */
            restartMifareRead();
            e.printStackTrace();
            return false;
        }
        return true;
    }

    /**
     * 读数据块
     *
     * @param blockCommand
     * @param block
     */
    private void readBlock(char[] blockCommand, char block) {
        blockCommand[4] = block;    // 要读的扇区的数据
        blockCommand[1] = block;    // 本次读取数据的请求头
        sendCommand(blockCommand);
    }

    /**
     * 将byte数组转换为有效的数组
     *
     * @param src
     * @param size
     * @return
     */
    private byte[] trans2Valid(byte[] src, int size) {
        byte[] des = new byte[size];
        System.arraycopy(src, 0, des, 0, size);
        return des;
    }

    private String[] chars2Hex(char[] chars) {
        if (chars == null) return null;
        String[] array = new String[chars.length];

        int length = array.length;
        for (int i = 0; i < length; i++) {

            String hex = Integer.toHexString(chars[i] & 0xFF);
            array[i] = (hex.length() < 2 ? "0" + hex : hex);
        }
        return array;
    }

    /**
     * 处理读出来的卡片信息
     *
     * @param info
     */
    private void handleNewIc(CardInfo info) {
        long delayMillis = 1000 * 1;
        int flag = CardInfo.calcFlag(info);
        debug(TAG, "handleNewIc: 0b" + Integer.toBinaryString(flag));
        if ((flag & CardInfo.NewIc_Key) == CardInfo.NewIc_Key) {
            OpenDoorManager.checkIcCard(info.cardId, info.communityId, info.buildNum1, info.buildNum2);
            debug(TAG, "handleNewIc success"+", "+info.toString());
            delayMillis = 1500;
        } else if ((flag & CardInfo.Key) == CardInfo.ReadIc_Key) {
            debug(TAG, "handleNewIc: 获取到了IC卡ID号, 但是所有数据块都获取不到数据, 无效卡"+info.toString());
            OpenDoorManager.playTryAgainVoice();
        } else {
            debug(TAG, "handleNewIc: 获取到了IC卡ID号, 获取了其中某个数据块的数据"+info.toString());
            OpenDoorManager.playTryAgainVoice();
        }
        handler.sendEmptyMessageDelayed(ReadMifareHandler.READ_DELAY, delayMillis);
    }

    /**
     * 本类中打印log的方法, 统一管理
     *
     * @param tag
     * @param msg
     */
    public void debug(String tag, String msg) {
        if (true) {
            Log.e(tag, msg);
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (serialPortClient != null) {
            serialPortClient.stop();
        }
        handler.removeCallbacksAndMessages(null);
        handler = null;
    }
}
