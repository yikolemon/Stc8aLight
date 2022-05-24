package com.yikolemon.c51_server_vue.server;

import com.yikolemon.c51_server_vue.controller.PortController;
import com.yikolemon.c51_server_vue.wesocket.SocketEndPoint;

import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.*;

public class ServerAcceptThread extends Thread {
    //服务端Socket
    private ServerSocket sc= null;

    //ClientList列表
    private ConcurrentHashMap<String,Socket> ClientList;
    private ExecutorService pool;
    //服务的端口号
    private int ServerPort;

    public ServerAcceptThread(int ServerPort) {
        this.ServerPort=ServerPort;
    }

    public ConcurrentHashMap<String, Socket> getClientList() {
        return ClientList;
    }

    public List<String> getClientListName(){
        List<String> list=new ArrayList<>();
        for (String x:ClientList.keySet()){
            list.add(x);
        }
        return list;
    }

    public ServerAcceptThread() {
    }

    //获取端口号
    public int getServerPort() {
        return ServerPort;
    }

    //关闭方法
    public boolean close() {
        try {
            if (sc!=null){
                sc.close();
            }
            if (pool!=null){
                pool.shutdownNow();
            }
            this.interrupt();
        } catch (IOException e) {
            e.printStackTrace();
            return false;//发送错误关闭失败
        }
        return true;//关闭成功
    }


    @Override
    public void run() {
        //=========开启服务器========
        try {
            sc = new ServerSocket(ServerPort);//开启服务器Socket，端口号为ServerPort
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("服务器已开启");

        //============监听客户端============
        ClientList=new ConcurrentHashMap<>();
        pool=new ThreadPoolExecutor(3, 5, 1000, TimeUnit.MILLISECONDS, new ArrayBlockingQueue<Runnable>(10), new ThreadPoolExecutor.CallerRunsPolicy());
        Socket socket;

        //循环监听等待客户端的连接
        while(true){
            // 监听客户端
            try {
                socket = sc.accept();
                if(socket!=null)
                {
                    //放入socket，用来后面发送数据
                    InetAddress address=socket.getInetAddress();//获取连接客户端socket的地址
                    String hostAddress = address.getHostAddress();
                    System.out.println("当前客户端的IP："+hostAddress);//显示客户端地址
                    String uuid = UUID.randomUUID().toString();
                    ClientList.put(uuid+hostAddress,socket);
                    pool.execute(new ServerThread(socket,ClientList,uuid+hostAddress));
                    SocketEndPoint.broadcastPortToAllUser();
                }
            } catch (IOException e) {
                //e.printStackTrace();
                while(true);
            }
        }

    }
}
