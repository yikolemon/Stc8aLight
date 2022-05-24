package com.yikolemon.c51_server_vue.wesocket;

import com.google.gson.Gson;
import com.yikolemon.c51_server_vue.common.lang.SocketResult;
import com.yikolemon.c51_server_vue.controller.PortController;
import org.springframework.stereotype.Component;

import javax.servlet.http.HttpSession;
import javax.websocket.*;
import javax.websocket.server.ServerEndpoint;
import java.io.IOException;
import java.util.List;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArraySet;


@ServerEndpoint(value = "/socket")  /*, configurator = GetHttpSessionConfigurator.class*/
@Component
public class SocketEndPoint {

    public static Set<SocketEndPoint> onlineUsers = new CopyOnWriteArraySet<>();

    //session对象，通过这个发送给指定的用户名
    private Session session;

/*    //httpSession存储了用户名
    private HttpSession httpSession;*/

    @OnOpen
    public void onOpen(Session session, EndpointConfig endpointConfig){

        //把局部session赋值给类session
        System.out.println("建立socket连接");
        this.session=session;
        HttpSession httpSession = (HttpSession) endpointConfig.getUserProperties().get(HttpSession.class.getName());
        //局部httpSession赋值类httpSession
        /*this.httpSession=httpSession;*/
        onlineUsers.add(this);

        if (PortController.serverAcceptThread==null){
            //如果还没有启动端口，那么serverAcceptThread为空
            SocketResult socketResult = SocketResult.systemMsg("连接服务器成功，端口未启动");
            String resStr = new Gson().toJson(socketResult);
            broadcastMsgToThisUser(resStr);
        }
        else {//已经启动端口了
            List<String> clients = PortController.serverAcceptThread.getClientListName();
            SocketResult socketResult = SocketResult.systemAndFunctionMsg("连接服务器成功，端口已启动",clients);
            String s = new Gson().toJson(socketResult);
            broadcastMsgToThisUser(s);
        }
    }

    /**关闭时调用*/
    @OnClose
    public void onClose(Session session) {
        onlineUsers.remove(this);
    }

    /*接收消息不需要使用WebSocket，使用Http的Get就可以解决*/


    /**
     * .
     * @param message 给客户端发送消息
     * @return void
     */
    public static void broadcastMsgToAllOnlineUsers(String message) {
        //所有登录用户名称
        for (SocketEndPoint socketEndPoint:onlineUsers) {
            RemoteEndpoint.Basic basicRemote = socketEndPoint.session.getBasicRemote();
            try {
                basicRemote.sendText(message);
                System.out.println("广播发送系统消息成功！");
            } catch (IOException e) {
                System.out.println("广播发送系统消息失败！");
                e.printStackTrace();
            }
        }
    }

    public static void broadcastMsgToAllOnlineUsers(SocketResult result) {
        String message = new Gson().toJson(result);
        //所有登录用户名称
        for (SocketEndPoint socketEndPoint:onlineUsers) {
            RemoteEndpoint.Basic basicRemote = socketEndPoint.session.getBasicRemote();
            try {
                basicRemote.sendText(message);
                System.out.println("广播发送系统消息成功！");
            } catch (IOException e) {
                System.out.println("广播发送系统消息失败！");
                e.printStackTrace();
            }
        }
    }

    private  void broadcastMsgToThisUser(String message){
        RemoteEndpoint.Basic basicRemote = this.session.getBasicRemote();
        try {
            basicRemote.sendText(message);
            System.out.println("广播发送系统消息成功！");
        } catch (IOException e) {
            System.out.println("广播发送系统消息失败！");
            e.printStackTrace();
        }
    }

    public static void broadcastPortToAllUser(){
        List<String> clients = PortController.serverAcceptThread.getClientListName();
        SocketResult socketResult = SocketResult.systemAndFunctionMsg("有新设备连接，更新列表",clients);
        String s = new Gson().toJson(socketResult);
        broadcastMsgToAllOnlineUsers(s);
    }

}
