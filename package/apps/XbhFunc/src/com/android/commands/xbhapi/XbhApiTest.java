package com.android.commands.xbhapi;

import com.xbh.eywa.api.XbhApi;
import android.content.Context;
import android.os.RemoteException;
import java.lang.reflect.Method;

public class XbhApiTest{

    /**
     * Command-line entry point.
     *
     * @param args The command-line arguments
     */
    public static void main(String[] args) {
        (new XbhApiTest()).run(args);
    }

    private void showUsage() {
        System.err.println("Usage: xbhapitest <command> [<arg>...]");
    }

    private void run(String[] args) {
        if (args.length < 1) {
            showUsage();
            return;
        }
        int value = 0, value2 = 0, resI = 0;
        boolean enable = false, resB = false;

        int len = args.length;
        String func = args[0];

        if(func.equals("setPictureMode")){
            if(args.length == 2){
                value = Integer.parseInt(args[1]);
                resB = XbhApi.getInstance().setPictureMode(value);
                System.err.println("setPictureMode resB = " + resB);
            }else{
                System.err.println("Usage: args error!!!!!");
            }
        }
    }
}
