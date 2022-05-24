package com.yikolemon.c51_server_vue.controller;

import com.yikolemon.c51_server_vue.common.lang.Result;
import com.yikolemon.c51_server_vue.server.ServerAcceptThread;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

@Controller
public class LightController {

   //public static boolean TurnModel=false;//false表示手动，true表示自动

    public static boolean TimeModel=false;//false表示是白天模式，true表示是晚上模式

    public static boolean forget=false;//false表示未忘记关掉，true表示忘记关灯


    @GetMapping("/setDay")//设置为白天模式
    @CrossOrigin
    @ResponseBody
    public void setDay(@RequestParam("device") String device){
        //先查看昨天有没有关灯
        TimeModel=false;
    }

    @GetMapping("/setNight")//设置为夜晚模式
    @CrossOrigin
    @ResponseBody
    public void setNight(@RequestParam("device") String device){
        //先看自动手动模式
        //先通过光敏电阻查看有没有关灯
        //设置为夜晚
        TimeModel=true;
        SendServer.sendLight(device);
        //下面内容再thread中书写
        //如果关灯了，那么forget=false，不设置为声控模式
        //如果没关灯，那么forget=true，设置为声控模式
    }

}
