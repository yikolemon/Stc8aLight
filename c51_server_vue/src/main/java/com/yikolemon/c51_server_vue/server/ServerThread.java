package com.yikolemon.c51_server_vue.server;

import com.yikolemon.c51_server_vue.common.lang.SocketResult;
import com.yikolemon.c51_server_vue.controller.LightController;
import com.yikolemon.c51_server_vue.controller.PortController;
import com.yikolemon.c51_server_vue.controller.SendServer;
import com.yikolemon.c51_server_vue.wesocket.SocketEndPoint;

import java.io.*;
import java.net.Socket;
import java.util.Date;
import java.util.concurrent.ConcurrentHashMap;

public class ServerThread  extends Thread {
    private Socket socket;
    private long time=System.currentTimeMillis();

    //流
    InputStream is;
    InputStreamReader isr;
    BufferedReader br;
    OutputStream os;
    PrintWriter pw;

    //HashMap，当结束的时候需要剔除当前线程
    private ConcurrentHashMap<String,Socket> ClientList;

    //用来剔除的String
    private String id;

    public ServerThread(Socket socket,ConcurrentHashMap<String,Socket> ClientList,String id) {
        //初始化函数
        this.socket = socket;
        this.id=id;
        this.ClientList=ClientList;
        //初始化时需要进行消息发送
    }

    public void run() {
        try {
            is = socket.getInputStream();
            isr = new InputStreamReader(is);
            br = new BufferedReader(isr);
            String info;

            while((info=br.readLine())!=null){
                //如果是温湿度的数据，或者是Tick
                System.out.println("我是服务器，客户端说："+info);
                if (info.equals("TICK")||info.contains("RH")){//是TICK或者是温湿度
                    //if (info.contains("RH")){
                    SocketResult socketResult = SocketResult.userMsg(id, info);
                    SocketEndPoint.broadcastMsgToAllOnlineUsers(socketResult);
                    //}
                    sendOk();
                }
                else {//说明这是light和sound数据
                    //对消息进行判断
                    if (LightController.TimeModel==true){
                        //说明这是我设置为夜晚模式的检测结果
                        if (info.charAt(0)=='L'){//第一次设置的返回light数据
                            int num=0;
                            for (int i = 2; info.charAt(i)!='.' ; i++) {
                                num=num*10+info.charAt(i)-'0';
                            }
                            if (num>30){//说明黑了,已经关灯了
                                LightController.forget=false;//没忘记关灯
                                //那么继电器（灯）置为通路,（还有已经关灯的含义）
                                SendServer.makeAccess(socket);
                                SocketResult night = SocketResult.userMsg(id, "已关灯，继电器通路");
                                SocketEndPoint.broadcastMsgToAllOnlineUsers(night);
                            }
                            else {//说明还亮着，需要关灯
                                LightController.forget=true;//忘记关灯
                                //那么继电器（灯）置为断路
                                SendServer.makeFail(socket);
                                SocketResult day = SocketResult.userMsg(id, "忘记关灯了，切断继电器");
                                SocketEndPoint.broadcastMsgToAllOnlineUsers(day);
                            }
                        }
                        //如果忘记关灯需要对声音进行处理
                        if (LightController.forget){
                            if (info.equals("this is sound")){
                                if (System.currentTimeMillis()>time){
                                    //说明超过了去重时间
                                    info="有声音";
                                    time=System.currentTimeMillis()+5000;//加5s去重时间
                                    //发送亮一小会指令
                                    SendServer.makeShortLight(socket);
                                    SocketResult nightLight = SocketResult.userMsg(id, "有声音，亮灯5s");
                                    SocketEndPoint.broadcastMsgToAllOnlineUsers(nightLight);
                                }
                                else {//没有超过去重时间，什么都不干
                                    continue;
                                }
                            }
                        }
                        //没有忘记关灯，不处理
                    }else {//这代表是白天了
                        //对于白天的声音处理，如果忘记关灯，设置为通路
                        //设置完后把forget置为false，只处理一次声音通路
                        if (LightController.forget){//忘记关掉
                            if (info.equals("this is sound")){
                                SendServer.makeAccess(socket);
                                LightController.forget=false;
                                SocketResult dayOpen = SocketResult.userMsg(id, "昨晚未关灯，有人来了，打开继电器");
                                SocketEndPoint.broadcastMsgToAllOnlineUsers(dayOpen);
                            }
                        }
                        //如果没有忘记关灯，对声音不处理
                    }
                }
            }
        } catch (Exception e) {
            // TODO: handle exception
        }  finally{
            //关闭资源
            close();
        }
    }

    //发送ok函数
    private boolean sendOk(){
        try {
            os = socket.getOutputStream();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        pw = new PrintWriter(os);
        pw.write("TICK\r\n");
        pw.flush();
        return true;
    }

    //关闭资源函数
    private boolean close(){
        try {
            if(pw!=null)
                pw.close();
            if(os!=null)
                os.close();
            if(br!=null)
                br.close();
            if(isr!=null)
                isr.close();
            if(is!=null)
                is.close();
            if(socket!=null)
            {
                if (ClientList.containsKey(id)) {
                    ClientList.remove(id);
                    SocketEndPoint.broadcastPortToAllUser();
                }
                socket.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }
}
