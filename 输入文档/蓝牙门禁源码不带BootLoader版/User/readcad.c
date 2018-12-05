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
 *     @author : Ԭ��о
 *     time   : 2017/04/26
 *     desc   : mifare����ģ�����, ʹ����BW13V������, ����ģ����TX522B
 *     version: 1.0
 * </pre>
 */

public class MifareReadService extends Service implements SerialPortClient.OnErrorListener, SerialPortClient.OnMsgListener {
    private static final String TAG = "MifareReadService";
    public static final boolean DEBUG = true;
    /**
     * ��֤������Կ
     */
    private static final char[] myKey = {0xF5, 0x79, 0xED, 0xD8, 0x9F, 0x77};

    /**
     * ��ģ����Ҫ�Ĳ���
     */
    private File device = new File("/dev/ttyS3");   // �豸
    private int baudrate = 9600;                    // ģ��Ĳ�����
    private int flag = 0;                           // ��ǩ

    private boolean isSectedCard = false;

    /**
     * ����ģ��Ĵ��������
     */
    private SerialPortClient serialPortClient;      // ���ڴ��������


    private ReadMifareHandler handler;


    /**
     * һ��CardInfo����, ��ʾIC����ID��, С��ID, ¥�����1, ¥�����2����Ϣ
     */
    private CardInfo cardInfo = new CardInfo();

    /**
     * <pre>
     *     @author Ԭ��о
     *     @date 2017-04-28
     *     @desc SEQNR�Ƕ�����ʱ��, �����Զ���Ĵ��ݸ�ģ��, Ȼ��ģ��ԭ�����ص�һ��ʮ�������ַ�;
     *     ���ڽ����������ָ���ģ�鷴��������һһ��Ӧ, ����: ��������һ��seqnrΪ0x01������A��,
     *     ģ�鷵��һ��seqnrҲΪ0x01������B, ��ô������Ϊ����B��ģ�����������A�ķ���
     * </pre>
     */
    interface SEQNR {
        /**
         * ��������ʱ��ģ�����������������0x00
         */
        char On_Card = 0x00;
        /**
         * ����key
         */
        char Load_Key = 0x01;

        /**
         * ���Ƭ����ȡ��Ƭ��
         */
        char Get_CardSnr = 0x02;

        /**
         * ����Ϊ������鿨Ƭģʽ IDLEģʽ
         */
        char SetDetectCard_ALL = 0x03;

        /**
         * ����0x14 �洢��С��ID��ϢcommunityId
         */
        char Block_0x14 = 0x14;

        /**
         * ����0x15 �洢��¥�������ϢbuildNum1
         */
        char Block_0x15 = 0x15;

        /**
         * ����0x16 �����¥�������ϢbuildNum2
         */
        char Block_0x16 = 0x16;

        /**
         * ���ݿ�10, ��IC���洢�û�ID��
         */
        char Block_0x10 = 0x10;

        /**
         * ���ݿ�10, ��IC���洢�ֻ�����Ŀ�
         */
        char Block_0x11 = 0x11;

    }

    /**
     * <pre>
     *     @author Ԭ��о
     *     @date 2017-04-28
     *     @desc ��������ģ�������. ��Щ�����ǰ���ģ��ĳ����ṩ��Э��ƴ�Ӷ��ɵ�, ������Щָ��ǰ, ��Ҫ����
     *     ָ���Ч����, �ٽ�ָ��תΪbyte[]���鷢��ģ��;
     *     �Ѿ���װ������������ֱ�ӵ���
     *     {@link #sendCommand(char[])} ����ָ��, �ڲ�ʵ����:
     *                                  1. ���ȼ���Ч���벢��ֵ����������(�����ڶ���Ԫ��);
     *                                  2. ���Ž�char����תΪbyte����;
     *                                  3. ��byte����ͨ��{@link #serialPortClient} ���ݸ�ģ��
     *     {@link #readBlock(char[], char)} ������, ����һ�Ƕ�������ָ��{@link #Read_Block},
     *                                             ��������Ҫ��������; �ڲ�ʵ����:
     *                                  1. ��Ҫ����������ַ��ֵ����������, ����������ַ��Ϊ���������
     *                                     seqnr��ֵ����������;
     *                                  2. ����sendCommand(char[])������������
     * </pre>
     */
    interface COMMANDS {


        /**
         * װ����Կ
         */
        char[] Load_Key = {0x20, 0x01, 0x20, 0x06, 0xF5, 0x79, 0xED, 0xD8, 0x9F, 0x77, 0x00, 0x03};

        /**
         * �������߷�Χ�ڵ����п�
         */
        char[] SetDetectCard_ALL = {0x20, 0x03, 0x25, 0x01, 0x01, 0x00, 0x03};

        /**
         * ��ȡģ�������:
         * 0x20��ʾ��ʼ��
         * 0x00��ʾ����(��ʵ��������д��ģ��, ģ��ԭ������)
         * 0x22��ʾ�Ƕ�����������
         * 0x01��ʾ����Я��������ֻ��һλ
         * 0x02��ʾ���ζ�ȡ��������02�е�����,
         * 0x00��ʾ������֤��, ��Ҫ����
         * 0x03��ʾ��������Ľ�����
         */
        char[] Read_Block = {0x20, 0x00, 0x22, 0x01, 0x02, 0x00, 0x03};
    }

    /**
     * <pre>
     *     @author Ԭ��о
     *     @date 2017-04-28
     *     @desc һ��RebackInfo�����ʾһ��ģ��������ķ�����Ϣ
     *
     * </pre>
     */
    private static class RebackInfo {
        /**
         * ��ʾ��
         */
        char seqnr;

        /**
         * ״̬��
         */
        char status;

        /**
         * ��������
         */
        char[] data;

        /**
         * <pre>
         *     @author Ԭ��о
         *     @desc ״̬���ֶ�
         *     @date 2017-04-28
         * </pre>
         */
        interface Status {

            char OK = 0x00;             // ģ��ĺ������óɹ�
            char QUIT = 0x1e;           // ��һ�η�������ʱ�����
            char COMM_ERR = 0xff;       // ����ͨ�Ŵ���
            char NO_TAG_ERR = 0x01;     // ����Ч����û�п�
            char CRC_ERR = 2;           // �ӿ��н��յ��˴����CRCУ���
            char NOT_AUTH_ERR = 0x0a;   // ��û����֤
            char PARITY_ERR = 5;        // �ӿ��н��յ��˴����Ч��λ
            char BIT_COUNT_ERR = 0x0b;  // �ӿ��н��յ��˴���������λ
        }
    }

    /**
     * <pre>
     *     @author Ԭ��о
     *     @date 2017-04-28
     *     @desc ��Ƭ��Ϣ
     * </pre>
     */
    private static class CardInfo {
        static final int OldIc_Key = 0b110001;
        static final int NewIc_Key = 0b001111;
        static final int ReadIc_Key = 0b000001;
        static final int Key = 0b111111;

        String cardId;          // ��ƬID
        String communityId;     // С��ID
        String buildNum1;       // ¥����Ϣ1
        String buildNum2;       // ¥����Ϣ2
        String phone;
        String uuid;

        char[] chipIdBytes = new char[4];     // ��ʾchipid���ֽ�����


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
     * ����Msg��handler
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
     * ����command�����bbcֵ;
     * bbc, Ч��ֵ: �Ӱ���(�����еڶ�λԪ��)��data���ݵ����һλԪ�ص����ֵ ȡ��
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
     * byte����ת��Ϊchar����
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
     * ������������Ϣ
     *
     * @param buffer ��ȡ����buffer����
     * @param size   ��ȡ������Ϣ�ĸ���
     * @return nullΪ��������
     */
    private RebackInfo decodeMsg(byte[] buffer, int size) {

        try {
            // ɸѡ�����õ�����
            byte[] datas = trans2Valid(buffer, size);

            // byteת��Ϊchar����
            char[] charDatas = byteArr2charArr(datas);

            // ת��Reback����
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
     * ��byte���͵ĸ���ת��Ϊchar���͵�����
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
     * ��ʼ�����ڲ���
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
     * ����ģ�鷴��������
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
            case SEQNR.On_Card:            // ��ǩλΪ0��ʾ��������
                decodeOnCard(rebackInfo);
                break;
            case SEQNR.Load_Key:           // ��ǩλΪLoad_Key��ʾ���Ǵӻ���������װ����Կ�ķ���
                decodeLoadKeyResult(rebackInfo);
                break;
            case SEQNR.SetDetectCard_ALL: // ��ʾ���Ǵӻ��������������Զ������ķ���
                decodeSetDetectCard(rebackInfo);
                break;


            case SEQNR.Block_0x10:         // ��ʾ���Ǵӻ���ȡ����0x10�ķ���
            case SEQNR.Block_0x11:         // ��ʾ���Ǵӻ���ȡ����0x11�ķ���
            case SEQNR.Block_0x14:         // ��ʾ���Ǵӻ���ȡ����0x14�ķ���
            case SEQNR.Block_0x15:         // ��ʾ���Ǵӻ���ȡ����0x15�ķ���
            case SEQNR.Block_0x16:         // ��ʾ���Ǵӻ���ȡ����0x16�ķ���
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
                    debug(TAG, "decodeReadBlock: ��ȡ 0x10 fail");
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

                /** �ɿ���Ҫ��chipIdת��Ϊ��д, �¿�����Ҫ */
                cardInfo.cardId = cardInfo.cardId.toUpperCase();
                handleOldIc(cardInfo);

                break;
            case SEQNR.Block_0x14:
                // ������ݿ�0x14��ȡ����ʧ��, �Ͷ�ȡ0x10���ݿ������
                // �����ȡ�ɹ��Ͷ�ȡ���ݿ�0x15������
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
     * ������IC��
     *
     * @param cardInfo
     */
    private void handleOldIc(CardInfo cardInfo) {
        long delayMillis = 1000;
        int flag = CardInfo.calcFlag(cardInfo);
        debug(TAG, "handleOldIc: 0b" + Integer.toBinaryString(flag));
        /** ��ʾ�Ѿ���ȡ���е���ic������*/
        if ((flag & CardInfo.OldIc_Key) == CardInfo.OldIc_Key) {
            OpenDoorManager.checkOldIcCard(cardInfo.cardId);
            debug(TAG, "handleOldIc: success");
            delayMillis = 1500;
        } else if ((flag & CardInfo.Key) == CardInfo.ReadIc_Key) {
            debug(TAG, "handleOldIc: ��ȡ����IC��ID��, �����������ݿ鶼��ȡ��������, ��Ч��"+cardInfo.toString());
            // OpenDoorManager.playUselessVoice();
            OpenDoorManager.checkByResidencePermit(cardInfo.chipIdBytes);
        } else {
            debug(TAG, "handleOldIc: ��ȡ����IC��ID��, ��ȡ������ĳ�����ݿ������"+cardInfo.toString());
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
     * �����ݿ�ʧ�ܵ�ʱ��Ĵ�����
     * <p>
     * <p>
     * ����Ƕ�ȡ��0x14��ʱ�����, ���Զ�0x10��(���ݾɿ�)
     * ����һ����ʾ��Ч��, ����������
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
     * �����п���ʱ��ģ�����������ݵ�����
     *
     * @param rebackInfo
     */
    private void decodeOnCard(RebackInfo rebackInfo) {
        char status = rebackInfo.status;

        switch (status) {
            case RebackInfo.Status.OK:
                debug(TAG, "decodeOnCard: ��⵽IC��, ���һ�ȡ����ic��ID��");
                // char[] validData = new char[rebackInfo.data[3]];
                System.arraycopy(rebackInfo.data, 4, cardInfo.chipIdBytes, 0, cardInfo.chipIdBytes.length);
                // System.arraycopy(rebackInfo.data, 4, validData, 0, validData.length);
                cardInfo.cardId = toHexString(cardInfo.chipIdBytes, false);
                readBlock(COMMANDS.Read_Block, (char) 0x14);
                break;
            default:
                debug(TAG, "decodeOnCard: ��⵽IC��, ��ȡIC����ʧ��");
                cardInfo.cardId = null;
                handler.sendEmptyMessageDelayed(ReadMifareHandler.READ_DELAY, 1000);
                OpenDoorManager.playUselessVoice();
                break;
        }
    }

    /**
     * DB
     * ���������Զ�����
     *
     * @param rebackInfo
     */
    private void decodeSetDetectCard(RebackInfo rebackInfo) {
        char status = rebackInfo.status;

        switch (status) {
            case RebackInfo.Status.OK:
                // �����Զ������ɹ�
                isSectedCard = true;
                break;
            default:
                // �����Զ�����ʧ��
                restartMifareRead();
                break;
        }
    }

    /**
     * ����װ��Key�Ľ��
     *
     * @param rebackInfo
     */
    private void decodeLoadKeyResult(RebackInfo rebackInfo) {
        char status = rebackInfo.status;

        switch (status) {
            case RebackInfo.Status.OK:
                // װ����Կ�ɹ�, ��������������ģ���Զ�����
                sendCommand(COMMANDS.SetDetectCard_ALL);
                break;
            default:

                // װ����Կ���ɹ�
                restartMifareRead();
                break;
        }
    }


    /**
     * 5��� ������������
     */
    private void restartMifareRead() {

        if (serialPortClient != null) {
            serialPortClient.stop();
        }

        handler.sendEmptyMessageDelayed(ReadMifareHandler.LOAD_KEY, 1000 * 5);
    }

    /**
     * ����ָ��
     *
     * @param command ָ������
     * @return false, ����ָ��ɹ�; true ����ָ��ɹ�
     */
    private boolean sendCommand(char[] command) {
        isSectedCard = false;
        if (serialPortClient == null) {
            restartMifareRead();
            return false;
        }
        // �ȼ�����ȷ��У��ֵ
        calcCommandsBBC(command);

        // �ٽ�char����ת��Ϊbyte����
        byte[] byteCommand = charArr2byteArr(command);

        try {
            // �ٽ��ֽ����鷢��ģ��
            serialPortClient.sendByteCommand(byteCommand);
        } catch (Exception e) {
            /** ���͵�ʱ�򱨴���, ��Ҫ����ģ�� */
            restartMifareRead();
            e.printStackTrace();
            return false;
        }
        return true;
    }

    /**
     * �����ݿ�
     *
     * @param blockCommand
     * @param block
     */
    private void readBlock(char[] blockCommand, char block) {
        blockCommand[4] = block;    // Ҫ��������������
        blockCommand[1] = block;    // ���ζ�ȡ���ݵ�����ͷ
        sendCommand(blockCommand);
    }

    /**
     * ��byte����ת��Ϊ��Ч������
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
     * ����������Ŀ�Ƭ��Ϣ
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
            debug(TAG, "handleNewIc: ��ȡ����IC��ID��, �����������ݿ鶼��ȡ��������, ��Ч��"+info.toString());
            OpenDoorManager.playTryAgainVoice();
        } else {
            debug(TAG, "handleNewIc: ��ȡ����IC��ID��, ��ȡ������ĳ�����ݿ������"+info.toString());
            OpenDoorManager.playTryAgainVoice();
        }
        handler.sendEmptyMessageDelayed(ReadMifareHandler.READ_DELAY, delayMillis);
    }

    /**
     * �����д�ӡlog�ķ���, ͳһ����
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
