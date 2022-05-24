package com.yikolemon.c51_server_vue.controller;

import com.yikolemon.c51_server_vue.common.lang.Result;
import com.yikolemon.c51_server_vue.server.SendToC51;
import com.yikolemon.c51_server_vue.server.ServerAcceptThread;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import java.net.Socket;
import java.util.concurrent.ConcurrentHashMap;

@Controller
public class SendStrController {

    @GetMapping("/getTem")
    @CrossOrigin
    @ResponseBody
    public Result getTem(@RequestParam("device") String device){
        ServerAcceptThread serverAcceptThread = PortController.serverAcceptThread;
        if (serverAcceptThread==null){
            return Result.falil("服务器未开启",null);
        }
        //设置str为单片机识别的命令
        Result result = SendToC51.sendMsgbyString(device, "NJUE,1,1,56;");
        System.out.println("发送温度命令完成");
        return result;
    }

    @GetMapping("/changeWifi")
    @CrossOrigin
    @ResponseBody
    public Result changeWifi(@RequestParam("device") String device,@RequestParam("str") String str){
        ServerAcceptThread serverAcceptThread = PortController.serverAcceptThread;
        if (serverAcceptThread==null){
            return Result.falil("服务器未开启",null);
        }
        //设置str为单片机识别的命令
        Result result = SendToC51.sendMsgbyString(device, str);
        System.out.println("发送切换热点命令完成");
        return result;
    }
}
