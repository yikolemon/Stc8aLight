package com.yikolemon.c51_server_vue.controller;

import com.yikolemon.c51_server_vue.server.SendToC51;

import java.net.Socket;

public class SendServer {

    public static void sendLight(String device){//获取光敏电阻数值命令
        SendToC51.sendMsgbyString(device,"NJUE,2,1,59;");
    }

    public static void makeAccess(Socket socket){//设置继电器通路
        SendToC51.sendMsgtbySocket(socket,"NJUE,3,1,58;");
    }

    public static void makeFail(Socket socket){//设置继电器断路
        SendToC51.sendMsgtbySocket(socket,"NJUE,3,2,57;");
    }

    public static void makeShortLight(Socket socket){//短时间亮灯
        SendToC51.sendMsgtbySocket(socket,"NJUE,5,1,60;");
    }


}
