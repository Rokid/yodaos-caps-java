package com.rokid.examples;

import java.io.InputStream;
import com.rokid.utils.Caps;
import com.rokid.utils.CapsException;

public class CapsExample {
  public static void main(String[] args) {
    Caps caps = Caps.create();
    Caps subcaps = Caps.create();
    byte[] binmsg = new byte[16];
    byte[] data = new byte[1024];
    int c;

    binmsg[0] = 1;
    binmsg[1] = 2;
    binmsg[2] = 3;
    binmsg[3] = 4;
    try {
      caps.write(0);
      caps.write((double)0.1);
      caps.write("hello world");
      subcaps.write(1);
      subcaps.write("foo");
      subcaps.write(binmsg);
      caps.write(subcaps);
      c = caps.serialize(data, 0);
    } catch (CapsException e) {
      e.printStackTrace();
      return;
    }
    subcaps.destroy();
    caps.destroy();
    System.out.println("caps serialize " + c + " bytes");

    try {
      caps = Caps.parse(data, 0, c);
      System.out.println("caps read int " + caps.readInt());
      System.out.println("caps read double " + caps.readDouble());
      System.out.println("caps read string " + caps.readString());
      subcaps = caps.readCaps();
      System.out.println("subcaps read int " + subcaps.readInt());
      System.out.println("subcaps read string " + subcaps.readString());
      InputStream is = subcaps.readBinary();
      System.out.println("subcaps read binary 0: " + is.read());
      System.out.println("subcaps read binary 1: " + is.read());
      System.out.println("subcaps read binary 2: " + is.read());
      System.out.println("subcaps read binary 3: " + is.read());
    } catch (Exception e) {
      e.printStackTrace();
      return;
    }
  }
}
