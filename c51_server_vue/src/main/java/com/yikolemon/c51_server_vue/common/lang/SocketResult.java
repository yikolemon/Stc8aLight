package com.yikolemon.c51_server_vue.common.lang;

import lombok.Data;

@Data
public class SocketResult {

    private boolean functionFlag;
    private boolean systemFlag;
    private String id;
    private String msg;
    private Object data;

    public static SocketResult systemMsg(String msg){
        SocketResult socketResult = new SocketResult();
        socketResult.setSystemFlag(true);
        socketResult.setMsg(msg);
        return socketResult;
    }

    public static SocketResult userMsg(String id,String msg){
        SocketResult socketResult = new SocketResult();
        socketResult.setId(id);
        socketResult.setMsg(msg);
        return socketResult;
    }

    public static SocketResult functionMsg(Object object){
        SocketResult socketResult=new SocketResult();
        socketResult.setFunctionFlag(true);
        socketResult.setData(object);
        return socketResult;
    }

    public static SocketResult systemAndFunctionMsg(String msg,Object object){
        SocketResult socketResult = new SocketResult();
        socketResult.setSystemFlag(true);
        socketResult.setMsg(msg);
        socketResult.setFunctionFlag(true);
        socketResult.setData(object);
        return socketResult;
    }

}
