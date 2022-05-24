package com.yikolemon.c51_server_vue.common.lang;

import lombok.Data;

@Data
public class Result {

    private int code;//200表示正常，非200表示异常
    private String msg;
    private  Object data;

    public  static  Result succ(int code,String msg,Object data){
        Result result = new Result();
        result.setCode(code);
        result.setMsg(msg);
        result.setData(data);
        return result;
    }

    public  static  Result succ(String msg,Object data){
        Result result = new Result();
        result.setCode(200);
        result.setMsg(msg);
        result.setData(data);
        return result;
    }

    public  static  Result falil(String msg,Object data){
        Result result = new Result();
        result.setCode(500);
        result.setMsg(msg);
        result.setData(data);
        return result;
    }

}
