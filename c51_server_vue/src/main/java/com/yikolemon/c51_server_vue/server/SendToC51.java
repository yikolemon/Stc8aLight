package com.yikolemon.c51_server_vue.server;

import com.yikolemon.c51_server_vue.common.lang.Result;
import com.yikolemon.c51_server_vue.controller.PortController;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.concurrent.ConcurrentHashMap;

public class SendToC51 {


    public static Result sendMsgbyString(String device, String str){
        ConcurrentHashMap<String, Socket> clientList = PortController.serverAcceptThread.getClientList();
        if (device.equals("")){
            return Result.falil("未选择设备",null);
        }
        if (device.equals("All")){
            //全部都发
            for (Socket socket:clientList.values()) {
                sendMsgtbySocket(socket,str);
            }
            return Result.falil("发送命令成功",null);

        }
        else {
            if (!clientList.containsKey(device)){
                return Result.falil("此设备不存在",null);
            }
            else {
                //单个发送
                sendMsgtbySocket(clientList.get(device),str);
                return Result.falil("发送命令成功",null);
            }
        }
    }

    public static void sendMsgtbySocket(Socket socket,String str){
        try {
            OutputStream outputStream = socket.getOutputStream();
            PrintWriter printWriter = new PrintWriter(outputStream);
            printWriter.write(str+"\r\n");
            printWriter.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}
