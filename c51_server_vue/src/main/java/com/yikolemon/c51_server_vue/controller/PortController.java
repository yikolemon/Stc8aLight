package com.yikolemon.c51_server_vue.controller;

import com.yikolemon.c51_server_vue.common.lang.Result;
import com.yikolemon.c51_server_vue.server.ServerAcceptThread;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

@Controller
public class PortController {

    //只有一个端口连接单片机
    public static ServerAcceptThread serverAcceptThread;

    @GetMapping("/portOpen")
    @CrossOrigin
    @ResponseBody
    public Result openPort(@RequestParam("port") int port){
        if (serverAcceptThread==null){
            PortController.serverAcceptThread=new ServerAcceptThread(port);
            serverAcceptThread.start();
            return Result.succ("服务器"+port+"端口已开启",null);//200
        }
        else {
            return Result.falil("服务器端口已开启，端口号:"+serverAcceptThread.getServerPort(),null);
        }
    }

    @GetMapping("/portClose")
    @CrossOrigin
    @ResponseBody
    public Result closePort(){
        if (serverAcceptThread==null){//为null表示已经关了
            return Result.succ("服务器已关闭",null);//200
        }
        //表示没关
        boolean falg = serverAcceptThread.close();
        if (falg){
            serverAcceptThread=null;//为了垃圾回收
            return Result.succ("服务器已关闭",null);//200
        }
        else {
            return  Result.falil("服务器关闭发生错误",null);
        }
    }


}
