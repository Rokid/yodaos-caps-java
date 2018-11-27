package com.rokid.utils;

public class CapsException extends Exception {
  CapsException(int code) {
    this.code = code;
  }

  CapsException(int code, int arg0) {
    this.code = code;
    this.arg0 = arg0;
  }

  public int code;
  public int arg0;

  public static final int CODE_INVALID_PARAM = -1;
  public static final int CODE_CORRUPTED_DATA = -2;
  public static final int CODE_INSUFF_BUFFER = -1000;
}
